#include "State_Manager.h"

void classify_state(VehicleState *state)
{
    uint8_t steerOffset;
    uint8_t steerWarnThreshold;
    uint8_t steerCritThreshold;
    int brakeSeverity = 0;
    int steerSeverity = 0;

    if (state == 0)
    {
        return;
    }

    state->systemstate = SYS_STATE_NORMAL;
    state->logcode = LOG_NONE;

    steerOffset = (state->steer.filtered > 128U)
                ? (state->steer.filtered - 128U)
                : (128U - state->steer.filtered);

    switch (state->speedBand)
    {
        case SPEED_HIGH:
            steerWarnThreshold = 40U;
            steerCritThreshold = 70U;
            break;

        case SPEED_MID:
            steerWarnThreshold = 55U;
            steerCritThreshold = 90U;
            break;

        case SPEED_LOW:
            steerWarnThreshold = 75U;
            steerCritThreshold = 110U;
            break;

        case SPEED_STOP:
        default:
            steerWarnThreshold = 255U;
            steerCritThreshold = 255U;
            break;
    }

    /* brake severity */
    if (state->brake.deltalevel == DELTA_CRITICAL)
    {
        brakeSeverity = 2;
    }
    else if (state->brake.deltalevel == DELTA_WARNING)
    {
        brakeSeverity = 1;
    }

    /* steer severity: delta + 절대 조향량 반영 */
    if ((state->steer.deltalevel == DELTA_CRITICAL) || (steerOffset >= steerCritThreshold))
    {
        steerSeverity = 2;
    }
    else if ((state->steer.deltalevel == DELTA_WARNING) || (steerOffset >= steerWarnThreshold))
    {
        steerSeverity = 1;
    }

    /* 속도별 보정 */
    if ((state->speedBand == SPEED_HIGH) && (brakeSeverity == 1))
    {
        brakeSeverity = 2;
    }

    if ((state->speedBand == SPEED_HIGH) && (steerSeverity == 1))
    {
        steerSeverity = 2;
    }

    if ((state->speedBand == SPEED_STOP) || (state->speedBand == SPEED_LOW))
    {
        if (brakeSeverity > 1)
        {
            brakeSeverity = 1;
        }
        if (steerSeverity > 1)
        {
            steerSeverity = 1;
        }
    }

    if ((brakeSeverity >= 2) || (steerSeverity >= 2))
    {
        state->systemstate = SYS_STATE_CRITICAL;

        if (brakeSeverity >= 2)
        {
            state->logcode = LOG_BRAKE_CRITICAL;
        }
        else
        {
            state->logcode = LOG_STEER_CRITICAL;
        }
        return;
    }

    if ((brakeSeverity >= 1) || (steerSeverity >= 1))
    {
        state->systemstate = SYS_STATE_WARNING;

        if (brakeSeverity >= 1)
        {
            state->logcode = LOG_BRAKE_WARNING;
        }
        else
        {
            state->logcode = LOG_STEER_WARNING;
        }
        return;
    }
}
