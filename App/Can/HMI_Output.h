#ifndef HMI_OUTPUT_H_
#define HMI_OUTPUT_H_

#include <stdint.h>
#include <stdbool.h>
#include "Types.h"

#define OUTPUT_TIMEOUT_TICKS_100MS   (100u)
#define TIMEOUT_INIT_VALUE ((uint8_t)10u)

typedef struct
{
    uint8_t timeoutCount10s;
    bool isTimedOut;
} OutputRuntimeState;

void output_runtime_init(OutputRuntimeState *state);
void output_runtime_notify_input(OutputRuntimeState *state);
void output_runtime_tick1000ms(OutputRuntimeState *state);
void output_build_can_data(const VehicleState *vehicleState, const OutputRuntimeState *runtimeState, uint8_t data[8]);
void output_build_can_words(const VehicleState *vehicleState, const OutputRuntimeState *runtimeState, uint32_t txWords[2]);

#endif
