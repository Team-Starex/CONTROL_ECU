#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "Ifx_Cfg_Ssw.h"
#include "IfxCan_Can.h"
#include "IfxPort.h"
#include "STM_Interrupt.h"

#include "Types.h"
#include "Input_Handler.h"
#include "State_Manager.h"
#include "HMI_Output.h"
#include "Actuator_Output.h"

#define LED                     &MODULE_P00, 5

#define RX_SENSOR_MSG_ID        0x001u
#define TX_HMI_MSG_ID           0x200u
#define TX_ACT_MSG_ID           0x201u

IFX_ALIGN(4) IfxCpu_syncEvent cpuSyncEvent = 0;

/* ===== 공통 상태 ===== */
static InputData g_inputData;
static VehicleState g_vehicleState;
static OutputRuntimeState g_outputRuntime;
static ActuatorTxRuntime g_actuatorTxRuntime;

/* ===== CAN ===== */
//static IfxCan_Can          g_mcmcan;
//static IfxCan_Can_Node     g_canNode;
//static IfxCan_Can_Pins     g_canPins;
//static IfxCan_Message      g_rxMsg;
//static IfxCan_Message      g_txMsgHmi;
//static IfxCan_Message      g_txMsgAct;

//static uint32_t g_rxData[2];
static uint32_t g_txDataHmi[2];
static uint32_t g_txDataAct[2];

/* ===== 디버그용 테스트 ===== */
static uint8_t test_step = 0u;
static boolean g_useDummyTest = TRUE;

//static void initCanTransceiver(void)
//{
//    IfxPort_setPinModeOutput(&MODULE_P20, 6, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_general);
//    IfxPort_setPinLow(&MODULE_P20, 6);   /* STB low = transceiver enable */
//}

//static void app_init_can(void)
//{
//    IfxCan_Can_Config canConfig;
//    IfxCan_Can_NodeConfig nodeConfig;
//
//    IfxCan_Can_initModuleConfig(&canConfig, &MODULE_CAN0);
//    IfxCan_Can_initModule(&g_mcmcan, &canConfig);
//
//    IfxCan_Can_initNodeConfig(&nodeConfig, &g_mcmcan);
//    nodeConfig.nodeId = IfxCan_NodeId_0;
//    nodeConfig.frame.type = IfxCan_FrameType_transmitAndReceive;
//    nodeConfig.frame.mode = IfxCan_FrameMode_standard;
//    nodeConfig.baudRate.baudrate = 500000;
//
//    nodeConfig.txConfig.txMode = IfxCan_TxMode_dedicatedBuffers;
//    nodeConfig.txConfig.dedicatedTxBuffersNumber = 2;
//    nodeConfig.txConfig.txBufferDataFieldSize = IfxCan_DataFieldSize_8;
//
//    nodeConfig.rxConfig.rxMode = IfxCan_RxMode_fifo0;
//    nodeConfig.rxConfig.rxFifo0Size = 8;
//    nodeConfig.rxConfig.rxFifo0DataFieldSize = IfxCan_DataFieldSize_8;
//
//    g_canPins.rxPin = &IfxCan_RXD00B_P20_7_IN;
//    g_canPins.rxPinMode = IfxPort_InputMode_noPullDevice;
//    g_canPins.txPin = &IfxCan_TXD00_P20_8_OUT;
//    g_canPins.txPinMode = IfxPort_OutputMode_pushPull;
//    g_canPins.padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;
//    nodeConfig.pins = &g_canPins;
//
//    IfxCan_Can_initNode(&g_canNode, &nodeConfig);
//
//    IfxCan_Can_initMessage(&g_rxMsg);
//    g_rxMsg.frameMode = IfxCan_FrameMode_standard;
//    g_rxMsg.messageIdLength = IfxCan_MessageIdLength_standard;
//    g_rxMsg.dataLengthCode = IfxCan_DataLengthCode_8;
//    g_rxMsg.messageId = RX_SENSOR_MSG_ID;
//
//    IfxCan_Can_initMessage(&g_txMsgHmi);
//    g_txMsgHmi.messageId = TX_HMI_MSG_ID;
//    g_txMsgHmi.bufferNumber = 0;
//    g_txMsgHmi.frameMode = IfxCan_FrameMode_standard;
//    g_txMsgHmi.messageIdLength = IfxCan_MessageIdLength_standard;
//    g_txMsgHmi.dataLengthCode = IfxCan_DataLengthCode_8;
//
//    IfxCan_Can_initMessage(&g_txMsgAct);
//    g_txMsgAct.messageId = TX_ACT_MSG_ID;
//    g_txMsgAct.bufferNumber = 1;
//    g_txMsgAct.frameMode = IfxCan_FrameMode_standard;
//    g_txMsgAct.messageIdLength = IfxCan_MessageIdLength_standard;
//    g_txMsgAct.dataLengthCode = IfxCan_DataLengthCode_8;
//}

//static void process_rx_frame_words(const uint32_t rxWords[2])
//{
//    uint8_t rxBytes[8];
//
//    rxBytes[0] = (uint8_t)(rxWords[0] & 0xFFu);
//    rxBytes[1] = (uint8_t)((rxWords[0] >> 8) & 0xFFu);
//    rxBytes[2] = (uint8_t)((rxWords[0] >> 16) & 0xFFu);
//    rxBytes[3] = (uint8_t)((rxWords[0] >> 24) & 0xFFu);
//    rxBytes[4] = (uint8_t)(rxWords[1] & 0xFFu);
//    rxBytes[5] = (uint8_t)((rxWords[1] >> 8) & 0xFFu);
//    rxBytes[6] = (uint8_t)((rxWords[1] >> 16) & 0xFFu);
//    rxBytes[7] = (uint8_t)((rxWords[1] >> 24) & 0xFFu);
//
//    input_handler_parse_can(rxBytes, &g_inputData);
//    input_handler_update(&g_vehicleState, &g_inputData);
//    classify_state(&g_vehicleState);
//    output_runtime_notify_input(&g_outputRuntime);
//    actuator_tx_runtime_update(&g_actuatorTxRuntime, &g_vehicleState);
//}

