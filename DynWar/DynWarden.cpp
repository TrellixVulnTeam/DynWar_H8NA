#include<time.h>

#include "DevicePCAPOffline.h"
#include "NormalizeManager.h"

#include "DynWarden.h"


DynWarden *DynWarden::s_instance = NULL;

DynWarden::DynWarden() {

    // *************************************************************************
    // Reset counters
    // *************************************************************************
    this->PacketCounter = 0;
    this->FlowCounter = 0;

    // *************************************************************************
    // Instantiate the sampling manager
    // *************************************************************************
    std::unique_ptr<SamplingManager> _sampler(SamplingManager::getInstance());


}

DynWarden::~DynWarden() {

    // *************************************************************************
    // Output performance data
    // *************************************************************************
    setlocale(LC_ALL, "");
    printf("\n");
    printf("Performance data:\n");
    printf("=================\n");
    printf("Packet Counter:     %'u packets\n", PacketCounter);
    printf("Flow Counter:       %'u flows\n", FlowCounter);
    printf("Avg. Delay Counter: %'u ns\n", delayCounter);

}

DynWarden * DynWarden::getInstance() {
    if (s_instance == NULL) {
        s_instance = new DynWarden();
    }
    return s_instance;
}

int DynWarden::start() {
    std::cout << "starting dynamic warden..." << std::endl;

    // *************************************************************************
    // Initialize cukoo filters for the list of innocent flows
    // when we find an innocent flow we will store its flow ID
    // in this table so we can skip any further processing
    // *************************************************************************
    //TODO: Verify how to proper instanciate the filters. Currently I seems still
    // to be a strange workaround by utilizing another variable
    // *************************************************************************

    cuckoofilter::CuckooFilter<unsigned __int128, 32, cuckoofilter::SingleTable > a_flows(1000000);
    this->innocentFlows = &a_flows;

    cuckoofilter::CuckooFilter<unsigned __int128, 32, cuckoofilter::SingleTable > b_flows(1000000);
    this->suspiciousFlows = &b_flows;

    // Initialize cuckoo based map to store any suspicious flow ID
    // and a pointer to the flow informations.
    //FIXME: Still needs to be implemented

    // *************************************************************************
    // initialize input device
    // *************************************************************************
    inputdevice = new DevicePCAPOffline();
    inputdevice->open(true);

    // *************************************************************************
    // initialize output
    // *************************************************************************
    outputdevice = new DevicePCAPOffline();
    outputdevice->open(false);

    // start processing incomming packets
    do {
        // loop thru until no further packets become available
    } while (inputdevice->start_rx() > 0);

    // *************************************************************************
    // Close input and outout devices
    // *************************************************************************
    inputdevice->close();
    outputdevice->close();

    return 0;
}

unsigned __int128* DynWarden::getFlowIDv4(ip* ipHeader) {

    unsigned __int128 *flowID = new unsigned __int128(0);

    // *************************************************************************
    // Calculate Flow ID
    // *************************************************************************
    // the 128 bit value is a combination of specific packets field to generate
    // an unique flow identifier.
    // 
    // Used mask:
    //  0 -  31 Source IP
    // 32 -  63 Destination IP
    // 64 - 159 Source Port
    // 80 - 191 Destination Port
    // 96 - 127 reserved

    *flowID += (uint32_t) ipHeader->ip_src.s_addr;
    *flowID = *flowID << 32;

    *flowID += (uint32_t) ipHeader->ip_dst.s_addr;
    *flowID = *flowID << 16;

    switch (ipHeader->ip_p) {

        case IPPROTO_ICMP:
        {
            // Since there are no ports used in ICMP we will leave the fake port
            // numbers at 0. 0 is an invalid port on TCP and UDP and should not
            // generate any conflict with other flow id's.
            *flowID = *flowID << 48;
            break;
        }

        case IPPROTO_UDP:
        {
            udphdr* udpHeader = (udphdr*) ((char*) ipHeader + 20);
            
            *flowID += udpHeader->source;
            *flowID = *flowID << 16;

            *flowID += udpHeader->dest;
            *flowID = *flowID << 32;

            break;
        }

        case IPPROTO_TCP:
        {
            tcphdr* tcpHeader = (tcphdr*) ((char*) ipHeader + 20);

            *flowID += tcpHeader->source;
            *flowID = *flowID << 16;

            *flowID += tcpHeader->dest;
            *flowID = *flowID << 32;

            break;
        }

        case IPPROTO_IPV6:  // may implement later
        case IPPROTO_ESP:   // may implement later
        case IPPROTO_GRE:   // may implement later
        case IPPROTO_AH:    // may implement later
        default:            // not implemented
        {
            return new unsigned __int128(0);
            break;
        }
    }

    return flowID;
}

