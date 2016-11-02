#include "stdafx.h"
#include "IOCPManager.h"
#include <windows.h>
#include <thread>
#include <iostream>


IOCPManager::IOCPManager(int sizeOfBuffer, int numberOfThreads)
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int numOfCPU = si.dwNumberOfProcessors;
	int numOfThreads = numOfCPU * 2;
	this->bufferSize = sizeOfBuffer;
	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, numOfThreads);

	for (int i = 0; i < numberOfThreads; i++)
	{
		threadPool.emplace_back(new std::thread(&ThreadProc));
	}

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(11000);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	connect(serverSocket, (sockaddr*)&addr, sizeof(sockaddr_in));

	CreateIoCompletionPort((HANDLE)serverSocket, iocp, (ULONG_PTR)nullptr, 0);

	for (int i = 0; i < numOfThreads; i++)
	{

		char* buffer = new char[bufferSize];
		OVERLAPPED_EX overio;
		overio.wsaBuffer.buf = buffer;
		overio.wsaBuffer.len = bufferSize;
		WSARecv(serverSocket, &overio.wsaBuffer, 1, nullptr, nullptr, &overio, nullptr);
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
	OVERLAPPED_EX overLapped;
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

		std::cout << transferedBytes + "bytes received." << std::endl;

		char* buffer = new char[bufferSize];
		OVERLAPPED_EX overio;
		overio.wsaBuffer.buf = buffer;
		overio.wsaBuffer.len = bufferSize;
		WSARecv(serverSocket, &overio.wsaBuffer, 1, nullptr, nullptr, &overio, nullptr);
	}
}


IOCPManager::~IOCPManager()
{
}
