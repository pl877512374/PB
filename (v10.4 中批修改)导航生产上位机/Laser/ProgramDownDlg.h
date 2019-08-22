#pragma once

#include "DatagramSocket.h"
// CProgramDownDlg �Ի���

class CProgramDownDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CProgramDownDlg)

public:
	CProgramDownDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CProgramDownDlg();
// �Ի�������
	enum { IDD = IDD_PROGRAMDOWNDLG };
	CDatagramSocket * m_pDatagramSocket;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButtonSelfile();
	afx_msg void OnBnClickedButton3();
	afx_msg LRESULT OnReceive(WPARAM wp,LPARAM lp);
	CString m_file_path;
	afx_msg LRESULT MES_reply(WPARAM wp,LPARAM lp);
	UINT m_port1;
	afx_msg void OnEnChangeEditDownpath();
	CIPAddressCtrl m_ipadd;
	UINT m_port;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnBnClickedButtonClearinfo();
};
