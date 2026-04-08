#include "Actuator_Output.h"

#define BRAKE_ACTIVE_THRESHOLD   (30u)

static uint8_t encode_safe_state(SystemState s, bool isTimedOut)
{
    if (isTimedOut == true)
    {
        return 0x8u;
    }

    switch (s)
    {
        case SYS_STATE_NORMAL:   return 0x1u;
        case SYS_STATE_WARNING:  return 0x2u;
        case SYS_STATE_CRITICAL: return 0x4u;
        case SYS_STATE_FATAL_NO_RESPONSE:
        default:                 return 0x8u;
    }
}

static uint8_t encode_speed_state(const VehicleState *vehicleState, bool isTimedOut)
{
    if ((vehicleState == 0) || (isTimedOut == true))
    {
        return 0x8u;
    }

    switch (vehicleState->speedBand)
    {
        case SPEED_STOP: return 0x1u;
        case SPEED_LOW:  return 0x2u;
        case SPEED_MID:  return 0x4u;
        case SPEED_HIGH: return 0x8u;
        default:         return 0x2u;
    }
}

static uint8_t build_brake_active(const VehicleState *vehicleState)
{
    if (vehicleState == 0)
    {
        return 0u;
    }

    return (vehicleState->brake.filtered >= BRAKE_ACTIVE_THRESHOLD) ? 1u : 0u;
}

static uint8_t build_ev_state(const VehicleState *vehicleState)
{
    uint8_t evState = 0u;
    uint8_t steerOffset;

    if (vehicleState == 0)
    {
        return 0u;
    }

    steerOffset = (vehicleState->steer.filtered > 128U)
                ? (vehicleState->steer.filtered - 128U)
                : (128U - vehicleState->steer.filtered);

    /* bit0 : Rapid Accel */
    if (vehicleState->accel.deltalevel >= DELTA_WARNING)
    {
        evState |= (1u << 0);
    }

    /* bit1 : Rapid Brake */
    if (vehicleState->brake.deltalevel >= DELTA_WARNING)
    {
        evState |= (1u << 1);
    }

    /* bit2 : Warning Steer */
    if (vehicleState->speedBand != SPEED_STOP)
    {
        if ((vehicleState->steer.deltalevel == DELTA_WARNING) || (steerOffset >= 55U))
        {
            evState |= (1u << 2);
        }

        /* bit3 : Critical Steer */
        if ((vehicleState->steer.deltalevel == DELTA_CRITICAL) || (steerOffset >= 90U))
        {
            evState |= (1u << 3);
        }
    }
    return evState;
}

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

void actuator_tx_runtime_update(ActuatorTxRuntime *state, const VehicleState *vehicleState)
{
    uint8_t curButton;

    if ((state == 0) || (vehicleState == 0))
    {
        return;
    }

    curButton = (vehicleState->button != 0u) ? 1u : 0u;

    state->ackPulse = 0u;

    if ((state->prevButton == 0u) && (curButton == 1u)) // 감지
    {
        state->ackPulse = 1u;
        state->ackSeq = (uint8_t)((state->ackSeq + 1u) & 0x0Fu);
    }

    state->prevButton = curButton;
}

void actuator_tx_runtime_on_periodic_send(ActuatorTxRuntime *state)
{
    if (state == 0)
    {
        return;
    }

    state->aliveCnt = (uint8_t)((state->aliveCnt + 1u) & 0x0Fu);
    state->ackPulse = 0u;
}

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
    ackButton   = (txRuntime->ackPulse != 0u) ? 1u : 0u;
    msgValid    = (runtimeState->isTimedOut == false) ? 1u : 0u;

    data[0] = (uint8_t)((safeState & 0x0Fu) |
                        ((speedState & 0x0Fu) << 4));

    data[1] = (uint8_t)((brakeActive & 0x01u) |
                        ((evState & 0x0Fu) << 1) |
                        ((ackButton & 0x01u) << 5) |
                        ((msgValid & 0x01u) << 6));

    data[2] = (uint8_t)((txRuntime->aliveCnt & 0x0Fu) |
                        ((txRuntime->ackSeq & 0x0Fu) << 4));

    data[3] = 0u;
    data[4] = 0u;
    data[5] = 0u;
    data[6] = 0u;
    data[7] = 0u;
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
