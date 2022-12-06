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
FINAL_IMAGE=${DISTDIR}/HFC-3100D_RD_V1.32_221124.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
else
IMAGE_TYPE=production
OUTPUT_SUFFIX=hex
DEBUGGABLE_SUFFIX=elf
FINAL_IMAGE=${DISTDIR}/HFC-3100D_RD_V1.32_221124.${IMAGE_TYPE}.${OUTPUT_SUFFIX}
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
SOURCEFILES_QUOTED_IF_SPACED=eeprom.s main.c ADC.c Alram_Sts.c RS232.c Initialize.c Definition.c

# Object Files Quoted if spaced
OBJECTFILES_QUOTED_IF_SPACED=${OBJECTDIR}/eeprom.o ${OBJECTDIR}/main.o ${OBJECTDIR}/ADC.o ${OBJECTDIR}/Alram_Sts.o ${OBJECTDIR}/RS232.o ${OBJECTDIR}/Initialize.o ${OBJECTDIR}/Definition.o
POSSIBLE_DEPFILES=${OBJECTDIR}/eeprom.o.d ${OBJECTDIR}/main.o.d ${OBJECTDIR}/ADC.o.d ${OBJECTDIR}/Alram_Sts.o.d ${OBJECTDIR}/RS232.o.d ${OBJECTDIR}/Initialize.o.d ${OBJECTDIR}/Definition.o.d

# Object Files
OBJECTFILES=${OBJECTDIR}/eeprom.o ${OBJECTDIR}/main.o ${OBJECTDIR}/ADC.o ${OBJECTDIR}/Alram_Sts.o ${OBJECTDIR}/RS232.o ${OBJECTDIR}/Initialize.o ${OBJECTDIR}/Definition.o

# Source Files
SOURCEFILES=eeprom.s main.c ADC.c Alram_Sts.c RS232.c Initialize.c Definition.c



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
	${MAKE}  -f nbproject/Makefile-default.mk ${DISTDIR}/HFC-3100D_RD_V1.32_221124.${IMAGE_TYPE}.${OUTPUT_SUFFIX}

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
	@${FIXDEPS} "${OBJECTDIR}/eeprom.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_AS} $(MP_EXTRA_AS_PRE)  eeprom.s -o ${OBJECTDIR}/eeprom.o -omf=elf -p=$(MP_PROCESSOR_OPTION) --defsym=__MPLAB_BUILD=1 --defsym=__MPLAB_DEBUG=1 --defsym=__ICD2RAM=1 --defsym=__DEBUG=1  -g  -MD "${OBJECTDIR}/eeprom.o.d"$(MP_EXTRA_AS_POST)
	
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
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o.ok ${OBJECTDIR}/main.o.err 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG  -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"." -mlarge-code -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    
	
${OBJECTDIR}/ADC.o: ADC.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ADC.o.d 
	@${RM} ${OBJECTDIR}/ADC.o.ok ${OBJECTDIR}/ADC.o.err 
	@${RM} ${OBJECTDIR}/ADC.o 
	@${FIXDEPS} "${OBJECTDIR}/ADC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG  -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"." -mlarge-code -MMD -MF "${OBJECTDIR}/ADC.o.d" -o ${OBJECTDIR}/ADC.o ADC.c    
	
${OBJECTDIR}/Alram_Sts.o: Alram_Sts.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Alram_Sts.o.d 
	@${RM} ${OBJECTDIR}/Alram_Sts.o.ok ${OBJECTDIR}/Alram_Sts.o.err 
	@${RM} ${OBJECTDIR}/Alram_Sts.o 
	@${FIXDEPS} "${OBJECTDIR}/Alram_Sts.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG  -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"." -mlarge-code -MMD -MF "${OBJECTDIR}/Alram_Sts.o.d" -o ${OBJECTDIR}/Alram_Sts.o Alram_Sts.c    
	
${OBJECTDIR}/RS232.o: RS232.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/RS232.o.d 
	@${RM} ${OBJECTDIR}/RS232.o.ok ${OBJECTDIR}/RS232.o.err 
	@${RM} ${OBJECTDIR}/RS232.o 
	@${FIXDEPS} "${OBJECTDIR}/RS232.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG  -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"." -mlarge-code -MMD -MF "${OBJECTDIR}/RS232.o.d" -o ${OBJECTDIR}/RS232.o RS232.c    
	
${OBJECTDIR}/Initialize.o: Initialize.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Initialize.o.d 
	@${RM} ${OBJECTDIR}/Initialize.o.ok ${OBJECTDIR}/Initialize.o.err 
	@${RM} ${OBJECTDIR}/Initialize.o 
	@${FIXDEPS} "${OBJECTDIR}/Initialize.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG  -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"." -mlarge-code -MMD -MF "${OBJECTDIR}/Initialize.o.d" -o ${OBJECTDIR}/Initialize.o Initialize.c    
	
${OBJECTDIR}/Definition.o: Definition.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Definition.o.d 
	@${RM} ${OBJECTDIR}/Definition.o.ok ${OBJECTDIR}/Definition.o.err 
	@${RM} ${OBJECTDIR}/Definition.o 
	@${FIXDEPS} "${OBJECTDIR}/Definition.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE) -g -D__DEBUG  -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"." -mlarge-code -MMD -MF "${OBJECTDIR}/Definition.o.d" -o ${OBJECTDIR}/Definition.o Definition.c    
	
