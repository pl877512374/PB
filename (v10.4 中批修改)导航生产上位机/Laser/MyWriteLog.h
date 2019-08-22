#pragma once

#define MYLOGMAXLEN 100

typedef struct _StructLogFile
{
	CString m_strLog;
	unsigned char au8buf[1050];
	unsigned short au16buflen;
	int m_n32id;
	int m_n32WndType;//用于区分窗口
	int m_n32txtflag;
	CString FileName;
}StructLogFile;

typedef struct _StructMyLog
{
	StructLogFile m_sLogFile[MYLOGMAXLEN];
	unsigned int  m_u32in;
	unsigned int  m_u32out;
}StructMyLog;

extern StructMyLog g_sMyLog;
extern DWORD WINAPI ThreadFuct_MyWriteLog(void *data);
extern void WriteMyLog(StructLogFile l_sMyLog);