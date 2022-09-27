// Absalyamov_IntSys_Client.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "framework.h"
#include "Absalyamov_IntSys_Client.h"
#include "../Absalyamov_IntSys/Message.h"
#include "../Absalyamov_IntSys/Message.cpp"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

map<int, string> ActiveUsers;
CCriticalSection cs;
string username = "";

void HistoryWrite(string str, string& uname)
{
    cs.Lock();
    ofstream hist("history" + uname + ".txt", ios::app);
    if (hist.is_open())
    {
        hist << str << endl;;
        hist.close();
    }
    cs.Unlock();
}
void HistoryRead(string& uname)
{
    string line;
    cs.Lock();
    ifstream hist("history" + uname + ".txt", ios::out);
    if (hist.is_open())
    {
        while (getline(hist, line))
        {
            cout << line << '\n';
        }
        hist.close();
    }
    cs.Unlock();
}

void PrintActiveUsers()
{
    cout << "__________________________________________\nОнлайн :" << endl;
    for (auto& user : ActiveUsers)
    {
        cout << user.second << "(" << user.first << "); ";
    }
    cout << endl;
}

void RefreshActiveUsers(string str)
{
    ActiveUsers.clear();
    stringstream NamesAndIds(str);
    while (true)
    {
        int uid;
        string uname;
        NamesAndIds >> uid;
        if (int(uid) == -1)
            break;
        NamesAndIds >> uname;
        ActiveUsers[int(uid)] = uname;
    }
}

int CheckCommands(string& str)
{
    if (str == "/Помогите")
        return 3;
    else if (str == "/Выходим")
        return 1;
    else if (str == "/Сообщение") //str ==  "/whisper"
        return 2;
    else
        return 0;
}

void ProcessMessages()
{
    bool ExitFlag = true;
    while (ExitFlag)
    {
        Message m = Message::Send(MR_BROKER, MT_REFRESH, to_string(ActiveUsers.size()));
        if (m.GetAction() != MT_DECLINE)
            RefreshActiveUsers(m.GetData());
        m = Message::Send(MR_BROKER, MT_GETDATA);
        switch (m.GetAction())
        {
        case MT_DATA:
        {
            HistoryWrite((ActiveUsers[m.GetFrom()] + ": " + m.GetData()), username);
            cout << string(100, '\n') << endl;
            HistoryRead(username);
            PrintActiveUsers();
            break;
        }
        case MT_EXIT:
            cout << "Вы былм отключены от сервера" << endl;
            m = Message::Send(MR_BROKER, MT_EXIT);
            ExitFlag = false;
        default:
            Sleep(1000);
            break;
        }
    }
    cout << "Please reconect" << endl;
}

void Client()
{
    AfxSocketInit();

    while (true)
    {
        cout << "Введите ваше имя, пожалуйста ^_^ ;D :3" << endl;
        cin >> username;

        ofstream hist("history" + username + ".txt", ios::trunc);
        if (hist.is_open())
        {
            hist << "";
            hist.close();
        }
        Message m = Message::Send(MR_BROKER, MT_INIT, username);
        if (m.GetAction() == MT_DECLINE)
        {
            cout << "К сожалению, человек с таким именем уже есть :(" << endl;
        }
        else
        {
            HistoryWrite("Что-то потустороннее: Привет)))))))))) " + username, username);
            cout << string(100, '\n') << endl;
            RefreshActiveUsers(m.GetData());
            HistoryRead(username);
            PrintActiveUsers();
            cout << "Добро пожаловать, " << username << endl;
            break;
        }
    }

    thread t(ProcessMessages);
    t.detach();

    bool ExitFlag = true;
    while (ExitFlag)
    {
        string str;
        cin >> str;
        int com = CheckCommands(str);
        switch (com)
        {
        case 0:
        {
            HistoryWrite("Вы: " + str, username);
            Message::Send(MR_ALL, MT_DATA, str);
            cout << string(100, '\n') << endl;
            HistoryRead(username);
            PrintActiveUsers();
            break;
        }
        case 1:
        {
            Message::Send(MR_BROKER, MT_EXIT);
            ExitFlag = false;
            break;
        }
        case 2:
        {
            cout << "Введите id человека, которому вы хотите написать сообщение: ";
            cin >> str;
            auto user = ActiveUsers.find(stoi(str));
            if (user != ActiveUsers.end())
            {
                cout << "Text: " << endl;
                cin.ignore();
                getline(cin, str);
                HistoryWrite("Вы написали человеку с именем " + user->second + ": " + str, username);
                Message::Send(user->first, MT_DATA, str);
                cout << string(100, '\n') << endl;
                HistoryRead(username);
                PrintActiveUsers();
            }
            break;
        }
        case 3:
        {
            cout << "/Выйти - чтобы выйти.\n /Помогите - чтобы вам помогли\n /Сообщение - чтобы отправить личное сообщение\n" << endl;
            break;
        }
        default:
        {
            break;
        }
        }
    }

}
// Единственный объект приложения

CWinApp theApp;

using namespace std;

int main()
{
    setlocale(LC_ALL, "Russian");
    system("chcp 1251");
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        // инициализировать MFC, а также печать и сообщения об ошибках про сбое
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            // TODO: вставьте сюда код для приложения.
            wprintf(L"Критическая ошибка: сбой при инициализации MFC\n");
            nRetCode = 1;
        }
        else
        {
            Client();
            if (remove(("history" + username + ".txt").c_str()) != 0)
                perror("Ошибка удаления файла");
            else
                puts("Файл успешно удален");
        }
    }
    else
    {
        // TODO: измените код ошибки в соответствии с потребностями
        wprintf(L"Критическая ошибка: сбой GetModuleHandle\n");
        nRetCode = 1;
    }

    return nRetCode;
}
