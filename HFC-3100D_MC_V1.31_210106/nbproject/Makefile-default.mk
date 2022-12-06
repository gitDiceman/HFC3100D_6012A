#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Include project Makefile
ifeq "${IGNORE_LOCAL}" "TRUE"
# do not include local makefile. User is passing all local related variables already
else
include Makefile
# Include makefile containing local settings
ifeq "$(wildcard nbproject/Makefile-local-default.mk)" "nbproject/Makefile-local-default.mk"
include nbproject/Makefile-local-default.mk
endif
endif

# Environment
MKDIR=gnumkdir -p
RM=rm -f 
MV=mv 
CP=cp 

# Macros
CND_CONF=default
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
IMAGE_TYPE=debug
OUTPUT_SUFFIX=elf
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/HFC-3100D_MC_V1.31_210106.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/HFC-3100D_MC_V1.31_210106.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
endif

ifeq ($(COMPARE_BUILD), true)
COMPARISON_BUILD=
else
COMPARISON_BUILD=
endif

ifdef SUB_IMAGE_ADDRESS

else
SUB_IMAGE_ADDRESS_COMMAND=
endif

# Object Directory
OBJECTDIR=build/${CND_CONF}/${IMAGE_TYPE}

# Distribution Directory
DISTDIR=dist/${CND_CONF}/${IMAGE_TYPE}

# Source Files Quoted if spaced
SOURCEFILES_QUOTED_IF_SPACED=ADC.c eeprom.s LED.c main.c RS232.c SPI_RS485.c Operation.c Initialization.c Memory.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/ADC.o ${OBJECTDIR}/eeprom.o ${OBJECTDIR}/LED.o ${OBJECTDIR}/main.o ${OBJECTDIR}/RS232.o ${OBJECTDIR}/SPI_RS485.o ${OBJECTDIR}/Operation.o ${OBJECTDIR}/Initialization.o ${OBJECTDIR}/Memory.o
POSSIBLE_DEPFILES=${OBJECTDIR}/ADC.o.d ${OBJECTDIR}/eeprom.o.d ${OBJECTDIR}/LED.o.d ${OBJECTDIR}/main.o.d ${OBJECTDIR}/RS232.o.d ${OBJECTDIR}/SPI_RS485.o.d ${OBJECTDIR}/Operation.o.d ${OBJECTDIR}/Initialization.o.d ${OBJECTDIR}/Memory.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/ADC.o ${OBJECTDIR}/eeprom.o ${OBJECTDIR}/LED.o ${OBJECTDIR}/main.o ${OBJECTDIR}/RS232.o ${OBJECTDIR}/SPI_RS485.o ${OBJECTDIR}/Operation.o ${OBJECTDIR}/Initialization.o ${OBJECTDIR}/Memory.o

# Source Files
SOURCEFILES=ADC.c eeprom.s LED.c main.c RS232.c SPI_RS485.c Operation.c Initialization.c Memory.c



CFLAGS=
ASFLAGS=
LDLIBSOPTIONS=

############# Tool locations ##########################################
# If you copy a project from one host to another, the path where the  #
# compiler is installed may be different.                             #
# If you open this project with MPLAB X in the new host, this         #
# makefile will be regenerated and the paths will be corrected.       #
#######################################################################
# fixDeps replaces a bunch of sed/cat/printf statements that slow down the build
FIXDEPS=fixDeps

.build-conf:  ${BUILD_SUBPROJECTS}
ifneq ($(INFORMATION_MESSAGE), )
	@echo $(INFORMATION_MESSAGE)
endif
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/HFC-3100D_MC_V1.31_210106.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

MP_PROCESSOR_OPTION=30F6012A
MP_LINKER_FILE_OPTION=,-Tp30F6012A.gld
# ------------------------------------------------------------------------------------
# Rules for buildStep: assemble
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/eeprom.o: eeprom.s  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/eeprom.o.d 
	@${RM} ${OBJECTDIR}/eeprom.o.ok ${OBJECTDIR}/eeprom.o.err 
	@${RM} ${OBJECTDIR}/eeprom.o 
	@${FIXDEPS} "${OBJECTDIR}/eeprom.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_AS} $(MP_EXTRA_AS_PRE)  eeprom.s -o ${OBJECTDIR}/eeprom.o -omf=elf -p=$(MP_PROCESSOR_OPTION) --defsym=__MPLAB_BUILD=1 --defsym=__MPLAB_DEBUG=1 --defsym=__ICD2RAM=1 --defsym=__DEBUG=1 --defsym=__MPLAB_DEBUGGER_PK3=1 -g  -MD "${OBJECTDIR}/eeprom.o.d"$(MP_EXTRA_AS_POST)
	
