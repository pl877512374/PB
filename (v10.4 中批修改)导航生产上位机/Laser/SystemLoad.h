#pragma once


// CSystemLoad �Ի���

class CSystemLoad : public CDialogEx
{
	DECLARE_DYNAMIC(CSystemLoad)

public:
	CSystemLoad(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSystemLoad();

// �Ի�������
	enum { IDD = IDD_SYSTEMLOADDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CComboBox m_CombUser;
	CString m_strKey;
	afx_msg void OnBnClickedCancel();
};
