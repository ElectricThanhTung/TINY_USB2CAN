using System;

namespace TinyDotNet {
    public class TinyDevice {
        public IntPtr USBDevice;
        public string Name;
    }

    public enum TinyCanIDE {
        IDE_STD = 0,
        IDE_EXT = 1,
    }

    public enum TinyCanRTR {
        RTR_DATA = 0,
        RTR_REMOTE = 1,
    }

    public enum TinyCanStatus {
        TINY_CAN_IDLE = 0,
        TINY_CAN_BUSY = 1,
        TINY_CAN_ERROR = 2,
    }

    public class TinyCanMsg {
        public int ID;
        public TinyCanIDE IDE;
        public TinyCanRTR RTR;
        private byte[] Data;

        public TinyCanMsg() {
            Data = new byte[8];
        }

        public TinyCanMsg(byte dlc) {
            Data = new byte[dlc];
        }

        public TinyCanMsg(byte[] data) {
            Data = data;
        }

        public byte DLC {
            get {
                return (byte)Data.Length;
            }
        }

        public byte this[int index] {
            get {
                return Data[index];
            }
            set {
                Data[index] = value;
            }
        }
    }
}
