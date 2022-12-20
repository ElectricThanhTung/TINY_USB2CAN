
#include <string.h>
#include "can.h"

#define PCLK_FREQ               48000000

static CAN_HandleTypeDef can_handle = {0};
static uint8_t is_connected = 0;
static CAN_MsgTypeDef rx_msg[MAX_RX_MSG_COUNT];
static volatile uint16_t rx_msg_index = 0;
static volatile uint16_t rx_msg_count = 0;
static uint8_t bs1;
static uint8_t bs2;

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan) {
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    if(hcan->Instance == CAN) {
        __HAL_RCC_CAN1_CLK_ENABLE();

        __HAL_RCC_GPIOB_CLK_ENABLE();

        GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
        GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull = GPIO_NOPULL;
        GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF4_CAN;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

        HAL_NVIC_SetPriority(CEC_CAN_IRQn, 0, 0);
        HAL_NVIC_EnableIRQ(CEC_CAN_IRQn);
    }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef *hcan) {
    if(hcan->Instance == CAN) {
        __HAL_RCC_CAN1_CLK_DISABLE();
        HAL_GPIO_DeInit(GPIOB, GPIO_PIN_8 | GPIO_PIN_9);
        HAL_NVIC_DisableIRQ(CEC_CAN_IRQn);
    }
}

HAL_StatusTypeDef CAN_SetBitrate(uint32_t bitrate) {
    if(is_connected)
        return HAL_ERROR;
    uint16_t prescaler = 0;
    uint32_t delta_min = -1;
    uint8_t i = PCLK_FREQ / (bitrate * 25);
    if(PCLK_FREQ % (bitrate * 25))
        i++;
    for(; i < 25; i++) {
        uint64_t temp = (uint64_t)bitrate * i;
        uint32_t scale = PCLK_FREQ / temp;
        if(scale <= 1) {
            prescaler = 1;
            break;
        }
        if(scale <= 1024) {
            uint32_t delta = PCLK_FREQ % temp;
            if(delta == 0) {
                prescaler = scale;
                break;
            }
            else {
                if(delta < delta_min) {
                    delta_min = delta;
                    prescaler = scale;
                }
            }
        }
    }
    uint8_t bs = PCLK_FREQ / bitrate / prescaler - 1;
    bs2 = bs / 2;
    if(bs2 > 8)
        bs2 = 8;
    bs1 = bs - bs2;
    can_handle.Init.Prescaler = prescaler;
    return HAL_OK;
}

uint32_t CAN_GetBitrate() {
    return PCLK_FREQ / (can_handle.Init.Prescaler * (1 + bs1 + bs2));
}