/* ===== 더미 테스트용 ===== */
static void make_dummy_rx(uint8_t data[8], uint8_t button, uint8_t brake, uint8_t accel, uint8_t steer)
{
    data[0] = button;
    data[1] = 0u;
    data[2] = brake;
    data[3] = 0u;
    data[4] = accel;
    data[5] = 0u;
    data[6] = steer;
    data[7] = 0u;
}

static boolean run_testcase_step(uint8_t step)
{
    uint8_t rxData[8];

    switch (step)
    {
        case 0u:
            make_dummy_rx(rxData, 0u, 50u, 60u, 70u);
            input_handler_parse_can(rxData, &g_inputData);
            input_handler_init(&g_vehicleState, &g_inputData);
            classify_state(&g_vehicleState);
            actuator_tx_runtime_update(&g_actuatorTxRuntime, &g_vehicleState);
            return TRUE;

        case 1u:
            make_dummy_rx(rxData, 1u, 50u, 145u, 70u);
            input_handler_parse_can(rxData, &g_inputData);
            input_handler_update(&g_vehicleState, &g_inputData);
            classify_state(&g_vehicleState);
            actuator_tx_runtime_update(&g_actuatorTxRuntime, &g_vehicleState);
            return TRUE;

        case 2u:
            make_dummy_rx(rxData, 1u, 50u, 255u, 70u);
            input_handler_parse_can(rxData, &g_inputData);
            input_handler_update(&g_vehicleState, &g_inputData);
            classify_state(&g_vehicleState);
            actuator_tx_runtime_update(&g_actuatorTxRuntime, &g_vehicleState);
            return TRUE;

        case 3u:
            make_dummy_rx(rxData, 1u, 160u, 60u, 70u);
            input_handler_parse_can(rxData, &g_inputData);
            input_handler_update(&g_vehicleState, &g_inputData);
            classify_state(&g_vehicleState);
            actuator_tx_runtime_update(&g_actuatorTxRuntime, &g_vehicleState);
            return TRUE;

        case 4u:
            make_dummy_rx(rxData, 1u, 255u, 60u, 70u);
            input_handler_parse_can(rxData, &g_inputData);
            input_handler_update(&g_vehicleState, &g_inputData);
            classify_state(&g_vehicleState);
            actuator_tx_runtime_update(&g_actuatorTxRuntime, &g_vehicleState);
            return TRUE;

        case 5u:
            make_dummy_rx(rxData, 1u, 50u, 60u, 200u);
            input_handler_parse_can(rxData, &g_inputData);
            input_handler_update(&g_vehicleState, &g_inputData);
            classify_state(&g_vehicleState);
            actuator_tx_runtime_update(&g_actuatorTxRuntime, &g_vehicleState);
            return TRUE;

        case 6u:
            make_dummy_rx(rxData, 1u, 50u, 60u, 255u);
            input_handler_parse_can(rxData, &g_inputData);
            input_handler_update(&g_vehicleState, &g_inputData);
            classify_state(&g_vehicleState);
            actuator_tx_runtime_update(&g_actuatorTxRuntime, &g_vehicleState);
            return TRUE;

        default:
            return FALSE; /* timeout 확인 */
    }
}

void core0_main(void)
{
    uint8_t isOn = 1u;
    boolean hasInput = FALSE;

    IfxCpu_enableInterrupts();

    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());
    IfxScuWdt_disableSafetyWatchdog(IfxScuWdt_getSafetyWatchdogPassword());

    IfxCpu_emitEvent(&cpuSyncEvent);
    IfxCpu_waitEvent(&cpuSyncEvent, 1);

    initPeripherals();

    g_inputData.button      = 0u;
    g_inputData.brake_value = 0u;
    g_inputData.accel_value = 0u;
    g_inputData.steer_value = 0u;

    input_handler_init(&g_vehicleState, &g_inputData);
    output_runtime_init(&g_outputRuntime);
    actuator_tx_runtime_init(&g_actuatorTxRuntime);

    while (1)
    {
        if (stm_get_10msflag() != FALSE)
        {
            /* 10ms마다 할 작업*/
            // can통신 해서 데이터 구조 초기화
        }
        if (stm_get_50msflag() != FALSE)
        {
            /* 50ms마다 할 작업*/
        }

        if (stm_get_100msflag() != FALSE)
        {
            output_runtime_tick100ms(&g_outputRuntime);

            output_build_can_words(&g_vehicleState,
                                   &g_outputRuntime,
                                   (int32_t *)g_txDataHmi);

            actuator_build_can_words(&g_vehicleState,
                                     &g_outputRuntime,
                                     &g_actuatorTxRuntime,
                                     g_txDataAct);

//            while (IfxCan_Can_sendMessage(&g_canNode, &g_txMsgHmi, g_txDataHmi) == IfxCan_Status_notSentBusy)
//            { }
//
//            while (IfxCan_Can_sendMessage(&g_canNode, &g_txMsgAct, g_txDataAct) == IfxCan_Status_notSentBusy)
//            { }
            actuator_tx_runtime_on_periodic_send(&g_actuatorTxRuntime);
        }

        if (stm_get_1000msflag() != FALSE)
        {

            }
        }
    }
}
