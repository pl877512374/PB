// DustDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "DustDlg.h"
#include "afxdialogex.h"
#include "NetParam.h"
#include "UserMsg.h"
// CDustDlg 对话框

IMPLEMENT_DYNAMIC(CDustDlg, CDialogEx)

CDustDlg::CDustDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDustDlg::IDD, pParent)
{

	m_fDustCh1 = 0.0f;
	m_fDustCh10 = 0.0f;
	m_fDustCh2 = 0.0f;
	m_fDustCh3 = 0.0f;
	m_fDustCh4 = 0.0f;
	m_fDustCh5 = 0.0f;
	m_fDustCh6 = 0.0f;
	m_fDustCh7 = 0.0f;
	m_fDustCh8 = 0.0f;
	m_fDustCh9 = 0.0f;
	m_fDust_M = 0.0f;
}

CDustDlg::~CDustDlg()
{
}

void CDustDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DUSTCH1, m_fDustCh1);
	DDX_Text(pDX, IDC_EDIT_DUSTCH10, m_fDustCh10);
	DDX_Text(pDX, IDC_EDIT_DUSTCH2, m_fDustCh2);
	DDX_Text(pDX, IDC_EDIT_DUSTCH3, m_fDustCh3);
	DDX_Text(pDX, IDC_EDIT_DUSTCH4, m_fDustCh4);
	DDX_Text(pDX, IDC_EDIT_DUSTCH5, m_fDustCh5);
	DDX_Text(pDX, IDC_EDIT_DUSTCH6, m_fDustCh6);
	DDX_Text(pDX, IDC_EDIT_DUSTCH7, m_fDustCh7);
	DDX_Text(pDX, IDC_EDIT_DUSTCH8, m_fDustCh8);
	DDX_Text(pDX, IDC_EDIT_DUSTCH9, m_fDustCh9);
	DDX_Text(pDX, IDC_EDIT_DUST_M, m_fDust_M);
	DDX_Control(pDX, IDC_CHECK1, m_ChBxDust1);
	DDX_Control(pDX, IDC_CHECK2, m_ChBxDust2);
	DDX_Control(pDX, IDC_BTN_DUSTSEARCH, m_btnDustSearch);
	DDX_Control(pDX, IDC_BTN_DUSTINIT, m_btnDustInit);
	/*DDX_Control(pDX, IDC_BTN_CHN1TEST, m_btn_chn1test);
	DDX_Control(pDX, IDC_BTN_CHN2TEST, m_btn_chn2test);
	DDX_Control(pDX, IDC_BTN_CHN3TEST, m_btn_chn3test);
	DDX_Control(pDX, IDC_BTN_CHN4TEST, m_btn_chn4test);
	DDX_Control(pDX, IDC_BTN_CHN5TEST, m_btn_chn5test);
	DDX_Control(pDX, IDC_BTN_CHN6TEST, m_btn_chn6test);*/
	//DDX_Control(pDX, IDC_BUTTON4, m_btn_DustTest);
	//DDX_Control(pDX, IDC_BTN_CHNLTEST, m_btn_chnltest);
	//DDX_Control(pDX, IDC_BTN_CHNYTEST, m_btn_chnrtest);
}


BEGIN_MESSAGE_MAP(CDustDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_DUSTINIT, &CDustDlg::OnBnClickedBtnDustinit)
	ON_BN_CLICKED(IDC_BTN_DUSTSEARCH, &CDustDlg::OnBnClickedBtnDustsearch)
	ON_MESSAGE(WM_DUST_U,OnDustR)
	ON_BN_CLICKED(IDC_CHECK1, &CDustDlg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_CHECK2, &CDustDlg::OnBnClickedCheck2)
	//ON_BN_CLICKED(IDC_BUTTON4, &CDustDlg::OnBnClickedButton4)
	/*ON_BN_CLICKED(IDC_BTN_CHN1TEST, &CDustDlg::OnBnClickedBtnChn1test)
	ON_BN_CLICKED(IDC_BTN_CHN2TEST, &CDustDlg::OnBnClickedBtnChn2test)
	ON_BN_CLICKED(IDC_BTN_CHN3TEST, &CDustDlg::OnBnClickedBtnChn3test)
	ON_BN_CLICKED(IDC_BTN_CHN4TEST, &CDustDlg::OnBnClickedBtnChn4test)
	ON_BN_CLICKED(IDC_BTN_CHN5TEST, &CDustDlg::OnBnClickedBtnChn5test)
	ON_BN_CLICKED(IDC_BTN_CHN6TEST, &CDustDlg::OnBnClickedBtnChn6test)
	ON_BN_CLICKED(IDC_BTN_CHNLTEST, &CDustDlg::OnBnClickedBtnChnltest)
	ON_BN_CLICKED(IDC_BTN_CHNYTEST, &CDustDlg::OnBnClickedBtnChnytest)*/
