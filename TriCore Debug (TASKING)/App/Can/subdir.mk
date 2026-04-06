################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../App/Can/Actuator_Output.c" \
"../App/Can/HMI_Output.c" 

COMPILED_SRCS += \
"App/Can/Actuator_Output.src" \
"App/Can/HMI_Output.src" 

C_DEPS += \
"./App/Can/Actuator_Output.d" \
"./App/Can/HMI_Output.d" 

OBJS += \
"App/Can/Actuator_Output.o" \
"App/Can/HMI_Output.o" 


# Each subdirectory must supply rules for building sources it contributes
"App/Can/Actuator_Output.src":"../App/Can/Actuator_Output.c" "App/Can/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc37x "-fC:/tools/aurix/tc375_control_ecu/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Wc-w508 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"App/Can/Actuator_Output.o":"App/Can/Actuator_Output.src" "App/Can/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"App/Can/HMI_Output.src":"../App/Can/HMI_Output.c" "App/Can/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc37x "-fC:/tools/aurix/tc375_control_ecu/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Wc-w508 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"App/Can/HMI_Output.o":"App/Can/HMI_Output.src" "App/Can/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-App-2f-Can

clean-App-2f-Can:
	-$(RM) ./App/Can/Actuator_Output.d ./App/Can/Actuator_Output.o ./App/Can/Actuator_Output.src ./App/Can/HMI_Output.d ./App/Can/HMI_Output.o ./App/Can/HMI_Output.src

.PHONY: clean-App-2f-Can