else
${OBJECTDIR}/main.o: main.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/main.o.d 
	@${RM} ${OBJECTDIR}/main.o.ok ${OBJECTDIR}/main.o.err 
	@${RM} ${OBJECTDIR}/main.o 
	@${FIXDEPS} "${OBJECTDIR}/main.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"." -mlarge-code -MMD -MF "${OBJECTDIR}/main.o.d" -o ${OBJECTDIR}/main.o main.c    
	
${OBJECTDIR}/ADC.o: ADC.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/ADC.o.d 
	@${RM} ${OBJECTDIR}/ADC.o.ok ${OBJECTDIR}/ADC.o.err 
	@${RM} ${OBJECTDIR}/ADC.o 
	@${FIXDEPS} "${OBJECTDIR}/ADC.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"." -mlarge-code -MMD -MF "${OBJECTDIR}/ADC.o.d" -o ${OBJECTDIR}/ADC.o ADC.c    
	
${OBJECTDIR}/Alram_Sts.o: Alram_Sts.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Alram_Sts.o.d 
	@${RM} ${OBJECTDIR}/Alram_Sts.o.ok ${OBJECTDIR}/Alram_Sts.o.err 
	@${RM} ${OBJECTDIR}/Alram_Sts.o 
	@${FIXDEPS} "${OBJECTDIR}/Alram_Sts.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"." -mlarge-code -MMD -MF "${OBJECTDIR}/Alram_Sts.o.d" -o ${OBJECTDIR}/Alram_Sts.o Alram_Sts.c    
	
${OBJECTDIR}/RS232.o: RS232.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/RS232.o.d 
	@${RM} ${OBJECTDIR}/RS232.o.ok ${OBJECTDIR}/RS232.o.err 
	@${RM} ${OBJECTDIR}/RS232.o 
	@${FIXDEPS} "${OBJECTDIR}/RS232.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"." -mlarge-code -MMD -MF "${OBJECTDIR}/RS232.o.d" -o ${OBJECTDIR}/RS232.o RS232.c    
	
${OBJECTDIR}/Initialize.o: Initialize.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Initialize.o.d 
	@${RM} ${OBJECTDIR}/Initialize.o.ok ${OBJECTDIR}/Initialize.o.err 
	@${RM} ${OBJECTDIR}/Initialize.o 
	@${FIXDEPS} "${OBJECTDIR}/Initialize.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"." -mlarge-code -MMD -MF "${OBJECTDIR}/Initialize.o.d" -o ${OBJECTDIR}/Initialize.o Initialize.c    
	
${OBJECTDIR}/Definition.o: Definition.c  nbproject/Makefile-${CND_CONF}.mk 
	@${MKDIR} "${OBJECTDIR}" 
	@${RM} ${OBJECTDIR}/Definition.o.d 
	@${RM} ${OBJECTDIR}/Definition.o.ok ${OBJECTDIR}/Definition.o.err 
	@${RM} ${OBJECTDIR}/Definition.o 
	@${FIXDEPS} "${OBJECTDIR}/Definition.o.d" $(SILENT) -rsi ${MP_CC_DIR}../ -c ${MP_CC} $(MP_EXTRA_CC_PRE)  -g -omf=elf -x c -c -mcpu=$(MP_PROCESSOR_OPTION) -Wall -I"." -mlarge-code -MMD -MF "${OBJECTDIR}/Definition.o.d" -o ${OBJECTDIR}/Definition.o Definition.c    
	
endif

# ------------------------------------------------------------------------------------
# Rules for buildStep: link
ifeq ($(TYPE_IMAGE), DEBUG_RUN)
${DISTDIR}/HFC-3100D_RD_V1.32_221124.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk    
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -D__DEBUG  -o ${DISTDIR}/HFC-3100D_RD_V1.32_221124.${IMAGE_TYPE}.${OUTPUT_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,-L".",-Map="${DISTDIR}/HFC-3100D_RD.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION),--defsym=__ICD2RAM=1,--defsym=__MPLAB_DEBUG=1,--defsym=__DEBUG=1
else
${DISTDIR}/HFC-3100D_RD_V1.32_221124.${IMAGE_TYPE}.${OUTPUT_SUFFIX}: ${OBJECTFILES}  nbproject/Makefile-${CND_CONF}.mk   
	@${MKDIR} ${DISTDIR} 
	${MP_CC} $(MP_EXTRA_LD_PRE)  -omf=elf -mcpu=$(MP_PROCESSOR_OPTION)  -o ${DISTDIR}/HFC-3100D_RD_V1.32_221124.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} ${OBJECTFILES_QUOTED_IF_SPACED}         -Wl,--defsym=__MPLAB_BUILD=1,-L".",-Map="${DISTDIR}/HFC-3100D_RD.${IMAGE_TYPE}.map",--report-mem$(MP_EXTRA_LD_POST)$(MP_LINKER_FILE_OPTION)
	${MP_CC_DIR}\\pic30-bin2hex ${DISTDIR}/HFC-3100D_RD_V1.32_221124.${IMAGE_TYPE}.${DEBUGGABLE_SUFFIX} -omf=elf
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
