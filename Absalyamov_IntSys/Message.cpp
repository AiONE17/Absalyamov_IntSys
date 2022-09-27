#include "pch.h"
#include "Message.h"
#include <vector>
#include <string>

std::string GetLastErrorString(DWORD ErrorID = 0)
{
	if (!ErrorID)
		ErrorID = GetLastError();
	if (!ErrorID)
		return std::string();

	LPSTR pBuff = nullptr;
	size_t size = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, ErrorID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&pBuff, 0, NULL);
	std::string s(pBuff, size);
	LocalFree(pBuff);

	return s;
}

int Message::ClientID = 0;

Message::Message(int to, int from, int action, const std::string& data)
{
	this->data = data;
	this->header = { to, from, action, int(data.length()) };
}

void Message::Send(CSocket& s)
{
	s.Send(&header, sizeof(MessageHeader));
	if (header.hsize)
	{
		s.Send(data.c_str(), header.hsize);
	}
}

int Message::Receive(CSocket& s)
{
	if (!s.Receive(&header, sizeof(MessageHeader)))
		return MT_NODATA;
	if (header.hsize)
	{
		std::vector<char> v(header.hsize);
		s.Receive(&v[0], header.hsize);
		data = std::string(&v[0], header.hsize);
	}
	return header.hactioncode;
}

void Message::Send(CSocket& s, int to, int from, int action, const std::string& data)
{
	Message m(to, from, action, data);
	m.Send(s);
}

Message Message::Send(int to, int action, const std::string& data)
{
	CSocket s;
	s.Create();
	if (!s.Connect("127.0.0.1", 17))
	{
		throw std::runtime_error(GetLastErrorString());
	}
	Message m(to, ClientID, action, data);
	m.Send(s);
	if (m.Receive(s) == MT_INIT)
	{
		ClientID = m.header.haddr;
	}
	return m;
}

std::string Message::GetHeaderData()
{
	return std::to_string(header.haddr) + ": " + std::to_string(header.hfrom) + ": " + std::to_string(header.hactioncode);
}

std::string Message::GetData()
{
	return data;
}

int Message::GetFrom()
{
	return header.hfrom;
}

int Message::GetAddr()
{
	return header.haddr;
}

int Message::GetAction()
{
	return header.hactioncode;
}