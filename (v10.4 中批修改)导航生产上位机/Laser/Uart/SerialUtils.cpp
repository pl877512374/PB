#include "StdAfx.h"
#include "SerialUtils.h"

typedef struct{
	BOOL bComOpen;
	BOOL bWrite;
	HANDLE hComm;
	HANDLE hPostMsgEvent;	
	CWinThread *pRecvThread;
	OVERLAPPED osRead;
	OVERLAPPED osWrite;

	SerPortPar serPortPar;

	//结构体默认构造函数，初始化变量
	void CommParam()	
	{
		void Init();
	}
	
	void Init()
	{
		this->bComOpen = FALSE;
		this->pRecvThread = NULL;
		this->bWrite = FALSE;
		this->serPortPar.Init();
	}
}CommParam;

CommParam ComPar[MAX_PORT_NUM];

BOOL InitComm(int nPort)
{
	ComPar[nPort].Init();

	if ((ComPar[nPort].hPostMsgEvent == CreateEvent(NULL,TRUE,TRUE,NULL)))
		return FALSE;
	memset(&ComPar[nPort].osRead,0,sizeof(OVERLAPPED));
	memset(&ComPar[nPort].osWrite,0,sizeof(OVERLAPPED));	
	if ((ComPar[nPort].osRead.hEvent == CreateEvent(NULL,TRUE,FALSE,NULL)))
		return FALSE;
	if ((ComPar[nPort].osWrite.hEvent == CreateEvent(NULL,TRUE,FALSE,NULL)))
		return FALSE;
	return TRUE;
}

DWORD ReadComm(CommParam *pCP, BYTE *buf, DWORD dwLength)
{
	DWORD length = 0;
	COMSTAT comStat;
	DWORD dwErrorFlags;
	ClearCommError(pCP->hComm,&dwErrorFlags,&comStat);
	length = min(dwLength,comStat.cbInQue);
	ReadFile(pCP->hComm,buf,length,&length,&pCP->osRead);
	return length;
}

DWORD WriteComm(CommParam *pCP, BYTE *buf, DWORD dwLength)
{
	BOOL bState;
	DWORD length = dwLength;
	COMSTAT comStat;
	DWORD dwErrorFlags;
	
	ClearCommError(pCP->hComm,&dwErrorFlags,&comStat);
	bState = WriteFile(pCP->hComm,buf,length,&length,&pCP->osWrite);
	if (!bState)
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			GetOverlappedResult(pCP->hComm,&pCP->osWrite,&length,TRUE);
		}
		else
		{
			length = 0;
		}
	}
	return length;
}

int OnCommRecv(CommParam *pCP)
{
	BYTE buf[MAX_BLOCK/4];
	DWORD dwLength;
	PFCALLBACK_RECV callBack = (PFCALLBACK_RECV)pCP->serPortPar.lpRecvFun;
	
	if (!pCP->bComOpen)
	{
		SetEvent(pCP->hPostMsgEvent);
		return 0;
	}
	dwLength = ReadComm(pCP,buf,MAX_BLOCK/4);
	if (callBack != NULL)
		(*callBack)(pCP->serPortPar.lpParam,buf,dwLength);	
	return 1;
}

UINT CommRecvProc(LPVOID pParam)
{
	OVERLAPPED os;
	DWORD dwMask;
	DWORD dwTrans;
	COMSTAT comStat;
	DWORD dwErrorFlags;
	CommParam *pCP = (CommParam *)pParam;
	
	memset(&os,0,sizeof(OVERLAPPED));
	os.hEvent = CreateEvent(NULL,TRUE,FALSE,NULL);
	if (os.hEvent == NULL)
		return (UINT)-1;
	
	while (pCP->bComOpen)
	{
		ClearCommError(pCP->hComm,&dwErrorFlags,&comStat);
		if (comStat.cbInQue)
		{
			WaitForSingleObject(pCP->hPostMsgEvent,INFINITE);
			ResetEvent(pCP->hPostMsgEvent);

			OnCommRecv(pCP);
			continue;
		}
		dwMask = 0;
		if (!WaitCommEvent(pCP->hComm,&dwMask,&os))
		{
			if (GetLastError() == ERROR_IO_PENDING)
			{
				GetOverlappedResult(pCP->hComm,&os,&dwTrans,TRUE);
			}
			else
			{
				CloseHandle(os.hEvent);
				return (UINT)-1;
			}
		}
	}
	CloseHandle(os.hEvent);
	return 0;
}

