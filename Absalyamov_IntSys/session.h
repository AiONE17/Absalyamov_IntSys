#pragma once
#include "Message.h"

class Session
{
private:
	std::string name;
	std::queue<Message> messages;
	CCriticalSection cs;
	std::chrono::steady_clock::time_point LastSeen = std::chrono::steady_clock::now();
public:
	int id;
	Session(int id, std::string name);
	~Session() {};

	void MessageAdd(Message& m);
	void MessageSend(CSocket& s);
	void RefreshUsers(CSocket& s, std::string users);
	void SetLastSeen();
	std::chrono::steady_clock::time_point GetLastSeen();

	std::string GetName();
};

