// TemperCfg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "TemperCfg.h"
#include "afxdialogex.h"
#include "NetParam.h"
#include "UserMsg.h"
// CTemperCfg �Ի���

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


// CTemperCfg ��Ϣ�������

void CTemperCfg::OnBnClickedButtonSearchtemp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32TemperRecordReadLen, (LPARAM)g_acRD_TemperRecordReadFrameBuf);
}


void CTemperCfg::OnBnClickedButtonCleartemp()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32TemperRecordClearLen, (LPARAM)g_acRD_TemperRecordClearFrameBuf);
}
