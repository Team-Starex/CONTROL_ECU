# TC375 Control ECU

Infineon AURIX TC375 TriCore 기반 단일코어 제어 ECU 프로젝트입니다.

## 📋 프로젝트 개요

이 프로젝트는 자동차 제어 시스템의 중앙 제어 ECU(Electronic Control Unit)로, 3개의 독립적인 CPU 코어(Cpu0, Cpu1, Cpu2)에서 병렬 실행되도록 설계되었습니다. CAN 통신을 통해 센서 입력을 수신하고 액추에이터 제어 신호를 송신합니다.

## 🎯 주요 기능

### 1. **CAN 통신**

- **ID 0x123**: 센서 입력 메시지 수신 (RX)
- **ID 0x201**: 액추에이터 제어 메시지 송신 (TX)
- **ID 0x202**: HMI 출력 메시지 송신 (TX)
- 통신 속도: 500 kbps
- FIFO0 기반 수신 처리

### 2. **입력 처리 (Input Handler)**

- 브레이크 값 처리
- 가속 값 처리
- 조향각 센서 처리
- 버튼 입력 처리

### 3. **상태 관리 (State Manager)**

- 차량 상태 분류 및 추적
- 시스템 상태 관리 (NORMAL / WARNING / CRITICAL / FATAL)
- 조향 상태 관리 (Steer State Manager)

### 4. **출력 제어**

- 액추에이터 제어 신호 생성
- HMI 피드백 신호
- 스테이터스 LED 제어

## 📁 디렉토리 구조

```
tc375_control_ecu/
├── Cpu0_Main.c / Cpu1_Main.c / Cpu2_Main.c  # 각 코어의 메인 프로그램
├── Common/
│   └── Types.h                                # 공통 데이터 타입 정의
├── App/
│   ├── Can/                                   # CAN 통신 처리
│   │   ├── Actuator_Output.{c,h}
│   │   └── HMI_Output.{c,h}
│   ├── Handlers/                              # 입력/상태 핸들러
│   │   ├── Input_Handler.{c,h}
│   │   └── State_Manager.{c,h}
│   ├── Steer/                                 # 조향 제어
│   │   └── Steer_State_Manager.{c,h}
│   └── Timer/                                 # 타이머 및 인터럽트
│       └── STM_Interrupt.{c,h}
├── Configurations/
│   └── Ifx_Cfg_Ssw.{c,h}                     # AURIX 설정
└── Libraries/
    ├── iLLD/                                  # Infineon Low Level Drivers
    ├── Infra/                                 # 인프라 라이브러리
    └── Service/                               # 서비스 라이브러리
```

## 🔄 실행 흐름

### Cpu0 메인 루프

```
1. 초기화 단계
   - CAN 인터페이스 초기화
   - 입력/출력 상태 초기화
   - 타이머(STM) 설정

2. 10ms 주기
   - CAN 메시지 수신 (센서 입력)
   - 입력 데이터 파싱

3. 50ms 주기
   - 추가 처리 작업

4. 100ms 주기
   - 상태 분류 및 업데이트
   - 출력 메시지 생성
   - CAN 메시지 송신 (액추에이터, HMI)

5. 1000ms 주기
   - LED 상태 업데이트
```

## 📊 데이터 구조

### InputData (입력 데이터)

```c
typedef struct {
    uint8_t button;        // 버튼 입력
    uint8_t brake_value;   // 브레이크 값
    uint8_t accel_value;   // 가속도 값
    uint8_t steer_value;   // 조향각 값
} InputData;
```

### VehicleState (차량 상태)

- 현재 및 이전 센서 상태
- 필터링된 값 (delta 계산)
- 각 채널별 경고 레벨 추적

### SystemState (시스템 상태)

- `NORMAL`: 정상 동작
- `WARNING`: 경고 상태
- `CRITICAL`: 심각한 상태
- `FATAL_NO_RESPONSE`: 치명적 오류

## ⚠️ 로그 및 경고 코드

시스템은 다음과 같은 경고 및 오류 상태를 추적합니다:

| 코드                 | 설명                    | 우선순위    |
| -------------------- | ----------------------- | ----------- |
| `LOG_ACCEL_WARNING`  | 가속 값 이상 (경고)     | ⚠️ Warning  |
| `LOG_ACCEL_CRITICAL` | 가속 값 이상 (심각)     | 🔴 Critical |
| `LOG_BRAKE_WARNING`  | 브레이크 값 이상 (경고) | ⚠️ Warning  |
| `LOG_BRAKE_CRITICAL` | 브레이크 값 이상 (심각) | 🔴 Critical |
| `LOG_STEER_WARNING`  | 조향각 이상 (경고)      | ⚠️ Warning  |
| `LOG_STEER_CRITICAL` | 조향각 이상 (심각)      | 🔴 Critical |
| `LOG_TIMEOUT`        | 통신 타임아웃           | 🔴 Critical |

## 🛠️ 빌드 및 실행

### 필수 도구

- TASKING VX-toolset for TriCore
- Infineon iLLD 라이브러리
- AURIX Development Studio

### 빌드 방법

```bash
# TASKING 컴파일러를 사용하여 빌드
# Lcf_Tasking_Tricore_Tc.lsl 링커 스크립트 사용
```

### 디버그

- TASKING 디버거 또는 Lauterbach TRACE32
- 시뮬레이션 모드에서 검증 가능

## 📝 개발 노트

### CAN 핀 할당

- **RX**: P20.7
- **TX**: P20.8
- **STB (Standby)**: P20.6

### LED 할당

- **상태 표시**: P00.5

### 타이머 주기

- 10ms: 센서 입력 처리
- 50ms: 추가 작업
- 100ms: 제어 신호 송신
- 1000ms: LED 토글 및 상태 업데이트

## 🔍 주요 특징

- ✅ 실시간 CAN 통신
- ✅ 센서 이상 감지 (WARNING/CRITICAL)
- ✅ 타임아웃 처리
- ✅ 모듈화된 구조
- ✅ 저지연 응답 시간

## 📚 참고 문서

- Infineon AURIX TC375 데이터시트
- iLLD 라이브러리 문서
- TASKING 컴파일러 매뉴얼

---

**Last Updated**: 2026년 4월 6일
