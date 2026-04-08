#ifndef LOG_HANDLER_H_
#define LOG_HANDLER_H_

#include "Types.h"

/* ===== 임계값 정의 ===== */
#define ACCEL_WARNING_THRESHOLD 30u
#define ACCEL_CRITICAL_THRESHOLD 50u

#define BRAKE_WARNING_THRESHOLD 30u
#define BRAKE_CRITICAL_THRESHOLD 50u

#define STEER_WARNING_THRESHOLD 30u
#define STEER_CRITICAL_THRESHOLD 50u

#define CAN_TIMEOUT_THRESHOLD 10u /* 100ms 주기 * 10 = 1000ms */

/* ===== 로그 처리 상태 ===== */
typedef struct
{
    uint32_t can_rx_timeout_count;
    LogCode last_logcode;
    uint8_t warning_consecutive_count;
} LogHandlerState;

/* ===== 함수 선언 ===== */

/**
 * @brief 로그 핸들러 초기화
 */
void log_handler_init(void);

/**
 * @brief 센서 상태 평가 및 로그 코드 결정
 * @param vehicle_state: 차량 상태 구조체
 * @return 결정된 LogCode
 */
LogCode log_handler_evaluate(VehicleState *vehicle_state);

/**
 * @brief CAN RX 타임아웃 카운트 증가
 */
void log_handler_increment_rx_timeout(void);

/**
 * @brief CAN RX 타임아웃 카운트 리셋
 */
void log_handler_reset_rx_timeout(void);

/**
 * @brief 로그 코드 번호로 로그 출력 (모니터링 시스템으로 전송)
 * @param logcode: 로그 코드
 * @note 실제 구현은 UART/CAN으로 모니터링 시스템에 전송
 */
void log_handler_send_log(LogCode logcode);

#endif /* LOG_HANDLER_H_ */
