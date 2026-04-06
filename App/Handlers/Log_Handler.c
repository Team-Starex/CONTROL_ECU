#include "Log_Handler.h"

/* ===== 로컬 상태 ===== */
static LogHandlerState g_logHandlerState = {0u};

/*********************************************************************************************************************/
/**
 * @brief 로그 핸들러 초기화
 */
void log_handler_init(void)
{
    g_logHandlerState.can_rx_timeout_count = 0u;
    g_logHandlerState.last_logcode = LOG_NONE;
    g_logHandlerState.warning_consecutive_count = 0u;
}

/*********************************************************************************************************************/
/**
 * @brief 센서 delta 값으로부터 로그 코드 판별 (가속도)
 */
static LogCode evaluate_accel_log(uint8_t delta)
{
    LogCode logcode = LOG_NONE;

    if (delta >= ACCEL_CRITICAL_THRESHOLD)
    {
        logcode = LOG_ACCEL_CRITICAL;
    }
    else if (delta >= ACCEL_WARNING_THRESHOLD)
    {
        logcode = LOG_ACCEL_WARNING;
    }

    return logcode;
}

/*********************************************************************************************************************/
/**
 * @brief 센서 delta 값으로부터 로그 코드 판별 (브레이크)
 */
static LogCode evaluate_brake_log(uint8_t delta)
{
    LogCode logcode = LOG_NONE;

    if (delta >= BRAKE_CRITICAL_THRESHOLD)
    {
        logcode = LOG_BRAKE_CRITICAL;
    }
    else if (delta >= BRAKE_WARNING_THRESHOLD)
    {
        logcode = LOG_BRAKE_WARNING;
    }

    return logcode;
}

/*********************************************************************************************************************/
/**
 * @brief 센서 delta 값으로부터 로그 코드 판별 (조향각)
 */
static LogCode evaluate_steer_log(uint8_t delta)
{
    LogCode logcode = LOG_NONE;

    if (delta >= STEER_CRITICAL_THRESHOLD)
    {
        logcode = LOG_STEER_CRITICAL;
    }
    else if (delta >= STEER_WARNING_THRESHOLD)
    {
        logcode = LOG_STEER_WARNING;
    }

    return logcode;
}

/*********************************************************************************************************************/
/**
 * @brief 센서 상태 평가 및 로그 코드 결정
 * 우선순위: CRITICAL > WARNING > TIMEOUT > NONE
 */
LogCode log_handler_evaluate(VehicleState *vehicle_state)
{
    LogCode accel_log = LOG_NONE;
    LogCode brake_log = LOG_NONE;
    LogCode steer_log = LOG_NONE;
    LogCode final_logcode = LOG_NONE;

    if (vehicle_state == NULL)
    {
        return LOG_NONE;
    }

    /* ===== 각 센서별 로그 판별 ===== */
    accel_log = evaluate_accel_log(vehicle_state->accel.delta);
    brake_log = evaluate_brake_log(vehicle_state->brake.delta);
    steer_log = evaluate_steer_log(vehicle_state->steer.delta);

    /* ===== 타임아웃 판별 ===== */
    LogCode timeout_log = LOG_NONE;
    if (g_logHandlerState.can_rx_timeout_count >= CAN_TIMEOUT_THRESHOLD)
    {
        timeout_log = LOG_TIMEOUT;
    }

    /* ===== 우선순위에 따라 최종 로그 결정 ===== */
    /* 1순위: CRITICAL (심각) */
    if (accel_log == LOG_ACCEL_CRITICAL ||
        brake_log == LOG_BRAKE_CRITICAL ||
        steer_log == LOG_STEER_CRITICAL ||
        timeout_log == LOG_TIMEOUT)
    {
        if (accel_log == LOG_ACCEL_CRITICAL)
            final_logcode = LOG_ACCEL_CRITICAL;
        else if (brake_log == LOG_BRAKE_CRITICAL)
            final_logcode = LOG_BRAKE_CRITICAL;
        else if (steer_log == LOG_STEER_CRITICAL)
            final_logcode = LOG_STEER_CRITICAL;
        else
            final_logcode = LOG_TIMEOUT;
    }
    /* 2순위: WARNING (경고) */
    else if (accel_log == LOG_ACCEL_WARNING ||
             brake_log == LOG_BRAKE_WARNING ||
             steer_log == LOG_STEER_WARNING)
    {
        if (accel_log == LOG_ACCEL_WARNING)
            final_logcode = LOG_ACCEL_WARNING;
        else if (brake_log == LOG_BRAKE_WARNING)
            final_logcode = LOG_BRAKE_WARNING;
        else
            final_logcode = LOG_STEER_WARNING;
    }
    /* 3순위: NONE (정상) */
    else
    {
        final_logcode = LOG_NONE;
    }

    /* ===== 시스템 상태 업데이트 ===== */
    if (final_logcode == LOG_NONE)
    {
        vehicle_state->systemstate = SYS_STATE_NORMAL;
    }
    else if (final_logcode == LOG_ACCEL_WARNING ||
             final_logcode == LOG_BRAKE_WARNING ||
             final_logcode == LOG_STEER_WARNING)
    {
        vehicle_state->systemstate = SYS_STATE_WARNING;
    }
    else /* CRITICAL or TIMEOUT */
    {
        vehicle_state->systemstate = SYS_STATE_CRITICAL;
    }

    vehicle_state->logcode = final_logcode;

    return final_logcode;
}

