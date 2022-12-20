
#ifndef __CAN_H
#define __CAN_H

#include "main.h"

#define MAX_RX_MSG_COUNT         768

typedef enum {
    IDE_STD = 0,
    IDE_EXT = 1,
} CAN_IDETypeDef;

typedef enum {
    RTR_DATA = 0,
    RTR_REMOTE = 1,
} CAN_RTRTypeDef;

typedef struct {
    uint32_t ID;
    uint32_t DLC : 8;
    CAN_IDETypeDef IDE;
    CAN_RTRTypeDef RTR;
    uint8_t Data[8];
} CAN_MsgTypeDef;

HAL_StatusTypeDef CAN_Connect(void);
HAL_StatusTypeDef CAN_SetBitrate(uint32_t bitrate);
uint32_t CAN_GetBitrate(void);
HAL_StatusTypeDef CAN_IsConnected(void);
HAL_StatusTypeDef CAN_Transmit(CAN_MsgTypeDef *msg);
HAL_StatusTypeDef CAN_IsBusy(void);
HAL_StatusTypeDef CAN_GetRxMsg(CAN_MsgTypeDef *msg);
void CAN_Disconnect(void);
void CAN_Init(void);

#endif // __CAN_H
