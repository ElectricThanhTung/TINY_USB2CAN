
#include "usb_def.h"
#include "usb_desc.h"

#define LOBYTE(x)                               ((uint8_t)((x) & 0x00FFU))
#define HIBYTE(x)                               ((uint8_t)(((x) & 0xFF00U) >> 8U))

#define MAX(a, b)                               ((a > b) ? a : b)

#define MANUFACTURER_STRING_LENGTH              (sizeof(USBD_MANUFACTURER_STRING) - 1)
#define PRODUCT_STRING_LENGTH                   (sizeof(USBD_PRODUCT_STRING) - 1)
#define SERIAL_STRING_LENGTH                    (sizeof(USBD_SERIAL_STRING) - 1)
#define CONFIGURATION_STRING_LENGTH             (sizeof(USBD_CONFIGURATION_STRING) - 1)
#define INTERFACE_STRING_LENGTH                 (sizeof(USBD_INTERFACE_STRING) - 1)

#define STRING_DESC_MAX_LENGHT                  MAX(MAX(MAX(MANUFACTURER_STRING_LENGTH, PRODUCT_STRING_LENGTH), MAX(SERIAL_STRING_LENGTH, CONFIGURATION_STRING_LENGTH)), MAX(INTERFACE_STRING_LENGTH, 2))

static uint8_t string_desc_buff[STRING_DESC_MAX_LENGHT * 2 + 2];

static const uint8_t USBD_DeviceDesc[] = {
    0x12,                                       // bLength
    USB_DESC_TYPE_DEVICE,                       // bDescriptorType
    0x00,                                       // bcdUSB
    0x02,
    0x00,                                       // bDeviceClass
    0x00,                                       // bDeviceSubClass
    0x00,                                       // bDeviceProtocol
    USB_MAX_EP0_SIZE,                           // bMaxPacketSize
    LOBYTE(USBD_VID),                           // idVendor
    HIBYTE(USBD_VID),                           // idVendor
    LOBYTE(USBD_PID),                           // idProduct
    HIBYTE(USBD_PID),                           // idProduct
    0x00,                                       // bcdDevice rel. 2.00
    0x01,
    0x01,                                       // Index of manufacturer string
    0x02,                                       // Index of product string
    0x03,                                       // Index of serial number string
    USBD_MAX_NUM_CONFIGURATION                  // bNumConfigurations
};

static const uint8_t USBD_ConfigDesc[] = {
    0x09,                                       // Size of Descriptor in Bytes
    USB_DESC_TYPE_CONFIGURATION,                // Configuration Descriptor (0x02)
    32, 0,                                      // Total length in bytes of data returned
    0x01,                                       // Number of Interfaces
    0x01,                                       // Value to use as an argument to select this configuration
    0x04,                                       // Index of String Descriptor describing this configuration
    0x80,                                       // Bus powered
    USB_CFG_MAX_BUS_POWER / 2,                  // Maximum Power Consumption in 2mA units

    /************** Descriptor of interface ****************/
    0x09,                                       // Size of Descriptor in Bytes (9 Bytes)
    USB_DESC_TYPE_INTERFACE,                    // Interface Descriptor (0x04)
    0x00,                                       // Number of Interface
    0x00,                                       // Value used to select alternative setting
    0x02,                                       // Number of Endpoints used for this interface
    0xFF,                                       // Class Code
    0xFF,                                       // Subclass Code
    0xFF,                                       // Protocol Code
    0x05,                                       // Index of String Descriptor Describing this interface
    
    /************** Endpoint descriptor ****************/

    0x07,                                       // Size of Descriptor in Bytes (7 Bytes)
    USB_DESC_TYPE_ENDPOINT,                     // Endpoint descriptor (0x05)
    0x01,                                       // OUT endpoint number 1 (0x01)
    USBD_EP_TYPE_BULK,                          // attrib: Interrupt endpoint
    0x40,
    0x00,                                       // maximum packet size
    0x00,                                       // POLL INTERVAL (ms)

    0x07,                                       // Size of Descriptor in Bytes (7 Bytes)
    USB_DESC_TYPE_ENDPOINT,                     // Endpoint descriptor (0x05)
    0x81,                                       // IN endpoint number 1 (0x81)
    USBD_EP_TYPE_BULK,                          // attrib: Interrupt endpoint
    0x40,
    0x00,                                       // maximum packet size
    0x00,                                       // POLL INTERVAL (ms)
};

