
#include "can.h"
#include <string.h>
#include "usb_types.h"
#include "usb_device.h"
#include "usb_to_can.h"

typedef enum {
    USB2CAN_CONNECT = 0,
    USB2CAN_SET_BITRATE,
    USB2CAN_READ_BITRATE,
    USB2CAN_READ_DATA,
    USB2CAN_TRANSMIT,
    USB2CAN_TRANSMIT_STATUS,
    USB2CAN_DISCONECT,
    UNKNOWN_CMD = 0xFF,
} USB2CAN_CmdTypeDef;

typedef enum {
    USB2CAN_OK = 0,
    USB2CAN_BUSY,
    USB2CAN_ERROR,
    USB2CAN_NOT_SUPPORTED,
    USB2CAN_SEND_FAIL,
    USB2CAN_NO_RESPONSE,
} USB2CAN_ResultTypeDef;

typedef struct {
    USB2CAN_CmdTypeDef Cmd;
    USB2CAN_ResultTypeDef Result;
    uint16_t Length;
    uint8_t Param[];
} USB2CAN_CmdMsgTypeDef;

static uint32_t usb_rx_cmd[64 / sizeof(uint32_t)];
static uint32_t usb_tx_cmd[64 / sizeof(uint32_t)];

static void GPIO_Init(void) {

}

static HAL_StatusTypeDef USB2CAN_SendResult(PCD_HandleTypeDef *hpcd, USB2CAN_CmdTypeDef cmd, USB2CAN_ResultTypeDef result) {
    USB2CAN_CmdMsgTypeDef *tx_cmd = (USB2CAN_CmdMsgTypeDef *)usb_tx_cmd;
    tx_cmd->Cmd = cmd;
    tx_cmd->Result = result;
    tx_cmd->Length = 0;
    HAL_PCD_EP_Transmit(hpcd, 1, (uint8_t *)tx_cmd, 4 + tx_cmd->Length);
    return HAL_OK;
}

static HAL_StatusTypeDef USB2CAN_SendData(PCD_HandleTypeDef *hpcd, USB2CAN_CmdTypeDef cmd, uint8_t *data, uint16_t length) {
    USB2CAN_CmdMsgTypeDef *tx_cmd = (USB2CAN_CmdMsgTypeDef *)usb_tx_cmd;
    tx_cmd->Cmd = cmd;
    tx_cmd->Result = USB2CAN_OK;
    tx_cmd->Length = length;
    
    memcpy(tx_cmd->Param, data, length);
    
    HAL_PCD_EP_Transmit(hpcd, 1, (uint8_t *)tx_cmd, 4 + tx_cmd->Length);
    return HAL_OK;
}

static void USB2CAN_CmdHandler(PCD_HandleTypeDef *hpcd) {
    USB2CAN_CmdMsgTypeDef *rx_cmd = (USB2CAN_CmdMsgTypeDef *)usb_rx_cmd;
    switch(rx_cmd->Cmd) {
        case USB2CAN_CONNECT:
            USB2CAN_SendResult(hpcd, USB2CAN_CONNECT, (CAN_Connect() == HAL_OK) ? USB2CAN_OK : USB2CAN_ERROR);
            break;
        case USB2CAN_SET_BITRATE: {
            uint32_t bitrate = rx_cmd->Param[0];
            bitrate |= rx_cmd->Param[1] << 8;
            bitrate |= rx_cmd->Param[2] << 16;
            bitrate |= rx_cmd->Param[3] << 24;
            USB2CAN_SendResult(hpcd, USB2CAN_SET_BITRATE, (CAN_SetBitrate(bitrate) == HAL_OK) ? USB2CAN_OK : USB2CAN_ERROR);
            break;
        }
        case USB2CAN_READ_BITRATE: {
            uint32_t bitrate = CAN_GetBitrate();
            USB2CAN_SendData(hpcd, USB2CAN_READ_BITRATE, (uint8_t *)&bitrate, sizeof(bitrate));
            break;
        }
        case USB2CAN_READ_DATA: {
            CAN_MsgTypeDef msg;
            if(CAN_GetRxMsg(&msg) == HAL_OK)
                USB2CAN_SendData(hpcd, USB2CAN_READ_DATA, (uint8_t *)&msg, sizeof(msg));
            else
                USB2CAN_SendResult(hpcd, USB2CAN_READ_DATA, USB2CAN_ERROR);
            break;
        }
        case USB2CAN_TRANSMIT: {
            static CAN_MsgTypeDef can_msg;
            uint32_t start_tick = HAL_GetTick();
            while(CAN_IsBusy()) {
                if(__HAL_PCD_GET_FLAG(hpcd, USB_ISTR_SUSP))
                    return;
                else if((uint32_t)(HAL_GetTick() - start_tick) >= 100) {
                    USB2CAN_SendResult(hpcd, USB2CAN_TRANSMIT, USB2CAN_ERROR);
                    return;
                }
            }
            memcpy(&can_msg, rx_cmd->Param, sizeof(CAN_MsgTypeDef));
            USB2CAN_SendResult(hpcd, USB2CAN_TRANSMIT, CAN_Transmit(&can_msg) == HAL_OK ? USB2CAN_OK : USB2CAN_ERROR);
            break;
        }
        case USB2CAN_TRANSMIT_STATUS:
            USB2CAN_SendResult(hpcd, USB2CAN_TRANSMIT_STATUS, (CAN_IsBusy() == HAL_OK) ? USB2CAN_OK : USB2CAN_BUSY);
            break;
        case USB2CAN_DISCONECT:
            CAN_Disconnect();
            USB2CAN_SendResult(hpcd, USB2CAN_DISCONECT, USB2CAN_OK);
            break;
        default:
            USB2CAN_SendResult(hpcd, UNKNOWN_CMD, USB2CAN_NOT_SUPPORTED);
            return;
    }
}

void USBD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
    USB2CAN_CmdHandler(hpcd);
    HAL_PCD_EP_Receive(hpcd, 1, (uint8_t *)usb_rx_cmd, sizeof(usb_rx_cmd));
}

void USBD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
    HAL_PCD_EP_Receive(hpcd, 1, (uint8_t *)usb_rx_cmd, sizeof(usb_rx_cmd));
}

void USBD_SuspendCallback(PCD_HandleTypeDef *hpcd) {
    CAN_Disconnect();
    HAL_PCD_EP_Receive(hpcd, 1, (uint8_t *)usb_rx_cmd, sizeof(usb_rx_cmd));
}

void USB2CAN_Init(void) {
    GPIO_Init();
    CAN_Init();
    HAL_PCD_EP_Receive(USBD_Init(), 1, (uint8_t *)&usb_rx_cmd, sizeof(usb_rx_cmd));
}
