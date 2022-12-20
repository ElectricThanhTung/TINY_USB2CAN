
#ifndef __USB_DEVICE_H
#define __USB_DEVICE_H

#include "main.h"

PCD_HandleTypeDef *USBD_Init(void);
void USBD_IRQHandler(void);
void USBD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void USBD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum);
void USBD_SuspendCallback(PCD_HandleTypeDef *hpcd);

#endif // __USB_DEVICE_H