else
${OBJECTDIR}/eeprom.o: eeprom.s  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/eeprom.o.d 
	@${RM} ${OBJECTDIR}/eeprom.o.ok ${OBJECTDIR}/eeprom.o.err 
	@${RM} ${OBJECTDIR}/eeprom.o 
	@${FIXDEPS} "${OBJECTDIR}/eeprom.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_AS} $(MP_EXTRA_AS_PRE)  eeprom.s -o ${OBJECTDIR}/eeprom.o -omf=elf -p=$(MP_PROCESSOR_OPTION) --defsym=__MPLAB_BUILD=1 -g  -MD "${OBJECTDIR}/eeprom.o.d"$(MP_EXTRA_AS_POST)
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: assembleWithPreprocess
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
else
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: compile
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${OBJECTDIR}/ADC.o: ADC.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ADC.o.d 
	@${RM} ${OBJECTDIR}/ADC.o.ok ${OBJECTDIR}/ADC.o.err 
	@${RM} ${OBJECTDIR}/ADC.o 
	@${FIXDEPS} "${OBJECTDIR}/ADC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ADC.o.d" -o ${OBJECTDIR}/ADC.o ADC.c    
	
${OBJECTDIR}/LED.o: LED.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/LED.o.d 
	@${RM} ${OBJECTDIR}/LED.o.ok ${OBJECTDIR}/LED.o.err 
	@${RM} ${OBJECTDIR}/LED.o 
	@${FIXDEPS} "${OBJECTDIR}/LED.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/LED.o.d" -o ${OBJECTDIR}/LED.o LED.c    
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o.ok ${OBJECTDIR}/main.o.err 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    
	
${OBJECTDIR}/RS232.o: RS232.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/RS232.o.d 
	@${RM} ${OBJECTDIR}/RS232.o.ok ${OBJECTDIR}/RS232.o.err 
	@${RM} ${OBJECTDIR}/RS232.o 
	@${FIXDEPS} "${OBJECTDIR}/RS232.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/RS232.o.d" -o ${OBJECTDIR}/RS232.o RS232.c    
	
${OBJECTDIR}/SPI_RS485.o: SPI_RS485.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/SPI_RS485.o.d 
	@${RM} ${OBJECTDIR}/SPI_RS485.o.ok ${OBJECTDIR}/SPI_RS485.o.err 
	@${RM} ${OBJECTDIR}/SPI_RS485.o 
	@${FIXDEPS} "${OBJECTDIR}/SPI_RS485.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/SPI_RS485.o.d" -o ${OBJECTDIR}/SPI_RS485.o SPI_RS485.c    
	
${OBJECTDIR}/Operation.o: Operation.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Operation.o.d 
	@${RM} ${OBJECTDIR}/Operation.o.ok ${OBJECTDIR}/Operation.o.err 
	@${RM} ${OBJECTDIR}/Operation.o 
	@${FIXDEPS} "${OBJECTDIR}/Operation.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Operation.o.d" -o ${OBJECTDIR}/Operation.o Operation.c    
	
${OBJECTDIR}/Initialization.o: Initialization.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Initialization.o.d 
	@${RM} ${OBJECTDIR}/Initialization.o.ok ${OBJECTDIR}/Initialization.o.err 
	@${RM} ${OBJECTDIR}/Initialization.o 
	@${FIXDEPS} "${OBJECTDIR}/Initialization.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Initialization.o.d" -o ${OBJECTDIR}/Initialization.o Initialization.c    
	
${OBJECTDIR}/Memory.o: Memory.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Memory.o.d 
	@${RM} ${OBJECTDIR}/Memory.o.ok ${OBJECTDIR}/Memory.o.err 
	@${RM} ${OBJECTDIR}/Memory.o 
	@${FIXDEPS} "${OBJECTDIR}/Memory.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Memory.o.d" -o ${OBJECTDIR}/Memory.o Memory.c    
	
