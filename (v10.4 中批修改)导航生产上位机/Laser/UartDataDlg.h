#pragma once
#include "tchart1.h"
#include "EasySize.h"
#include "UartCfg.h"
// CUartDataDlg 对话框

class CUartDataDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CUartDataDlg)
	DECLARE_EASYSIZE
public:
	CUartDataDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUartDataDlg();

// 对话框数据
	enum { IDD = IDD_UARTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CTchart1 m_ChartUart;
	CUartCfg m_sUartCfgDlg;
	DECLARE_EVENTSINK_MAP()
	void OnDblClickTchartUartdata();
};
