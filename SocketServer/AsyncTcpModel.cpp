#include "stdafx.h"
#include "AsyncTcpModel.h"
#include <algorithm>
#include <exception>



CAsyncTcpModel::CAsyncTcpModel(CHAR* pNetAddress, u_short uPort)
	: HostPort(uPort)
{
	WSAStartup(MAKEWORD(2, 2), &m_wsaData);

}

CAsyncTcpModel::~CAsyncTcpModel()
{

	WSACleanup();
}

VOID CAsyncTcpModel::AddSocketEvent(SOCKET hSocket, WSAEVENT SockEvent)
{
	SOCKETEVENTLIST* pSocketEvent = new SOCKETEVENTLIST;

	pSocketEvent->sock = hSocket;
	pSocketEvent->wsaevent = SockEvent;
	m_vecSocketEvent.push_back(pSocketEvent);
}

VOID CAsyncTcpModel::DelSocketEvent(SOCKET hSocket)
{
	PSOCKETEVENTLIST pResult = 0;
	std::vector<PSOCKETEVENTLIST>::iterator pValue = std::find_if(m_vecSocketEvent.begin(), m_vecSocketEvent.end(), [hSocket](std::vector<PSOCKETEVENTLIST>::value_type v) {
		if (v->sock == hSocket)
			return TRUE;
		else
			return FALSE;
	});

	if (pValue != m_vecSocketEvent.end())
	{
		pResult = *pValue;
		m_vecSocketEvent.erase(pValue);
		closesocket(pResult->sock);
		WSACloseEvent(pResult->wsaevent);
	}
}

WSAEVENT* CAsyncTcpModel::GetSocketList()
{
	WSAEVENT* pResult = NULL;
	INT_PTR iCount = 0;

	if (m_vecSocketEvent.size())
	{
		pResult = new WSAEVENT[m_vecSocketEvent.size()];
		for each(auto var in m_vecSocketEvent)
		{
			pResult[iCount++] = var->wsaevent;
		}
	}

	return pResult;
}

VOID CAsyncTcpModel::ReleaseSocketList(WSAEVENT* pListSocket)
{
	delete[] pListSocket;
}

SOCKET CAsyncTcpModel::GetSocketByWSAEvent(WSAEVENT hWSAEvent)
{
	SOCKET hResult = INVALID_SOCKET;

	std::vector<PSOCKETEVENTLIST>::iterator itResult = std::find_if(m_vecSocketEvent.begin(), m_vecSocketEvent.end(), [hWSAEvent](std::vector<PSOCKETEVENTLIST>::value_type v) {
		if (v->wsaevent == hWSAEvent)
			return TRUE;
		else
			return FALSE;
	});

	if (itResult != m_vecSocketEvent.end())
	{
		hResult = (*itResult)->sock;
	}

	return hResult;
}

BOOL CAsyncTcpModel::InitServerSocket(CHAR* pNetAddress)
{
	BOOL bResult = FALSE;
	SOCKET hSocket = INVALID_SOCKET;
	WSAEVENT hWSAEvent = WSA_INVALID_EVENT;
	SOCKADDR_IN siAddr = { 0 };

	try
	{
		hSocket = socket(AF_INET, SOCK_STREAM, 0);
		siAddr.sin_family = AF_INET;
		siAddr.sin_port = htons(HostPort);
		if (inet_pton(AF_INET, pNetAddress, &siAddr.sin_addr) == -1)
			throw std::exception("转换IP地址错误！");
		if (bind(hSocket, (SOCKADDR*)&siAddr, sizeof(siAddr)) == SOCKET_ERROR)
			throw std::exception("绑定IP地址与SOCKET时发生错误！");
		hWSAEvent = WSACreateEvent();
		if (hWSAEvent == WSA_INVALID_EVENT)
			throw std::exception("创建WSA事件句柄发生错误！");
		WSAEventSelect(hSocket, hWSAEvent, FD_ACCEPT);
		if (listen(hSocket, 5) == SOCKET_ERROR)
			throw std::exception("监听端口时发生错误！");
		AddSocketEvent(hSocket, hWSAEvent);
		bResult = TRUE;
	}
	catch (std::exception &e)
	{
		throw e;
	}

	return bResult;
}

VOID CAsyncTcpModel::Run(CHAR* pNetAddress)
{
	SOCKET tSock = INVALID_SOCKET;
	LPSTR pRecvBuff = NULL;
	SOCKADDR_IN tAddr = { 0 }, tComeinAddr = { 0 };
	WSAEVENT hWSAEvent = WSA_INVALID_EVENT;
	INT_PTR iEventIndex = 0;
	WSAEVENT* pSocketList = NULL;
	WSANETWORKEVENTS tNetEvents = { 0 };

	__try
	{
		InitServerSocket(pNetAddress);
		while (TRUE)
		{
			pSocketList = GetSocketList();
			iEventIndex = WSAWaitForMultipleEvents(m_vecSocketEvent.size(), pSocketList, FALSE, WSA_INFINITE, FALSE);
			tSock = GetSocketByWSAEvent(pSocketList[iEventIndex - WSA_WAIT_EVENT_0]);
			assert(tSock != INVALID_SOCKET);
			WSAEnumNetworkEvents(tSock, pSocketList[iEventIndex - WSA_WAIT_EVENT_0], &tNetEvents);
			if (FD_ACCEPT & tNetEvents.lNetworkEvents)
			{
				if (tNetEvents.iErrorCode[FD_ACCEPT_BIT])
				{
					throw std::exception("accept error occured");
				}
				else
				{
					iEventIndex = sizeof(tComeinAddr);
					SOCKET tSockAccept = accept(tSock, (sockaddr*)&tComeinAddr, &iEventIndex);
					hWSAEvent = WSACreateEvent();
					WSAEventSelect(tSockAccept, hWSAEvent, FD_READ | FD_CLOSE);
					AddSocketEvent(tSockAccept, hWSAEvent);
					printf("new come in...\n");
					send(tSockAccept, "You are welcome", strlen("You are welcome") + 1, 0);
				}
			}
			else if (FD_READ & tNetEvents.lNetworkEvents)
			{
				if (tNetEvents.iErrorCode[FD_READ_BIT])
				{
					throw std::exception("read error occured");
				}
				else
				{
					iEventIndex = recv(tSock, pRecvBuff, 1024, 0);
					if(iEventIndex==SOCKET_ERROR)
						break;
					else
					{
						printf("client...data: %s\n", pRecvBuff);
					}
				}
			}
			else if (FD_CLOSE & tNetEvents.lNetworkEvents)
			{
				iEventIndex = sizeof(tComeinAddr);
				getpeername(tSock, (sockaddr*)&tComeinAddr, &iEventIndex);
				DelSocketEvent(tSock);
				InetNtopA(AF_INET, &tComeinAddr.sin_addr, pRecvBuff, 1024);
				printf("client... %s closed!\n", pRecvBuff);
			}
			ReleaseSocketList(pSocketList);
		}

	}
	__finally
	{
		WSACloseEvent(hWSAEvent);
		delete[] pRecvBuff;
		closesocket(tSock);
	}
}