/*********************************************************************************************************************/
/**
 * @brief CAN RX 타임아웃 카운트 증가
 * 10ms 주기로 호출 (메시지 없을 때)
 */
void log_handler_increment_rx_timeout(void)
{
    if (g_logHandlerState.can_rx_timeout_count < 0xFFFFFFFFu)
    {
        g_logHandlerState.can_rx_timeout_count++;
    }
}

/*********************************************************************************************************************/
/**
 * @brief CAN RX 타임아웃 카운트 리셋
 * 10ms 주기로 메시지 수신했을 때 호출
 */
void log_handler_reset_rx_timeout(void)
{
    g_logHandlerState.can_rx_timeout_count = 0u;
}

/*********************************************************************************************************************/
/**
 * @brief 로그 코드 번호로 로그 출력 (모니터링 시스템으로 전송)
 *
 * 실제 구현:
 * - UART로 시리얼 통신
 * - CAN으로 진단 메시지 송신 (0x7DF)
 * - Flash 메모리에 기록
 * - 원격 모니터링 시스템으로 전송
 */
void log_handler_send_log(LogCode logcode)
{
    /* ===== 로그 코드만 기록 (printf 제거 - 성능 개선) ===== */
    /* printf는 Aurix에서 블로킹 작업으로 CAN 수신 지연 발생 */

    switch (logcode)
    {
    case LOG_NONE:
        /* 정상 상태 - 로그 없음 */
        break;

    case LOG_ACCEL_WARNING:
        /* ACCEL_WARNING: 가속도 센서 경고 임계값 초과 */
        break;

    case LOG_ACCEL_CRITICAL:
        /* ACCEL_CRITICAL: 가속도 센서 심각 임계값 초과 */
        break;

    case LOG_BRAKE_WARNING:
        /* BRAKE_WARNING: 브레이크 센서 경고 임계값 초과 */
        break;

    case LOG_BRAKE_CRITICAL:
        /* BRAKE_CRITICAL: 브레이크 센서 심각 임계값 초과 */
        break;

    case LOG_STEER_WARNING:
        /* STEER_WARNING: 조향각 센서 경고 임계값 초과 */
        break;

    case LOG_STEER_CRITICAL:
        /* STEER_CRITICAL: 조향각 센서 심각 임계값 초과 */
        break;

    case LOG_TIMEOUT:
        /* TIMEOUT: CAN 메시지 수신 타임아웃 */
        break;

    default:
        /* Unknown log code */
        break;
    }

    /* ===== TODO: 실제 시스템 구현 ===== */
    /*
     * 1. UART 전송 실装
     * extern void uart_send_byte(uint8_t data);
     * uart_send_byte((uint8_t)logcode);
     *
     * 2. CAN 진단 메시지로 전송 (ISO-TP)
     * IfxCan_Can_Message diagMsg;
     * diagMsg.messageId = 0x7DF;  // UDS Functional Addressing
     * diagMsg.data[0] = logcode;
     * IfxCan_Can_sendMessage(&g_canNode, &diagMsg, ...);
     *
     * 3. 또는 외부 메モ리(Flash/EEPROM)에 로그 저장
     * log_storage_write(logcode, timestamp);
     */
}
