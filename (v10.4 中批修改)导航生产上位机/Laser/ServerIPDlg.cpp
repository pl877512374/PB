// ServerIPDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "ServerIPDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// CServerIPDlg 对话框

IMPLEMENT_DYNAMIC(CServerIPDlg, CDialogEx)

CServerIPDlg::CServerIPDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CServerIPDlg::IDD, pParent)
{

	m_u32ServerPortSet = 0;
}

unsigned long m_dwServerIP;
UINT m_port;
CServerIPDlg::CServerIPDlg(NET_CONNECTION sNetConnect)
{
	m_port = sNetConnect.unServerPort;
	m_dwServerIP = sNetConnect.dwServerIP;

}
CServerIPDlg::~CServerIPDlg()
{
}

void CServerIPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ServerIPSet);
	//  DDX_Control(pDX, IDC_EDIT1, m_ServerPortSet);
	DDX_Text(pDX, IDC_EDIT1, m_u32ServerPortSet);
}


BEGIN_MESSAGE_MAP(CServerIPDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &CServerIPDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CServerIPDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CServerIPDlg 消息处理程序


void CServerIPDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	m_ServerIPSet.GetAddress(pMainFrame->m_NetConnection.dwServerIP);
	pMainFrame->m_NetConnection.unServerPort = m_u32ServerPortSet;

	CDialogEx::OnOK();
}


void CServerIPDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


BOOL CServerIPDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	BYTE b_ip[4]={192,168,0,2};
	m_ServerIPSet.SetAddress(m_dwServerIP);
	/*BYTE b_ip[4]={192,168,2,85};
	m_ServerIPSet.SetAddress(b_ip[0],b_ip[1],b_ip[2],b_ip[3]);*/
	m_u32ServerPortSet = m_port;
	UpdateData(false);
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
