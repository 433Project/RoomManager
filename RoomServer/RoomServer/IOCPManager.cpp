#include <iostream>
#include "IOCPManager.h"


#pragma comment(lib, "Ws2_32.lib")



IOCPManager::~IOCPManager()
{
}

void IOCPManager::Init(int bufferSize, int csPort, std::string csIp)
{
	WSADATA wsaData;
	port = csPort;
	ip = csIp;
	InitializeCriticalSectionAndSpinCount(&messageQueueCriticalSection, 4000);
	this->bufferSize = bufferSize;
	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if (0 != ::WSAStartup(0x202, &wsaData)) {

		std::cout << "WinSock Initialization Failed." << std::endl;

		return;
	}
}

void IOCPManager::Connect()
{
	serverSocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (serverSocket == INVALID_SOCKET)
	{
		std::cout << "Invalid Socket" << std::endl;
	}

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = inet_addr(ip.c_str());

	while (true)
	{
		int ret = connect(serverSocket, (sockaddr*)&addr, sizeof(sockaddr_in));
		if (ret == SOCKET_ERROR)
		{
			std::cout << "Connecting to Connection Server" << std::endl;
			Sleep(2000);
			continue;
		}
		std::cout << "Connect Success" << std::endl;
		break;
	}

	compKey = (ULONG_PTR)"CS";
	CreateIoCompletionPort((HANDLE)serverSocket, iocp, compKey, 0);
}

void IOCPManager::Start()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int numOfCPU = si.dwNumberOfProcessors;
	int numOfThreads = numOfCPU * 2;
	this->numOfThreads = numOfThreads;

	for (int i = 0; i < numOfThreads; i++)
	{
		threadPool.push_back(new std::thread(&IOCPManager::ThreadProc, this));
	}

	for (int i = 0; i < numOfThreads; i++)
	{

		char* buffer = new char[bufferSize];
		OVERLAPPED_EX* overio = new OVERLAPPED_EX();
		overio->wsaBuffer = new WSABUF();
		overio->wsaBuffer->buf = buffer;
		overio->wsaBuffer->len = bufferSize;
		DWORD flags = MSG_WAITALL;
		int ret = WSARecv(serverSocket, overio->wsaBuffer, 1, nullptr, &flags, overio, nullptr);
		DWORD error = GetLastError();
		if (ret == SOCKET_ERROR && error != WSA_IO_PENDING)
		{
			std::cout << "Connection Server is Off" << error << std::endl;
			Sleep(2000);
			continue;
		}
	}
}

void IOCPManager::ThreadProc()
{

	DWORD transferedBytes;
	OVERLAPPED_EX* overLapped = nullptr;
	//PULONG_PTR dummy = nullptr;
	ULONG_PTR compKEY;
	while (TRUE)
	{
		GetQueuedCompletionStatus(
			iocp,
			&transferedBytes,
			&compKEY,
			(LPOVERLAPPED*)&overLapped,
			INFINITE
		);
		if (transferedBytes == 0)
		{
			// 구현 해야함.
			if (TryEnterCriticalSection(&csSocketCriticalSection))
			{
				std::cout << "Connection Server is Off" << std::endl;
				closesocket(serverSocket);
				Connect();
				LeaveCriticalSection(&csSocketCriticalSection);
			}
			else
			{
				Sleep(1000);
				continue;
			}
		}
		std::cout << (int)transferedBytes << " bytes received." << std::endl;

		EnterCriticalSection(&messageQueueCriticalSection);
		receivedMessages.push(overLapped->wsaBuffer);
		LeaveCriticalSection(&messageQueueCriticalSection);
		delete overLapped;
		char* buffer = new char[bufferSize];
		overLapped = new OVERLAPPED_EX();
		overLapped->wsaBuffer = new WSABUF();
		overLapped->wsaBuffer->buf = buffer;
		overLapped->wsaBuffer->len = bufferSize;

		DWORD flags = MSG_WAITALL;
		int ret = WSARecv(serverSocket, overLapped->wsaBuffer, 1, nullptr, &flags, overLapped, nullptr);
		DWORD error = GetLastError();
		if (ret == SOCKET_ERROR && error != WSA_IO_PENDING)
		{
			std::cout << "Connection Server is Off" << error << std::endl;
			Sleep(2000);
			continue;
		}
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
	EnterCriticalSection(&messageQueueCriticalSection);
	if (receivedMessages.size() > 0)
	{
		WSABUF* buf = receivedMessages.front();
		receivedMessages.pop();
		LeaveCriticalSection(&messageQueueCriticalSection);
		return buf;
	}

	LeaveCriticalSection(&messageQueueCriticalSection);
	return nullptr;
}

void IOCPManager::ShutDown()
{
	if (threadPool.size() > 0)
	{
		for each(auto thread in threadPool)
		{
			thread->join();
			delete thread;
		}
		threadPool.clear();
	}
}
