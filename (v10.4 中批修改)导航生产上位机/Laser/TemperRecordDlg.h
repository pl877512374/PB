#pragma once
#include "EasySize.h"
#include "tchart1.h"
#include "TemperCfg.h"

// CTemperRecordDlg 对话框

class CTemperRecordDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTemperRecordDlg)
	DECLARE_EASYSIZE
public:
	CTemperRecordDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTemperRecordDlg();
	CTemperCfg m_sTemperCfg;
	CTchart1 m_ChartTemper;
// 对话框数据
	enum { IDD = IDD_TEMPER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();
	DECLARE_EVENTSINK_MAP()
	void OnDblClickTchartTemper();
	
};
