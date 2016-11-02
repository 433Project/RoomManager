#pragma once
#include <WinSock2.h>
#include <vector>

class IOCPManager
{
public:
	IOCPManager(int bufferSize, int numberOfThreads);
	~IOCPManager();
	
private:

	struct OVERLAPPED_EX : OVERLAPPED
	{
		WSABUF wsaBuffer;
	};

	void ThreadProc();

private:
	int bufferSize;
	HANDLE iocp;
	SOCKET serverSocket;
	std::vector<std::thread*> threadPool;
};

