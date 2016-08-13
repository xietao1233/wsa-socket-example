#pragma once
#include "AsyncSocketModel.h"
class CAsyncServer :
	public CAsyncSocketModel
{
public:
	CAsyncServer(u_short uPort);
	~CAsyncServer();

	virtual VOID Run(std::function<VOID(SOCKET, INT_PTR)> pfnAcceptFunc, std::function<VOID(SOCKET, INT_PTR)> pfnRecvFunc);

private:
	VOID InitServerSocket(std::function<VOID(SOCKET, INT_PTR)> pfnAcceptFunc);
};

