
#include <Windows.h>
#include <stdio.h>
#include <conio.h>
#include "tiny.h"

static void PrintRxMsg(CAN_MsgTypeDef *msg) {
    printf("Data Received:\t%.3X  %d  { ", msg->ID, msg->DLC);
    for(int i = 0; i < msg->DLC; i++)
        printf("%.2X ", (unsigned char)msg->Data[i]);
    printf("}\r\n");
}

static void PrintErrorMsg(const char *err) {
    printf("%s\r\n", err);
    printf("Press any to close application\r\n");
    _getch();
}

int main() {
    TINY_DeviceTypeDef devices[64];
    int actual_count;
    bool first_send = 1;
    bool receive_enable = 0;

    /* Init Tiny Library */
    TINY_Init();

    /* Get all the Tiny devices connected to the computer  */
    TINY_GetDevices(devices, 64, &actual_count);

    if(actual_count == 0) {
        PrintErrorMsg("Can't find any Tiny device!");
        return 1;
    }
    else {
        printf("Press 'S' to stop transmit and receive data\r\n");
        printf("Press 'R' to enable or disable revice data\r\n");
        printf("Press 'C' to clear the screen\r\n\r\n");

        /* Open the first device found */
        int *dev = TINY_Open(&devices[0]);
        if(dev == 0) {
            PrintErrorMsg("Can't open the Tiny device!");
            return 1;
        }

        /* Make sure to disconnect before continuing */
        USB2CAN_ResultTypeDef ret = TINY_Disconnect(dev);
        if(ret != USB2CAN_OK) {
            PrintErrorMsg("Error when disconnecting!");
            return 1;
        }

        /* Set bitrate (500K) */
        ret = TINY_SetBitrate(dev, 500000);
        if(ret != USB2CAN_OK) {
            PrintErrorMsg("Can't set bitrate!");
            return 1;
        }

        /* Get actual bitrate value */
        int value;
        ret = TINY_GetBitrate(dev, &value);
        if(ret != USB2CAN_OK) {
            PrintErrorMsg("Can't get bitrate!");
            return 1;
        }
        else
            printf("Actual bitrate: %d\r\n", value);

        /* Connect to enable transmit and receive data */
        ret = TINY_Connect(dev);
        if(ret != USB2CAN_OK) {
            PrintErrorMsg("Can't connect to Tiny device!");
            return 1;
        }

        char count = 0;

        while(1) {
            /* Prepare data for transmission */
            CAN_MsgTypeDef tx_msg;
            tx_msg.ID = 0x123;
            tx_msg.DLC = 8;
            tx_msg.IDE = IDE_STD;
            tx_msg.RTR = RTR_DATA;
            for(int i = 0; i < tx_msg.DLC; i++)
                tx_msg.Data[i] = count;
            count++;

            /* Data transmission over CAN-Bus */
            if(first_send) {
                first_send = 0;
                printf("Transmitting data\r\n");
            }
            ret = TINY_TransmitData(dev, &tx_msg);
            if(ret != USB2CAN_OK) {
                PrintErrorMsg("Transmit failed!");
                return 1;
            }

            /* Wait for data to be sent */
            /*
            while(1) {
                ret = TINY_TransmitStatus(dev);
                if(ret == USB2CAN_OK)
                    break;
                else if(ret == USB2CAN_BUSY) {
                    //Sleep(1);
                    continue;
                }
                else if(ret != USB2CAN_ERROR) {
                    PrintErrorMsg("Transmit data failed!");
                    return 1;
                }
            }
            */

            /* Read data from CAN-Bus */
            if(receive_enable) {
                do {
                    CAN_MsgTypeDef rx_msg;
                    ret = TINY_ReadData(dev, &rx_msg);
                    if(ret == USB2CAN_OK && rx_msg.RTR == RTR_DATA)
                        PrintRxMsg(&rx_msg);
                    else {
                        /* Do Nothing */
                    }
                } while(ret == USB2CAN_OK);
            }

            /* User control */
            if(_kbhit()) {
                char c = _getch();
                if(c == 's' || c == 'S') {
                    /* Stop transmit and receive */
                    TINY_Disconnect(dev);
                    TINY_Close(dev);
                    return 0;
                }
                else if(c == 'r' || c == 'R') {
                    /* Enable receive and show received data to the screen */
                    receive_enable = !receive_enable;
                    if(receive_enable)
                        printf("Enable receive data\r\n");
                    else
                        printf("Disable receive data\r\n");
                }
                else if(c == 'c' || c == 'C') {
                    /* Clear screen */
                    system("cls");
                    printf("Press 'S' to stop transmit and receive data\r\n");
                    printf("Press 'R' to enable or disable revice data\r\n");
                    printf("Press 'C' to clear the screen\r\n\r\n");
                    printf("Transmitting data\r\n");
                }
            }

            Sleep(100);
        }
    }
}
