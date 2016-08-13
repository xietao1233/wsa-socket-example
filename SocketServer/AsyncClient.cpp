#include "stdafx.h"
#include "AsyncClient.h"
#include <exception>


CAsyncClient::CAsyncClient(CHAR* pIpAddress, u_short uPort)
	: CAsyncSocketModel(uPort)
	, m_strIpAddress(pIpAddress)
	, m_hSocket(INVALID_SOCKET)
{
}

CAsyncClient::~CAsyncClient()
{
}

VOID CAsyncClient::InitServerSocket(std::function<VOID(SOCKET, INT_PTR)> pfnRecvFunc)
{
	SOCKET hSocket = INVALID_SOCKET;
	SOCKADDR_IN siAddr = { 0 };
	WSAEVENT hWSAEvent = WSA_INVALID_EVENT;

	try
	{
		hSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (hSocket == INVALID_SOCKET)
			throw std::exception("创建SOCKET句柄失败！");
		siAddr.sin_family = AF_INET;
		siAddr.sin_port = htons(m_uPort);
		if (inet_pton(AF_INET, m_strIpAddress.c_str(), &siAddr.sin_addr)==-1)
			throw std::exception("转换IP地址错误！");
		if (connect(hSocket, (SOCKADDR*)&siAddr, sizeof(SOCKADDR)) == SOCKET_ERROR)
			throw std::exception("连接服务器失败");
		hWSAEvent = WSACreateEvent();
		WSAEventSelect(hSocket, hWSAEvent, FD_READ | FD_WRITE | FD_CLOSE);
		AddSocketEvent(hSocket, hWSAEvent, pfnRecvFunc);
		m_hSocket = hSocket;
	}
	catch (std::exception &e)
	{
		throw e;
	}
}

VOID CAsyncClient::Run(std::function<VOID(SOCKET, INT_PTR)> pfnRecvFunc)
{
	WSAEVENT* pSocketList = NULL;
	INT_PTR iEventIndex = 0;
	SOCKET hSocket = INVALID_SOCKET;
	std::function<VOID(SOCKET, INT_PTR)> pfnCallback;
	WSANETWORKEVENTS tNetEvents = { 0 };

	try
	{
		InitServerSocket(pfnRecvFunc);
		while(TRUE)
		{
			pSocketList = GetSocketList();
			iEventIndex = WSAWaitForMultipleEvents(m_vecSocketEvent.size(), pSocketList, FALSE, WSA_INFINITE, TRUE);
			if (iEventIndex != WSA_WAIT_IO_COMPLETION && iEventIndex != WSA_WAIT_TIMEOUT && iEventIndex != WSA_WAIT_FAILED)
			{
				hSocket = GetSocketByWSAEvent(pSocketList[iEventIndex - WSA_WAIT_EVENT_0]);
				pfnCallback = GetCallbackByWSAEvent(pSocketList[iEventIndex - WSA_WAIT_EVENT_0]);
				assert(hSocket != INVALID_SOCKET);
				WSAEnumNetworkEvents(hSocket, pSocketList[iEventIndex - WSA_WAIT_EVENT_0], &tNetEvents);
				if (FD_READ & tNetEvents.lNetworkEvents)
				{
					if (tNetEvents.iErrorCode[FD_READ_BIT])
						throw std::exception("接收数据时发生错误！");
					printf("Client: 接收数据...\n");
					pfnCallback(hSocket, FD_READ);
				}
				else if (FD_CLOSE & tNetEvents.lNetworkEvents)
				{
					if (tNetEvents.iErrorCode[FD_CLOSE_BIT])
						throw std::exception("关闭连接时发生错误！");
					printf("Client: 关闭连接...\n");
					pfnCallback(hSocket, FD_CLOSE);
				}
				else if (FD_WRITE & tNetEvents.lNetworkEvents)
				{
					if (tNetEvents.iErrorCode[FD_WRITE_BIT])
						throw std::exception("发送数据时发生错误！");
					printf("Client: 发送数据...\n");
					pfnCallback(hSocket, FD_WRITE);
				}
			}

			ReleaseSocketList(pSocketList);
		}
	}
	catch (std::exception &e)
	{
		throw e;
	}
}

VOID CAsyncClient::Run(std::function<VOID(SOCKET, INT_PTR)> pfnAcceptFunc, std::function<VOID(SOCKET, INT_PTR)> pfnRecvFunc)
{

}

INT CAsyncClient::ClientSend(LPBYTE pData, INT iDataLen, INT iFlags)
{
	return send(m_hSocket, (CONST CHAR*)pData, iDataLen, iFlags);
}
