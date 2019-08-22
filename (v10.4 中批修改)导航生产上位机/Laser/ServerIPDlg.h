#pragma once
#include "NetWork\NetworkUtils.h"

// CServerIPDlg 对话框

class CServerIPDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CServerIPDlg)

public:
	CServerIPDlg(CWnd* pParent = NULL);   // 标准构造函数
	CServerIPDlg(NET_CONNECTION sNetConnect);
	virtual ~CServerIPDlg();

// 对话框数据
	enum { IDD = IDD_SERVERIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CIPAddressCtrl m_ServerIPSet;
//	CEdit m_ServerPortSet;
	UINT m_u32ServerPortSet;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
