#include <HMI_Output.h>

void output_runtime_init(OutputRuntimeState *state)
{
    if (state == 0)
    {
        return;
    }

    state->timeoutCount10s = TIMEOUT_INIT_VALUE;
    state->isTimedOut = false;
}

void output_runtime_notify_input(OutputRuntimeState *state)
{
    if (state == 0)
    {
        return;
    }

    state->timeoutCount10s = TIMEOUT_INIT_VALUE;
    state->isTimedOut = false;
}

void output_runtime_tick1000ms(const VehicleState *vehicleState, OutputRuntimeState *state)
{
    if ((state == 0) || (vehicleState == 0))
    {
        return;
    }

    if (vehicleState->systemstate != SAFE_MODE_CRITICAL_CHECK)
    {
        return;
    }

    if (state->timeoutCount10s > 0u)
    {
        state->timeoutCount10s--;
    }

    if (state->timeoutCount10s == 0u)
    {
        state->isTimedOut = true;
    }
}

void output_build_can_data(VehicleState *vehicleState,
                           const OutputRuntimeState *runtimeState,
                           uint8_t data[8])
{

    if ((vehicleState == 0) || (runtimeState == 0) || (data == 0))
    {
        return;
    }

    if (runtimeState->isTimedOut == true)
    {
        vehicleState->systemstate = (uint8_t)SYS_STATE_FATAL_NO_RESPONSE;
        vehicleState->logcode     = (uint8_t)LOG_TIMEOUT;
    }

    data[0] = vehicleState->virtualSpeedKph_x10;;
    data[1] = vehicleState->steer.filtered;
    data[2] = vehicleState->systemstate;
    data[3] = vehicleState->logcode;
    data[4] = runtimeState->timeoutCount10s;
    data[5] = 0u;
    data[6] = 0u;
    data[7] = 0u;
}

void output_build_can_words(const VehicleState *vehicleState,
                            const OutputRuntimeState *runtimeState,
                            uint32_t txWords[2])
{
    uint8_t data[8];

    if ((vehicleState == 0) || (runtimeState == 0) || (txWords == 0))
    {
        return;
    }

    output_build_can_data(vehicleState, runtimeState, data);

    txWords[0] =
        ((uint32_t)data[0]) |
        ((uint32_t)data[1] << 8) |
        ((uint32_t)data[2] << 16) |
        ((uint32_t)data[3] << 24);

    txWords[1] =
        ((uint32_t)data[4]) |
        ((uint32_t)data[5] << 8) |
        ((uint32_t)data[6] << 16) |
        ((uint32_t)data[7] << 24);
}
