################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../App/Timer/STM_Interrupt.c" 

COMPILED_SRCS += \
"App/Timer/STM_Interrupt.src" 

C_DEPS += \
"./App/Timer/STM_Interrupt.d" 

OBJS += \
"App/Timer/STM_Interrupt.o" 


# Each subdirectory must supply rules for building sources it contributes
"App/Timer/STM_Interrupt.src":"../App/Timer/STM_Interrupt.c" "App/Timer/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc37x "-fC:/CONTROL_ECU/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Wc-w508 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"App/Timer/STM_Interrupt.o":"App/Timer/STM_Interrupt.src" "App/Timer/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-App-2f-Timer

clean-App-2f-Timer:
	-$(RM) ./App/Timer/STM_Interrupt.d ./App/Timer/STM_Interrupt.o ./App/Timer/STM_Interrupt.src

.PHONY: clean-App-2f-Timer

