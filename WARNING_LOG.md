# 경고 로그 판별 기준

로그 메시지는 번호(LogCode)로만 전송되며, 각 번호에 따른 판별 이유는 다음과 같습니다.

---

## 📌 로그 코드 및 판별 이유

### 0️⃣ LOG_NONE

- **레벨**: -
- **설명**: 로그 없음 (정상 상태)

---

## ⚠️ WARNING 레벨

### 1️⃣ LOG_ACCEL_WARNING

- **레벨**: WARNING
- **판별 이유**:
  - 가속도 센서 delta 값이 중간 임계값(MTH) 이상
  - `SensorState.deltalevel == DELTA_WARNING`
  - 연속 3회 이상 경고 상태 유지 시 발생

**구현 로직**:

```c
if (accel_sensor.delta >= ACCEL_WARNING_THRESHOLD && accel_sensor.delta < ACCEL_CRITICAL_THRESHOLD) {
    log_code = LOG_ACCEL_WARNING;
}
```

---

### 3️⃣ LOG_BRAKE_WARNING

- **레벨**: WARNING
- **판별 이유**:
  - 브레이크 센서 delta 값이 중간 임계값(MTH) 이상
  - `SensorState.deltalevel == DELTA_WARNING`
  - 연속 3회 이상 경고 상태 유지 시 발생

**구현 로직**:

```c
if (brake_sensor.delta >= BRAKE_WARNING_THRESHOLD && brake_sensor.delta < BRAKE_CRITICAL_THRESHOLD) {
    log_code = LOG_BRAKE_WARNING;
}
```

---

### 5️⃣ LOG_STEER_WARNING

- **레벨**: WARNING
- **판별 이유**:
  - 조향각 센서 delta 값이 중간 임계값(MTH) 이상
  - `SensorState.deltalevel == DELTA_WARNING`
  - 연속 3회 이상 경고 상태 유지 시 발생

**구현 로직**:

```c
if (steer_sensor.delta >= STEER_WARNING_THRESHOLD && steer_sensor.delta < STEER_CRITICAL_THRESHOLD) {
    log_code = LOG_STEER_WARNING;
}
```

---

## 🔴 CRITICAL 레벨

### 2️⃣ LOG_ACCEL_CRITICAL

- **레벨**: CRITICAL
- **판별 이유**:
  - 가속도 센서 delta 값이 높은 임계값(CTH) 이상
  - `SensorState.deltalevel == DELTA_CRITICAL`
  - 또는 센서 값이 물리적 범위 초과 (0~255 범위 벗어남)

**구현 로직**:

```c
if (accel_sensor.delta >= ACCEL_CRITICAL_THRESHOLD || accel_value > SENSOR_MAX) {
    log_code = LOG_ACCEL_CRITICAL;
}
```

---

### 4️⃣ LOG_BRAKE_CRITICAL

- **레벨**: CRITICAL
- **판별 이유**:
  - 브레이크 센서 delta 값이 높은 임계값(CTH) 이상
  - `SensorState.deltalevel == DELTA_CRITICAL`
  - 또는 센서 값이 물리적 범위 초과 (0~255 범위 벗어남)

**구현 로직**:

```c
if (brake_sensor.delta >= BRAKE_CRITICAL_THRESHOLD || brake_value > SENSOR_MAX) {
    log_code = LOG_BRAKE_CRITICAL;
}
```

---

### 6️⃣ LOG_STEER_CRITICAL

- **레벨**: CRITICAL
- **판별 이유**:
  - 조향각 센서 delta 값이 높은 임계값(CTH) 이상
  - `SensorState.deltalevel == DELTA_CRITICAL`
  - 또는 센서 값이 물리적 범위 초과 (0~255 범위 벗어남)

**구현 로직**:

```c
if (steer_sensor.delta >= STEER_CRITICAL_THRESHOLD || steer_value > SENSOR_MAX) {
    log_code = LOG_STEER_CRITICAL;
}
```

---

### 7️⃣ LOG_TIMEOUT

- **레벨**: CRITICAL
- **판별 이유**:
  - CAN 메시지 수신 실패
  - 연속 N개 주기(예: 100ms) 동안 0x123 메시지 미수신
  - `g_rxInitialized == FALSE` 상태가 일정 시간 지속

**구현 로직**:

```c
if (stm_10ms_count - last_rx_timestamp > TIMEOUT_THRESHOLD) {
    log_code = LOG_TIMEOUT;
}
```

---

## 📊 임계값 정의 (예상 값)

| 센서    | WARNING THR(MTH) | CRITICAL THR(CTH) | 용도              |
| ------- | ---------------- | ----------------- | ----------------- |
| ACCEL   | 30               | 50                | delta 값 비교     |
| BRAKE   | 30               | 50                | delta 값 비교     |
| STEER   | 30               | 50                | delta 값 비교     |
| TIMEOUT | 10 cycles        | -                 | 100ms 이상 미수신 |

---

## 🔄 주기별 로그 발생 시점

| 주기  | 동작        | 로그 발생 조건                                      |
| ----- | ----------- | --------------------------------------------------- |
| 10ms  | CAN RX 처리 | LOG_TIMEOUT (메시지 없을 때)                        |
| 100ms | 상태 분류   | LOG*ACCEL*_, LOG*BRAKE*_, LOG*STEER*\* (delta 계산) |

---

## 💡 사용 예시

로그는 다음과 같은 형태로 전송됨:

```c
// 예시: 가속도 센서에서 CRITICAL 발생
SendLogToMonitoring(2);  // LOG_ACCEL_CRITICAL

// 예시: CAN 타임아웃 발생
SendLogToMonitoring(7);  // LOG_TIMEOUT

// 예시: 지금은 정상
SendLogToMonitoring(0);  // LOG_NONE
```

---

**Last Updated**: 2026년 4월 6일
