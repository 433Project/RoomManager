#include "stdafx.h"
#include "RoomManager.h"
#include <windows.h>


RoomManager::RoomManager()
{
}


RoomManager::~RoomManager()
{
}

void RoomManager::Start()
{
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	int numOfCPU = si.dwNumberOfProcessors;
	int numOfThreads = numOfCPU * 2;
	iocp = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, numOfThreads);

	SOCKET listeningSocket;

	listeningSocket = socket(AF_INET, SOCK_STREAM, 0);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(11000);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(listeningSocket, (struct sockaddr *)&addr, sizeof(addr));

	listen(listeningSocket, 5);


}

void RoomManager::ThreadProc()
{
	DWORD readn;
	int coKey;
	int flags = 0;
	struct Session *sInfo;
	while (true)
	{
		GetQueuedCompletionStatus(iocp, &readn, sInfo, &overlapped, INFINITE);
	}
}