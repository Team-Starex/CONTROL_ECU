#include "Ifx_Types.h"
#include "IfxCpu.h"
#include "IfxScuWdt.h"
#include "IfxPort.h"
#include "IfxCan_Can.h"
#include "Bsp.h"

#define TX_MSG_ID           0x123u

#define BUTTON_PORT         (&MODULE_P00)
#define BUTTON_PIN          7

#define CAN_STB_PORT        (&MODULE_P20)
#define CAN_STB_PIN         6

static IfxCan_Can         g_mcmcan;
static IfxCan_Can_Node    g_canNode;
static IfxCan_Can_Pins    g_canPins;

static IfxCan_Message     g_txMsg;
static uint32             g_txData[2];
static uint8 timeout_counter = 10;  // ← 정적 변수로 값 유지
static uint8 response_button = 0;

typedef struct {
    uint8 speed_value;
    uint8 steer_angle;
    uint8 safe_state;
    uint8 log_num;
    uint8 timeout;
} CanPerceiveData;

static CanPerceiveData data;
/* 버튼 상태 읽기
 * 지금은 HIGH면 눌림이라고 가정
 * 만약 보드 버튼이 Active Low면 low로 바꿔야 함
 */
static boolean button_pressed(void)
{
    return (IfxPort_getPinState(BUTTON_PORT, BUTTON_PIN) == IfxPort_State_high);
}

void init_gpio(void)
{
    IfxPort_setPinMode(BUTTON_PORT, BUTTON_PIN, IfxPort_Mode_inputPullUp);

    IfxPort_setPinMode(CAN_STB_PORT, CAN_STB_PIN, IfxPort_Mode_outputPushPullGeneral);
    IfxPort_setPinLow(CAN_STB_PORT, CAN_STB_PIN);
}

void init_can(void)
{
    IfxCan_Can_Config canConfig;
    IfxCan_Can_NodeConfig nodeConfig;

    data.speed_value = 0u;
    data.steer_angle = 0u;
    data.safe_state  = 0u;
    data.log_num     = 0u;
    data.timeout     = 10u;  // ← 현재 카운터 값 전송

    IfxCan_Can_initModuleConfig(&canConfig, &MODULE_CAN0);
    IfxCan_Can_initModule(&g_mcmcan, &canConfig);

    IfxCan_Can_initNodeConfig(&nodeConfig, &g_mcmcan);

    nodeConfig.nodeId = IfxCan_NodeId_0;
    nodeConfig.frame.type = IfxCan_FrameType_transmitAndReceive;
    nodeConfig.frame.mode = IfxCan_FrameMode_standard;
    nodeConfig.baudRate.baudrate = 500000;

    nodeConfig.txConfig.txMode = IfxCan_TxMode_dedicatedBuffers;
    nodeConfig.txConfig.dedicatedTxBuffersNumber = 1;
    nodeConfig.txConfig.txBufferDataFieldSize = IfxCan_DataFieldSize_8;

    nodeConfig.rxConfig.rxMode = IfxCan_RxMode_fifo0;
    nodeConfig.rxConfig.rxFifo0Size = 8;
    nodeConfig.rxConfig.rxFifo0DataFieldSize = IfxCan_DataFieldSize_8;

    g_canPins.rxPin = &IfxCan_RXD00B_P20_7_IN;
    g_canPins.rxPinMode = IfxPort_InputMode_noPullDevice;
    g_canPins.txPin = &IfxCan_TXD00_P20_8_OUT;
    g_canPins.txPinMode = IfxPort_OutputMode_pushPull;
    g_canPins.padDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;

    nodeConfig.pins = &g_canPins;

    IfxCan_Can_initNode(&g_canNode, &nodeConfig);

    IfxCan_Can_initMessage(&g_txMsg);
    g_txMsg.messageId = TX_MSG_ID;
    g_txMsg.bufferNumber = 0;
    g_txMsg.frameMode = IfxCan_FrameMode_standard;
    g_txMsg.messageIdLength = IfxCan_MessageIdLength_standard;
    g_txMsg.dataLengthCode = IfxCan_DataLengthCode_1;
}

static void send_button_state(boolean pressed)
{
    uint8 packed = 0u;

    /* bit0에 버튼 상태 저장 */
    packed |= (pressed ? 1u : 0u) << 0;

    g_txData[0] = 0u;
    g_txData[1] = 0u;

    /* data byte0에 packed 값 저장 */
    g_txData[0] = packed;

    IfxCan_Can_sendMessage(&g_canNode, &g_txMsg, g_txData);
}

static void send_perceive_data(const CanPerceiveData *d)
{
    uint8 retry = 0u;

    g_txData[0] = 0u;
    g_txData[1] = 0u;

    g_txData[0]  = (uint32)d->speed_value;
    g_txData[0] |= (uint32)d->steer_angle << 8;
    g_txData[0] |= (uint32)d->safe_state  << 16;
    g_txData[0] |= (uint32)d->log_num     << 24;
    g_txData[1]  = (uint32)d->timeout;

    g_txMsg.dataLengthCode = IfxCan_DataLengthCode_5;

    while (IfxCan_Can_sendMessage(&g_canNode, &g_txMsg, g_txData) == IfxCan_Status_notSentBusy)
    {
        retry++;
        if (retry >= 3u)
        {
            /* 3회 실패 시 포기, 다음 100ms에 재시도 */
            break;
        }
    }
}

/*
 * 고려할 점
 * 상태가 critical 첫 진입하면 10으로 초기화
 * */

static volatile uint8 temp_safe_mode = 2u;

void critical_response_timer_down(void){
    if(data.safe_state != 2u){
        return;
    }
    timeout_counter = timeout_counter >0? timeout_counter - 1: 10u;
}

void can_perceive_ras_100ms(void)
{
    if(data.safe_state == 2u && !button_pressed())//나중에 버튼 상태 불러와서 적용하기
    {
        timeout_counter = 10u;  // 0이 되면 다시 10으로 리셋
        //임시로 버튼 누르면 해제 되도록 테스팅
        temp_safe_mode = 0u;
    }
    else if(data.safe_state == 2u && timeout_counter==0){
        temp_safe_mode = 3u;
    }
    //받아온 값 최신화 해주기
    data.speed_value = 10u;
    data.steer_angle = 90u;
    data.safe_state  = temp_safe_mode;
    data.log_num     = 42u;
    data.timeout     = timeout_counter;  // ← 현재 카운터 값 전송

    send_perceive_data(&data);  // 한 번만 호출!

}
