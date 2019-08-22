#pragma once


// CDiagnosisDlg 对话框

class CDiagnosisDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDiagnosisDlg)

public:
	CDiagnosisDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDiagnosisDlg();

// 对话框数据
	enum { IDD = IDD_DIAGNOSISDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	UINT m_u16FPGAdata;
	UINT m_u16FPGAaddr;
	UINT m_u165300data;
//	CString m_u165300addr;
	UINT m_u165300addr;
	UINT m_u16FPGAErr;
	afx_msg void OnBnClickedBtnDiagnosisen();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LONG OnDiagnosisQuery(WPARAM wParam, LPARAM lParam); 
	void WriteLog(CRichEditCtrl *pREC, CString sLog, unsigned char *pbuf, unsigned short u16datalen);
	CButton m_btnDiagnosis;

	UINT m_u16FPGAStateErr;
	UINT m_u16GPIOErr;
	float m_f18B20State;
	afx_msg void OnStnClickedStatic12();
	int m_n32ExFlashState;
	int m_n32ExTieDianState;
};
