#include "pch.h"
#include "Session.h"

Session::Session(int id, std::string name)
{
	this->id = id;
	this->name = name;
}

void Session::MessageAdd(Message& m)
{
	cs.Lock();
	messages.push(m);
	cs.Unlock();
}


void Session::MessageSend(CSocket& s)
{
	cs.Lock();
	if (messages.empty())
	{
		Message::Send(s, id, MR_BROKER, MT_NODATA);
	}
	else
	{
		messages.front().Send(s);
		messages.pop();
	}
	cs.Unlock();
}

void Session::RefreshUsers(CSocket& s, std::string users)
{
	Message::Send(s, id, MR_BROKER, MT_REFRESH, users);
}

void Session::SetLastSeen()
{
	LastSeen = std::chrono::steady_clock::now();
}

std::chrono::steady_clock::time_point Session::GetLastSeen()
{
	return LastSeen;
}

std::string Session::GetName()
{
	return name;
}
