#include <HMI_Output.h>

//시스템 상태 초기화
void output_runtime_init(OutputRuntimeState *state)
{
    if (state == 0)
    {
        return;
    }

    state->timeoutCount10s = 0u;
    state->isTimedOut = false;
}

//can 입력 받았음을 notify 입력 받을때마다 타이머 리셋
void output_runtime_notify_input(OutputRuntimeState *state)
{
    if (state == 0)
    {
        return;
    }

    state->timeoutCount10s = 0u;
    state->isTimedOut = false;
}

// timeout count
void output_runtime_tick100ms(OutputRuntimeState *state)
{
    if (state == 0)
    {
        return;
    }

    if (state->timeoutCount10s < OUTPUT_TIMEOUT_TICKS_100MS)
    {
        state->timeoutCount10s++;
    }

    if (state->timeoutCount10s >= OUTPUT_TIMEOUT_TICKS_100MS)
    {
        state->timeoutCount10s = OUTPUT_TIMEOUT_TICKS_100MS;
        state->isTimedOut = true;
    }
}

//hmi에 전송할 can data 생성
void output_build_can_data(const VehicleState *vehicleState, const OutputRuntimeState *runtimeState, uint8_t data[8])
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
    data[1] = vehicleState->steer.cur;
    data[2] = safeState;
    data[3] = logCode;
    data[4] = runtimeState->timeoutCount10s;
    data[5] = 0u;
    data[6] = 0u;
    data[7] = 0u;
}

//data 삽입
void output_build_can_words(const VehicleState *vehicleState, const OutputRuntimeState *runtimeState, int32_t txWords[2])
{
    uint8_t data[8];

    if ((vehicleState == 0) || (runtimeState == 0) || (txWords == 0))
    {
        return;
    }

    output_build_can_data(vehicleState, runtimeState, data);

    // 0 ~ 3byte
    txWords[0] =
        ((uint32_t)data[0]) |
        ((uint32_t)data[1] << 8) |
        ((uint32_t)data[2] << 16) |
        ((uint32_t)data[3] << 24);
    // 4 ~ 7byte
    txWords[1] =
        ((uint32_t)data[4]) |
        ((uint32_t)data[5] << 8) |
        ((uint32_t)data[6] << 16) |
        ((uint32_t)data[7] << 24);
}
