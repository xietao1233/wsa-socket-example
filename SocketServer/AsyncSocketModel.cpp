#include "stdafx.h"
#include "AsyncSocketModel.h"
#include <algorithm>


CAsyncSocketModel::CAsyncSocketModel(u_short uPort)
	: m_uPort(uPort)
{
	WSAStartup(MAKEWORD(2, 2), &m_wsaData);

}

CAsyncSocketModel::~CAsyncSocketModel()
{

	ClearSocketHandle();		//清理仍然存在的句柄
	WSACleanup();
}

VOID CAsyncSocketModel::ClearSocketHandle()
{
	for each(auto var in m_vecSocketEvent)
	{
		closesocket(var->sock);
		WSACloseEvent(var->wsaevent);
	}
}

VOID CAsyncSocketModel::AddSocketEvent(SOCKET hSocket, WSAEVENT SockEvent, std::function<VOID(SOCKET, INT_PTR)> pfnCallback)
{
	SOCKETEVENTLIST* pSocketEvent = new SOCKETEVENTLIST;

	pSocketEvent->sock = hSocket;
	pSocketEvent->wsaevent = SockEvent;
	pSocketEvent->callbackfunc = pfnCallback;

	m_vecSocketEvent.push_back(pSocketEvent);
}

VOID CAsyncSocketModel::DeleteSocketEvent(SOCKET hSocket)
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

WSAEVENT* CAsyncSocketModel::GetSocketList()
{
	WSAEVENT* pResult = NULL;
	INT_PTR iCount = 0;

	if (m_vecSocketEvent.size())
	{
		iCount = m_vecSocketEvent.size();
		pResult = new WSAEVENT[iCount];
		iCount = 0;
		for each(auto var in m_vecSocketEvent)
		{
			pResult[iCount++] = var->wsaevent;
		}
	}

	return pResult;
}

VOID CAsyncSocketModel::ReleaseSocketList(WSAEVENT* pListSocket)
{
	delete[] pListSocket;
}

SOCKET CAsyncSocketModel::GetSocketByWSAEvent(WSAEVENT hWSAEvent)
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

std::function<VOID(SOCKET, INT_PTR)> CAsyncSocketModel::GetCallbackByWSAEvent(WSAEVENT hWSAEvent)
{
	std::function<VOID(SOCKET, INT_PTR)> pfnResult;

	std::vector<PSOCKETEVENTLIST>::iterator itResult = std::find_if(m_vecSocketEvent.begin(), m_vecSocketEvent.end(), [hWSAEvent](std::vector<PSOCKETEVENTLIST>::value_type v) {
		if (v->wsaevent == hWSAEvent)
			return TRUE;
		else
			return FALSE;
	});

	if (itResult != m_vecSocketEvent.end())
	{
		pfnResult = (*itResult)->callbackfunc;
	}

	return pfnResult;
}
