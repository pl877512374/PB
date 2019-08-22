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

int  OpenDb(CString& strDbName, CString& strPassword);//�����ݿ�
int  ChangeDbPassword(CString& strNewPassword, CString& strOldPassword);//�������ݿ�����
int  WriteALog(CString& strOpera); //д������־
int  ReadAllLog(CString &strLog);//�����в�����־
int GetUserKey(CString &key);//��ȡ�û�����
int SetUserKey(CString &Key);//�����û�����
int myStrToByte(CString src,byte *dec,int len);
int WriteKeyLog(CString& Id, CString& ProductCode, CString& Key);//д��Կ��Ϣ
int ReadKeyDBLog(HWND hWnd,CString& TimeStart,CString& TimeEnd,CString& Id,CString& ProductCode);//�����ݿ�����Ϣ
int  DelKeyDBLog(HWND hWnd, CString& Time,CString& Id,CString& ProductCode);//ɾ�����ݿ���ĳ������
int  CloseDb();//�ر����ݿ�
int WriteFailedIdLog(CString &Id);
int ReadFailedIdLog(CString &Id);
int DeletFailedIdLog(CString &Id);
extern int SaveParamDB(CString str[]);
extern _ConnectionPtr m_pConnection; // ���Ӷ���ָ��
extern _RecordsetPtr m_pRecordset; // ��¼������ָ��
//_CommandPtr m_pCommand; //ָ��ִ��ָ��
// ���ݿ����Ӷ���
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