#pragma once

#include <vector>
#include <functional>


#ifndef TAGSOCKETEVENTLIST
#define TAGSOCKETEVENTLIST
typedef struct tagSocketEventList
{
	SOCKET sock;
	WSAEVENT wsaevent;
	std::function<VOID(SOCKET, INT_PTR)> callbackfunc;
}SOCKETEVENTLIST, *PSOCKETEVENTLIST;
#endif // !TAGSOCKETEVENTLIST


class CAsyncTcpModel
{
public:
	CAsyncTcpModel(CHAR* pNetAddress/*IP地址，非域名*/, u_short uPort);
	virtual ~CAsyncTcpModel();

	u_short HostPort;		//连接端口

protected:
	VOID AddSocketEvent(SOCKET hSocket, WSAEVENT SockEvent);
	VOID DelSocketEvent(SOCKET hSocket);
	WSAEVENT* GetSocketList();
	VOID ReleaseSocketList(WSAEVENT* pListSocket);
	SOCKET GetSocketByWSAEvent(WSAEVENT hWSAEvent);

private:
	WSADATA m_wsaData;
	std::vector<PSOCKETEVENTLIST> m_vecSocketEvent;

	BOOL InitServerSocket(CHAR* pNetAddress);

public:
	VOID Run(CHAR* pNetAddress);
};

