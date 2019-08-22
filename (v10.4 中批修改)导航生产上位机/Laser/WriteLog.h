#pragma once

/*#include "stdafx.h"*/

#define SUCCESS 0
#define ERR_OPEN_DB -100
#define ERR_READ_DB -101
#define ERR_WRITE_DB -101
#define ERR_CHANGE_DB_PASSWORD -102
#define ERR_GET_USERKEY -103
#define ERR_SET_USERKEY -104

#define WM_SENDDBDATA WM_USER+100

typedef struct TagKeyDBStruct
{
	CString Date;
	CString ID;
	CString ProductCode;
	CString Key;
}KeyDBStruct;

int  OpenDb(CString& strDbName, CString& strPassword);//打开数据库
int  ChangeDbPassword(CString& strNewPassword, CString& strOldPassword);//更改数据库密码
int  WriteALog(CString& strOpera); //写操作日志
int  ReadAllLog(CString &strLog);//读所有操作日志
int GetUserKey(CString &key);//读取用户密码
int SetUserKey(CString &Key);//设置用户密码
int myStrToByte(CString src,byte *dec,int len);
int WriteKeyLog(CString& Id, CString& ProductCode, CString& Key);//写密钥信息
int ReadKeyDBLog(HWND hWnd,CString& TimeStart,CString& TimeEnd,CString& Id,CString& ProductCode);//读数据库中信息
int  DelKeyDBLog(HWND hWnd, CString& Time,CString& Id,CString& ProductCode);//删除数据库中某条数据
int  CloseDb();//关闭数据库
int WriteFailedIdLog(CString &Id);
int ReadFailedIdLog(CString &Id);
int DeletFailedIdLog(CString &Id);
extern int SaveParamDB(CString str[]);
extern _ConnectionPtr m_pConnection; // 连接对象指针
extern _RecordsetPtr m_pRecordset; // 记录集对象指针
//_CommandPtr m_pCommand; //指令执行指针
// 数据库连接对象
extern HRESULT hr;

extern CString g_strLoginKey;
extern byte g_cszDbPasswordKey[];
extern byte g_szDbOldPassword[16];
extern CString g_strDbOldPassword;

extern byte g_szKeyHead[];
extern byte g_szKeyTail[];
extern CString g_strKeyHead;
extern CString g_strKeyTail;
extern KeyDBStruct KeyDB;