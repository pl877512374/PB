#pragma once
#include "afxwin.h"

extern HANDLE g_FPGAevnet;
extern CEvent Event_SUCCEED;//������д�ɹ�
extern CEvent Event_ERROR;//������д����
extern int FPGAfile_index;  //��ǰ����

// CFPGADownloadDlg �Ի���

class CFPGADownloadDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFPGADownloadDlg)

public:
	CFPGADownloadDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CFPGADownloadDlg();

// �Ի�������
	enum { IDD = IDD_FPGADOWNLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CString m_FPGAfile_path;
	afx_msg void OnBnClickedButtonGetfile();
	afx_msg void OnBnClickedButtonBegindownload();
	afx_msg void OnBnClickedButtonStopdownload();
	afx_msg void OnBnClickedButtonFpgaclearinfo();
	CComboBox m_CombLoadChoose;
	afx_msg void OnSelchangeComboLoadchoose();
};
