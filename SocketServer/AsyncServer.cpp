#include "stdafx.h"
#include "AsyncServer.h"
#include <exception>


CAsyncServer::CAsyncServer(u_short uPort)
	: CAsyncSocketModel(uPort)
{

}

CAsyncServer::~CAsyncServer()
{
}

VOID CAsyncServer::Run(std::function<VOID(SOCKET, INT_PTR)> pfnAcceptFunc, std::function<VOID(SOCKET, INT_PTR)> pfnRecvFunc)
{
	WSAEVENT* pSocketList = NULL;
	SOCKET hSocket = INVALID_SOCKET, hSocketAccept = INVALID_SOCKET;
	INT_PTR iEventIndex = 0;
	WSANETWORKEVENTS tNetEvents = { 0 };
	std::function<VOID(SOCKET, INT_PTR)> pfnCallback;
	WSAEVENT hWSAEvent = WSA_INVALID_EVENT;
	LPSTR pRecvBuff = NULL;
	SOCKADDR_IN tSockAddr = { 0 };

	try
	{
		InitServerSocket(pfnAcceptFunc);
		while (TRUE)
		{
			pSocketList = GetSocketList();
			iEventIndex = WSAWaitForMultipleEvents(m_vecSocketEvent.size(), pSocketList, FALSE, WSA_INFINITE, FALSE);
			hSocket = GetSocketByWSAEvent(pSocketList[iEventIndex - WSA_WAIT_EVENT_0]);
			pfnCallback = GetCallbackByWSAEvent(pSocketList[iEventIndex - WSA_WAIT_EVENT_0]);
			assert(hSocket != INVALID_SOCKET);
			WSAEnumNetworkEvents(hSocket, pSocketList[iEventIndex - WSA_WAIT_EVENT_0], &tNetEvents);
			if (FD_ACCEPT & tNetEvents.lNetworkEvents)
			{
				if (tNetEvents.iErrorCode[FD_ACCEPT_BIT])
					throw std::exception("接受客户端连接时发生错误！");
				iEventIndex = sizeof(tSockAddr);
				hSocketAccept = accept(hSocket, (SOCKADDR*)&tSockAddr, &iEventIndex);
				hWSAEvent = WSACreateEvent();
				WSAEventSelect(hSocketAccept, hWSAEvent, FD_READ | FD_CLOSE | FD_WRITE);
				AddSocketEvent(hSocketAccept, hWSAEvent, pfnRecvFunc);
				printf("New client come in...\n");
				pfnCallback(hSocket, FD_ACCEPT);
				send(hSocketAccept, "You are welcome", strlen("You are welcome") + 1, 0);
			}
			else if (FD_READ & tNetEvents.lNetworkEvents)
			{
				if (tNetEvents.iErrorCode[FD_READ_BIT])
					throw std::exception("接收数据时发生错误！");
				//printf("client... %s recved!\n", pRecvBuff);
				pfnCallback(hSocket, FD_READ);
			}
			else if (FD_CLOSE & tNetEvents.lNetworkEvents)
			{
				pRecvBuff = new CHAR[1024];
				iEventIndex = sizeof(tSockAddr);
				getpeername(hSocket, (sockaddr*)&tSockAddr, &iEventIndex);
				DeleteSocketEvent(hSocket);
				InetNtopA(AF_INET, &tSockAddr.sin_addr, pRecvBuff, 1024);
				printf("client... %s closed!\n", pRecvBuff);
				delete[] pRecvBuff;
				pfnCallback(hSocket, FD_CLOSE);
			}
			else if (FD_WRITE & tNetEvents.lNetworkEvents)
			{
				printf("Send data to client...\n");
			}
			ReleaseSocketList(pSocketList);
		}
	}
	catch (std::exception &e)
	{
		throw e;
	}
}

VOID CAsyncServer::InitServerSocket(std::function<VOID(SOCKET, INT_PTR)> pfnAcceptFunc)
{
	SOCKET hSocket = INVALID_SOCKET;
	WSAEVENT hWSAEvent = WSA_INVALID_EVENT;
	SOCKADDR_IN siAddr = { 0 };

	try
	{
		hSocket = socket(AF_INET, SOCK_STREAM, 0);
		siAddr.sin_family = AF_INET;
		siAddr.sin_port = htons(m_uPort);
		if (inet_pton(AF_INET, "127.0.0.1", &siAddr.sin_addr) == -1)
			throw std::exception("转换IP地址错误！");
		if (bind(hSocket, (SOCKADDR*)&siAddr, sizeof(siAddr)) == SOCKET_ERROR)
			throw std::exception("绑定IP地址与SOCKET时发生错误！");
		hWSAEvent = WSACreateEvent();
		if (hWSAEvent == WSA_INVALID_EVENT)
			throw std::exception("创建WSA事件句柄发生错误！");
		WSAEventSelect(hSocket, hWSAEvent, FD_ACCEPT);
		if (listen(hSocket, 5) == SOCKET_ERROR)
			throw std::exception("监听端口时发生错误！");
		AddSocketEvent(hSocket, hWSAEvent, pfnAcceptFunc);
	}
	catch (std::exception &e)
	{
		throw e;
	}
}
