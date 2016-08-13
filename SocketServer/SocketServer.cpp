// SocketServer.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
// #include "AsyncTcpModel.h"
#include "AsyncServer.h"

CHAR szMsg[] = "Array that contains any associated error codes, with an array index that corresponds to the position of event bits in lNetworkEvents. ";

VOID MyAccept(SOCKET hSocket, INT_PTR iType)
{

}

VOID MyHandle(SOCKET hSocket, INT_PTR iType)
{
	LPSTR pBuff = NULL;

	if (iType == FD_READ)
	{
		//接受数据
		pBuff = new CHAR[1024];

		recv(hSocket, pBuff, 1024, 0);
		printf("Recv client data: %s\n", pBuff);
		send(hSocket, szMsg, sizeof(szMsg), 0);
		printf("Begin send data !!!\n");
		delete[] pBuff;
	}
}

int main()
{
	//CAsyncTcpModel tcp("127.0.0.1", 6886);
	CAsyncServer server(6886);

	server.Run(MyAccept, MyHandle);

    return 0;
}

