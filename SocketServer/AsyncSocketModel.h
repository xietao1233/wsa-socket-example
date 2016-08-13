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


class CAsyncSocketModel
{
public:
	CAsyncSocketModel(u_short uPort);
	virtual ~CAsyncSocketModel();

private:
	WSADATA m_wsaData;

	VOID ClearSocketHandle();

protected:
	u_short m_uPort;
	std::vector<PSOCKETEVENTLIST> m_vecSocketEvent;

	VOID AddSocketEvent(SOCKET hSocket, WSAEVENT SockEvent, std::function<VOID(SOCKET, INT_PTR)> pfnCallback);
	VOID DeleteSocketEvent(SOCKET hSocket);
	WSAEVENT* GetSocketList();
	VOID ReleaseSocketList(WSAEVENT* pListSocket);
	SOCKET GetSocketByWSAEvent(WSAEVENT hWSAEvent);
	std::function<VOID(SOCKET, INT_PTR)> GetCallbackByWSAEvent(WSAEVENT hWSAEvent);

public:
	virtual VOID Run(std::function<VOID(SOCKET, INT_PTR)> pfnAcceptFunc, std::function<VOID(SOCKET, INT_PTR)> pfnRecvFunc) = 0;			//初始化socket信息
};

