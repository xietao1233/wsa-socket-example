#include "stdafx.h"


#define LOCAL_ADDR "127.0.0.1"
#define LOCAL_PORT 6886

SOCKET hSocket = NULL;

BOOL SocketSend(LPVOID pData, UINT_PTR uSize)
{
	BOOL bResult = FALSE;
	INT err = 0;

	__try
	{
		err = send(hSocket, (LPCSTR)pData, uSize, 0);
		if (err == SOCKET_ERROR)
		{
			MessageBoxW(NULL, L"发送数据失败! ", L"LSP_Hook", MB_OK);
			__leave;
		}
		bResult = TRUE;
	}
	__finally
	{

	}

	return bResult;
}

//////////////////////////////////////////////////////////////////////////
//	连接网络服务器
//
SOCKET ConnectServer()
{
	SOCKET hResult = INVALID_SOCKET;
	LPWSTR pBuff = NULL;
	WORD wVersionRequested = 0;
	WSADATA wsaData = { 0 };
	SOCKADDR_IN addrSrv = { 0 };
	INT err = 0;

	__try
	{
		wVersionRequested = MAKEWORD(1, 1);//版本号1.1
		//1.加载套接字库 
		err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0)
		{
			MessageBoxW(NULL, L"Error  at  WSAStartup()\n", L"LSP_Hook", MB_OK);
			__leave;
		}
		pBuff = new WCHAR[1024];

		//判断是否我们请求的winsocket版本，如果不是
		//则调用WSACleanup终止winsocket的使用并返回             

		if (LOBYTE(wsaData.wVersion) != 1 ||
			HIBYTE(wsaData.wVersion) != 1)			__leave;
		//2.创建套接字
		hSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (hSocket == INVALID_SOCKET)
		{
			swprintf_s(pBuff, 1024, L"Error  at  socket():%ld\n", WSAGetLastError());
			MessageBoxW(NULL, pBuff, L"LSP_Hook", MB_OK);
			__leave;
		}

		//3.连接服务器端
		//addrSrv.sin_addr.S_un.S_addr = inet_addr(LOCAL_ADDR);
		inet_pton(AF_INET, LOCAL_ADDR, &addrSrv.sin_addr);
		//设定服务器端IP地址
		addrSrv.sin_family = AF_INET;
		addrSrv.sin_port = htons(LOCAL_PORT); //端口号要与服务器端保持一致
		// connect(sockClient,(SOCKADDR *)&addrSrv,sizeof(SOCKADDR));
		//连接服务器

		if (connect(hSocket, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR)) < 0)
		{
			hSocket = INVALID_SOCKET;
			//MessageBoxW(NULL, L"连接服务器失败。", L"LSP_Hook", MB_OK);
			printf("[ JFZR_ACTION ]: 连接服务器失败。");
			__leave;
		}

		hResult = hSocket;
	}
	__finally
	{
		delete[] pBuff;
	}

	return hSocket;
}

//////////////////////////////////////////////////////////////////////////
//	初始化网络连接
//
VOID InitSocket()
{
	WORD  wVersionRequested;
	WSADATA  wsaData;
// 	CHAR  name[255];
	INT err;
	LPSTR ip = NULL;

// 	PHOSTENT  hostinfo;
	ADDRINFOA *addrinfo = NULL;
	wVersionRequested = MAKEWORD(1, 1);//版本号1.1

	//1.加载套接字库 
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return;
	}

	//判断是否我们请求的winsocket版本，如果不是
	//则调用WSACleanup终止winsocket的使用并返回             

	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion) != 1)
	{
		WSACleanup();
		return;
	}

// 	if (err == 0)
// 	{
// 
// 		if (gethostname(name, sizeof(name)) == 0)
// 		{
// 			if (!GetAddrInfoA(name, NULL, NULL, &addrinfo))
// 			{
// 				ip = inet_ntoa(*(struct  in_addr  *)*hostinfo->h_addr_list);
// 				inet_ntop(AF_INET, &addrinfo,ip,);
// 			}
// 			FreeAddrInfoA(addrinfo);
// 		}
// 
// 		WSACleanup();
// 	}
}

VOID CloseSocket()
{
	closesocket(hSocket);
	WSACleanup();
}