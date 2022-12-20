
#include "usb_def.h"
#include "usb_desc.h"
#include "usb_types.h"
#include "usb_ctrl_req.h"

static USBD_EpTxDataTypeDef ep0_tx = {NULL, 0, 0, 64};

static HAL_StatusTypeDef USBD_CtlContinueSendData(PCD_HandleTypeDef *hpcd) {
    uint32_t packet_size;
    uint8_t *data = (uint8_t *)&ep0_tx.Data[ep0_tx.Length - ep0_tx.RemLength];
    if(ep0_tx.RemLength > ep0_tx.MaxPacketSize) {
        ep0_tx.RemLength -= ep0_tx.MaxPacketSize;
        packet_size = ep0_tx.MaxPacketSize;
    }
    else {
        packet_size = ep0_tx.RemLength;
        ep0_tx.RemLength = 0;
    }
    HAL_StatusTypeDef ret = HAL_PCD_EP_Transmit(hpcd, 0x00, data, packet_size);
    if(ep0_tx.RemLength == 0)
        HAL_PCD_EP_Receive(hpcd, 0x00, NULL, 0);
    return ret;
}

static HAL_StatusTypeDef USBD_CtrlSendData(PCD_HandleTypeDef *hpcd, const uint8_t *data, uint32_t length) {
    if(ep0_tx.RemLength == 0) {
        ep0_tx.Data = data;
        ep0_tx.Length = length;
        ep0_tx.RemLength = length;
        return USBD_CtlContinueSendData(hpcd);
    }
    return HAL_BUSY;
}

static HAL_StatusTypeDef USBD_CtrlStall(PCD_HandleTypeDef *hpcd) {
    HAL_StatusTypeDef ret = HAL_PCD_EP_SetStall(hpcd, 0x00);
    if(ret != HAL_OK)
        return ret;
    return HAL_PCD_EP_SetStall(hpcd, 0x80);
}

static void USBD_GetDescriptor(PCD_HandleTypeDef *hpcd) {
    uint8_t *rx_buff = (uint8_t *)hpcd->Setup;
    uint16_t length;
    uint8_t *desc;
    switch(rx_buff[3]) {
        case USB_DESC_TYPE_DEVICE: {
            desc = USBD_GetDeviceDesc(&length);
            break;
        }
        case USB_DESC_TYPE_CONFIGURATION:
            desc = USBD_GetConfiguration(&length);
            break;
        case USB_DESC_TYPE_STRING:
            desc = USBD_GetStringDesc(rx_buff[2], &length);
            break;
        default:
            USBD_CtrlStall(hpcd);
            return;
    }
    uint16_t wLength = rx_buff[7] << 8 | rx_buff[6];
    if(wLength < length)
        length = wLength;
    USBD_CtrlSendData(hpcd, desc, length);
}

void USBD_CtrlSetupCallback(PCD_HandleTypeDef *hpcd) {
    uint8_t *rx_buff = (uint8_t *)hpcd->Setup;
    switch(rx_buff[0]) {
        case USB_REQ_STD_FROM_DEVICE:
            switch(rx_buff[1]) {
                case USB_REQ_GET_DESCRIPTOR:
                    USBD_GetDescriptor(hpcd);
                    break;
                case USB_REQ_GET_STATUS:
                    USBD_CtrlStall(hpcd);
                    break;
                default:
                    break;
            }
            break;
        case USB_REQ_STD_TO_DEVICE:
            switch(rx_buff[1]) {
                case USB_REQ_SET_ADDRESS:
                    HAL_PCD_SetAddress(hpcd, rx_buff[2]);
                    HAL_PCD_EP_Transmit(hpcd, 0x00, NULL, 0);
                    break;
                case USB_REQ_SET_CONFIGURATION:
                    HAL_PCD_EP_Transmit(hpcd, 0x00, NULL, 0);
                    break;
                default:
                    break;
            }
            break;
        case USB_REQ_STD_FROM_INTERFACE:
            break;
        case USB_REQ_CLASS_TO_INTERFACE:
            USBD_CtrlStall(hpcd);
            break;
        case USB_REQ_CLASS_FROM_INTERFACE:
            break;
    }
}

void USBD_CtrlDataOutStageCallback(PCD_HandleTypeDef *hpcd) {
    HAL_PCD_EP_Transmit(hpcd, 0x00, NULL, 0);
}

void USBD_CtrlDataInStageCallback(PCD_HandleTypeDef *hpcd) {
    if(ep0_tx.RemLength)
        USBD_CtlContinueSendData(hpcd);
}

void USBD_CtrlSuspendCallback(PCD_HandleTypeDef *hpcd) {
    ep0_tx.RemLength = 0;
    HAL_PCD_EP_Receive(hpcd, 0x00, NULL, 0);
}
