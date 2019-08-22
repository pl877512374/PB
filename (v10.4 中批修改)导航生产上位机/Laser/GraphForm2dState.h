#pragma once
#include "EasySize.h"
#include "afxcmn.h"
#include "Si2dCfg.h"
// CGraphForm2dState 窗体视图

class CGraphForm2dState : public CFormView
{
	DECLARE_DYNCREATE(CGraphForm2dState)
	DECLARE_EASYSIZE
protected:
	CGraphForm2dState();           // 动态创建所使用的受保护的构造函数
	virtual ~CGraphForm2dState();

public:
	enum { IDD = IDD_GRAPHFORM2DSTATE };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP();
public:
	virtual void OnInitialUpdate();
//	CSi2dCfg m_sSi2dCfgDlg;
	bool m_bFisrtEasySize;

	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg LONG OnStateQuery(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnStopTimer(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnStartTimer(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnResetNumQuery(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnHeartQuery(WPARAM wParam, LPARAM lParam); 
	afx_msg LONG OnVersion(WPARAM wParam, LPARAM lParam);
	CProgressCtrl m_ProCtrl_MotorTemper;
	CProgressCtrl m_ProCtrl_APDTemper;
	CProgressCtrl m_ProCtrl_APDHV;
	byte m_u8ProCtrlNet;
	float m_fAPDHV;
	float m_fAPDTemper;
//	CEdit m_nHumm;
	float m_fMotorTemper;
	afx_msg void OnBnClickedButton1();
	CString m_strLaserEn;
	CString m_strMotorEn;
	CString m_strProgramVer;
	CString m_strHeartState;
	CProgressCtrl m_ProCtrl_NetConnect;
	CString m_strNetConnect;
	afx_msg void OnPaint();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);

	CString m_strProgramVerFPGA;
//	UINT m_u32IWDGRstNum;
//	UINT m_u32IWDGRstNum;

	afx_msg void OnBnClickedBtnClearRstnum();
//	UINT m_u32FPGA;
//	UINT m_u32FPGAErr;

	CProgressCtrl m_ProCtrl_NetHeart;
//	CString m_strNetHeartFlag;


	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton5();
	//UINT m_u32SysPibFlash;
	//UINT m_u32FactorFlash;
	afx_msg void OnBnClickedBtnClearRstnum2();
	afx_msg void OnBnClickedBtnClearRstnum3();

	UINT m_u16W5300RstNum_Heart;
	UINT m_u16W5300RstNum_SendOk;
	afx_msg void OnBnClickedBtnSearchRstnum();
	CRichEditCtrl m_ResetInfo;
	CString m_HardVersion;
	CProgressCtrl m_ProCtrl_WirelessVoltage;
	float m_fWirelessVoltage;
	afx_msg void OnBnClickedButtonAutomonitor();
	void OnCreateMonitorFile(CString fileName);  //创建文件
	void OnPrepareStartMonitor(CString TestName);  //开始测试

protected:
	afx_msg LRESULT OnWritelog(WPARAM wParam, LPARAM lParam);
public:
	CString m_strProgramVerBOOT;
	afx_msg void OnBnClickedBtnClearDisplay();
	CString m_strHeatingState;
	float m_fInputVoltage;
	CProgressCtrl m_ProCtrl_InputVoltage;
	afx_msg void OnBnClickedBtnSaveproparam();
	afx_msg void OnBnClickedBtnRecoveryparam();
};


