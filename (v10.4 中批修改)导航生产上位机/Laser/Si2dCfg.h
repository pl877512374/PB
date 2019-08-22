#pragma once
#include "afxcmn.h"

#include "NetParam.h"
#include "afxwin.h"
// CSi2dCfg �Ի���
//д��־�߳�
extern HANDLE hthread_WriteLog;
extern DWORD WINAPI ThreadFuct_WriteLog(void *data);
#define MAXLOGINFO 500

//��־�ļ���
extern CString g_strLogfile;
extern CString g_strMeasureDisfile;  
//��־�ļ�ָ��
extern FILE *g_pfLogfile;  
extern FILE *g_pfMeasureDisfile;
extern STRUCT_LOG structLog;
//��ǰӦ�ó���Ŀ¼
extern CString g_strCurDir;
//��־�ļ�Ŀ¼
extern CString g_strLogDir;

extern int PauseSendCount;

//ƫ�ı��������ڸô����������Զ�ƫ�����������б�־
extern bool f_PXAtSi2dCfg;//pl 20190410
class CSi2dCfg : public CDialog
{
	DECLARE_DYNAMIC(CSi2dCfg)

public:
	CSi2dCfg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSi2dCfg();

// �Ի�������
	enum { IDD = IDD_SI2DCFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_nInterval;
	int m_nOneChartNum;
	int m_nCaCuNum;

	
	

	

	void WriteLog(CRichEditCtrl *pREC, CString sLog, unsigned char *pbuf = NULL, unsigned short u16datalen = 0);
	virtual BOOL OnInitDialog();
	afx_msg LONG OnCheckSend(WPARAM wParam, LPARAM lParam);
	afx_msg void OnEnChangeEditInterval();
	afx_msg void OnEnChangeEditOnechart();
	CRichEditCtrl m_recOperLog;
	int m_nRecvNum;
	int m_nSaveNum;
	int m_nDrawNum;
	int m_nsubdata;
	CButton m_btnPauseSend;
	afx_msg void OnBnClickedButtonPauseSend();
	afx_msg void OnBnClickedOk7();
	afx_msg void OnBnClickedButtonRevise();
	CString m_cacpath;
	CRichEditCtrl m_recDataLog;
	CButton m_btnCaculate;
	CString m_sFilePath;

	BOOL bFILEFitting;
	BOOL m_bCacuFlag;
    BOOL m_btnCacuEnable;//�жϼ��㰴ť�Ƿ����
	afx_msg void OnBnClickedOk4();
	afx_msg void OnBnClickedButtonCaculate();
	afx_msg void OnBnClickedButtonFitting();
	CComboBox m_CombChSel;
	afx_msg void OnBnClickedOk8();
	afx_msg void OnBnClickedOk2();
	afx_msg void OnBnClickedButtonSendData();
	afx_msg void OnBnClickedButtonQuery();
	afx_msg void OnEnChangeEditCaculate();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonClean();
	afx_msg void OnEnChangeRicheditOperatemessage();
protected:
	afx_msg LRESULT OnWritelog(WPARAM wParam, LPARAM lParam);
public:
	CComboBox m_DataType;
	int m_ReadAddress;
	afx_msg void OnSelchangeComboDatatype();
	afx_msg void OnChangeEditReadaddress();
};
