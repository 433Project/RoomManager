#include <iostream>
#include "IOCPManager.h"



IOCPManager::~IOCPManager()
{
}

void IOCPManager::Init(int bufferSize)
{
	this->bufferSize = bufferSize;
	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(11000);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	connect(serverSocket, (sockaddr*)&addr, sizeof(sockaddr_in));

	CreateIoCompletionPort((HANDLE)serverSocket, iocp, (ULONG_PTR)nullptr, 0);
}

void IOCPManager::Start()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int numOfCPU = si.dwNumberOfProcessors;
	int numOfThreads = numOfCPU * 2;
	this->numOfThreads = numOfThreads;
	this->bufferSize = bufferSize;

	for (int i = 0; i < numOfThreads; i++)
	{
		threadPool.push_back(new std::thread(&IOCPManager::ThreadProc, this));
	}

	for (int i = 0; i < numOfThreads; i++)
	{

		char* buffer = new char[bufferSize];
		OVERLAPPED_EX* overio = new OVERLAPPED_EX();
		overio->wsaBuffer->buf = buffer;
		overio->wsaBuffer->len = bufferSize;
		WSARecv(serverSocket, overio->wsaBuffer, 1, nullptr, nullptr, overio, nullptr);
	}

	for each(auto thread in threadPool)
	{
		thread->join();
		delete thread;
	}
	threadPool.clear();
}

void IOCPManager::ThreadProc()
{

	DWORD transferedBytes;
	OVERLAPPED_EX* overLapped = nullptr;
	PULONG_PTR dummy;

	while (TRUE)
	{
		GetQueuedCompletionStatus(
			iocp,
			&transferedBytes,
			dummy,
			(LPOVERLAPPED*)&overLapped,
			INFINITE
		);

		std::cout << transferedBytes + " bytes received." << std::endl;

		receivedMessages.push(overLapped->wsaBuffer);
		
		char* buffer = new char[bufferSize];
		overLapped = new OVERLAPPED_EX();
		overLapped->wsaBuffer->buf = buffer;
		overLapped->wsaBuffer->len = bufferSize;
		WSARecv(serverSocket, overLapped->wsaBuffer, 1, NULL, NULL, overLapped, NULL);
	}
}

void IOCPManager::Send(char* data, int length)
{
	OVERLAPPED_EX* overLapped;
	overLapped = new OVERLAPPED_EX();
	overLapped->wsaBuffer->buf = data;
	overLapped->wsaBuffer->len = length;
	WSASend(serverSocket, overLapped->wsaBuffer, 1, NULL, NULL, overLapped, NULL);
}

WSABUF* IOCPManager::GetReceivedMessage()
{
	if (receivedMessages.size() > 0)
	{
		WSABUF* buf = receivedMessages.front();
		receivedMessages.pop();
		return buf;
	}

	return nullptr;
}

void IOCPManager::ShutDown()
{
	if (threadPool.size() > 0)
	{
		for each(auto thread in threadPool)
		{
			delete thread;
		}
		threadPool.clear();
	}
}
