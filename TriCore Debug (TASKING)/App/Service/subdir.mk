################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
"../App/Service/Can_Percieve_Ras.c" 

COMPILED_SRCS += \
"App/Service/Can_Percieve_Ras.src" 

C_DEPS += \
"./App/Service/Can_Percieve_Ras.d" 

OBJS += \
"App/Service/Can_Percieve_Ras.o" 


# Each subdirectory must supply rules for building sources it contributes
"App/Service/Can_Percieve_Ras.src":"../App/Service/Can_Percieve_Ras.c" "App/Service/subdir.mk"
	cctc -cs --dep-file="$*.d" --misrac-version=2012 -D__CPU__=tc37x "-fC:/tools/aurix/tc375_control_ecu/TriCore Debug (TASKING)/TASKING_C_C___Compiler-Include_paths__-I_.opt" --iso=99 --c++14 --language=+volatile --exceptions --anachronisms --fp-model=3 -O0 --tradeoff=4 --compact-max-size=200 -g -Wc-w544 -Wc-w557 -Wc-w508 -Ctc37x -Y0 -N0 -Z0 -o "$@" "$<"
"App/Service/Can_Percieve_Ras.o":"App/Service/Can_Percieve_Ras.src" "App/Service/subdir.mk"
	astc -Og -Os --no-warnings= --error-limit=42 -o  "$@" "$<"

clean: clean-App-2f-Service

clean-App-2f-Service:
	-$(RM) ./App/Service/Can_Percieve_Ras.d ./App/Service/Can_Percieve_Ras.o ./App/Service/Can_Percieve_Ras.src

.PHONY: clean-App-2f-Service

