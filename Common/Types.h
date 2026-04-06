#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef enum{
    SYS_STATE_NORMAL = 0,
    SYS_STATE_WARNING,
    SYS_STATE_CRITICAL,
    SYS_STATE_FATAL_NO_RESPONSE
} SystemState;

typedef enum{
    DELTA_NORMAL = 0,
    DELTA_WARNING,
    DELTA_CRITICAL
} DeltaLevel;

typedef enum{
    LOG_NONE = 0u,
    LOG_ACCEL_WARNING,
    LOG_ACCEL_CRITICAL,
    LOG_BRAKE_WARNING,
    LOG_BRAKE_CRITICAL,
    LOG_STEER_WARNING,
    LOG_STEER_CRITICAL,
    LOG_TIMEOUT
} LogCode;

typedef enum{
    SPEED_STOP = 0,
    SPEED_LOW,
    SPEED_MID,
    SPEED_HIGH
} SpeedBand;

typedef struct{
    uint8_t button;
    uint8_t brake_value;
    uint8_t accel_value;
    uint8_t steer_value;
} InputData;

typedef struct{
    uint8_t prev;
    uint8_t cur;
    uint8_t filtered;
    uint8_t delta;
    DeltaLevel deltalevel;
} SensorState;

typedef struct{
    SensorState brake;
    SensorState accel;
    SensorState steer;

    uint8_t button;

    uint16_t virtualSpeedKph_x10;   /* ex) 253 => 25.3 km/h */
    SpeedBand speedBand;

    SystemState systemstate;
    LogCode logcode;
} VehicleState;

#endif
