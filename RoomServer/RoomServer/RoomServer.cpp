#include "RoomServer.h"
#include <iostream>

RoomServer::~RoomServer()
{
}

void RoomServer::Start()
{
	iocpManager.Init(100, 9433, "10.100.10.6");
	iocpManager.Connect();

	std::thread* thread = new std::thread(&RoomServer::ProcessMessage, this);

	iocpManager.Start();
	iocpManager.ShutDown();
	thread->join();
}

void RoomServer::ProcessMessage()
{

	std::cout << "Start Receiving" << std::endl;
	while (true)
	{
		WSABUF* buffer = iocpManager.GetReceivedMessage();
		if (buffer == nullptr)
		{
			Sleep(100);
			continue;
		}
		AddressInfo receivedInfo;
		int length;

		memcpy(&length, (byte*)buffer->buf, 4);
		memcpy(&receivedInfo.srcType, (byte*)buffer->buf + 4, 4);
		memcpy(&receivedInfo.srcCode, (byte*)buffer->buf + 8, 4);

		memcpy(&receivedInfo.dstType, (byte*)buffer->buf + 12, 4);
		memcpy(&receivedInfo.dstCode, (byte*)buffer->buf + 16, 4);

		std::cout << "SRC TYPE: " << receivedInfo.srcType << std::endl;
		std::cout << "SRC CODE: " << receivedInfo.srcCode << std::endl;
		std::cout << "DST TYPE: " << receivedInfo.dstType << std::endl;
		std::cout << "DST CODE: " << receivedInfo.dstCode << std::endl;
	}
}