else
${OBJECTDIR}/ADC.o: ADC.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ADC.o.d 
	@${RM} ${OBJECTDIR}/ADC.o.ok ${OBJECTDIR}/ADC.o.err 
	@${RM} ${OBJECTDIR}/ADC.o 
	@${FIXDEPS} "${OBJECTDIR}/ADC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/ADC.o.d" -o ${OBJECTDIR}/ADC.o ADC.c    
	
${OBJECTDIR}/LED.o: LED.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/LED.o.d 
	@${RM} ${OBJECTDIR}/LED.o.ok ${OBJECTDIR}/LED.o.err 
	@${RM} ${OBJECTDIR}/LED.o 
	@${FIXDEPS} "${OBJECTDIR}/LED.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/LED.o.d" -o ${OBJECTDIR}/LED.o LED.c    
	
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o.ok ${OBJECTDIR}/main.o.err 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    
	
${OBJECTDIR}/RS232.o: RS232.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/RS232.o.d 
	@${RM} ${OBJECTDIR}/RS232.o.ok ${OBJECTDIR}/RS232.o.err 
	@${RM} ${OBJECTDIR}/RS232.o 
	@${FIXDEPS} "${OBJECTDIR}/RS232.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/RS232.o.d" -o ${OBJECTDIR}/RS232.o RS232.c    
	
${OBJECTDIR}/SPI_RS485.o: SPI_RS485.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/SPI_RS485.o.d 
	@${RM} ${OBJECTDIR}/SPI_RS485.o.ok ${OBJECTDIR}/SPI_RS485.o.err 
	@${RM} ${OBJECTDIR}/SPI_RS485.o 
	@${FIXDEPS} "${OBJECTDIR}/SPI_RS485.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/SPI_RS485.o.d" -o ${OBJECTDIR}/SPI_RS485.o SPI_RS485.c    
	
${OBJECTDIR}/Operation.o: Operation.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Operation.o.d 
	@${RM} ${OBJECTDIR}/Operation.o.ok ${OBJECTDIR}/Operation.o.err 
	@${RM} ${OBJECTDIR}/Operation.o 
	@${FIXDEPS} "${OBJECTDIR}/Operation.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Operation.o.d" -o ${OBJECTDIR}/Operation.o Operation.c    
	
${OBJECTDIR}/Initialization.o: Initialization.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Initialization.o.d 
	@${RM} ${OBJECTDIR}/Initialization.o.ok ${OBJECTDIR}/Initialization.o.err 
	@${RM} ${OBJECTDIR}/Initialization.o 
	@${FIXDEPS} "${OBJECTDIR}/Initialization.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Initialization.o.d" -o ${OBJECTDIR}/Initialization.o Initialization.c    
	
${OBJECTDIR}/Memory.o: Memory.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Memory.o.d 
	@${RM} ${OBJECTDIR}/Memory.o.ok ${OBJECTDIR}/Memory.o.err 
	@${RM} ${OBJECTDIR}/Memory.o 
	@${FIXDEPS} "${OBJECTDIR}/Memory.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION)  -MMD -MF "${OBJECTDIR}/Memory.o.d" -o ${OBJECTDIR}/Memory.o Memory.c    
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/HFC-3100D_MC_V1.31_210106.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG -D__MPLAB_DEBUGGER_PK3=1 -o ${DISTDIR}/HFC-3100D_MC_V1.31_210106.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,-L".",-Map="${DISTDIR}/HFC-3100D_MC.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1,--defsym=__MPLAB_DEBUGGER_PK3=1
else
${DISTDIR}/HFC-3100D_MC_V1.31_210106.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -o ${DISTDIR}/HFC-3100D_MC_V1.31_210106.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,-L".",-Map="${DISTDIR}/HFC-3100D_MC.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}\\pic30-bin2hex ${DISTDIR}/HFC-3100D_MC_V1.31_210106.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=elf
endif


# Subprojects
.build-subprojects:


# Subprojects
.clean-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${OBJECTDIR}
	${RM} -r ${DISTDIR}

# Enable dependency checking
.dep.inc: .depcheck-impl

DEPFILES=$(shell mplabwildcard ${POSSIBLE_DEPFILES})
ifneq (${DEPFILES},)
include ${DEPFILES}
endif
