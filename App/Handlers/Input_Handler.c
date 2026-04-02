#include "Input_Handler.h"

#define LOOKS_LIKE_NOISE 20U
#define BRAKE_THRESHOLD_1   100U    //Brake delta warn
#define BRAKE_THRESHOLD_2   180U    //Brake delta critical
#define ACCEL_THRESHOLD_1   80U     //ACCEL delta warn
#define ACCEL_THRESHOLD_2   150U    //ACCEL delta critical
#define STEER_THRESHOLD_1   120U    //STEER delta warn
#define STEER_THRESHOLD_2   220U    //STEER delta warn

//delta값 계산 함수 (센서에 들어온 현재 값, 이전 값만 보고 비교중)
static uint8_t cal_delta(uint8_t cur, uint8_t prev)
{
    return ((cur > prev) ? (cur - prev) : (prev - cur));
}

//delta 값을 받은 후 이의 범위를 계산
static DeltaLevel classify_delta(uint8_t delta, uint8_t warn, uint8_t critical)
{
    if (delta >= critical)
    {
        return DELTA_CRITICAL;
    }
    else if (delta >= warn)
    {
        return DELTA_WARNING;
    }
    else
    {
        return DELTA_NORMAL;
    }
}

// 센서의 데이터를 갱신
static void update_sensor_data(SensorState *sensor, uint8_t new_data, uint8_t warn, uint8_t critical)
{
    sensor->prev = sensor->cur;
    sensor->cur = new_data;
    sensor->delta = cal_delta(sensor->cur, sensor->prev);

    if (sensor->delta < LOOKS_LIKE_NOISE)
    {
        sensor->delta = 0U;
    }

    sensor->deltalevel = classify_delta(sensor->delta, warn, critical);
}

// can데이터 구조체 변환
void input_handler_parse_can(const uint8_t rxData[8], InputData *data)
{
    if ((rxData == 0) || (data == 0))
    {
        return;
    }

    data->button      = rxData[0];
    data->brake_value = rxData[2];
    data->accel_value = rxData[4];
    data->steer_value = rxData[6];
}

// 각 센서들의 데이터를 init
void input_handler_init(VehicleState *state, const InputData *data)
{
    if ((state == 0) || (data == 0))
    {
        return;
    }

    state->button = data->button;

    state->brake.prev = data->brake_value;
    state->brake.cur = data->brake_value;
    state->brake.delta = 0U;
    state->brake.deltalevel = DELTA_NORMAL;

    state->accel.prev = data->accel_value;
    state->accel.cur = data->accel_value;
    state->accel.delta = 0U;
    state->accel.deltalevel = DELTA_NORMAL;

    state->steer.prev = data->steer_value;
    state->steer.cur = data->steer_value;
    state->steer.delta = 0U;
    state->steer.deltalevel = DELTA_NORMAL;

    state->systemstate = SYS_STATE_NORMAL;
    state->logcode = LOG_NONE;
}

// 센서 데이터 유무 판단 후 센서 값 갱신
void input_handler_update(VehicleState *state, const InputData *data)
{
    if ((state == 0) || (data == 0))
    {
        return;
    }

    state->button = data->button;
    update_sensor_data(&state->brake, data->brake_value, BRAKE_THRESHOLD_1, BRAKE_THRESHOLD_2);
    update_sensor_data(&state->accel, data->accel_value, ACCEL_THRESHOLD_1, ACCEL_THRESHOLD_2);
    update_sensor_data(&state->steer, data->steer_value, STEER_THRESHOLD_1, STEER_THRESHOLD_2);
}
