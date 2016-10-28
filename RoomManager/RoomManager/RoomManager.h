#pragma once
class RoomManager
{
public:
	RoomManager();
	~RoomManager();

private:

	struct Session : OVERLAPPED
	{
		SOCKET socket;

	};
	void Start();
	void ThreadProc();

private:
	HANDLE iocp;
};

