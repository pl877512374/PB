// DustDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "DustDlg.h"
#include "afxdialogex.h"
#include "NetParam.h"
#include "UserMsg.h"
// CDustDlg �Ի���

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


// CDustDlg ��Ϣ�������


void CDustDlg::OnBnClickedBtnDustinit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	INT_PTR nRes = MessageBox(_T("��ȷ�����������ڳ����£��������˹�Ƭ������࣡"),"��ʼ��",MB_OKCANCEL|MB_ICONQUESTION);

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
	// TODO: �ڴ���ӿؼ�֪ͨ����������

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

LONG CDustDlg::OnDustR(WPARAM wParam, LPARAM lParam)       //��Ӧ����������ѯ
{
	char *pcBuf = (char *)wParam;
	int nBufSize = (int)lParam;
	u16 l_u16len;//���ݲ��ֳ��ȣ���ȥǰ2���ֽ�,֡ͷ
	l_u16len = nBufSize - 2;
	u16  checktype = 1;  // У������:0 ��У�� 1 ���У�� 2 CRCУ��
	u16  checkres=0;
	u8 *pu8buf = new u8[nBufSize];
	memcpy(pu8buf,&pcBuf[2],l_u16len);
	checktype = pu8buf[8];
	switch(checktype) { // У�����
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CDustDlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString l_strbtnName;
	//m_btn_DustTest.GetWindowText(l_strbtnName);
	if ("��ʼ����" == l_strbtnName)
	{
		//m_btn_DustTest.SetWindowText("�رղ���");
		/*m_btn_chn1test.EnableWindow(TRUE);
		m_btn_chn2test.EnableWindow(TRUE);
		m_btn_chn3test.EnableWindow(TRUE);
		m_btn_chn4test.EnableWindow(TRUE);
		m_btn_chn5test.EnableWindow(TRUE);
		m_btn_chn6test.EnableWindow(TRUE);
		m_btn_chnltest.EnableWindow(TRUE);
		m_btn_chnrtest.EnableWindow(TRUE);*/
	}
	else if ("�رղ���" == l_strbtnName)
	{
		//m_btn_DustTest.SetWindowText("��ʼ����");
		/*m_btn_chn1test.SetWindowText("ͨ��1");
		m_btn_chn2test.SetWindowText("ͨ��2");
		m_btn_chn3test.SetWindowText("ͨ��3");
		m_btn_chn4test.SetWindowText("ͨ��4");
		m_btn_chn5test.SetWindowText("ͨ��5");
		m_btn_chn6test.SetWindowText("ͨ��6");
		m_btn_chnltest.SetWindowText("��ο�");
		m_btn_chnrtest.SetWindowText("�Ҳο�");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString l_strbtnName;
	//m_btn_chn1test.GetWindowText(l_strbtnName);
	if ("ͨ��1" == l_strbtnName)
	{
		m_btn_chn1test.SetWindowText("�ر�");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd1;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("��۲��һͨ���Ƿ�����");
	}
	else if ("�ر�" == l_strbtnName)
	{
		m_btn_chn1test.SetWindowText("ͨ��1");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString l_strbtnName;
	m_btn_chn2test.GetWindowText(l_strbtnName);
	if ("ͨ��2" == l_strbtnName)
	{
		m_btn_chn2test.SetWindowText("�ر�");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd2;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("��۲�ڶ�ͨ���Ƿ�����");
	}
	else if ("�ر�" == l_strbtnName)
	{
		m_btn_chn2test.SetWindowText("ͨ��2");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString l_strbtnName;
	m_btn_chn3test.GetWindowText(l_strbtnName);
	if ("ͨ��3" == l_strbtnName)
	{
		m_btn_chn3test.SetWindowText("�ر�");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd3;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("��۲����ͨ���Ƿ�����");
	}
	else if ("�ر�" == l_strbtnName)
	{
		m_btn_chn3test.SetWindowText("ͨ��3");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString l_strbtnName;
	m_btn_chn4test.GetWindowText(l_strbtnName);
	if ("ͨ��4" == l_strbtnName)
	{
		m_btn_chn4test.SetWindowText("�ر�");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd4;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("��۲����ͨ���Ƿ�����");
	}
	else if ("�ر�" == l_strbtnName)
	{
		m_btn_chn4test.SetWindowText("ͨ��4");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString l_strbtnName;
	m_btn_chn5test.GetWindowText(l_strbtnName);
	if ("ͨ��5" == l_strbtnName)
	{
		m_btn_chn5test.SetWindowText("�ر�");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd5;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("��۲����ͨ���Ƿ�����");
	}
	else if ("�ر�" == l_strbtnName)
	{
		m_btn_chn5test.SetWindowText("ͨ��5");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString l_strbtnName;
	m_btn_chn6test.GetWindowText(l_strbtnName);
	if ("ͨ��6" == l_strbtnName)
	{
		m_btn_chn6test.SetWindowText("�ر�");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd6;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("��۲����ͨ���Ƿ�����");
	}
	else if ("�ر�" == l_strbtnName)
	{
		m_btn_chn6test.SetWindowText("ͨ��6");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString l_strbtnName;
	m_btn_chnltest.GetWindowText(l_strbtnName);
	if ("��ο�" == l_strbtnName)
	{
		m_btn_chnltest.SetWindowText("�ر�");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd7;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("��۲���ο�ͨ���Ƿ�����");
	}
	else if ("�ر�" == l_strbtnName)
	{
		m_btn_chnltest.SetWindowText("��ο�");
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString l_strbtnName;
	m_btn_chnrtest.GetWindowText(l_strbtnName);
	if ("�Ҳο�" == l_strbtnName)
	{
		m_btn_chnrtest.SetWindowText("�ر�");

		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0xd8;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
		MessageBox("��۲��Ҳο�ͨ���Ƿ�����");
	}
	else if ("�ر�" == l_strbtnName)
	{
		m_btn_chnrtest.SetWindowText("�Ҳο�");
		g_n32Dust_Test = 26;
		UINT l_u32len = g_n32Dust_Test-8;
		g_n32Dust_TestBuf[6] = (l_u32len>> 8);
		g_n32Dust_TestBuf[7] = (l_u32len & 0xff);
		g_n32Dust_TestBuf[4] = 0x00;
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Dust_Test, (LPARAM)g_n32Dust_TestBuf);
	}
}
