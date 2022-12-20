#pragma once

extern "C" {
    typedef struct {
        int *usb_device;
        char name[128];
    } TINY_DeviceTypeDef;

    typedef enum : char {
        IDE_STD = 0,
        IDE_EXT = 1,
    } CAN_IDETypeDef;

    typedef enum : char {
        RTR_DATA = 0,
        RTR_REMOTE = 1,
    } CAN_RTRTypeDef;

    typedef struct {
        int ID;
        unsigned char DLC;
        CAN_IDETypeDef IDE;
        CAN_RTRTypeDef RTR;
        char Data[8];
    } CAN_MsgTypeDef;

    typedef enum : unsigned char {
        USB2CAN_CONNECT = 0,
        USB2CAN_SET_BITRATE,
        USB2CAN_READ_BITRATE,
        USB2CAN_READ_DATA,
        USB2CAN_TRANSMIT,
        USB2CAN_TRANSMIT_STATUS,
        USB2CAN_DISCONECT,
        UNKNOWN_CMD = 0xFF,
    } USB2CAN_CmdTypeDef;

    typedef enum : unsigned char {
        USB2CAN_OK = 0,
        USB2CAN_BUSY,
        USB2CAN_ERROR,
        USB2CAN_NOT_SUPPORTED,
        USB2CAN_SEND_FAIL,
        USB2CAN_NO_RESPONSE,
    } USB2CAN_ResultTypeDef;

    typedef struct {
        USB2CAN_CmdTypeDef Cmd;
        USB2CAN_ResultTypeDef Result;
        short Length;
        char Param[1];
    } USB2CAN_CmdMsgTypeDef;

    __declspec(dllexport) void TINY_Init(void);
    __declspec(dllexport) void TINY_GetDevices(TINY_DeviceTypeDef *channels, int channel_count, int *actual_count);
    __declspec(dllexport) int *TINY_Open(TINY_DeviceTypeDef *tiny_device);
    __declspec(dllexport) USB2CAN_ResultTypeDef TINY_Connect(int *dev);
    __declspec(dllexport) USB2CAN_ResultTypeDef TINY_SetBitrate(int *dev, int value);
    __declspec(dllexport) USB2CAN_ResultTypeDef TINY_GetBitrate(int *dev, int *value);
    __declspec(dllexport) USB2CAN_ResultTypeDef TINY_TransmitData(int *dev, CAN_MsgTypeDef *msg);
    __declspec(dllexport) USB2CAN_ResultTypeDef TINY_TransmitStatus(int *dev);
    __declspec(dllexport) USB2CAN_ResultTypeDef TINY_ReadData(int *dev, CAN_MsgTypeDef *msgs);
    __declspec(dllexport) USB2CAN_ResultTypeDef TINY_Disconnect(int *dev);
    __declspec(dllexport) void TINY_Close(int *dev);
}
