################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../App/Handlers/Input_Handler.c" \
"../App/Handlers/State_Manager.c" 

COMPILED_SRCS += \
"App/Handlers/Input_Handler.src" \
"App/Handlers/State_Manager.src" 

C_DEPS += \
"./App/Handlers/Input_Handler.d" \
"./App/Handlers/State_Manager.d" 

OBJS += \
"App/Handlers/Input_Handler.o" \
"App/Handlers/State_Manager.o" 


# Each subdirectory must supply rules for building sources it contributes
"App/Handlers/Input_Handler.src":"../App/Handlers/Input_Handler.c" "App/Handlers/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc37x "-fC:/CONTROL_ECU/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Wc-w508 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"App/Handlers/Input_Handler.o":"App/Handlers/Input_Handler.src" "App/Handlers/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"
"App/Handlers/State_Manager.src":"../App/Handlers/State_Manager.c" "App/Handlers/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc37x "-fC:/CONTROL_ECU/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Wc-w508 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"App/Handlers/State_Manager.o":"App/Handlers/State_Manager.src" "App/Handlers/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-App-2f-Handlers

clean-App-2f-Handlers:
	-$(RM) ./App/Handlers/Input_Handler.d ./App/Handlers/Input_Handler.o ./App/Handlers/Input_Handler.src ./App/Handlers/State_Manager.d ./App/Handlers/State_Manager.o ./App/Handlers/State_Manager.src

.PHONY: clean-App-2f-Handlers