void DynWarden::receivedPacket(pcap_umbrella_t *pum) {

    // FIXME: Need to adjust to the new packet structure transfered over.
    //ip* ipPacket = (ip*) pu->pcap_packet ;
            
    // Received a packet from the input, so let's check
    // which link-layer header type we have received.
    // We can not assume its always an Ethernet type.
    ip* ip_hdr;
    timespec tm;
    
    switch (pcap_datalink(pum->pcap_desc)) {

        case DLT_EN10MB: //We got an standard Ethernet frame
            ip_hdr = (struct ip*) ((u_char*) pum->pcap_packet + sizeof(struct ether_header));
            break;

        case DLT_RAW: //this is a raw IP frame
            ip_hdr = (struct ip*) pum->pcap_packet;
            break;

        default:
            printf("Error: Unknown datalink header received.\n");
            break;
    }
    
    this->PacketCounter++;

    // Do we have an IPv4 packet ? If this is a IPv6 packet we skip the dynamic
    // warden as IPv6 handling is not implemented yet!!!
    if (ip_hdr->ip_v == 4) {

        // *********************************************************************
        // Get unique Flow ID
        // *********************************************************************
        // the 128 bit value is a combination of specific packets field to 
        // generate an unique flow identifier. When flow id = 0, we received an 
        // protocol which is not implemented yet. In this case we skip further
        // processing.    
        unsigned __int128 flowID = *getFlowIDv4(ip_hdr);
        if (flowID != 0) {

            // *****************************************************************
            // Received an known innocent flow ?
            // *****************************************************************
            // If we can identify an innocent flow by its characteristics we 
            // will store its flowID so we can skip any further processing of 
            // the packet.
            // *****************************************************************
            if (innocentFlows->Contain(flowID) == cuckoofilter::Ok) {
                outputdevice->send(pum, &tm);
                return;
            }

            // *****************************************************************
            // Is this a known flow which is already under surveillance ? If so 
            // we will process the defined normalization rules on this packet 
            // before we proceed.
            // *****************************************************************
            uint* flowInfo; //TODO: correct to proper struct type
            if (suspiciousFlows->Contain(flowID) == cuckoofilter::Ok) {

                // Lookup FlowInfo - datastructure to keep information on the 
                // various attributes and characterisitics of the IP flow.
                flowInfo = NULL;
            } else {

                // *************************************************************
                // We have received an packet of an unknown flow. Let's check 
                // with the sampling manager if this flow sould get monitored.
                // *************************************************************
                if (SamplingMgr.consider(ip_hdr)) {// yes, if the flow is sampled
                    // Add flow to suspicious flow filter.
                    if (suspiciousFlows->Add(flowID) != cuckoofilter::Ok) {
                        //printf("Error adding flow to suspicious flow filter\n");                        
                    } else this->FlowCounter++;
                }
                flowInfo = NULL;
            }
            
            // *****************************************************************
            // If we have a valid FlowInfo structure than we will start the 
            // Normalizer on this packet
            // *****************************************************************
            if (flowInfo != NULL ) {
                        // Pass IP packet to the normalizer component to execute 
                        // the various normalization techniques.
                        uint64_t* techVector = new u_int64_t(1);

                        NormManager.clean( techVector, ip_hdr);
                
            }
        }
    }

    // *************************************************************************
    // When all processing is completed we will send this packet to the output
    // *************************************************************************
    outputdevice->send(pum, &tm);
     
    // How much time did the processing take in average ?
    delayCounter =+ tm.tv_nsec;
    delayCounter = delayCounter / 2;
}