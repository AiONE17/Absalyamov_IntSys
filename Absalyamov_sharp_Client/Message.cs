using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.InteropServices;
using System.Net;
using System.Net.Sockets;

namespace Absalyamov_sharp_Client
{
    public enum MessageTypes : int
    {
        MT_INIT,
        MT_EXIT,
        MT_GETDATA,
        MT_DATA,
        MT_NODATA,
        MT_CONFIRM,
        MT_DECLINE,
        MT_REFRESH
    };

    public enum MessageRecipients : int
    {
        MR_BROKER = 1,
        MR_ALL = 0,
        MR_USER = 100
    };


    [StructLayout(LayoutKind.Sequential)]
    struct MessageHeader
    {
        [MarshalAs(UnmanagedType.I4)]
        public MessageRecipients haddr;
        [MarshalAs(UnmanagedType.I4)]
        public MessageRecipients hfrom;
        [MarshalAs(UnmanagedType.I4)]
        public MessageTypes hactioncode;
        [MarshalAs(UnmanagedType.I4)]
        public int hsize;
    };

    internal class Message
    {
        public MessageHeader header;
        public string data;
        static MessageRecipients ClientID;
        static Encoding? cp866 = null;

        Encoding get866()
        {
            if (cp866 is null)
            {
                Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);
                cp866 = Encoding.GetEncoding("CP866");
            }
            return cp866;
        }

        public Message(MessageRecipients to, MessageRecipients from, MessageTypes action = MessageTypes.MT_DATA, string data = "")
        {
            this.data = data;
            header = new MessageHeader() { haddr = to, hfrom = from, hactioncode = action, hsize = data.Length };
        }

        public MessageTypes GetAction()
        {
            return this.header.hactioncode;
        }

        public string GetData()
        {
            return this.data;
        }

        public MessageRecipients GetFrom()
        {
            return this.header.hfrom;
        }

        static byte[] toBytes(object obj)
        {
            int size = Marshal.SizeOf(obj);
            byte[] buff = new byte[size];
            IntPtr ptr = Marshal.AllocHGlobal(size);
            Marshal.StructureToPtr(obj, ptr, true);
            Marshal.Copy(ptr, buff, 0, size);
            Marshal.FreeHGlobal(ptr);
            return buff;
        }

        static T fromBytes<T>(byte[] buff) where T : struct
        {
            T data = default(T);
            int size = Marshal.SizeOf(data);
            IntPtr i = Marshal.AllocHGlobal(size);
            Marshal.Copy(buff, 0, i, size);
            var d = Marshal.PtrToStructure(i, data.GetType());
            if (d is not null)
            {
                data = (T)d;
            }
            Marshal.FreeHGlobal(i);
            return data;
        }

        void send(Socket s)
        {
            s.Send(toBytes(header), Marshal.SizeOf(header), SocketFlags.None);
            if (header.hsize != 0)
            {
                s.Send(get866().GetBytes(data), header.hsize, SocketFlags.None);
            }
        }

        MessageTypes receive(Socket s)
        {
            byte[] buff = new byte[Marshal.SizeOf(header)];
            if (s.Receive(buff, Marshal.SizeOf(header), SocketFlags.None) == 0)
            {
                return MessageTypes.MT_NODATA;
            }
            header = fromBytes<MessageHeader>(buff);
            if (header.hsize > 0)
            {
                byte[] b = new byte[header.hsize];
                s.Receive(b, header.hsize, SocketFlags.None);
                data = get866().GetString(b, 0, header.hsize);
            }
            return header.hactioncode;
        }

        public static void send(Socket s, MessageRecipients to, MessageRecipients from, MessageTypes type = MessageTypes.MT_DATA, string data = "")
        {
            new Message(to, from, type, data).send(s);
        }

        public static Message send(MessageRecipients to, MessageTypes type = MessageTypes.MT_DATA, string data = "")
        {
            int nPort = 17;
            IPEndPoint endPoint = new IPEndPoint(IPAddress.Parse("127.0.0.1"), nPort);
            Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp);
            s.Connect(endPoint);
            if (!s.Connected)
            {
                throw new Exception("Connection error");
            }
            var m = new Message(to, ClientID, type, data);
            m.send(s);
            if (m.receive(s) == MessageTypes.MT_INIT)
            {
                ClientID = m.header.haddr;
            }
            return m;
        }
    }
}
