#pragma once
#include "tchart1.h"


// CDInterpDlg 对话框

class CDInterpDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDInterpDlg)

public:
	CDInterpDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDInterpDlg();

// 对话框数据
	enum { IDD = IDD_DIALOG_INTERP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_start;
	CString m_savepath;
	int m_interval;
	int m_end;
	int m_movedata;
	afx_msg void OnBnClickedButtonPath();
	afx_msg void OnBnClickedButtonStart();
	CTchart1 m_tchartinterp;
};
