// SocketClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "..\\SocketServer\\AsyncClient.h"

// SOCKET ConnectServer();
// VOID InitSocket();
// VOID CloseSocket();

CAsyncClient* client = NULL;
SOCKET hSocketConnect = INVALID_SOCKET;
CHAR szMsg[] = "========== Rebuild All: 2 succeeded, 0 failed, 0 skipped ==========";


VOID MyHandle(SOCKET hSocket, INT_PTR iType)
{
	LPSTR pBuff = NULL;

	hSocketConnect = hSocket;
	if (iType == FD_READ)
	{
		//接收数据
		pBuff = new CHAR[1024];

		recv(hSocket, pBuff, 1024, 0);
		printf("Recv server data: %s\n", pBuff);
		delete[] pBuff;
	}

}

VOID WINAPI MyAPCFunc(ULONG_PTR dwParam)
{
	client->ClientSend((LPBYTE)dwParam, strlen((LPSTR)dwParam) + 1, 0);
}

INT_PTR SocketThread(LPARAM lParam)
{
	client = new CAsyncClient("127.0.0.1", 6886);
	SetEvent((HANDLE)lParam);

	client->Run(MyHandle);

	delete client;

	return 0;
}

int main()
{
	HANDLE hThread = 0, hEvent = 0;

	hEvent = CreateEventW(NULL, FALSE, FALSE, NULL);
	hThread = (HANDLE)_beginthreadex(NULL, NULL, (unsigned(__stdcall *)(void *))SocketThread, hEvent, NULL, NULL);
	WaitForSingleObject(hEvent, INFINITE);

	while (hSocketConnect == INVALID_SOCKET)
	{
		Sleep(100);
	}
	//client->ClientSend((LPBYTE)szMsg, sizeof(szMsg), 0);
	getchar();
	QueueUserAPC(MyAPCFunc, hThread, (ULONG_PTR)szMsg);

	CloseHandle(hEvent);

	getchar();

	return 0;
}

