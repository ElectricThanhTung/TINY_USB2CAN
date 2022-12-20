using System;
using System.Runtime.InteropServices;

namespace TinyDotNet {
    internal static class TinyCore {
        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        public unsafe struct TINY_DeviceTypeDef {
            public IntPtr usb_device;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 128)]
            public byte[] name;
        }

        public enum CAN_IDETypeDef : byte {
            IDE_STD = 0,
            IDE_EXT = 1,
        }

        public enum CAN_RTRTypeDef : byte {
            RTR_DATA = 0,
            RTR_REMOTE = 1,
        }

        [StructLayout(LayoutKind.Sequential, Pack = 4)]
        public unsafe struct CAN_MsgTypeDef {
            public int ID;
            public byte DLC;
            public CAN_IDETypeDef IDE;
            public CAN_RTRTypeDef RTR;
            [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
            public byte[] Data;
        }

        public enum USB2CAN_CmdTypeDef : byte {
            USB2CAN_CONNECT = 0,
            USB2CAN_SET_BITRATE,
            USB2CAN_READ_BITRATE,
            USB2CAN_READ_DATA,
            USB2CAN_TRANSMIT,
            USB2CAN_TRANSMIT_STATUS,
            USB2CAN_DISCONECT,
            UNKNOWN_CMD = 0xFF,
        }

        public enum USB2CAN_ResultTypeDef : byte {
            USB2CAN_OK = 0,
            USB2CAN_BUSY,
            USB2CAN_ERROR,
            USB2CAN_NOT_SUPPORTED,
            USB2CAN_SEND_FAIL,
            USB2CAN_NO_RESPONSE,
        }

        [DllImport("tiny.dll", CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern void TINY_Init();

        [DllImport("tiny.dll", CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern void TINY_GetDevices([MarshalAs(UnmanagedType.Struct)] ref TINY_DeviceTypeDef channels, int channel_count, out int actual_count);

        [DllImport("tiny.dll", CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern IntPtr TINY_Open(ref TINY_DeviceTypeDef tiny_device);

        [DllImport("tiny.dll", CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern USB2CAN_ResultTypeDef TINY_Connect(IntPtr dev);

        [DllImport("tiny.dll", CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern USB2CAN_ResultTypeDef TINY_SetBitrate(IntPtr dev, int value);

        [DllImport("tiny.dll", CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern USB2CAN_ResultTypeDef TINY_GetBitrate(IntPtr dev, out int value);

        [DllImport("tiny.dll", CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern USB2CAN_ResultTypeDef TINY_TransmitData(IntPtr dev, [MarshalAs(UnmanagedType.Struct)] ref CAN_MsgTypeDef msg);

        [DllImport("tiny.dll", CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern USB2CAN_ResultTypeDef TINY_TransmitStatus(IntPtr dev);

        [DllImport("tiny.dll", CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern USB2CAN_ResultTypeDef TINY_ReadData(IntPtr dev, [MarshalAs(UnmanagedType.Struct)] ref CAN_MsgTypeDef msgs);

        [DllImport("tiny.dll", CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern USB2CAN_ResultTypeDef TINY_Disconnect(IntPtr dev);

        [DllImport("tiny.dll", CallingConvention = CallingConvention.Cdecl)]
        public unsafe static extern void TINY_Close(IntPtr dev);
    }
}
