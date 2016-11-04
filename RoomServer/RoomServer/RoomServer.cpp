#include "RoomServer.h"


RoomServer::~RoomServer()
{
}

void RoomServer::Start()
{
	iocpManager.Init(1000);
	iocpManager.Start();

	while (true)
	{
		WSABUF* buffer = iocpManager.GetReceivedMessage();
		AddressInfo receivedInfo;
		int length;

		memcpy(&length, (byte*)buffer->buf, 4);
		memcpy(&receivedInfo.srcType, (byte*)buffer->buf + 4, 4);
		memcpy(&receivedInfo.srcCode, (byte*)buffer->buf + 8, 4);
												   
		memcpy(&receivedInfo.dstType, (byte*)buffer->buf + 12, 4);
		memcpy(&receivedInfo.dstCode, (byte*)buffer->buf + 16, 4);
	}
}




