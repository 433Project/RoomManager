#pragma once
#include <winSock2.h>
#include <vector>
#include <queue>
#include <thread>

class IOCPManager
{
public:
	~IOCPManager();
	void Init(int bufferSize, int csPort, std::string csIp);
	void Start();
	void ShutDown();
	void Send(char* data, int length);
	WSABUF* GetReceivedMessage();
	void Connect();

private:

	struct OVERLAPPED_EX : OVERLAPPED
	{
		WSABUF* wsaBuffer;
	};

	void ThreadProc();

private:
	int bufferSize;
	int numOfThreads;
	HANDLE iocp;
	SOCKET serverSocket;
	std::vector<std::thread*> threadPool;
	std::queue<WSABUF*> receivedMessages;
	ULONG_PTR compKey;
	CRITICAL_SECTION messageQueueCriticalSection;
	CRITICAL_SECTION csSocketCriticalSection;
	int port;
	std::string ip;
};

