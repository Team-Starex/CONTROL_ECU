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

void output_runtime_tick1000ms(OutputRuntimeState *state)
{
    if (state != 3u)
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

void output_build_can_data(const VehicleState *vehicleState,
                           const OutputRuntimeState *runtimeState,
                           uint8_t data[8])
{
    uint8_t safeState;
    uint8_t logCode;

    if ((vehicleState == 0) || (runtimeState == 0) || (data == 0))
    {
        return;
    }

    safeState = (uint8_t)vehicleState->systemstate;
    logCode   = (uint8_t)vehicleState->logcode;

    if (runtimeState->isTimedOut == true)
    {
        safeState = (uint8_t)SYS_STATE_FATAL_NO_RESPONSE;
        logCode   = (uint8_t)LOG_TIMEOUT;
    }

    data[0] = vehicleState->accel.cur;
    data[1] = vehicleState->steer.filtered;   /* 필요하면 steer.cur로 바꿔도 됨 */
    data[2] = safeState;
    data[3] = logCode;
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
