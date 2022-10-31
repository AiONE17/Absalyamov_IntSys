using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Net;
using System.Net.Sockets;
using Absalyamov_sharp_Client;

namespace Abslyamov_sharp_Client
{
    class Program
    {
        public static string path_user = "C:\\Users\\vanya\\Documents\\IntSys\\Absalyamov_IntSys\\x64\\Debug\\";
        static SortedDictionary<int, string> ActiveUsers = new SortedDictionary<int, string>();
        public static string username="";
        public static void HistoryWrite(string str, string username)
        {
            using (StreamWriter stream = new StreamWriter(path_user + "history" + username + ".dat", true))
                stream.WriteLine(str);
        }

        public static void HistoryRead(string uname)
        {
            try
            {
                using (StreamReader stream = new StreamReader(path_user + "history" + uname + ".dat", Encoding.Default))
                {
                    string str;
                    while ((str = stream.ReadLine()) != null)
                        Console.WriteLine(str);
                }
            }
            catch (Exception ex)
            {

                Console.WriteLine("Error" + ex);
            }
        }

        static void PrintActiveUsers()
        {
            Console.WriteLine("__________________________________________\nActive Users :");
            foreach (var user in ActiveUsers)
            {
                Console.WriteLine($"{user.Value} ({user.Key})");
            }
        }
        static void RefreshActiveUsers(string str)
        {
            ActiveUsers.Clear();
            string[] buf = str.Split(' ');
            for (int i = 0; i < buf.Length - 1; i = i + 2)
            {
                ActiveUsers.Add(int.Parse(buf[i]), buf[i + 1]);
            }
            Console.WriteLine("All users");
            foreach (var user in ActiveUsers)
            {
                if (user.Value != username)
                    Console.WriteLine($"{user.Value} ({user.Key})");
            }
        }
        static int CheckCommands(string str)
        {
            if (str == "/Помогите")
                return 3;
            if (str == "/Выйти")
                return 1;
            if (str == "/Сообщение")
                return 2;
            if (str == "/reconect")
                return 4;
            return 0;
        }
        public static void ProcessMessages()
        {
            bool ExitFlag = true;
            while (ExitFlag)
            {
                var m = Message.send(MessageRecipients.MR_BROKER, MessageTypes.MT_REFRESH, ActiveUsers.Count.ToString());
                if (m.GetAction() != MessageTypes.MT_DECLINE)
                {
                    RefreshActiveUsers(m.GetData());
                    //cout << string(100, '\n') << endl;
                    HistoryRead(username);
                    PrintActiveUsers();
                }
                m = Message.send(MessageRecipients.MR_BROKER, MessageTypes.MT_GETDATA);
                switch (m.GetAction())
                {
                    case MessageTypes.MT_DATA:
                        Console.WriteLine($"{ActiveUsers[(int)m.GetFrom()]}: {m.GetData()}");
                        break;
                    case MessageTypes.MT_EXIT:
                        m = Message.send(MessageRecipients.MR_BROKER, MessageTypes.MT_EXIT);
                        ExitFlag = false;
                        break;
                    default:
                        Thread.Sleep(1000);
                        break;
                }
            }
        }
        static int Client()
        {
            while (true)
            {
                Console.WriteLine("Как вас зовут?");
                username = Console.ReadLine();
                var m = Message.send(MessageRecipients.MR_BROKER, MessageTypes.MT_INIT, username);
                if (m.GetAction() == MessageTypes.MT_DECLINE)
                {
                    Console.WriteLine("Недоступное имя"); ;
                }
                else
                {
                    HistoryWrite("Сервер: Привет " + username, username);
                    //cout << string(100, '\n') << endl;
                    RefreshActiveUsers(m.GetData());
                    HistoryRead(username);
                    PrintActiveUsers();
                    break;
                }
            }
            Thread t = new Thread(() => ProcessMessages());
            t.Start();
            while (true)
            {
                string str = Console.ReadLine();
                int com = CheckCommands(str);
                switch (com)
                {
                    case 0:
                        {
                            HistoryWrite("Вы: " + str, username);
                            var m = Message.send(MessageRecipients.MR_ALL, MessageTypes.MT_DATA, str);
                            HistoryRead(username);
                            PrintActiveUsers();
                            break;
                        }
                    case 1:
                        {
                            var m = Message.send(MessageRecipients.MR_BROKER, MessageTypes.MT_EXIT);
                            return 0;
                            break;
                        }
                    case 2:
                        {
                            HistoryWrite("Вы: " + str, username);
                            Console.WriteLine("id адресата: ");
                            var number = int.TryParse(Console.ReadLine(), out int id);
                            var text = Console.ReadLine();
                            Message.send((MessageRecipients)id, MessageTypes.MT_DATA, ("private ") + text);
                            HistoryRead(username);
                            PrintActiveUsers();
                            break;
                        }
                    case 3:
                        {
                            Console.WriteLine("/Выйти, /Помогите, /Сообщение");
                            break;
                        }
                    case 4:
                        {
                            var m = Message.send(MessageRecipients.MR_BROKER, MessageTypes.MT_EXIT);
                            return 1;
                            break;
                        }
                    default:
                        {
                            break;
                        }
                }
            }
        }
        static void Main(string[] args)
        {
            int ExitCode = 1;
            while (ExitCode != 0)
                ExitCode = Client();

        }
    }
}
