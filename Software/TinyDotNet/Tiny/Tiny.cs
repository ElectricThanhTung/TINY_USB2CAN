using System;
using System.Collections.Generic;
using System.Text;
using static TinyDotNet.TinyCore;

namespace TinyDotNet {
    public class Tiny {
        private IntPtr tinyHandle;

        static Tiny() {
            TinyCore.TINY_Init();
        }

        public Tiny() {
            tinyHandle = IntPtr.Zero;
        }

        private static string GetString(byte[] bytes) {
            StringBuilder stringBuilder = new StringBuilder();
            int i = 0;
            while(bytes[i] != 0) {
                stringBuilder.Append((char)bytes[i]);
                i++;
            }
            return stringBuilder.ToString();
        }

        public static List<TinyDevice> GetDevices() {
            TINY_DeviceTypeDef[] tinyDevices = new TINY_DeviceTypeDef[64];
            int count;
            TinyCore.TINY_GetDevices(ref tinyDevices[0], tinyDevices.Length, out count);
            if(count == 0)
                return null;
            List<TinyDevice> devices = new List<TinyDevice>();
            for(int i = 0; i < count; i++) {
                TinyDevice device = new TinyDevice();
                device.USBDevice = tinyDevices[i].usb_device;
                device.Name = GetString(tinyDevices[i].name);
                devices.Add(device);
            }
            return devices;
        }

        public bool Open(TinyDevice device) {
            if(tinyHandle != IntPtr.Zero)
                return false;
            TINY_DeviceTypeDef tinyDevice = new TINY_DeviceTypeDef();
            tinyDevice.usb_device = device.USBDevice;
            tinyDevice.name = new byte[128];
            byte[] deviceName = Encoding.UTF8.GetBytes(device.Name);
            Array.Copy(deviceName, tinyDevice.name, deviceName.Length);
            tinyHandle = TINY_Open(ref tinyDevice);
            if(tinyHandle == IntPtr.Zero)
                return false;
            return true;
        }

        public bool Connect() {
            if(tinyHandle == IntPtr.Zero)
                return false;
            return TinyCore.TINY_Connect(tinyHandle) == USB2CAN_ResultTypeDef.USB2CAN_OK;
        }

        public bool SetBitrate(int bitrate) {
            if(tinyHandle == IntPtr.Zero)
                return false;
            return TinyCore.TINY_SetBitrate(tinyHandle, bitrate) == USB2CAN_ResultTypeDef.USB2CAN_OK;
        }

        public int GetBitrate() {
            if(tinyHandle == IntPtr.Zero)
                return -1;
            int ret;
            if(TinyCore.TINY_GetBitrate(tinyHandle, out ret) != USB2CAN_ResultTypeDef.USB2CAN_OK)
                return -1;
            return ret;
        }

        public bool TransmitData(TinyCanMsg msg) {
            if(tinyHandle == IntPtr.Zero)
                return false;
            CAN_MsgTypeDef canMsg = new CAN_MsgTypeDef();
            canMsg.ID = msg.ID;
            canMsg.DLC = (byte)msg.DLC;
            canMsg.Data = new byte[8];
            for(int i = 0; i < canMsg.DLC; i++)
                canMsg.Data[i] = msg[i];
            return TinyCore.TINY_TransmitData(tinyHandle, ref canMsg) == USB2CAN_ResultTypeDef.USB2CAN_OK;
        }

        public TinyCanStatus TransmitStatus() {
            if(tinyHandle == IntPtr.Zero)
                return TinyCanStatus.TINY_CAN_ERROR;
            switch(TinyCore.TINY_TransmitStatus(tinyHandle)) {
                case USB2CAN_ResultTypeDef.USB2CAN_OK:
                    return TinyCanStatus.TINY_CAN_IDLE;
                case USB2CAN_ResultTypeDef.USB2CAN_BUSY:
                    return TinyCanStatus.TINY_CAN_BUSY;
                default:
                    return TinyCanStatus.TINY_CAN_ERROR;
            }
        }

        public TinyCanMsg ReadData() {
            if(tinyHandle == IntPtr.Zero)
                return null;
            CAN_MsgTypeDef msg = new CAN_MsgTypeDef();
            if(TINY_ReadData(tinyHandle, ref msg) != USB2CAN_ResultTypeDef.USB2CAN_OK)
                return null;
            TinyCanMsg ret = new TinyCanMsg(msg.DLC) {
                ID = msg.ID,
                IDE = (TinyCanIDE)msg.IDE,
                RTR = (TinyCanRTR)msg.RTR,
            };
            for(int i = 0; i < msg.DLC; i++)
                ret[i] = msg.Data[i];
            return ret;
        }

        public bool Disconnect() {
            if(tinyHandle == IntPtr.Zero)
                throw new Exception("Tiny Device is not connected yet!");
            bool ret = TinyCore.TINY_Disconnect(tinyHandle) == USB2CAN_ResultTypeDef.USB2CAN_OK;
            return ret;
        }

        public void Close() {
            if(tinyHandle != IntPtr.Zero) {
                TinyCore.TINY_Close(tinyHandle);
                tinyHandle = IntPtr.Zero;
            }
        }
    }
}
