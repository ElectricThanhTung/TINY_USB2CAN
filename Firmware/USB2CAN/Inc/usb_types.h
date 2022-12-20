
#ifndef __USB_TYPES_H
#define __USB_TYPES_H

#include <string.h>

typedef struct {
    const uint8_t *Data;
    uint32_t Length;
    volatile uint32_t RemLength;
    const uint32_t MaxPacketSize;
} USBD_EpTxDataTypeDef;

#endif // __USB_TYPES_H
