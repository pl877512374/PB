#pragma once



// CSetFormView 窗体视图
#include "EasySize.h"
#include "SetAppDlg.h"
#include "SetProdDlg.h"
#include "SetRdDlg.h"
#include "DiagnosisDlg.h"
#include "IssueDlg.h"
#include "ProgramDownDlg.h"
#include "DustDlg.h"
#include "ArmUartDlg.h"
#include "FPGADownloadDlg.h"

class CSetFormView : public CFormView
{
	DECLARE_DYNCREATE(CSetFormView)
	DECLARE_EASYSIZE
protected:
	CSetFormView();           // 动态创建所使用的受保护的构造函数
	virtual ~CSetFormView();

public:
	enum { IDD = IDD_SETFORMVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持
	
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_TabDataSet;
	CSetAppDlg m_SetAppDlg;
	CSetProdDlg m_SetProdDlg;
	//CSetRdDlg   m_SetRdDlg;
	CDiagnosisDlg m_DiagnosisDlg;
	CIssueDlg m_IssueDlg;
	CProgramDownDlg m_ProgramDlg;
	//CDustDlg m_DustDlg;
	CArmUartDlg m_ArmUartDlg;
	CFPGADownloadDlg m_FPGADownloadDlg;
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTcnSelchangeTabDataset(NMHDR *pNMHDR, LRESULT *pResult);
};


