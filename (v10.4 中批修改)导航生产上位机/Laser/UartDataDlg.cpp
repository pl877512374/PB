// UartDataDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "UartDataDlg.h"
#include "afxdialogex.h"


// CUartDataDlg �Ի���

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


// CUartDataDlg ��Ϣ�������
BEGIN_EASYSIZE_MAP(CUartDataDlg)
    EASYSIZE(IDC_TCHART_UARTDATA,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	//EASYSIZE(IDD_SCANF2DPROPDLG,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)	
END_EASYSIZE_MAP

BOOL CUartDataDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_sUartCfgDlg.Create(IDD_UARTCFG);
	INIT_EASYSIZE;
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CUartDataDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
	// TODO: �ڴ˴������Ϣ����������
}
BEGIN_EVENTSINK_MAP(CUartDataDlg, CDialogEx)
	ON_EVENT(CUartDataDlg, IDC_TCHART_UARTDATA, 8, CUartDataDlg::OnDblClickTchartUartdata, VTS_NONE)
END_EVENTSINK_MAP()


void CUartDataDlg::OnDblClickTchartUartdata()
{
	// TODO: �ڴ˴������Ϣ����������
	m_sUartCfgDlg.ShowWindow(true);
}
