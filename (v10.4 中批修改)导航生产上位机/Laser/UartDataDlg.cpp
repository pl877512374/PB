// UartDataDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "UartDataDlg.h"
#include "afxdialogex.h"


// CUartDataDlg 对话框

IMPLEMENT_DYNAMIC(CUartDataDlg, CDialogEx)

CUartDataDlg::CUartDataDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUartDataDlg::IDD, pParent)
{

}

CUartDataDlg::~CUartDataDlg()
{
}

void CUartDataDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TCHART_UARTDATA, m_ChartUart);
}


BEGIN_MESSAGE_MAP(CUartDataDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CUartDataDlg 消息处理程序
BEGIN_EASYSIZE_MAP(CUartDataDlg)
    EASYSIZE(IDC_TCHART_UARTDATA,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	//EASYSIZE(IDD_SCANF2DPROPDLG,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)	
END_EASYSIZE_MAP

BOOL CUartDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_sUartCfgDlg.Create(IDD_UARTCFG);
	INIT_EASYSIZE;
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CUartDataDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
	// TODO: 在此处添加消息处理程序代码
}
BEGIN_EVENTSINK_MAP(CUartDataDlg, CDialogEx)
	ON_EVENT(CUartDataDlg, IDC_TCHART_UARTDATA, 8, CUartDataDlg::OnDblClickTchartUartdata, VTS_NONE)
END_EVENTSINK_MAP()


void CUartDataDlg::OnDblClickTchartUartdata()
{
	// TODO: 在此处添加消息处理程序代码
	m_sUartCfgDlg.ShowWindow(true);
}
