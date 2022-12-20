
#include "usb_def.h"
#include "usb_ctrl_req.h"
#include "usb_device.h"

static PCD_HandleTypeDef hpcd_USB_FS;

static HAL_StatusTypeDef PCD_Init() {
    hpcd_USB_FS.Instance = USB;
    hpcd_USB_FS.Init.dev_endpoints = 8;
    hpcd_USB_FS.Init.speed = PCD_SPEED_FULL;
    hpcd_USB_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
    hpcd_USB_FS.Init.low_power_enable = DISABLE;
    hpcd_USB_FS.Init.lpm_enable = DISABLE;
    hpcd_USB_FS.Init.battery_charging_enable = DISABLE;
    if(HAL_PCD_Init(&hpcd_USB_FS) != HAL_OK)
        return HAL_ERROR;

    HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x00, PCD_SNG_BUF, 0x18);
    HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x80, PCD_SNG_BUF, 0x58);

    HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x01, PCD_SNG_BUF, 0x98);
    HAL_PCDEx_PMAConfig(&hpcd_USB_FS, 0x81, PCD_SNG_BUF, 0xD8);
    
    return HAL_OK;
}

static HAL_StatusTypeDef PCD_Start() {
    return HAL_PCD_Start(&hpcd_USB_FS);
}

void HAL_PCD_MspInit(PCD_HandleTypeDef *pcdHandle) {
    if(pcdHandle->Instance == USB) {
        __HAL_RCC_USB_CLK_ENABLE();

        HAL_NVIC_SetPriority(USB_IRQn, 1, 0);
        HAL_NVIC_EnableIRQ(USB_IRQn);
    }
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef *pcdHandle) {
    if(pcdHandle->Instance == USB) {
        __HAL_RCC_USB_CLK_DISABLE();

        HAL_NVIC_DisableIRQ(USB_IRQn);
    }
}

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd) {
    USBD_CtrlSetupCallback(hpcd);
}

__weak void USBD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
    
}

__weak void USBD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
    
}

__weak void USBD_SuspendCallback(PCD_HandleTypeDef *hpcd) {
    
}

void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
    if(epnum == 0)
        USBD_CtrlDataOutStageCallback(hpcd);
    else
        USBD_DataOutStageCallback(hpcd, epnum);
}

void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
    if(epnum == 0)
        USBD_CtrlDataInStageCallback(hpcd);
    else
        USBD_DataInStageCallback(hpcd, epnum);
}

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd) {
    
}

void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd) {
    HAL_PCD_EP_Open(hpcd, 0x00, USB_MAX_EP0_SIZE, USBD_EP_TYPE_CTRL);
    HAL_PCD_EP_Open(hpcd, 0x80, USB_MAX_EP0_SIZE, USBD_EP_TYPE_CTRL);
    HAL_PCD_EP_Open(hpcd, 0x01, 64, USBD_EP_TYPE_BULK);
    HAL_PCD_EP_Open(hpcd, 0x81, 64, USBD_EP_TYPE_BULK);
}

void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd) {
    USBD_CtrlSuspendCallback(hpcd);
    USBD_SuspendCallback(hpcd);
}

void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd) {
    
}

void USB_IRQHandler(void) {
    HAL_PCD_IRQHandler(&hpcd_USB_FS);
}

PCD_HandleTypeDef *USBD_Init(void) {
    if(PCD_Init() != HAL_OK)
        Error_Handler();
    
    if(PCD_Start() != HAL_OK)
        Error_Handler();
    
    return &hpcd_USB_FS;
}
