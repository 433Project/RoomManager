#pragma once
#include "IOCPManager.h"
#include "protocol.h"

class RoomServer
{
public:
	~RoomServer();
	void Start();

private:
	void ProcessMessage();
	struct Room
	{
		int player1;
		int player2;
		int roomNo;
		int leftTime = 0;
	};

	struct AddressInfo
	{
		SrcDstType srcType;
		int srcCode;
		SrcDstType dstType;
		int dstCode;
	};

private:
	IOCPManager iocpManager;
	std::vector<Room> rooms;
};

