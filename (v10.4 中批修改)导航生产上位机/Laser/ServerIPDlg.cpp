// ServerIPDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "ServerIPDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// CServerIPDlg �Ի���

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


// CServerIPDlg ��Ϣ�������


void CServerIPDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	m_ServerIPSet.GetAddress(pMainFrame->m_NetConnection.dwServerIP);
	pMainFrame->m_NetConnection.unServerPort = m_u32ServerPortSet;

	CDialogEx::OnOK();
}


void CServerIPDlg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
