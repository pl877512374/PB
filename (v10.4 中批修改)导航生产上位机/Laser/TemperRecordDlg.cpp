// TemperRecordDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "TemperRecordDlg.h"
#include "afxdialogex.h"


// CTemperRecordDlg �Ի���

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


// CTemperRecordDlg ��Ϣ�������

// CUartDataDlg ��Ϣ�������
BEGIN_EASYSIZE_MAP(CTemperRecordDlg)
    EASYSIZE(IDC_TCHART_TEMPER,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	//EASYSIZE(IDD_SCANF2DPROPDLG,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)	
END_EASYSIZE_MAP

void CTemperRecordDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	
	UPDATE_EASYSIZE;
	// TODO: �ڴ˴������Ϣ����������
}


BOOL CTemperRecordDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	INIT_EASYSIZE;
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_sTemperCfg.Create(IDD_TEMPER_CFG);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
BEGIN_EVENTSINK_MAP(CTemperRecordDlg, CDialogEx)
	ON_EVENT(CTemperRecordDlg, IDC_TCHART_TEMPER, 8, CTemperRecordDlg::OnDblClickTchartTemper, VTS_NONE)
END_EVENTSINK_MAP()


void CTemperRecordDlg::OnDblClickTchartTemper()
{
	// TODO: �ڴ˴������Ϣ����������
	m_sTemperCfg.ShowWindow(true);
}