END_MESSAGE_MAP()


// CDustDlg 消息处理程序


void CDustDlg::OnBnClickedBtnDustinit()
{
	// TODO: 在此添加控件通知处理程序代码
	INT_PTR nRes = MessageBox(_T("请确定激光器处于常温下，并保持滤光片表面清洁！"),"初始化",MB_OKCANCEL|MB_ICONQUESTION);

	if(nRes == IDCANCEL)
		return;
	//AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_InitLen, (LPARAM)g_acDust_InitBuf);
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x04,0x08,0x02};
	ZhiLing(DustReset_zhiling,pn,0);
	AfxGetMainWnd()->PostMessage(WM_SENDCMD,DustReset_zhilingSize, (LPARAM)DustReset_zhiling);
}


void CDustDlg::OnBnClickedBtnDustsearch()
{
	// TODO: 在此添加控件通知处理程序代码

	UpdateData(true);
	
	/*if(m_ChBxDust1.GetCheck() == FALSE)
	{
		g_acDust_ReadBuf[24] = 0;
	}
	else
	{
		if(m_ChBxDust2.GetCheck() == FALSE)
		{
			g_acDust_ReadBuf[24] = 1;
		}
		else
		{
			g_acDust_ReadBuf[24] = 2;
		}
	}
	g_n32Dust_ReadLen = 26;
	UINT l_u32len = g_n32Dust_ReadLen-8;
	g_acDust_ReadBuf[6] = (l_u32len>> 8);
	g_acDust_ReadBuf[7] = (l_u32len & 0xff);
	UpdateData(false);
	AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_ReadLen, (LPARAM)g_acDust_ReadBuf);*/
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x04,0x08,0x01};
	ZhiLing(DustTest_zhiling,pn,0);
	AfxGetMainWnd()->PostMessage(WM_SENDCMD,DustTest_zhilingSize, (LPARAM)DustTest_zhiling);
}

LONG CDustDlg::OnDustR(WPARAM wParam, LPARAM lParam)       //响应基本参数查询
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
	int index=24;
	m_fDustCh1 =  ((pu8buf[24]<<8)+(pu8buf[25]))/1000.0;
	
	m_fDustCh2 = ((pu8buf[26]<<8)+(pu8buf[27]))/1000.0;
	m_fDustCh3 = ((pu8buf[28]<<8)+(pu8buf[29]))/1000.0;
	m_fDustCh4 =((pu8buf[30]<<8)+(pu8buf[31]))/1000.0;
	m_fDustCh5 = ((pu8buf[32]<<8)+(pu8buf[33]))/1000.0;
	m_fDustCh6 =((pu8buf[34]<<8)+(pu8buf[35]))/1000.0;
	m_fDustCh7 = ((pu8buf[36]<<8)+(pu8buf[37]))/1000.0;
	m_fDustCh8 = ((pu8buf[38]<<8)+(pu8buf[39]))/1000.0;
	m_fDustCh9 = ((pu8buf[40]<<8)+(pu8buf[41]))/1000.0;
	m_fDustCh10 = ((pu8buf[42]<<8)+(pu8buf[43]))/1000.0;
	
	UpdateData(false);
}


void CDustDlg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ChBxDust2.SetCheck(false);
	if(m_ChBxDust1.GetCheck() == FALSE)
	{
		m_ChBxDust2.EnableWindow(false);
	}
	else
	{
		m_ChBxDust2.EnableWindow(true);
	}
}


void CDustDlg::OnBnClickedCheck2()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CDustDlg::OnBnClickedButton4()
{
	// TODO: 在此添加控件通知处理程序代码
	CString l_strbtnName;
	//m_btn_DustTest.GetWindowText(l_strbtnName);
	if ("开始测试" == l_strbtnName)
	{
		//m_btn_DustTest.SetWindowText("关闭测试");
		/*m_btn_chn1test.EnableWindow(TRUE);
		m_btn_chn2test.EnableWindow(TRUE);
		m_btn_chn3test.EnableWindow(TRUE);
		m_btn_chn4test.EnableWindow(TRUE);
		m_btn_chn5test.EnableWindow(TRUE);
		m_btn_chn6test.EnableWindow(TRUE);
		m_btn_chnltest.EnableWindow(TRUE);
		m_btn_chnrtest.EnableWindow(TRUE);*/
	}
	else if ("关闭测试" == l_strbtnName)
	{
		//m_btn_DustTest.SetWindowText("开始测试");
		/*m_btn_chn1test.SetWindowText("通道1");
		m_btn_chn2test.SetWindowText("通道2");
		m_btn_chn3test.SetWindowText("通道3");
		m_btn_chn4test.SetWindowText("通道4");
		m_btn_chn5test.SetWindowText("通道5");
		m_btn_chn6test.SetWindowText("通道6");
		m_btn_chnltest.SetWindowText("左参考");
		m_btn_chnrtest.SetWindowText("右参考");
		m_btn_chn1test.EnableWindow(false);
		m_btn_chn2test.EnableWindow(false);
		m_btn_chn3test.EnableWindow(false);
		m_btn_chn4test.EnableWindow(false);
		m_btn_chn5test.EnableWindow(false);
		m_btn_chn6test.EnableWindow(false);
		m_btn_chnltest.EnableWindow(false);
		m_btn_chnrtest.EnableWindow(false);*/
	}
}