uint8_t *USBD_GetDeviceDesc(uint16_t *length) {
    *length = sizeof(USBD_DeviceDesc);
    return (uint8_t *)USBD_DeviceDesc;
}

uint8_t *USBD_GetConfiguration(uint16_t *length) {
    *length = sizeof(USBD_ConfigDesc);
    return (uint8_t *)USBD_ConfigDesc;
}

static uint8_t *USBD_GetStringLanguageDesc(uint16_t *length) {
    static const uint8_t USBD_StringLanguageDesc[] = {
        0x04,
        USB_DESC_TYPE_STRING,
        LOBYTE(USBD_LANGID_STRING),
        HIBYTE(USBD_LANGID_STRING),
    };
    *length = USBD_StringLanguageDesc[0];
    return (uint8_t *)USBD_StringLanguageDesc;
}

static uint8_t *USBD_GetManufacturerString(uint16_t *length) {
    string_desc_buff[0] = MANUFACTURER_STRING_LENGTH * 2 + 2;
    string_desc_buff[1] = USB_DESC_TYPE_STRING;
    for(uint8_t i = 0; i < MANUFACTURER_STRING_LENGTH; i++) {
        string_desc_buff[i * 2 + 2] = USBD_MANUFACTURER_STRING[i];
        string_desc_buff[i * 2 + 3] = 0;
    }
    *length = string_desc_buff[0];
    return string_desc_buff;
};

static uint8_t *USBD_GetProductString(uint16_t *length) {
    string_desc_buff[0] = PRODUCT_STRING_LENGTH * 2 + 2;
    string_desc_buff[1] = USB_DESC_TYPE_STRING;
    for(uint8_t i = 0; i < PRODUCT_STRING_LENGTH; i++) {
        string_desc_buff[i * 2 + 2] = USBD_PRODUCT_STRING[i];
        string_desc_buff[i * 2 + 3] = 0;
    }
    *length = string_desc_buff[0];
    return string_desc_buff;
};

static uint8_t *USBD_GetSerialNumberString(uint16_t *length) {
    string_desc_buff[0] = SERIAL_STRING_LENGTH * 2 + 2;
    string_desc_buff[1] = USB_DESC_TYPE_STRING;
    for(uint8_t i = 0; i < SERIAL_STRING_LENGTH; i++) {
        string_desc_buff[i * 2 + 2] = USBD_SERIAL_STRING[i];
        string_desc_buff[i * 2 + 3] = 0;
    }
    *length = string_desc_buff[0];
    return string_desc_buff;
};

static uint8_t *USBD_GetConfigurationString(uint16_t *length) {
    string_desc_buff[0] = CONFIGURATION_STRING_LENGTH * 2 + 2;
    string_desc_buff[1] = USB_DESC_TYPE_STRING;
    for(uint8_t i = 0; i < CONFIGURATION_STRING_LENGTH; i++) {
        string_desc_buff[i * 2 + 2] = USBD_CONFIGURATION_STRING[i];
        string_desc_buff[i * 2 + 3] = 0;
    }
    *length = string_desc_buff[0];
    return string_desc_buff;
};

static uint8_t *USBD_GetInterfaceString(uint16_t *length) {
    string_desc_buff[0] = INTERFACE_STRING_LENGTH * 2 + 2;
    string_desc_buff[1] = USB_DESC_TYPE_STRING;
    for(uint8_t i = 0; i < INTERFACE_STRING_LENGTH; i++) {
        string_desc_buff[i * 2 + 2] = USBD_INTERFACE_STRING[i];
        string_desc_buff[i * 2 + 3] = 0;
    }
    *length = string_desc_buff[0];
    return string_desc_buff;
};

uint8_t *USBD_GetStringDesc(uint8_t index, uint16_t *length) {
    static uint8_t *(* const desc_list[])(uint16_t *) = {
        USBD_GetStringLanguageDesc,
        USBD_GetManufacturerString,
        USBD_GetProductString,
        USBD_GetSerialNumberString,
        USBD_GetConfigurationString,
        USBD_GetInterfaceString,
    };
    if(index >= (sizeof(desc_list) / sizeof(desc_list[0]))) {
        *length = 0;
        return NULL;
    }
    return desc_list[index](length);
}
