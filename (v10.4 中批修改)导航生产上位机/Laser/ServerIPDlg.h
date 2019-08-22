#pragma once
#include "NetWork\NetworkUtils.h"

// CServerIPDlg �Ի���

class CServerIPDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CServerIPDlg)

public:
	CServerIPDlg(CWnd* pParent = NULL);   // ��׼���캯��
	CServerIPDlg(NET_CONNECTION sNetConnect);
	virtual ~CServerIPDlg();

// �Ի�������
	enum { IDD = IDD_SERVERIP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CIPAddressCtrl m_ServerIPSet;
//	CEdit m_ServerPortSet;
	UINT m_u32ServerPortSet;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
