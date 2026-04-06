################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../Drivers/Mcal/Stm/STM_Interrupt.c" 

COMPILED_SRCS += \
"Drivers/Mcal/Stm/STM_Interrupt.src" 

C_DEPS += \
"./Drivers/Mcal/Stm/STM_Interrupt.d" 

OBJS += \
"Drivers/Mcal/Stm/STM_Interrupt.o" 


# Each subdirectory must supply rules for building sources it contributes
"Drivers/Mcal/Stm/STM_Interrupt.src":"../Drivers/Mcal/Stm/STM_Interrupt.c" "Drivers/Mcal/Stm/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc37x "-fC:/tools/aurix/tc375_control_ecu/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Wc-w508 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"Drivers/Mcal/Stm/STM_Interrupt.o":"Drivers/Mcal/Stm/STM_Interrupt.src" "Drivers/Mcal/Stm/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-Drivers-2f-Mcal-2f-Stm

clean-Drivers-2f-Mcal-2f-Stm:
	-$(RM) ./Drivers/Mcal/Stm/STM_Interrupt.d ./Drivers/Mcal/Stm/STM_Interrupt.o ./Drivers/Mcal/Stm/STM_Interrupt.src

.PHONY: clean-Drivers-2f-Mcal-2f-Stm

