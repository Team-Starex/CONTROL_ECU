#include "Actuator_Output.h"

#define BRAKE_ACTIVE_THRESHOLD   (1u) // 브레이크 활성 상태 임계값

//SystemState를 4bit one-hot 상태로 변환
static uint8_t encode_safe_state(SystemState s, bool isTimedOut)
{
    if (isTimedOut == true)
    {
        return 0x8u;   /* 1000 : FATAL */
    }

    switch (s)
    {
        case SYS_STATE_NORMAL:   return 0x1u; //0001
        case SYS_STATE_WARNING:  return 0x2u; //0010
        case SYS_STATE_CRITICAL: return 0x4u; //0100
        case SYS_STATE_FATAL_NO_RESPONSE:
        default:                 return 0x8u; //1000
    }
}

//speed 계산 로직 필요 일단 시스템 상태로 속도 경계값 넣어놨음
static uint8_t encode_speed_state(const VehicleState *vehicleState, bool isTimedOut)
{
    if ((vehicleState == 0) || (isTimedOut == true))
    {
        return 0x1u; /* STOP */
    }
     //CRITICAL/FATAL -> STOP
     //WARNING        -> LOW
     //NORMAL         -> MID
    switch (vehicleState->systemstate)
    {
        case SYS_STATE_CRITICAL:
        case SYS_STATE_FATAL_NO_RESPONSE:
            return 0x1u; //STOP  = 0001

        case SYS_STATE_WARNING:
            return 0x2u; //LOW   = 0010 */

        case SYS_STATE_NORMAL:
        default:
            return 0x4u; //MID   = 0100 */
    }
}

// brake 값이 존재하면 1 없으면 0
static uint8_t build_brake_active(const VehicleState *vehicleState)
{
    if (vehicleState == 0)
    {
        return 0u;
    }

    return (vehicleState->brake.cur >= BRAKE_ACTIVE_THRESHOLD) ? 1u : 0u;
}

//현재 브레이크, 액셀, 조향 센서의 변화량에 의해 정의한 level에 따라 이벤트 상태를 비트필드로 정의
static uint8_t build_ev_state(const VehicleState *vehicleState)
{
    uint8_t evState = 0u;

    if (vehicleState == 0)
    {
        return 0u;
    }

    //bit0 : Rapid Accel
    if (vehicleState->accel.deltalevel >= DELTA_CRITICAL)
    {
        evState |= (1u << 0);
    }

    //bit1 : Rapid Brake
    if (vehicleState->brake.deltalevel >= DELTA_CRITICAL)
    {
        evState |= (1u << 1);
    }

    //bit2 : Warning Steer
    if (vehicleState->steer.deltalevel == DELTA_WARNING)
    {
        evState |= (1u << 2);
    }

    //bit3 : Critical Steer
    if (vehicleState->steer.deltalevel == DELTA_CRITICAL)
    {
        evState |= (1u << 3);
    }

    return evState;
}

// 송신용 데이터 초기화 함수
void actuator_tx_runtime_init(ActuatorTxRuntime *state)
{
    if (state == 0)
    {
        return;
    }

    state->aliveCnt   = 0u;
    state->ackSeq     = 0u;
    state->prevButton = 0u;
    state->ackPulse   = 0u;
}

//버튼의 상태가 0->1로 변하는 순간에만 pulse 발생
//이전 버튼이 0이고 현재 버튼이 1이면 ackPulse = 1, ackSeq++
void actuator_tx_runtime_update(ActuatorTxRuntime *state, const VehicleState *vehicleState)
{
    uint8_t curButton;

    if ((state == 0) || (vehicleState == 0))
    {
        return;
    }

    curButton = (vehicleState->button != 0u) ? 1u : 0u;

    // pulse는 기본 0, 상승엣지에서만 1
    state->ackPulse = 0u;

    if ((state->prevButton == 0u) && (curButton == 1u))
    {
        state->ackPulse = 1u;
        state->ackSeq = (uint8_t)((state->ackSeq + 1u) & 0x0Fu);
    }

    state->prevButton = curButton;
}

//주기 송신이 끝난 뒤 호출하는 함수
void actuator_tx_runtime_on_periodic_send(ActuatorTxRuntime *state)
{
    if (state == 0)
    {
        return;
    }

    state->aliveCnt = (uint8_t)((state->aliveCnt + 1u) & 0x0Fu); // 현재 송신중 의미

    //이번 프레임에서만 1로 유지, 끝나면 0으로 초기화
    state->ackPulse = 0u;
}

