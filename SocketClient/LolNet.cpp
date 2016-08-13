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
			MessageBoxW(NULL, L"��������ʧ��! ", L"LSP_Hook", MB_OK);
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
//	�������������
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
		wVersionRequested = MAKEWORD(1, 1);//�汾��1.1
		//1.�����׽��ֿ� 
		err = WSAStartup(wVersionRequested, &wsaData);
		if (err != 0)
		{
			MessageBoxW(NULL, L"Error  at  WSAStartup()\n", L"LSP_Hook", MB_OK);
			__leave;
		}
		pBuff = new WCHAR[1024];

		//�ж��Ƿ����������winsocket�汾���������
		//�����WSACleanup��ֹwinsocket��ʹ�ò�����             

		if (LOBYTE(wsaData.wVersion) != 1 ||
			HIBYTE(wsaData.wVersion) != 1)			__leave;
		//2.�����׽���
		hSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (hSocket == INVALID_SOCKET)
		{
			swprintf_s(pBuff, 1024, L"Error  at  socket():%ld\n", WSAGetLastError());
			MessageBoxW(NULL, pBuff, L"LSP_Hook", MB_OK);
			__leave;
		}

		//3.���ӷ�������
		//addrSrv.sin_addr.S_un.S_addr = inet_addr(LOCAL_ADDR);
		inet_pton(AF_INET, LOCAL_ADDR, &addrSrv.sin_addr);
		//�趨��������IP��ַ
		addrSrv.sin_family = AF_INET;
		addrSrv.sin_port = htons(LOCAL_PORT); //�˿ں�Ҫ��������˱���һ��
		// connect(sockClient,(SOCKADDR *)&addrSrv,sizeof(SOCKADDR));
		//���ӷ�����

		if (connect(hSocket, (SOCKADDR *)&addrSrv, sizeof(SOCKADDR)) < 0)
		{
			hSocket = INVALID_SOCKET;
			//MessageBoxW(NULL, L"���ӷ�����ʧ�ܡ�", L"LSP_Hook", MB_OK);
			printf("[ JFZR_ACTION ]: ���ӷ�����ʧ�ܡ�");
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
//	��ʼ����������
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
	wVersionRequested = MAKEWORD(1, 1);//�汾��1.1

	//1.�����׽��ֿ� 
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0) {
		return;
	}

	//�ж��Ƿ����������winsocket�汾���������
	//�����WSACleanup��ֹwinsocket��ʹ�ò�����             

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