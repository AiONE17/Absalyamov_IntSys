#pragma once
struct MessageHeader
{
	int haddr;
	int hfrom;
	int hactioncode;
	int hsize;
};

enum MessageTypes
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

enum MessageRecipients
{
	MR_BROKER = 1,
	MR_ALL = 0,
	MR_USER = 100
};


class Message
{
private:
	MessageHeader header = { 0 };
	std::string data;
	static int ClientID;

public:
	Message() {};
	~Message() {};
	Message(int to, int from, int action, const std::string& data = "");

	void Send(CSocket& s);
	int Receive(CSocket& s);

	static void Send(CSocket& s, int to, int from, int action, const std::string& data = "");
	static Message Send(int to, int action = MT_DATA, const std::string& data = "");

	std::string GetHeaderData();
	std::string GetData();
	int GetFrom();
	int GetAddr();
	int GetAction();
};

