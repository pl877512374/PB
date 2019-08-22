#pragma once
#include "afxwin.h"

extern HANDLE g_FPGAevnet;
extern CEvent Event_SUCCEED;//单包烧写成功
extern CEvent Event_ERROR;//单包烧写错误
extern int FPGAfile_index;  //当前包好

// CFPGADownloadDlg 对话框

class CFPGADownloadDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CFPGADownloadDlg)

public:
	CFPGADownloadDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CFPGADownloadDlg();

// 对话框数据
	enum { IDD = IDD_FPGADOWNLOAD };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

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