BOOL OpenComm(SerPortPar *pSerPortPar)
{
	CString sPort;
	CString sBaud;
	COMMTIMEOUTS timeOuts;
	int nPort = pSerPortPar->nPort;
	int nBaud = pSerPortPar->nBaud;
	
	if (nPort<1 || nPort>MAX_PORT_NUM || nBaud<0)
		return FALSE;

	nPort--;

	InitComm(nPort);

	ComPar[nPort].serPortPar = *pSerPortPar;

	sPort.Format("COM%d",nPort+1);
	if (nPort >= 10)
		sPort = "\\\\.\\"+sPort;
	
	ComPar[nPort].hComm = CreateFile(sPort,GENERIC_READ|GENERIC_WRITE,0,NULL,OPEN_EXISTING,
		                               FILE_ATTRIBUTE_NORMAL|FILE_FLAG_OVERLAPPED,NULL);
	if (ComPar[nPort].hComm == INVALID_HANDLE_VALUE)
		return FALSE;
	
	SetupComm(ComPar[nPort].hComm,MAX_BLOCK,MAX_BLOCK);
	SetCommMask(ComPar[nPort].hComm,EV_RXCHAR);
	
	timeOuts.ReadIntervalTimeout = MAXDWORD;
	timeOuts.ReadTotalTimeoutMultiplier = 0;
	timeOuts.ReadTotalTimeoutConstant = 0;
	timeOuts.WriteTotalTimeoutMultiplier = 50;
	timeOuts.WriteTotalTimeoutConstant = 2000;
	SetCommTimeouts(ComPar[nPort].hComm,&timeOuts);
	
	DCB dcb;
	if (!GetCommState(ComPar[nPort].hComm,&dcb))
		return FALSE;

	dcb.fBinary = 1;	
	dcb.BaudRate = nBaud;
	
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;	
	
	ComPar[nPort].serPortPar.nPort = nPort ;
	if (SetCommState(ComPar[nPort].hComm,&dcb))
	{
		ComPar[nPort].pRecvThread = ::AfxBeginThread(CommRecvProc,&ComPar[nPort],THREAD_PRIORITY_NORMAL,
			0,CREATE_SUSPENDED,NULL);
		if (ComPar[nPort].pRecvThread == NULL)
		{
			CloseHandle(ComPar[nPort].hComm);
			return FALSE;
		}
		else
		{
			ComPar[nPort].bComOpen = TRUE;
			ComPar[nPort].pRecvThread->ResumeThread();
		}
	}
	else
	{
		CloseHandle(ComPar[nPort].hComm);
		return FALSE;
	}
	return TRUE;
}

BOOL CloseComm(int nPort)
{
	if (nPort<1 || nPort>MAX_PORT_NUM)
		return FALSE;

	nPort--;
	if (ComPar[nPort].bComOpen)
	{
		SetEvent(ComPar[nPort].hPostMsgEvent);
		SetCommMask(ComPar[nPort].hComm,0);
		WaitForSingleObject(ComPar[nPort].pRecvThread->m_hThread,INFINITE);
		CloseHandle(ComPar[nPort].hComm);
		InitComm(nPort);
		return TRUE;
	}		
	return FALSE;
}

UINT OnCommSend(LPVOID pParam)
{
	CommParam *pCP = (CommParam *)pParam;
	DWORD dwLen;
	int nSendSize;
	PFCALLBACK_SEND callBack = (PFCALLBACK_SEND)pCP->serPortPar.lpSendFun;
	nSendSize = 0;

	while(pCP->bWrite && nSendSize<pCP->serPortPar.nDataBufSize)
	{
		dwLen = MAX_BLOCK/2;
		if (pCP->serPortPar.nDataBufSize-nSendSize < dwLen)
		{
			dwLen = pCP->serPortPar.nDataBufSize - nSendSize;
		}
		nSendSize += WriteComm(pCP,pCP->serPortPar.pDataBuf+nSendSize,dwLen);
	}
	
	pCP->bWrite = FALSE;
	if (callBack != NULL)
	{
		if (nSendSize < pCP->serPortPar.nDataBufSize)
			(*callBack)(pCP->serPortPar.lpParam,1);
		else
			(*callBack)(pCP->serPortPar.lpParam,0);
	}

	return 0;
}

BOOL SendComm(SerPortPar *pSerPortPar)
{
	int nPort;

	nPort = pSerPortPar->nPort;
	if (nPort<1 || nPort>MAX_PORT_NUM)
		return FALSE;

	nPort--;
	ComPar[nPort].serPortPar.pDataBuf = pSerPortPar->pDataBuf;
	ComPar[nPort].serPortPar.nDataBufSize = pSerPortPar->nDataBufSize;
	ComPar[nPort].serPortPar.lpSendFun = pSerPortPar->lpSendFun;
	ComPar[nPort].bWrite = TRUE;
	if(::AfxBeginThread(OnCommSend,&ComPar[nPort],THREAD_PRIORITY_NORMAL,0,0,NULL) == NULL)
		return FALSE;
	return TRUE;
}

BOOL StopSendComm(int nPort)
{
	if (nPort<1 || nPort>MAX_PORT_NUM)
		return FALSE;

	nPort--;
	if (ComPar[nPort].bWrite)
	{
		ComPar[nPort].bWrite = FALSE;
		return TRUE;
	}
	return FALSE;
}