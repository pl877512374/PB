// TemperRecordDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "TemperRecordDlg.h"
#include "afxdialogex.h"


// CTemperRecordDlg 对话框

IMPLEMENT_DYNAMIC(CTemperRecordDlg, CDialogEx)

CTemperRecordDlg::CTemperRecordDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTemperRecordDlg::IDD, pParent)
{

}

CTemperRecordDlg::~CTemperRecordDlg()
{
}

void CTemperRecordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TCHART_TEMPER, m_ChartTemper);
}


BEGIN_MESSAGE_MAP(CTemperRecordDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTemperRecordDlg 消息处理程序

// CUartDataDlg 消息处理程序
BEGIN_EASYSIZE_MAP(CTemperRecordDlg)
    EASYSIZE(IDC_TCHART_TEMPER,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	//EASYSIZE(IDD_SCANF2DPROPDLG,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)	
END_EASYSIZE_MAP

void CTemperRecordDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	UPDATE_EASYSIZE;
	// TODO: 在此处添加消息处理程序代码
}


BOOL CTemperRecordDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	INIT_EASYSIZE;
	// TODO:  在此添加额外的初始化
	m_sTemperCfg.Create(IDD_TEMPER_CFG);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
BEGIN_EVENTSINK_MAP(CTemperRecordDlg, CDialogEx)
	ON_EVENT(CTemperRecordDlg, IDC_TCHART_TEMPER, 8, CTemperRecordDlg::OnDblClickTchartTemper, VTS_NONE)
END_EVENTSINK_MAP()


void CTemperRecordDlg::OnDblClickTchartTemper()
{
	// TODO: 在此处添加消息处理程序代码
	m_sTemperCfg.ShowWindow(true);
}
