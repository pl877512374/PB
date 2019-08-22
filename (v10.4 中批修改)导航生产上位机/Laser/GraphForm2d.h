#pragma once
#include "stdafx.h"
#include "EasySize.h"
#include "Scanf2dDlg.h"
#include "Single2dDlg.h"
#include "Scanf3dDlg.h"
#include "UartDataDlg.h"
#include "UserMsg.h"
#include "TemperRecordDlg.h"
#include "SetRdDlg.h"
#include "AGVTestCfg.h"
#include "AGVTestDlg.h"
#include "SIMUAGVTESTDLG.h"
// GraphForm2d 窗体视图

class GraphForm2d : public CFormView
{
	DECLARE_DYNCREATE(GraphForm2d)
	DECLARE_EASYSIZE
protected:
	GraphForm2d();           // 动态创建所使用的受保护的构造函数
	virtual ~GraphForm2d();

public:
	enum { IDD = IDD_GRAPHFORM2D };
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
	virtual void OnInitialUpdate();
	CTabCtrl m_TabGraphSet;
	CScanf2dDlg m_scanf2ddlg;
	CScan2dCfg m_scan2dcfg;
	CSingle2dDlg m_single2ddlg;
	CUartDataDlg m_UartDatadlg;
	CTemperRecordDlg m_TemperRecordDlg;
	CSetRdDlg   m_SetRdDlg;
	AGVTestDlg m_AGVTestDlg;
	CSIMUAGVTESTDLG m_SIMUAGVTESTDLG;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	void DrawLine(LPVOID pAddr,double *pX,double *pY,long nNum);
private:
	bool m_bFisrtEasySize;
public:
//	afx_msg void OnTcnSelchangeTab2dgraph(NMHDR *pNMHDR, LRESULT *pResult);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnTcnSelchangeTab2dgraph(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnClearChart(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnDrawChartSi(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnClearChartSc(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnDrawChartSc(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnMyChartClose(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnUartDataShow(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnUartDataClear(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnTemperRecordShow(WPARAM wParam,LPARAM lParam);
	afx_msg LRESULT OnDianJiRecordShow(WPARAM wParam,LPARAM lParam);
    u8 s_u8MaxMinFlag;
	
	afx_msg void OnClose();
};


