#include "State_Manager.h"

//센서들의 delta값을 근거로 시스템의 상태를 판단 및 로그 값 갱신
//하나라도 CRITICAL -> SYS_STATE_CRITICAL
//하나라도 WARNING  -> SYS_STATE_WARNING
//해당 없을 시 NORMAL
void classify_state(VehicleState *state)
{
    if (state == 0)
    {
        return;
    }

    state->systemstate = SYS_STATE_NORMAL;
    state->logcode = LOG_NONE;

    if ((state->brake.deltalevel == DELTA_CRITICAL) ||
        (state->accel.deltalevel == DELTA_CRITICAL) ||
        (state->steer.deltalevel == DELTA_CRITICAL))
    {
        state->systemstate = SYS_STATE_CRITICAL;

        if (state->brake.deltalevel == DELTA_CRITICAL)
        {
            state->logcode = LOG_BRAKE_CRITICAL;
        }
        else if (state->accel.deltalevel == DELTA_CRITICAL)
        {
            state->logcode = LOG_ACCEL_CRITICAL;
        }
        else
        {
            state->logcode = LOG_STEER_CRITICAL;
        }
        return;
    }

    if ((state->brake.deltalevel == DELTA_WARNING) ||
        (state->accel.deltalevel == DELTA_WARNING) ||
        (state->steer.deltalevel == DELTA_WARNING))
    {
        state->systemstate = SYS_STATE_WARNING;

        if (state->brake.deltalevel == DELTA_WARNING)
        {
            state->logcode = LOG_BRAKE_WARNING;
        }
        else if (state->accel.deltalevel == DELTA_WARNING)
        {
            state->logcode = LOG_ACCEL_WARNING;
        }
        else
        {
            state->logcode = LOG_STEER_WARNING;
        }
        return;
    }
}
