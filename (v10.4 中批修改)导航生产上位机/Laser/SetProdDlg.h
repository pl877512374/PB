#pragma once
#include "afxwin.h"
extern bool f_SetProDialog;
// CSetProdDlg 对话框

class CSetProdDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetProdDlg)

public:
	CSetProdDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetProdDlg();

// 对话框数据
	enum { IDD = IDD_SET_PRODDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	
	virtual BOOL OnInitDialog();
	char ConvertHexChar(char ch);
	afx_msg LONG OnProdAPDPibR(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnProdNetWork(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnProdAPDTest(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnProdMotoPid(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnProdMotoPidSet(WPARAM wParam, LPARAM lParam);
	afx_msg	LONG OnAPDTestBack(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnLaserGLTestBack(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedBtnApdpibWr();
	float m_fAPDHvValue;
	float m_fAPDTemperValue;
	float m_fAPDHV_OP_Ratio;
	float m_fAPDHVTestWr;
	afx_msg void OnBnClickedBtnApdpibRead();
	afx_msg void OnBnClickedBtnProdApdtest();
	afx_msg void OnBnClickedBtnApdtestD1();
	afx_msg void OnBnClickedBtnPidparamUp1();
	afx_msg void OnBnClickedBtnPidparamD1();
	CIPAddressCtrl m_CIPAddSubMaskSer;
	CIPAddressCtrl m_CIPAddGetWaySer;
	CIPAddressCtrl m_CIPAddIPSer;
	CString m_strEquipMac;
	float m_fPID_p1;
	float m_fPID_i1;
	float m_fPID_d1;
	float m_fLaserPower1;
	CComboBox m_CombMotoLoop;
	CComboBox m_CombMotoDir;
	CSliderCtrl m_SliCtrl_APDTest;
	UINT m_u32PortSer;
	CButton m_btnAPDTest;
	CButton m_btnAPDPibWr;
	CButton m_btnAPDTestD1;
	CButton m_btnPIDDn1;
	afx_msg void OnBnClickedBtnNetworkRead();
	afx_msg void OnBnClickedBtnNetworkWr();
	afx_msg void OnNMReleasedcaptureSliderApdtest(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdrawSliderApdtest(NMHDR *pNMHDR, LRESULT *pResult);

	UINT m_MotoSpeed;
	CComboBox m_CombLocateMode;
	CComboBox m_CombDataMode;
	CComboBox m_CombPointsRatio;
	CButton m_btnPIDRead;
};
