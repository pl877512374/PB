// SetRdDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "SetRdDlg.h"
#include "afxdialogex.h"
#include "UserMsg.h"
#include "NetParam.h"
#include "CAxis.h"
#include "CAxes.h"
#include "CLegend.h"
#include "CSeries.h"
#include "math.h"
#include "CPointSeries.h"
#include "CPointer.h"
#include "CPen0.h"
#include "MainFrm.h"
#include "SetFormView.h"
// CSetRdDlg 对话框
extern u8 g_u8CurClkmode;
u16 s_u16SpeedCnt=0;
IMPLEMENT_DYNAMIC(CSetRdDlg, CDialog)

CSetRdDlg::CSetRdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetRdDlg::IDD, pParent)
{

	
}

CSetRdDlg::~CSetRdDlg()
{
}

void CSetRdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TCHART_MOTORSPEED, m_ChartMotorSpeed);
	DDX_Control(pDX, IDC_BTN_PIDSTART_CHECK, m_MotoSpeedTest);
}


BEGIN_MESSAGE_MAP(CSetRdDlg, CDialog)

	ON_WM_SIZE()
	ON_MESSAGE(WM_SETRD_MOTOSPEED_TEST,OnMotoSpeedTest)
	ON_MESSAGE(WM_MOTOSPEED_TEST,OnMotoSpeedTestTeeChart)
	
	ON_BN_CLICKED(IDC_BTN_PIDSTART_CHECK, &CSetRdDlg::OnBnClickedBtnPidstartCheck)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CSetRdDlg)

    EASYSIZE(IDC_TCHART_MOTORSPEED,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	//EASYSIZE(IDC_TCHART_SINGLE,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	//EASYSIZE(IDD_SCANF2DPROPDLG,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)	
END_EASYSIZE_MAP

// CSetRdDlg 消息处理程序


BOOL CSetRdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//m_slid1111.SetRange(0,600);
	// TODO:  在此添加额外的初始化
	INIT_EASYSIZE;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


//void CSetRdDlg::OnEnChangeEditHlseldis()
//{
//	// TODO:  如果该控件是 RICHEDIT 控件，它将不
//	// 发送此通知，除非重写 CDialog::OnInitDialog()
//	// 函数并调用 CRichEditCtrl().SetEventMask()，
//	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
//
//	// TODO:  在此添加控件通知处理程序代码
//	UpdateData(TRUE);
//	if (m_u16HLSelDis <= 0)
//	{ 
//		m_u16HLSelDis = 0;
//	}	
//	if (m_u16HLSelDis >65535)
//	{ 
//		m_u16HLSelDis = 65535;
//	}
//	UpdateData(FALSE);
//}


void CSetRdDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	
	CRect rs;
	UPDATE_EASYSIZE;
	/*if (IsWindowVisible()&&GetWindow(GW_CHILD)!=NULL)
		UPDATE_EASYSIZE;*/
	/* if (IsWindowVisible()&&GetWindow(GW_CHILD)!=NULL)
	 {
		 this->GetClientRect(&rs);
		 rs.top += 10;
		 rs.bottom -= 5;
		 rs.left += 1;
		 rs.right -= 2;
		 rsJi.top = rs.top;
		 rsJi.bottom = rs.bottom/2-5;
		 rsJi.left = rs.left;
		 rsJi.right = rs.right;
		 m_ChartJi.MoveWindow(&rsJi);

		 rsZhi.top = (rs.bottom-rs.top)/2+5;
		 rsZhi.bottom = rs.bottom;
		 rsZhi.left = rs.left;
		 rsZhi.right = rs.right;
		 m_ChartZhi.MoveWindow(&rsZhi);
	 }*/
	 //IsWindowVisible()&& 
	//if (IsWindowVisible()&& GetWindow(GW_CHILD)!=NULL)
	// {
	//	 this->GetClientRect(&rs);
	//	 rs.top += 10;
	//	 rs.bottom -= 5;
	//	 rs.left += 1;
	//	 rs.right -= 2;

	//	 m_ChartJi.MoveWindow(&rs);
	// }
	// TODO: 在此处添加消息处理程序代码
}

void CSetRdDlg::OnBnClickedBtnPidstartCheck()
{
	CString l_strbtnName;
	m_MotoSpeedTest.GetWindowText(l_strbtnName);
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x08};
	ZhiLing(DianJiTest_zhiling,pn,0);
	if ("开启测试" == l_strbtnName)
	{
	
	DianJiTest_zhiling[26]=0xAA;
	
	}else{
	DianJiTest_zhiling[26]=0xBB;
	}
	AfxGetMainWnd()->PostMessage(WM_SENDCMD,DianJiTest_zhilingSize, (LPARAM)DianJiTest_zhiling);
}

