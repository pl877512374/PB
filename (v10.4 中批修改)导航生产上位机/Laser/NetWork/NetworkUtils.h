
#if !defined(__NETWORKUTILS_H)
#define __NETWORKUTILS_H
#include "stdafx.h"
#include "Mwic_32.h"
#include "WriteLog.h"
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// �������ӽṹ��
typedef struct tagNetConnection{
	int nID;
	DWORD dwServerIP;
	UINT unServerPort;
	BOOL bConnected;

	char *pcSendDataBuf;
	int nSendDataBufSize;
	LPVOID lpRecvFun;
	LPVOID lpWnd;

	//�ṹ��Ĭ�Ϲ��캯������ʼ������
	void NET_CONNECTION()	
	{
		void Init();
	}
	
	void Init()
	{
		this->dwServerIP = 0xc0a80202;
		this->unServerPort = 0x0000083e;
		this->bConnected = FALSE;
		this->nID = -1;
		this->pcSendDataBuf = NULL;
		this->nSendDataBufSize = 0;
		this->lpRecvFun = NULL;
		this->lpWnd = NULL;
	}
}NET_CONNECTION;

typedef void (*PFCB_NET_RECV)(LPVOID lpParam, char *pDataBuf, int nDataBufSize);

//const int MAX_BLOCK = 10000;     //�봮��SerialUtils.h�еĶ����ظ�����stdafx.h�ж���
const int MAX_CONNECT_NUM = 10;		// ���������


/**************************************************
* �������ƣ�ConnectServer
* ���������1������ 
*           ����1:NET_CONNECTION *pNetConnection
* ���������BOOL 
* �������ܣ�
* �������ߣ�zyy
* ����ʱ�䣺2011-9-19 16:53:14
**************************************************/
BOOL ConnectServer(NET_CONNECTION *pNetConnection);
BOOL ConnectServer7070(NET_CONNECTION *pNetConnection);
BOOL UDP_Connect(NET_CONNECTION *pNetConnection);

/**************************************************
* �������ƣ�DisconnectServer
* ���������1������ 
*           ����1:NET_CONNECTION *pNetConnection
* ���������BOOL 
* �������ܣ�
* �������ߣ�zyy
* ����ʱ�䣺2011-9-19 16:53:19
**************************************************/
BOOL DisconnectServer(NET_CONNECTION *pNetConnection);

BOOL SendData(NET_CONNECTION *pNetConnection);
BOOL UDP_SendData(NET_CONNECTION *pNetConnection, SOCKADDR_IN *pAddrSock);
UINT ConnectThreadProc(LPVOID lpParam);
UINT ConnectThread7070(LPVOID lpParam);
void OnNetRecv(LPVOID lpParam, char *pDataBuf, int nDataBufSize);

void Xor(char* pbuf,int len );
 char Xor_download( char x, char* pbuf,int len );
BOOL checkXor(char* recvbuf,int recvlen);
short  XorMaiKuan_download( short x, short * sendbuf,int sendlen);
#endif