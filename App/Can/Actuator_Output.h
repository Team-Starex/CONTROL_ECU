#ifndef ACTUATOR_OUTPUT_H_
#define ACTUATOR_OUTPUT_H_

#include <stdint.h>
#include <stdbool.h>
#include "Types.h"
#include "HMI_Output.h"

// 구동부 송신 전용 구조체
typedef struct
{
    uint8_t aliveCnt;   // 매 주기 송신 시 0 ~ 15 증가
    uint8_t ackSeq;     // ACK 버튼 상승엣지 발생 시 0 ~ 15 증가
    uint8_t prevButton; // 버튼 상승엣지 검출용 data
    uint8_t ackPulse;   // 이번 프레임에서만 1
} ActuatorTxRuntime;

void actuator_tx_runtime_init(ActuatorTxRuntime *state);
void actuator_tx_runtime_update(ActuatorTxRuntime *state, const VehicleState *vehicleState);
void actuator_tx_runtime_on_periodic_send(ActuatorTxRuntime *state);

void actuator_build_can_data(const VehicleState *vehicleState,
                             const OutputRuntimeState *runtimeState,
                             const ActuatorTxRuntime *txRuntime,
                             uint8_t data[8]);

void actuator_build_can_words(const VehicleState *vehicleState,
                              const OutputRuntimeState *runtimeState,
                              const ActuatorTxRuntime *txRuntime,
                              uint32_t txWords[2]);

#endif
