#pragma once


// InputDlg 对话框

class InputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(InputDlg)
private:
	CString strText;
	CString strStaticText;
public:
	InputDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~InputDlg();
	CString GetInputText();
	void SetStaticText(CString& str);
// 对话框数据
	enum { IDD = IDD_INPUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
