#ifndef HANDLERS_INPUT_HANDLER_H_
#define HANDLERS_INPUT_HANDLER_H_

#include <stdint.h>
#include "Types.h"

/* 수신 CAN Frame 구조
 * byte0 : button
 * byte1 : reserved
 * byte2 : brake_value
 * byte3 : reserved
 * byte4 : accel_value
 * byte5 : reserved
 * byte6 : steer_value
 * byte7 : reserved
 */

void input_handler_parse_can(const uint8_t rxData[8], InputData *data);
void input_handler_init(VehicleState *state, const InputData *data);
void input_handler_update(VehicleState *state, const InputData *data);

#endif
