// Absalyamov_IntSys.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include "framework.h"
#include "Absalyamov_IntSys.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include "Message.h"
#include "session.h"


// Единственный объект приложения

CWinApp theApp;

using namespace std;

int maxID = MR_USER;
map<int, shared_ptr<Session>> sessions;
CCriticalSection cs;

void LaunchClient()
{
    STARTUPINFO si = { sizeof(si) };
    PROCESS_INFORMATION pi;
    CreateProcess(NULL, (LPSTR)"Absalyamov_IntSys_Client.exe", NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi);
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
}

string GetActiveUsers()
{
    string NamesAndIds = "";
    for (auto& session : sessions)
    {
        NamesAndIds = NamesAndIds + to_string(session.second->id) + " " + session.second->GetName() + " ";
    }
    return NamesAndIds;
}

int main()
{
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
            // TODO: вставьте сюда код для приложения.
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
