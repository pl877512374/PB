// UartCfg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "UartCfg.h"
#include "UserMsg.h"
#include "NetParam.h"
#include "NetWork\NetworkUtils.h"
#include "afxdialogex.h"
#include "GraphForm2d.h"
#include "MainFrm.h"
// CUartCfg 对话框

IMPLEMENT_DYNAMIC(CUartCfg, CDialogEx)

CUartCfg::CUartCfg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CUartCfg::IDD, pParent)
{

	//  m_fAPDHV = 0.0;
	//  m_DataHUp = 0.0f;
	m_fAPDHv = 0.0f;
	m_fDataHDn = 0.0f;
	m_fDATAHUp = 0.0f;
	m_fDataLDn = 0.0f;
	m_fDataLUp = 0.0f;
	m_fHumm = 0.0f;
	m_fLaserHv = 0.0f;
	m_fSetH = 0.0f;
	m_fSetL = 0.0f;
	m_fStart = 0.0f;
	m_fMotorDa = 0.0f;
	m_fScanfEnd = 0.0f;
	m_fScanfSta = 0.0f;
	m_fPID_P = 0.0f;
	m_fPID_I = 0.0f;
	m_fPID_D = 0.0f;
	m_fZeroPluseNum = 0.0f;
	m_fUartApdTemper = 0.0f;
	m_fUartMotorTemper = 0.0f;
}

CUartCfg::~CUartCfg()
{
}

void CUartCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	//  DDX_Text(pDX, IDC_EDIT_APDHV, m_fAPDHV);
	//  DDX_Text(pDX, IDC_EDIT_DATAHD, m_DataHUp);
	DDX_Text(pDX, IDC_EDIT_APDHV, m_fAPDHv);
	DDX_Text(pDX, IDC_EDIT_DATAHD, m_fDataHDn);
	DDX_Text(pDX, IDC_EDIT_DATAHU, m_fDATAHUp);
	DDX_Text(pDX, IDC_EDIT_DATALD, m_fDataLDn);
	DDX_Text(pDX, IDC_EDIT_DATALU, m_fDataLUp);
	DDX_Text(pDX, IDC_EDIT_HUMM, m_fHumm);
	DDX_Text(pDX, IDC_EDIT_LASERHV, m_fLaserHv);
	DDX_Text(pDX, IDC_EDIT_SETH, m_fSetH);
	DDX_Text(pDX, IDC_EDIT_SETL, m_fSetL);
	DDX_Text(pDX, IDC_EDIT_START, m_fStart);
	DDX_Text(pDX, IDC_EDIT_MOTORDA, m_fMotorDa);
	DDX_Text(pDX, IDC_EDIT_SCANF_END, m_fScanfEnd);
	//  DDX_Control(pDX, IDC_EDIT_SCANF_STA, m_fScanfSta);
	DDX_Text(pDX, IDC_EDIT_SCANF_STA, m_fScanfSta);
	DDX_Control(pDX, IDC_BTN_UARTEN, m_btnUartEn);
	//  DDX_Control(pDX, IDC_EDIT_PID_P, m_fPID_P);
	DDX_Text(pDX, IDC_EDIT_PID_P, m_fPID_P);
	//  DDX_Control(pDX, IDC_EDIT_PID_I, m_fPID_I);
	DDX_Text(pDX, IDC_EDIT_PID_I, m_fPID_I);
	DDX_Text(pDX, IDC_EDIT_PID_D, m_fPID_D);
	DDX_Text(pDX, IDC_EDIT_ZEROPLUSENUM, m_fZeroPluseNum);
	DDX_Text(pDX, IDC_EDIT_UART_APDTEMPER, m_fUartApdTemper);
	DDX_Text(pDX, IDC_EDIT_UART_MOTORTEMPER, m_fUartMotorTemper);
}


BEGIN_MESSAGE_MAP(CUartCfg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_UARTEN, &CUartCfg::OnBnClickedBtnUarten)
	ON_BN_CLICKED(IDC_BTN_UARTCLEAR, &CUartCfg::OnBnClickedBtnUartclear)
END_MESSAGE_MAP()


// CUartCfg 消息处理程序


void CUartCfg::OnBnClickedBtnUarten()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	CString l_strbtnName;
	m_btnUartEn.GetWindowText(l_strbtnName);
	if ("开启检测" == l_strbtnName)
	{
		m_btnUartEn.SetWindowText("关闭检测");
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32UART_StartLen, (LPARAM)g_acUART_StartFrameBuf);
		//SetTimer(0,1000,0);
	}
	else if ("关闭检测" == l_strbtnName)
	{
		m_btnUartEn.SetWindowText("开启检测");
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32UART_StopLen, (LPARAM)g_acUART_StopFrameBuf);
		//KillTimer(0);
	}
}

void CUartCfg::OnBnClickedBtnUartclear()
{
	// TODO: 在此添加控件通知处理程序代码
	GraphForm2d * pView	;
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pView = (GraphForm2d*) pMainFrame->m_wndLeftSplitter.GetPane(0,0);
	// TODO: 在此添加控件通知处理程序代码
	pView->SendMessage(WM_NETCMD_UARTDATA,g_nNetRecvInd,1);
	m_fSetH = 0;
	m_fSetL = 0;
	m_fAPDHv = 0;
	m_fLaserHv = 0;

	m_fHumm = 0;
	m_fScanfSta = 0;
	m_fScanfEnd = 0;
	m_fMotorDa = 0;

	m_fPID_P = 0;
	m_fPID_I =0;
	m_fPID_D = 0;
	m_fZeroPluseNum =0;
	m_fUartApdTemper = 0;
	m_fUartMotorTemper = 0;
	UpdateData(false);
}
