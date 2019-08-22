// DiagnosisDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "DiagnosisDlg.h"
#include "afxdialogex.h"
#include "UserMsg.h"
#include "NetParam.h"

// CDiagnosisDlg 对话框


IMPLEMENT_DYNAMIC(CDiagnosisDlg, CDialogEx)

CDiagnosisDlg::CDiagnosisDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDiagnosisDlg::IDD, pParent)
{

	m_u16FPGAdata = 0;
	m_u16FPGAStateErr = 0;
	m_u16GPIOErr = 0;
	m_f18B20State = 0.0f;
	m_n32ExFlashState = 0;
	m_n32ExTieDianState = 0;
}

CDiagnosisDlg::~CDiagnosisDlg()
{
}

void CDiagnosisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FPGADATA, m_u16FPGAdata);
	DDX_Text(pDX, IDC_EDIT_FPGAADDR, m_u16FPGAaddr);
	DDX_Text(pDX, IDC_EDIT_5300DATA, m_u165300data);
	//  DDX_Text(pDX, IDC_EDIT_5300ADDR, m_u165300addr);
	DDX_Text(pDX, IDC_EDIT_5300ADDR, m_u165300addr);
	DDX_Text(pDX, IDC_EDIT_FPGAERR, m_u16FPGAErr);
	DDX_Control(pDX, IDC_BTN_DIAGNOSISEN, m_btnDiagnosis);
	DDX_Text(pDX, IDC_EDIT_FPGASTATEERR, m_u16FPGAStateErr);
	DDX_Text(pDX, IDC_EDIT_GPIOERR, m_u16GPIOErr);
	DDX_Text(pDX, IDC_EDIT_18B20, m_f18B20State);
	DDX_Text(pDX, IDC_EDIT_EXFLASH, m_n32ExFlashState);
	DDX_Text(pDX, IDC_EDIT_EXTIEDIAN, m_n32ExTieDianState);
}

	//m_u16FPGAErr = pu16Buf[12];
	//m_u16FPGAdata =  pu16Buf[13];
	//m_u16FPGAaddr =  pu16Buf[14];
	//m_u165300data =  pu16Buf[15];
	//m_u165300data =  pu16Buf[16];
BEGIN_MESSAGE_MAP(CDiagnosisDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_DIAGNOSISEN, &CDiagnosisDlg::OnBnClickedBtnDiagnosisen)
	ON_WM_TIMER()
	ON_MESSAGE(WM_DIAPP_DIAGNOSIS_U,OnDiagnosisQuery)
	ON_STN_CLICKED(IDC_STATIC12, &CDiagnosisDlg::OnStnClickedStatic12)
END_MESSAGE_MAP()


// CDiagnosisDlg 消息处理程序

void CDiagnosisDlg::OnBnClickedBtnDiagnosisen()
{
	// TODO: 在此添加控件通知处理程序代码
	CString l_strbtnName;
	m_btnDiagnosis.GetWindowText(l_strbtnName);
	if ("开启检测" == l_strbtnName)
	{
		m_btnDiagnosis.SetWindowText("关闭测试");
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32DI_StartLen, (LPARAM)g_acDI_StartFrameBuf);
		//SetTimer(0,1000,0);
	}
	else if ("关闭测试" == l_strbtnName)
	{
		m_btnDiagnosis.SetWindowText("开启检测");
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32DI_StopLen, (LPARAM)g_acDI_StopFrameBuf);
		//KillTimer(0);
	}
}
void CDiagnosisDlg::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent == 0)
	{

	}
}

LONG CDiagnosisDlg::OnDiagnosisQuery(WPARAM wParam, LPARAM lParam) 
{
	char *pcBuf = (char *)wParam;
	int nBufSize = (int)lParam;

	u16 l_u16len;//数据部分长度，除去前8个字节
	l_u16len = nBufSize - 8;
	u8 *pu8buf = new u8[nBufSize];
	u8 l_u8Bcc_Value;

	memcpy(pu8buf,&pcBuf[8],l_u16len);//去掉前8个字节的头
	l_u8Bcc_Value	= *(pu8buf+l_u16len-1);
	if (l_u8Bcc_Value != BCC_CHECK(pu8buf,l_u16len))
	{
		//m_sLog = "接收 应用界面状态失败(校验错误！):";
		//WriteLog(&m_recOperLog,m_sLog,(unsigned char *)pcBuf,nBufSize);	
		return 0;
	}
	UpdateData(true);
	u8 l_u8VersionLen = (u8)pu8buf[16];//版本号长度
	u8 l_u8VersionLenFPGA = (u8)pu8buf[17];//版本号长度
	CString strtmp;

	//绘制进度条信息
	u16 *pu16Buf = (u16*)(&pu8buf[16]);
	m_u16FPGAErr = pu16Buf[1];
	m_u16FPGAStateErr = pu16Buf[2];
	m_u16FPGAdata = pu16Buf[13];
	m_u16FPGAaddr = pu16Buf[14];

	m_u165300data = pu16Buf[15];
	m_u165300addr = pu16Buf[16];

	m_u16GPIOErr = pu16Buf[3];
	m_f18B20State = pu16Buf[4];
	m_n32ExFlashState = pu16Buf[5];
	m_n32ExTieDianState = pu16Buf[6];
	UpdateData(false);
	return 1;
}


void CDiagnosisDlg::OnStnClickedStatic12()
{
	// TODO: 在此添加控件通知处理程序代码
}
