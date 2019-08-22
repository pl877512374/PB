// TemperCfg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "TemperCfg.h"
#include "afxdialogex.h"
#include "NetParam.h"
#include "UserMsg.h"
// CTemperCfg 对话框

IMPLEMENT_DYNAMIC(CTemperCfg, CDialogEx)

CTemperCfg::CTemperCfg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTemperCfg::IDD, pParent)
{

}

CTemperCfg::~CTemperCfg()
{
}

void CTemperCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTemperCfg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_SEARCHTEMP, &CTemperCfg::OnBnClickedButtonSearchtemp)
	ON_BN_CLICKED(IDC_BUTTON_CLEARTEMP, &CTemperCfg::OnBnClickedButtonCleartemp)
END_MESSAGE_MAP()


// CTemperCfg 消息处理程序

void CTemperCfg::OnBnClickedButtonSearchtemp()
{
	// TODO: 在此添加控件通知处理程序代码
	AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32TemperRecordReadLen, (LPARAM)g_acRD_TemperRecordReadFrameBuf);
}


void CTemperCfg::OnBnClickedButtonCleartemp()
{
	// TODO: 在此添加控件通知处理程序代码
	AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32TemperRecordClearLen, (LPARAM)g_acRD_TemperRecordClearFrameBuf);
}
