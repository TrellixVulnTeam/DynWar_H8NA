#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/ConfigurationManager.o \
	${OBJECTDIR}/DevicePCAPOffline.o \
	${OBJECTDIR}/DynWarden.o \
	${OBJECTDIR}/NormalizeManager.o \
	${OBJECTDIR}/SamplingManager.o \
	${OBJECTDIR}/main.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64
CXXFLAGS=-m64

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L/usr/local/lib -lpcap

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynwar

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynwar: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynwar ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/ConfigurationManager.o: nbproject/Makefile-${CND_CONF}.mk ConfigurationManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../cuckoofilter/src -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/ConfigurationManager.o ConfigurationManager.cpp

${OBJECTDIR}/DevicePCAPOffline.o: nbproject/Makefile-${CND_CONF}.mk DevicePCAPOffline.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../cuckoofilter/src -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DevicePCAPOffline.o DevicePCAPOffline.cpp

${OBJECTDIR}/DynWarden.o: nbproject/Makefile-${CND_CONF}.mk DynWarden.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../cuckoofilter/src -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/DynWarden.o DynWarden.cpp

${OBJECTDIR}/NormalizeManager.o: nbproject/Makefile-${CND_CONF}.mk NormalizeManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../cuckoofilter/src -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/NormalizeManager.o NormalizeManager.cpp

${OBJECTDIR}/SamplingManager.o: nbproject/Makefile-${CND_CONF}.mk SamplingManager.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../cuckoofilter/src -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/SamplingManager.o SamplingManager.cpp

${OBJECTDIR}/main.o: nbproject/Makefile-${CND_CONF}.mk main.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -I../../../cuckoofilter/src -std=c++14 -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/main.o main.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/dynwar

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