//각 데이터를 종합 후 ACT ECU 통신 프로토콜에 맞춰 payload를 제작
void actuator_build_can_data(const VehicleState *vehicleState,
                             const OutputRuntimeState *runtimeState,
                             const ActuatorTxRuntime *txRuntime,
                             uint8_t data[8])
{
    uint8_t safeState;
    uint8_t speedState;
    uint8_t brakeActive;
    uint8_t evState;
    uint8_t ackButton;
    uint8_t msgValid;

    if ((vehicleState == 0) || (runtimeState == 0) || (txRuntime == 0) || (data == 0))
    {
        return;
    }

    safeState   = encode_safe_state(vehicleState->systemstate, runtimeState->isTimedOut);
    speedState  = encode_speed_state(vehicleState, runtimeState->isTimedOut);
    brakeActive = build_brake_active(vehicleState);
    evState     = build_ev_state(vehicleState);
    ackButton   = (txRuntime->ackPulse != 0u) ? 1u : 0u;        //button ack pulse가 이번 frame안에 존재?
    msgValid    = (runtimeState->isTimedOut == false) ? 1u : 0u;//timeout이 아니여서 현재 메시지가 유효?

    //Byte0
    // bit0~3 : SAFE_STATE
    // bit4~7 : SPEED_STATE
    data[0] = (uint8_t)((safeState & 0x0Fu) |
                        ((speedState & 0x0Fu) << 4));

    // Byte1
    // bit0   : BRAKE_ACTIVE
    // bit1~4 : EV_STATE
    // bit5   : ACK_BUTTON
    // bit6   : MSG_VALID
    // bit7   : RESERVED
    data[1] = (uint8_t)((brakeActive & 0x01u) |
                        ((evState & 0x0Fu) << 1) |
                        ((ackButton & 0x01u) << 5) |
                        ((msgValid & 0x01u) << 6));

    // Byte2
    // bit0~3 : ALIVE_CNT
    // bit4~7 : ACK_SEQ
    data[2] = (uint8_t)((txRuntime->aliveCnt & 0x0Fu) |
                        ((txRuntime->ackSeq & 0x0Fu) << 4));

    data[3] = 0u;
    data[4] = 0u;
    data[5] = 0u;
    data[6] = 0u;
    data[7] = 0u;
}

//전에 만든 data를 can 드라이버가 송신하기 쉬운 형태#include "Actuator_Output.h"

#define BRAKE_ACTIVE_THRESHOLD   (1u) // 브레이크 활성 상태 임계값

//SystemState를 4bit one-hot 상태로 변환
static uint8_t encode_safe_state(SystemState s, bool isTimedOut)
{
    if (isTimedOut == true)
    {
        return 0x8u;   /* 1000 : FATAL */
    }

    switch (s)
    {
        case SYS_STATE_NORMAL:   return 0x1u; //0001
        case SYS_STATE_WARNING:  return 0x2u; //0010
        case SYS_STATE_CRITICAL: return 0x4u; //0100
        case SYS_STATE_FATAL_NO_RESPONSE:
        default:                 return 0x8u; //1000
    }
}

//speed 계산 로직 필요 일단 시스템 상태로 속도 경계값 넣어놨음
static uint8_t encode_speed_state(const VehicleState *vehicleState, bool isTimedOut)
{
    if ((vehicleState == 0) || (isTimedOut == true))
    {
        return 0x1u; /* STOP */
    }
     //CRITICAL/FATAL -> STOP
     //WARNING        -> LOW
     //NORMAL         -> MID
    switch (vehicleState->systemstate)
    {
        case SYS_STATE_CRITICAL:
        case SYS_STATE_FATAL_NO_RESPONSE:
            return 0x1u; //STOP  = 0001

        case SYS_STATE_WARNING:
            return 0x2u; //LOW   = 0010 */

        case SYS_STATE_NORMAL:
        default:
            return 0x4u; //MID   = 0100 */
    }
}

// brake 값이 존재하면 1 없으면 0
static uint8_t build_brake_active(const VehicleState *vehicleState)
{
    if (vehicleState == 0)
    {
        return 0u;
    }

    return (vehicleState->brake.cur >= BRAKE_ACTIVE_THRESHOLD) ? 1u : 0u;
}

//현재 브레이크, 액셀, 조향 센서의 변화량에 의해 정의한 level에 따라 이벤트 상태를 비트필드로 정의
static uint8_t build_ev_state(const VehicleState *vehicleState)
{
    uint8_t evState = 0u;

    if (vehicleState == 0)
    {
        return 0u;
    }

    //bit0 : Rapid Accel
    if (vehicleState->accel.deltalevel >= DELTA_CRITICAL)
    {
        evState |= (1u << 0);
    }

    //bit1 : Rapid Brake
    if (vehicleState->brake.deltalevel >= DELTA_CRITICAL)
    {
        evState |= (1u << 1);
    }

    //bit2 : Warning Steer
    if (vehicleState->steer.deltalevel == DELTA_WARNING)
    {
        evState |= (1u << 2);
    }

    //bit3 : Critical Steer
    if (vehicleState->steer.deltalevel == DELTA_CRITICAL)
    {
        evState |= (1u << 3);
    }

    return evState;
}

