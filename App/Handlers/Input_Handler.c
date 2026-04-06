#include "Input_Handler.h"

#define LOOKS_LIKE_NOISE      15U

#define BRAKE_THRESHOLD_1     10U
#define BRAKE_THRESHOLD_2     25U
#define ACCEL_THRESHOLD_1     8U
#define ACCEL_THRESHOLD_2     20U
#define STEER_THRESHOLD_1     10U
#define STEER_THRESHOLD_2     20U

#define FILTER_DIV            4U

#define ACCEL_DEADZONE        40U
#define BRAKE_DEADZONE        40U

#define SPEED_MAX_X10         2550U      //255.0 km/h
#define SPEED_LOW_MAX_X10     400U       //40.0 km/h
#define SPEED_MID_MAX_X10     1000U      //100.0 km/h

static uint8_t abs_u8(uint8_t a, uint8_t b)
{
    return (a > b) ? (a - b) : (b - a);
}

static uint8_t low_pass_u8(uint8_t prevFiltered, uint8_t raw)
{
    int16_t diff = (int16_t)raw - (int16_t)prevFiltered;
    int16_t next = (int16_t)prevFiltered + (diff / (int16_t)FILTER_DIV);

    if (next < 0)
    {
        next = 0;
    }
    if (next > 255)
    {
        next = 255;
    }

    return (uint8_t)next;
}

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

static void update_sensor_data(SensorState *sensor, uint8_t newData, uint8_t warn, uint8_t critical)
{
    uint8_t prevFiltered;

    if (sensor == 0)
    {
        return;
    }

    sensor->prev = sensor->cur;
    sensor->cur  = newData;

    prevFiltered = sensor->filtered;
    sensor->filtered = low_pass_u8(sensor->filtered, newData);

    sensor->delta = abs_u8(sensor->filtered, prevFiltered);

    if (sensor->delta < LOOKS_LIKE_NOISE)
    {
        sensor->delta = 0U;
    }

    sensor->deltalevel = classify_delta(sensor->delta, warn, critical);
}

static uint8_t apply_deadzone(uint8_t value, uint8_t dz)
{
    return (value > dz) ? (value - dz) : 0U;
}

static void update_virtual_speed(VehicleState *state)
{
    uint8_t accelEff;
    uint8_t brakeEff;
    int16_t speed;
    int16_t accelStep;
    int16_t brakeStep;
    int16_t dragStep;

    if (state == 0)
    {
        return;
    }

    accelEff = apply_deadzone(state->accel.filtered, ACCEL_DEADZONE);
    brakeEff = apply_deadzone(state->brake.filtered, BRAKE_DEADZONE);

    speed = (int16_t)state->virtualSpeedKph_x10;

    //10ms마다 가상 속도 갱신
    accelStep = ((int16_t)accelEff * 2) / 255;   //가속
    brakeStep = ((int16_t)brakeEff * 2) / 255;   //브레이크
    dragStep  = 1 + (speed / 1000);     //자연 감속

    speed += accelStep;
    speed -= brakeStep;
    speed -= dragStep;

    if (speed < 0)
    {
        speed = 0;
    }
    else if (speed > (int16_t)SPEED_MAX_X10)
    {
        speed = SPEED_MAX_X10;
    }

    state->virtualSpeedKph_x10 = (uint16_t)speed;

    if (speed == 0)
    {
        state->speedBand = SPEED_STOP;
    }
    else if (speed <= SPEED_LOW_MAX_X10)
    {
        state->speedBand = SPEED_LOW;
    }
    else if (speed <= SPEED_MID_MAX_X10)
    {
        state->speedBand = SPEED_MID;
    }
    else
    {
        state->speedBand = SPEED_HIGH;
    }
}

void input_handler_parse_can(const uint8_t rxData[8], InputData *data)
{
    if ((rxData == 0) || (data == 0))
    {
        return;
    }

    data->button      = rxData[0]; // 파싱
    data->brake_value = rxData[2];
    data->accel_value = rxData[4];
    data->steer_value = rxData[6];
}

void input_handler_init(VehicleState *state, const InputData *data)
{
    if ((state == 0) || (data == 0))
    {
        return;
    }

    state->button = data->button; // 데이터 반영

    state->brake.prev = data->brake_value;
    state->brake.cur = data->brake_value;
    state->brake.filtered = data->brake_value;
    state->brake.delta = 0U;
    state->brake.deltalevel = DELTA_NORMAL;

    state->accel.prev = data->accel_value;
    state->accel.cur = data->accel_value;
    state->accel.filtered = data->accel_value;
    state->accel.delta = 0U;
    state->accel.deltalevel = DELTA_NORMAL;

    state->steer.prev = data->steer_value;
    state->steer.cur = data->steer_value;
    state->steer.filtered = data->steer_value;
    state->steer.delta = 0U;
    state->steer.deltalevel = DELTA_NORMAL;

    state->virtualSpeedKph_x10 = 0U;
    state->speedBand = SPEED_STOP;

    state->systemstate = SYS_STATE_NORMAL;
    state->logcode = LOG_NONE;
}

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

    update_virtual_speed(state);
}