HAL_StatusTypeDef CAN_Connect() {
    is_connected = 0;
    if(can_handle.Instance == CAN)
        HAL_CAN_DeInit(&can_handle);
    can_handle.Instance = CAN;
//    can_handle.Init.Prescaler = 8;
    can_handle.Init.Mode = CAN_MODE_NORMAL;
    can_handle.Init.SyncJumpWidth = CAN_SJW_1TQ;
    can_handle.Init.TimeSeg1 = (bs1 - 1) << CAN_BTR_TS1_Pos;
    can_handle.Init.TimeSeg2 = (bs2 - 1) << CAN_BTR_TS2_Pos;
    can_handle.Init.TimeTriggeredMode = DISABLE;
    can_handle.Init.AutoBusOff = DISABLE;
    can_handle.Init.AutoWakeUp = DISABLE;
    can_handle.Init.AutoRetransmission = DISABLE;
    can_handle.Init.ReceiveFifoLocked = DISABLE;
    can_handle.Init.TransmitFifoPriority = DISABLE;
    
    if(HAL_CAN_Init(&can_handle) != HAL_OK)
        return HAL_ERROR;
    
    CAN_FilterTypeDef canfilterconfig;

    canfilterconfig.FilterActivation = CAN_FILTER_ENABLE;
    canfilterconfig.FilterBank = 0;
    canfilterconfig.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    canfilterconfig.FilterIdHigh = 0;
    canfilterconfig.FilterIdLow = 0;
    canfilterconfig.FilterMaskIdHigh = 0;
    canfilterconfig.FilterMaskIdLow = 0;
    canfilterconfig.FilterMode = CAN_FILTERMODE_IDMASK;
    canfilterconfig.FilterScale = CAN_FILTERSCALE_32BIT;

    if(HAL_CAN_ConfigFilter(&can_handle, &canfilterconfig) != HAL_OK)
        return HAL_ERROR;
    
    if(HAL_CAN_ActivateNotification(&can_handle, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK)
	  return HAL_ERROR;
    
    if(HAL_CAN_Start(&can_handle) != HAL_OK)
        return HAL_ERROR;
    
    if(HAL_CAN_AbortTxRequest(&can_handle, CAN_TX_MAILBOX0 | CAN_TX_MAILBOX1 | CAN_TX_MAILBOX2) != HAL_OK)
        return HAL_ERROR;
    
    is_connected = 1;
    return HAL_OK;
}

HAL_StatusTypeDef CAN_IsConnected(void) {
    return (is_connected == 1) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef CAN_Transmit(CAN_MsgTypeDef *msg) {
    static CAN_TxHeaderTypeDef tx_header;
    uint32_t tx_mailbox;
    
    tx_header.IDE = msg->IDE;
    if(msg->IDE == IDE_STD)
        tx_header.StdId = msg->ID;
    else
        tx_header.ExtId = msg->ID;
    tx_header.RTR = (msg->RTR == RTR_DATA) ? CAN_RTR_DATA : CAN_RTR_REMOTE;
    tx_header.DLC = msg->DLC;
    
    return HAL_CAN_AddTxMessage(&can_handle, &tx_header, msg->Data, &tx_mailbox);
}

HAL_StatusTypeDef CAN_IsBusy(void) {
    return (HAL_CAN_GetTxMailboxesFreeLevel(&can_handle) == 3) ? HAL_OK : HAL_BUSY;
}

HAL_StatusTypeDef CAN_GetRxMsg(CAN_MsgTypeDef *msg) {
    if(is_connected && rx_msg_count) {
        HAL_NVIC_DisableIRQ(CEC_CAN_IRQn);
        uint16_t index = rx_msg_index;
        *msg = rx_msg[(index + MAX_RX_MSG_COUNT - rx_msg_count) % MAX_RX_MSG_COUNT];
        rx_msg_count--;
        HAL_NVIC_EnableIRQ(CEC_CAN_IRQn);
        return HAL_OK;
    }

    return HAL_ERROR;
}

void CAN_Disconnect() {
    if(is_connected) {
        is_connected = 0;
        HAL_CAN_Stop(&can_handle);
        HAL_CAN_DeInit(&can_handle);
        rx_msg_index = 0;
        rx_msg_count = 0;
    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef rx_header;
    CAN_MsgTypeDef *rx = &rx_msg[rx_msg_index];
    if(HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx->Data) == HAL_OK) {
        rx->ID = (rx_header.IDE == CAN_ID_STD) ? rx_header.StdId : rx_header.ExtId;
        rx->IDE = (rx_header.IDE == CAN_ID_STD) ? IDE_STD : IDE_EXT;
        rx->RTR = (rx_header.RTR == CAN_RTR_DATA) ? RTR_DATA : RTR_REMOTE;
        rx->DLC = rx_header.DLC;
        rx_msg_index = (rx_msg_index + 1) % MAX_RX_MSG_COUNT;
        if(rx_msg_count < MAX_RX_MSG_COUNT)
            rx_msg_count++;
    }
}

void CEC_CAN_IRQHandler() {
    HAL_CAN_IRQHandler(&can_handle);
}

void CAN_Init(void) {
    CAN_SetBitrate(500000);
}
