
#ifndef __USB_DESC_H
#define __USB_DESC_H

#include "main.h"

#define USBD_VID                        0x1234
#define USBD_PID                        0x5678
#define USBD_LANGID_STRING              0x0409

#define USBD_MANUFACTURER_STRING        "Thanh Tung"
#define USBD_PRODUCT_STRING             "Tiny USB-to-CAN"
#define USBD_SERIAL_STRING              "0123-4567-89AB-CDEF"
#define USBD_CONFIGURATION_STRING       "Tiny USB-to-CAN config"
#define USBD_INTERFACE_STRING           "Tiny USB-to-CAN interface"

uint8_t *USBD_GetDeviceDesc(uint16_t *length);
uint8_t *USBD_GetConfiguration(uint16_t *length);
uint8_t *USBD_GetStringDesc(uint8_t index, uint16_t *length);

#endif // __USB_DESC_H
