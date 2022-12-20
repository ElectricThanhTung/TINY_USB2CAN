

#ifndef __USB_DEF_H
#define __USB_DEF_H

#define USB_MAX_EP0_SIZE                                64U

#define USBD_MAX_NUM_CONFIGURATION                      1U

#define USBD_IDX_LANGID_STR                             0x00U
#define USBD_IDX_MFC_STR                                0x01U
#define USBD_IDX_PRODUCT_STR                            0x02U
#define USBD_IDX_SERIAL_STR                             0x03U
#define USBD_IDX_CONFIG_STR                             0x04U
#define USBD_IDX_INTERFACE_STR                          0x05U

/////////////////////////////////////////////////////////////

#define USB_DESC_TYPE_DEVICE                            0x01U
#define USB_DESC_TYPE_CONFIGURATION                     0x02U
#define USB_DESC_TYPE_STRING                            0x03U
#define USB_DESC_TYPE_INTERFACE                         0x04U
#define USB_DESC_TYPE_ENDPOINT                          0x05U
#define USB_DESC_TYPE_DEVICE_QUALIFIER                  0x06U
#define USB_DESC_TYPE_OTHER_SPEED_CONFIGURATION         0x07U
#define USB_DESC_TYPE_BOS                               0x0FU

/////////////////////////////////////////////////////////////

#define USBD_EP_TYPE_CTRL                               0x00U
#define USBD_EP_TYPE_ISOC                               0x01U
#define USBD_EP_TYPE_BULK                               0x02U
#define USBD_EP_TYPE_INTR                               0x03U

/////////////////////////////////////////////////////////////

#define USB_REQ_STD_TO_DEVICE                           0x00U
#define USB_REQ_STD_TO_INTERFACE                        0x01U
#define USB_REQ_STD_TO_ENDPOINT                         0x02U
#define USB_REQ_STD_FROM_DEVICE                         0x80U
#define USB_REQ_STD_FROM_INTERFACE                      0x81U
#define USB_REQ_STD_FROM_ENDPOINT                       0x82U

#define USB_REQ_CLASS_TO_DEVICE                         0x20U
#define USB_REQ_CLASS_TO_INTERFACE                      0x21U
#define USB_REQ_CLASS_TO_ENDPOINT                       0x22U
#define USB_REQ_CLASS_FROM_DEVICE                       0xA0U
#define USB_REQ_CLASS_FROM_INTERFACE                    0xA1U
#define USB_REQ_CLASS_FROM_ENDPOINT                     0xA2U

#define USB_REQ_VENDOR_TO_DEVICE                        0x40U
#define USB_REQ_VENDOR_TO_INTERFACE                     0x41U
#define USB_REQ_VENDOR_TO_ENDPOINT                      0x42U
#define USB_REQ_VENDOR_FROM_DEVICE                      0xc0U
#define USB_REQ_VENDOR_FROM_INTERFACE                   0xC1U
#define USB_REQ_VENDOR_FROM_ENDPOINT                    0xC2U

/////////////////////////////////////////////////////////////

#define USB_REQ_GET_STATUS                              0x00U
#define USB_REQ_CLEAR_FEATURE                           0x01U
#define USB_REQ_SET_FEATURE                             0x03U
#define USB_REQ_SET_ADDRESS                             0x05U
#define USB_REQ_GET_DESCRIPTOR                          0x06U
#define USB_REQ_SET_DESCRIPTOR                          0x07U
#define USB_REQ_GET_CONFIGURATION                       0x08U
#define USB_REQ_SET_CONFIGURATION                       0x09U
#define USB_REQ_GET_INTERFACE                           0x0AU
#define USB_REQ_SET_INTERFACE                           0x0BU
#define USB_REQ_SYNCH_FRAME                             0x0CU

/////////////////////////////////////////////////////////////

#define USB_CFG_MAX_BUS_POWER                           100U

/////////////////////////////////////////////////////////////

#endif // __USB_DEF_H
