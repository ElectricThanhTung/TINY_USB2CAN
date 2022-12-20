
#ifndef __USB_CTRL_REQ_H
#define __USB_CTRL_REQ_H

#include "main.h"

void USBD_CtrlSetupCallback(PCD_HandleTypeDef *hpcd);
void USBD_CtrlDataOutStageCallback(PCD_HandleTypeDef *hpcd);
void USBD_CtrlDataInStageCallback(PCD_HandleTypeDef *hpcd);
void USBD_CtrlSuspendCallback(PCD_HandleTypeDef *hpcd);

#endif // __USB_CTRL_REQ_H
