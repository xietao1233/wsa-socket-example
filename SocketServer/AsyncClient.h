#pragma once
#include "AsyncSocketModel.h"
class CAsyncClient :
	public CAsyncSocketModel
{
public:
	CAsyncClient(CHAR* pIpAddress, u_short uPort);
	~CAsyncClient();

private:
	std::string m_strIpAddress;
	SOCKET m_hSocket;

	VOID InitServerSocket(std::function<VOID(SOCKET, INT_PTR)> pfnRecvFunc);
	virtual VOID Run(std::function<VOID(SOCKET, INT_PTR)> pfnAcceptFunc, std::function<VOID(SOCKET, INT_PTR)> pfnRecvFunc);			//初始化socket信息

public:
	virtual VOID Run(std::function<VOID(SOCKET, INT_PTR)> pfnRecvFunc);
	INT ClientSend(LPBYTE pData, INT iDataLen, INT iFlags);
};