void CDustDlg::OnBnClickedBtnChn1test()
{
	// TODO: 在此添加控件通知处理程序代码
	CString l_strbtnName;
	//m_btn_chn1test.GetWindowText(l_strbtnName);
	if ("通道1" == l_strbtnName)
	{
		m_btn_chn1test.SetWindowText("关闭");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd1;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("请观测第一通道是否亮灯");
	}
	else if ("关闭" == l_strbtnName)
	{
		m_btn_chn1test.SetWindowText("通道1");
		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0x00;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);

	}
}


void CDustDlg::OnBnClickedBtnChn2test()
{
	// TODO: 在此添加控件通知处理程序代码
	CString l_strbtnName;
	m_btn_chn2test.GetWindowText(l_strbtnName);
	if ("通道2" == l_strbtnName)
	{
		m_btn_chn2test.SetWindowText("关闭");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd2;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("请观测第二通道是否亮灯");
	}
	else if ("关闭" == l_strbtnName)
	{
		m_btn_chn2test.SetWindowText("通道2");
		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0x00;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);

	}
}


void CDustDlg::OnBnClickedBtnChn3test()
{
	// TODO: 在此添加控件通知处理程序代码
	CString l_strbtnName;
	m_btn_chn3test.GetWindowText(l_strbtnName);
	if ("通道3" == l_strbtnName)
	{
		m_btn_chn3test.SetWindowText("关闭");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd3;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("请观测第三通道是否亮灯");
	}
	else if ("关闭" == l_strbtnName)
	{
		m_btn_chn3test.SetWindowText("通道3");
		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0x00;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);

	}
}


void CDustDlg::OnBnClickedBtnChn4test()
{
	// TODO: 在此添加控件通知处理程序代码
	CString l_strbtnName;
	m_btn_chn4test.GetWindowText(l_strbtnName);
	if ("通道4" == l_strbtnName)
	{
		m_btn_chn4test.SetWindowText("关闭");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd4;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("请观测第四通道是否亮灯");
	}
	else if ("关闭" == l_strbtnName)
	{
		m_btn_chn4test.SetWindowText("通道4");
		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0x00;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
	}
}


void CDustDlg::OnBnClickedBtnChn5test()
{
	// TODO: 在此添加控件通知处理程序代码
	CString l_strbtnName;
	m_btn_chn5test.GetWindowText(l_strbtnName);
	if ("通道5" == l_strbtnName)
	{
		m_btn_chn5test.SetWindowText("关闭");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd5;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("请观测第五通道是否亮灯");
	}
	else if ("关闭" == l_strbtnName)
	{
		m_btn_chn5test.SetWindowText("通道5");
		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0x00;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
	}
}


void CDustDlg::OnBnClickedBtnChn6test()
{
	// TODO: 在此添加控件通知处理程序代码
	CString l_strbtnName;
	m_btn_chn6test.GetWindowText(l_strbtnName);
	if ("通道6" == l_strbtnName)
	{
		m_btn_chn6test.SetWindowText("关闭");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd6;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("请观测第六通道是否亮灯");
	}
	else if ("关闭" == l_strbtnName)
	{
		m_btn_chn6test.SetWindowText("通道6");
		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0x00;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
	}
}


void CDustDlg::OnBnClickedBtnChnltest()
{
	// TODO: 在此添加控件通知处理程序代码
	CString l_strbtnName;
	m_btn_chnltest.GetWindowText(l_strbtnName);
	if ("左参考" == l_strbtnName)
	{
		m_btn_chnltest.SetWindowText("关闭");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd7;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("请观测左参考通道是否亮灯");
	}
	else if ("关闭" == l_strbtnName)
	{
		m_btn_chnltest.SetWindowText("左参考");
		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0x00;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
	}
}


void CDustDlg::OnBnClickedBtnChnytest()
{
	// TODO: 在此添加控件通知处理程序代码
	CString l_strbtnName;
	m_btn_chnrtest.GetWindowText(l_strbtnName);
	if ("右参考" == l_strbtnName)
	{
		m_btn_chnrtest.SetWindowText("关闭");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd8;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("请观测右参考通道是否亮灯");
	}
	else if ("关闭" == l_strbtnName)
	{
		m_btn_chnrtest.SetWindowText("右参考");
		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0x00;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
	}
}
