#pragma once


// CSystemLoad 对话框

class CSystemLoad : public CDialogEx
{
	DECLARE_DYNAMIC(CSystemLoad)

public:
	CSystemLoad(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSystemLoad();

// 对话框数据
	enum { IDD = IDD_SYSTEMLOADDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CComboBox m_CombUser;
	CString m_strKey;
	afx_msg void OnBnClickedCancel();
};
