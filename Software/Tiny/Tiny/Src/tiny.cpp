
#include "lusb0_usb.h"
#include "tiny.h"
#include <stdio.h>

#define TINY_USB2CAN_VIP    0x1234
#define TINY_USB2CAN_PID    0x5678

static void TINY_UnicodeToASCII(char *ascii, char *unicode) {
    short *temp = (short *)unicode;
    while(*temp) {
        *ascii = (char)*temp;
        ascii++;
        temp++;
    }
    *ascii = 0;
}

static USB2CAN_ResultTypeDef TINY_SendCommand(int *dev, USB2CAN_CmdMsgTypeDef *cmd) {
    cmd->Result = USB2CAN_OK;
    if(usb_bulk_write((usb_dev_handle *)dev, 1, (char *)cmd, 4 + cmd->Length, 1000) < 0)
        return USB2CAN_SEND_FAIL;
    USB2CAN_CmdMsgTypeDef result;
    if(usb_bulk_read((usb_dev_handle *)dev, 0x81, (char *)&result, 4, 1000) < 0)
        return USB2CAN_NO_RESPONSE;
    if(cmd->Cmd != result.Cmd)
        return USB2CAN_NO_RESPONSE;
    return result.Result;
}

static USB2CAN_ResultTypeDef TINY_SendCommand(int *dev, USB2CAN_CmdMsgTypeDef *cmd, char *out_value, int length) {
    cmd->Result = USB2CAN_OK;
    if(usb_bulk_write((usb_dev_handle *)dev, 1, (char *)cmd, 4 + cmd->Length, 1000) < 0)
        return USB2CAN_SEND_FAIL;
    char buff[64];
    USB2CAN_CmdMsgTypeDef *result = (USB2CAN_CmdMsgTypeDef *)buff;
    if(usb_bulk_read((usb_dev_handle *)dev, 0x81, (char *)result, sizeof(buff), 1000) < 0)
        return USB2CAN_NO_RESPONSE;
    if(cmd->Cmd != result->Cmd)
        return USB2CAN_NO_RESPONSE;
    if(result->Result == USB2CAN_OK) {
        for(int i = 0; i < length; i++)
            out_value[i] = result->Param[i];
    }
    return result->Result;
}

void TINY_Init(void) {
    usb_init();
}

void TINY_GetDevices(TINY_DeviceTypeDef *channels, int channel_count, int *actual_count) {
    struct usb_bus *bus;
    struct usb_device *dev;
    char str_buff[255];
    int count = 0;

    usb_find_busses();
    usb_find_devices();

    for(bus = usb_get_busses(); bus; bus = bus->next) {
        for(dev = bus->devices; dev; dev = dev->next) {
            if(dev->descriptor.idVendor == TINY_USB2CAN_VIP && dev->descriptor.idProduct == TINY_USB2CAN_PID) {
                if(count < channel_count) {
                    usb_dev_handle *usb_handle = usb_open(dev);
                    memset(str_buff, 0, sizeof(str_buff));
                    usb_get_string(usb_handle, dev->descriptor.iProduct, 0x0409, str_buff, sizeof(str_buff));
                    usb_close(usb_handle);
                    TINY_UnicodeToASCII(channels[count].name, &str_buff[2]);
                    channels[count].usb_device = (int *)dev;
                    if(++count == channel_count) {
                        *actual_count = count;
                        return;
                    }
                }
            }
        }
    }
    *actual_count = count;
}

int *TINY_Open(TINY_DeviceTypeDef *tiny_device) {
    usb_dev_handle *dev = usb_open((struct usb_device *)tiny_device->usb_device);
    if(dev == 0)
        return NULL;
    if(usb_set_configuration(dev, 1) < 0) {
        usb_close(dev);
        return NULL;
    }
    if(usb_claim_interface(dev, 0) < 0) {
        usb_close(dev);
        return NULL;
    }
    return (int *)dev;
}

USB2CAN_ResultTypeDef TINY_Connect(int *dev) {
    USB2CAN_CmdMsgTypeDef cmd;
    cmd.Cmd = USB2CAN_CONNECT;
    cmd.Length = 0;
    return TINY_SendCommand(dev, &cmd);
}

USB2CAN_ResultTypeDef TINY_SetBitrate(int *dev, int value) {
    char buff[4 + 4];
    USB2CAN_CmdMsgTypeDef *cmd = (USB2CAN_CmdMsgTypeDef *)buff;
    cmd->Cmd = USB2CAN_SET_BITRATE;
    cmd->Length = 4;
    *(int *)cmd->Param = value;
    return TINY_SendCommand(dev, cmd);
}

USB2CAN_ResultTypeDef TINY_GetBitrate(int *dev, int *value) {
    USB2CAN_CmdMsgTypeDef cmd;
    cmd.Cmd = USB2CAN_READ_BITRATE;
    cmd.Length = 0;
    return TINY_SendCommand(dev, &cmd, (char *)value, 4);
}

USB2CAN_ResultTypeDef TINY_TransmitData(int *dev, CAN_MsgTypeDef *msg) {
    if(msg == NULL)
        return USB2CAN_ERROR;
    char buff[4 + 64];
    USB2CAN_CmdMsgTypeDef *cmd = (USB2CAN_CmdMsgTypeDef *)buff;
    cmd->Cmd = USB2CAN_TRANSMIT;
    cmd->Length = sizeof(CAN_MsgTypeDef) - (8 - msg->DLC);
    memcpy(cmd->Param, msg, sizeof(CAN_MsgTypeDef));
    return TINY_SendCommand(dev, cmd);
}

USB2CAN_ResultTypeDef TINY_TransmitStatus(int *dev) {
    USB2CAN_CmdMsgTypeDef cmd;
    cmd.Cmd = USB2CAN_TRANSMIT_STATUS;
    cmd.Length = 0;
    return TINY_SendCommand(dev, &cmd);
}

USB2CAN_ResultTypeDef TINY_ReadData(int *dev, CAN_MsgTypeDef *msgs) {
    USB2CAN_CmdMsgTypeDef cmd;
    cmd.Cmd = USB2CAN_READ_DATA;
    cmd.Length = 0;
    return TINY_SendCommand(dev, &cmd, (char *)msgs, sizeof(CAN_MsgTypeDef));
}

USB2CAN_ResultTypeDef TINY_Disconnect(int *dev) {
    USB2CAN_CmdMsgTypeDef cmd;
    cmd.Cmd = USB2CAN_DISCONECT;
    cmd.Length = 0;
    return TINY_SendCommand(dev, &cmd);
}

void TINY_Close(int *dev) {
    usb_close((usb_dev_handle *)dev);
}
