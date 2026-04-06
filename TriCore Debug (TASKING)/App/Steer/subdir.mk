################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../App/Steer/Steer_State_Manager.c" 

COMPILED_SRCS += \
"App/Steer/Steer_State_Manager.src" 

C_DEPS += \
"./App/Steer/Steer_State_Manager.d" 

OBJS += \
"App/Steer/Steer_State_Manager.o" 


# Each subdirectory must supply rules for building sources it contributes
"App/Steer/Steer_State_Manager.src":"../App/Steer/Steer_State_Manager.c" "App/Steer/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc37x "-fC:/CONTROL_ECU/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Wc-w508 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"App/Steer/Steer_State_Manager.o":"App/Steer/Steer_State_Manager.src" "App/Steer/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-App-2f-Steer

clean-App-2f-Steer:
	-$(RM) ./App/Steer/Steer_State_Manager.d ./App/Steer/Steer_State_Manager.o ./App/Steer/Steer_State_Manager.src

.PHONY: clean-App-2f-Steer

