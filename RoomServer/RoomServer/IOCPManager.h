#pragma once
#include <WinSock2.h>
#include <vector>
#include <queue>
#include <thread>

class IOCPManager
{
public:
	~IOCPManager();
	void Init(int bufferSize);
	void Start();
	void ShutDown();
	void Send(char* data, int length);
	WSABUF* GetReceivedMessage();

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
};

