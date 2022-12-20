using System;
using System.Collections.Generic;
using System.Threading;
using TinyDotNet;

namespace TinySharp {
    internal class Program {
        private static void PrintRxMsg(TinyCanMsg msg) {
            Console.Write("Data Received:\t" + msg.ID.ToString("X3") + "  " + msg.DLC.ToString() + "  { ");
            for(int i = 0; i < msg.DLC; i++)
                Console.Write(msg[i].ToString("X2") + " ");
            Console.WriteLine("}");
        }

        private static void PrintErrorMsg(string err) {
            Console.WriteLine(err);
            Console.WriteLine("Press any to close application");
            Console.ReadKey(true);
        }
        
        static void Main(string[] args) {
            bool first_send = true;
            bool receive_enable = false;

            /* Get all the Tiny devices connected to the computer  */
            List<TinyDevice> devices = Tiny.GetDevices();
            if(devices == null) {
                PrintErrorMsg("Can't find any Tiny device!");
                return;
            }
            else {
                Console.WriteLine("Press 'S' to stop transmit and receive data");
                Console.WriteLine("Press 'R' to enable or disable revice data");
                Console.WriteLine("Press 'C' to clear the screen\r\n");

                Tiny dev = new Tiny();

                /* Open the first device found */
                bool ret = dev.Open(devices[0]);
                if(ret != true) {
                    PrintErrorMsg("Can't open the Tiny device!");
                    return;
                }

                /* Make sure to disconnect before continuing */
                ret = dev.Disconnect();
                if(ret != true) {
                    PrintErrorMsg("Error when disconnecting!");
                    return;
                }

                /* Set bitrate (500K) */
                ret = dev.SetBitrate(500000);
                if(ret != true) {
                    PrintErrorMsg("Can't set bitrate!");
                    return;
                }

                /* Get actual bitrate value */
                int value = dev.GetBitrate();
                if(value < 0) {
                    PrintErrorMsg("Can't get bitrate!");
                    return;
                }
                else
                    Console.WriteLine("Actual bitrate: " + value);

                /* Connect to enable transmit and receive data */
                ret = dev.Connect();
                if(ret != true) {
                    PrintErrorMsg("Can't connect to Tiny device!");
                    return;
                }

                byte count = 0;

                while(true) {
                    /* Prepare data for transmission */
                    TinyCanMsg tx_msg = new TinyCanMsg(8);
                    tx_msg.ID = 0x123;
                    tx_msg.IDE = TinyCanIDE.IDE_STD;
                    tx_msg.RTR = TinyCanRTR.RTR_DATA;
                    for(int i = 0; i < tx_msg.DLC; i++)
                        tx_msg[i] = count;
                    count++;

                    /* Data transmission over CAN-Bus */
                    if(first_send) {
                        first_send = false;
                        Console.WriteLine("Transmitting data");
                    }
                    ret = dev.TransmitData(tx_msg);
                    if(ret != true) {
                        PrintErrorMsg("Transmit failed!");
                        return;
                    }

                    /* Wait for data to be sent */
                    /*
                    while(true) {
                        TinyCanStatus status = dev.TransmitStatus();
                        if(status == TinyCanStatus.TINY_CAN_IDLE)
                            break;
                        else if(status == TinyCanStatus.TINY_CAN_BUSY) {
                            Thread.Sleep(1);
                            continue;
                        }
                        else if(status != TinyCanStatus.TINY_CAN_ERROR) {
                            PrintErrorMsg("Transmit data failed!");
                            return;
                        }
                    }
                    */

                    /* Read data from CAN-Bus */
                    if(receive_enable) {
                        TinyCanMsg rx_msg;
                        do {
                            rx_msg = dev.ReadData();
                            if(rx_msg != null && rx_msg.RTR == TinyCanRTR.RTR_DATA)
                                PrintRxMsg(rx_msg);
                            else {
                                /* Do Nothing */
                            }
                        } while(rx_msg != null);
                    }

                    /* User control */
                    if(Console.KeyAvailable) {
                        ConsoleKey k = Console.ReadKey(true).Key;
                        if(k == ConsoleKey.S) {
                            /* Stop transmit and receive */
                            dev.Disconnect();
                            return;
                        }
                        else if(k == ConsoleKey.R) {
                            /* Enable receive and show received data to the screen */
                            receive_enable = !receive_enable;
                            if(receive_enable)
                                Console.WriteLine("Enable receive data");
                            else
                                Console.WriteLine("Disable receive data");
                        }
                        else if(k == ConsoleKey.C) {
                            /* Clear screen */
                            Console.Clear();
                            Console.WriteLine("Press 'S' to stop transmit and receive data");
                            Console.WriteLine("Press 'R' to enable or disable revice data");
                            Console.WriteLine("Press 'C' to clear the screen");
                            Console.WriteLine("Transmitting data");
                        }
                    }

                    Thread.Sleep(100);
                }
            }
        }
    }
}