// 송신용 데이터 초기화 함수
void actuator_tx_runtime_init(ActuatorTxRuntime *state)
{
    if (state == 0)
    {
        return;
    }

    state->aliveCnt   = 0u;
    state->ackSeq     = 0u;
    state->prevButton = 0u;
    state->ackPulse   = 0u;
}

//버튼의 상태가 0->1로 변하는 순간에만 pulse 발생
//이전 버튼이 0이고 현재 버튼이 1이면 ackPulse = 1, ackSeq++
void actuator_tx_runtime_update(ActuatorTxRuntime *state, const VehicleState *vehicleState)
{
    uint8_t curButton;

    if ((state == 0) || (vehicleState == 0))
    {
        return;
    }

    curButton = (vehicleState->button != 0u) ? 1u : 0u;

    // pulse는 기본 0, 상승엣지에서만 1
    state->ackPulse = 0u;

    if ((state->prevButton == 0u) && (curButton == 1u))
    {
        state->ackPulse = 1u;
        state->ackSeq = (uint8_t)((state->ackSeq + 1u) & 0x0Fu);
    }

    state->prevButton = curButton;
}

//주기 송신이 끝난 뒤 호출하는 함수
void actuator_tx_runtime_on_periodic_send(ActuatorTxRuntime *state)
{
    if (state == 0)
    {
        return;
    }

    state->aliveCnt = (uint8_t)((state->aliveCnt + 1u) & 0x0Fu); // 현재 송신중 의미

    //이번 프레임에서만 1로 유지, 끝나면 0으로 초기화
    state->ackPulse = 0u;
}

//각 데이터를 종합 후 ACT ECU 통신 프로토콜에 맞춰 payload를 제작
void actuator_build_can_data(const VehicleState *vehicleState,
                             const OutputRuntimeState *runtimeState,
                             const ActuatorTxRuntime *txRuntime,
                             uint8_t data[8])
{
    uint8_t safeState;
    uint8_t speedState;
    uint8_t brakeActive;
    uint8_t evState;
    uint8_t ackButton;
    uint8_t msgValid;

    if ((vehicleState == 0) || (runtimeState == 0) || (txRuntime == 0) || (data == 0))
    {
        return;
    }

    safeState   = encode_safe_state(vehicleState->systemstate, runtimeState->isTimedOut);
    speedState  = encode_speed_state(vehicleState, runtimeState->isTimedOut);
    brakeActive = build_brake_active(vehicleState);
    evState     = build_ev_state(vehicleState);
    ackButton   = (txRuntime->ackPulse != 0u) ? 1u : 0u;        //button ack pulse가 이번 frame안에 존재?
    msgValid    = (runtimeState->isTimedOut == false) ? 1u : 0u;//timeout이 아니여서 현재 메시지가 유효?

    //Byte0
    // bit0~3 : SAFE_STATE
    // bit4~7 : SPEED_STATE
    data[0] = (uint8_t)((safeState & 0x0Fu) |
                        ((speedState & 0x0Fu) << 4));

    // Byte1
    // bit0   : BRAKE_ACTIVE
    // bit1~4 : EV_STATE
    // bit5   : ACK_BUTTON
    // bit6   : MSG_VALID
    // bit7   : RESERVED
    data[1] = (uint8_t)((brakeActive & 0x01u) |
                        ((evState & 0x0Fu) << 1) |
                        ((ackButton & 0x01u) << 5) |
                        ((msgValid & 0x01u) << 6));

    // Byte2
    // bit0~3 : ALIVE_CNT
    // bit4~7 : ACK_SEQ
    data[2] = (uint8_t)((txRuntime->aliveCnt & 0x0Fu) |
                        ((txRuntime->ackSeq & 0x0Fu) << 4));

    data[3] = 0u;
    data[4] = 0u;
    data[5] = 0u;
    data[6] = 0u;
    data[7] = 0u;
}

//전에 만든 data를 can 드라이버가 송신하기 쉬운 형태로 convert
void actuator_build_can_words(const VehicleState *vehicleState,
                              const OutputRuntimeState *runtimeState,
                              const ActuatorTxRuntime *txRuntime,
                              uint32_t txWords[2])
{
    uint8_t data[8];

    if ((vehicleState == 0) || (runtimeState == 0) || (txRuntime == 0) || (txWords == 0))
    {
        return;
    }

    actuator_build_can_data(vehicleState, runtimeState, txRuntime, data);

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

void actuator_build_can_words(const VehicleState *vehicleState,
                              const OutputRuntimeState *runtimeState,
                              const ActuatorTxRuntime *txRuntime,
                              uint32_t txWords[2])
{
    uint8_t data[8];

    if ((vehicleState == 0) || (runtimeState == 0) || (txRuntime == 0) || (txWords == 0))
    {
        return;
    }

    actuator_build_can_data(vehicleState, runtimeState, txRuntime, data);

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
