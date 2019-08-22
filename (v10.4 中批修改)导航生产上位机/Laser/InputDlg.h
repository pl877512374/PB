#pragma once


// InputDlg �Ի���

class InputDlg : public CDialogEx
{
	DECLARE_DYNAMIC(InputDlg)
private:
	CString strText;
	CString strStaticText;
public:
	InputDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~InputDlg();
	CString GetInputText();
	void SetStaticText(CString& str);
// �Ի�������
	enum { IDD = IDD_INPUT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	virtual BOOL OnInitDialog();
};