LONG CSetRdDlg::OnMotoSpeedTest(WPARAM wParam, LPARAM lParam)
{
	char *pcBuf = (char *)wParam;
	int nBufSize = (int)lParam;
	u16 l_u16len;//数据部分长度，除去前2个字节,帧头
	l_u16len = nBufSize - 2;
	u16  checktype = 1;  // 校验类型:0 不校验 1 异或校验 2 CRC校验
	u16  checkres=0;
	u8 *pu8buf = new u8[nBufSize];
	memcpy(pu8buf,&pcBuf[2],l_u16len);
	checktype = pu8buf[8];
	switch(checktype) { // 校验过程
			case 0: checkres = TRUE; break;
		  case 1: checkres = CHECK_BCC(&pu8buf[0], l_u16len); break;
			//case 2: checkres = CHECK_CRC(&buf[2], l_u16len); break;
			default: checkres = FALSE; break;
			}
	if (!checkres)
	{
		return 0;
	}
	CString l_strbtnName;
	m_MotoSpeedTest.GetWindowText(l_strbtnName);
	if ("开启测试" == l_strbtnName)
	{
		m_MotoSpeedTest.SetWindowText("关闭测试");
		s_u16SpeedCnt = 0;
		((CSeries)m_ChartMotorSpeed.Series(0)).Clear();	
	}
	else if ("关闭测试" == l_strbtnName)
	{
		m_MotoSpeedTest.SetWindowText("开启测试");	
	}
	delete []pu8buf;
	return 1;
}

LONG CSetRdDlg::OnMotoSpeedTestTeeChart(WPARAM wParam, LPARAM lParam)
{
	char *pcBuf = (char *)wParam;
	int nBufSize = (int)lParam;
	u16 l_u16len;//数据部分长度，除去前2个字节,帧头
	l_u16len = nBufSize - 2;
	u16  checktype = 1;  // 校验类型:0 不校验 1 异或校验 2 CRC校验
	u16  checkres=0;
	u8 *pu8buf = new u8[nBufSize];
	memcpy(pu8buf,&pcBuf[2],l_u16len);
	checktype = pu8buf[8];
	switch(checktype) { // 校验过程
			case 0: checkres = TRUE; break;
		  case 1: checkres = CHECK_BCC(&pu8buf[0], l_u16len); break;
			//case 2: checkres = CHECK_CRC(&buf[2], l_u16len); break;
			default: checkres = FALSE; break;
			}
	if (!checkres)
	{
		return 0;
	}
	
	((CAxis)((CAxes)m_ChartMotorSpeed.get_Axis()).get_Bottom()).put_Maximum(100);//100
	((CAxis)((CAxes)m_ChartMotorSpeed.get_Axis()).get_Bottom()).put_Minimum(0);
	((CAxis)((CAxes)m_ChartMotorSpeed.get_Axis()).get_Bottom()).put_Automatic(false);
	((CAxis)((CAxes)m_ChartMotorSpeed.get_Axis()).get_Left()).put_Automatic(true);
	//CString l_sName;
	//m_MotoSpeedTest.GetWindowText(l_sName);	
	float m_fMSpeed_cur=((pu8buf[24]<<8)+pu8buf[25]);
	//s_u16SpeedCnt++;
	//if(s_u16SpeedCnt < 100)//100
	//{
	//	((CSeries)m_ChartMotorSpeed.Series(0)).Add(m_fMSpeed_cur,"",0);
	//}		
	//else
	//{
	//	s_u16SpeedCnt = 0;
	//	((CSeries)m_ChartMotorSpeed.Series(0)).Clear();
	//}
	if(s_u16SpeedCnt >= 100)//100
	{
		s_u16SpeedCnt = 0;
		((CSeries)m_ChartMotorSpeed.Series(0)).Clear();		
	}		
	((CSeries)m_ChartMotorSpeed.Series(0)).Add(m_fMSpeed_cur,"",0);
	s_u16SpeedCnt++;

	if (g_bAutoMoniterRunning == true)
	{
		if (m_fMSpeed_cur<450 || (m_fMSpeed_cur>500 && (m_fMSpeed_cur<930)) || (m_fMSpeed_cur>1000))
		{
			g_bMoniterFaultedMotor = true;
			g_nFaultMSCount++;
			g_nCurrentMotorSpeed=m_fMSpeed_cur; //当前电机转速
		}
		else
		{
			g_bMoniterFaultedMotor = false;
		}
	}


	delete []pu8buf;
	return 1;
}