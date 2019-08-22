#pragma once


// CSetAppDlg 对话框
#include "EasySize.h"
#include "afxwin.h"

class CSetAppDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetAppDlg)
	DECLARE_EASYSIZE
public:
	CSetAppDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CSetAppDlg();

// 对话框数据
	enum { IDD = IDD_SET_APPDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	afx_msg LONG OnProdGetParm(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnHeatBack(WPARAM wParam, LPARAM lParam);
	//afx_msg LONG OnRestLaser(WPARAM wParam, LPARAM lParam);
	float m_fLLevel1;
	UINT m_u8LWaveSet;
	float m_fZeroDisc2;
	float m_fLWholeDisc;
	CComboBox m_CombSi2ScSet;
	CComboBox m_CombErrModify1;
	CComboBox m_CombModify;
	CComboBox m_CombWdgSet;
		char ConvertHexChar(char ch);
	afx_msg void OnBnClickedBtnBasicstateRead();
	afx_msg void OnBnClickedBtnBasicstateWrite();
	afx_msg void OnBnClickedBtnReset();
	CComboBox m_ClkMode;
	CComboBox m_BoTeLv;
	UINT m_MaPan;
	CString m_Version;
	UINT m_ScanRangeNum;
	CComboBox m_Heart;
	float m_WirePowerFrequency;
	CComboBox m_CombFPGAMode;
	int m_LMThreshold;
	int m_Test1;
	int m_Test2;
	int m_fOffCenterAngle;
	CButton m_DownApplicPara;
	CButton m_ReadApplicPara;
};
