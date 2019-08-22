
// MainFrm.cpp : CMainFrame ���ʵ��
//

#include "stdafx.h"
#include "Laser.h"
#include "fcntl.h"
#include <io.h>
#include "MainFrm.h"
#include "EasySize.h"
#include "NetParam.h"
#include "..\Laser\NetWork\NetworkUtils.h"
#include "CAxis.h"
#include "CAxes.h"
#include "CLegend.h"
#include "CSeries.h"
#include "GraphForm2dState.h"
#include "SetFormView.h"
#include "GraphForm3d.h"
#include "GraphForm2d.h"
#include "SystemLoad.h"
#include "InsideApI.h"
#include "MyWriteLog.h"
#include "FPGADownloadDlg.h"
#include <fstream>
#include "io.h"
using namespace std;

////��ȡ����������Ϣ
//#include "Wlanapi.h"
//#pragma comment(lib, "Wlanapi.lib")

#pragma comment(lib,"IPHLPAPI.LIB")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif
extern int  createflag; //�����Ի����־
/* Table of CRC values for high�Corder byte */
static unsigned char auchCRCHi[] = {
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01,
	0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
	0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01,
	0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
	0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81,
	0x40
};

/* Table of CRC values for low�Corder byte */
static char auchCRCLo[] = {
	0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06, 0x07, 0xC7, 0x05, 0xC5, 0xC4,
	0x04, 0xCC, 0x0C, 0x0D, 0xCD, 0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
	0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A, 0x1E, 0xDE, 0xDF, 0x1F, 0xDD,
	0x1D, 0x1C, 0xDC, 0x14, 0xD4, 0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
	0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3, 0xF2, 0x32, 0x36, 0xF6, 0xF7,
	0x37, 0xF5, 0x35, 0x34, 0xF4, 0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
	0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29, 0xEB, 0x2B, 0x2A, 0xEA, 0xEE,
	0x2E, 0x2F, 0xEF, 0x2D, 0xED, 0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
	0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60, 0x61, 0xA1, 0x63, 0xA3, 0xA2,
	0x62, 0x66, 0xA6, 0xA7, 0x67, 0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
	0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68, 0x78, 0xB8, 0xB9, 0x79, 0xBB,
	0x7B, 0x7A, 0xBA, 0xBE, 0x7E, 0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
	0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71, 0x70, 0xB0, 0x50, 0x90, 0x91,
	0x51, 0x93, 0x53, 0x52, 0x92, 0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
	0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B, 0x99, 0x59, 0x58, 0x98, 0x88,
	0x48, 0x49, 0x89, 0x4B, 0x8B, 0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
	0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42, 0x43, 0x83, 0x41, 0x81, 0x80,
	0x40
};
BOOL bJiaoyan = TRUE;
/*��������*/
#define  arrayXYsize  100
extern int g_nRevcNum;          //���հ���
extern int g_nSaveNum;          //�������
extern int g_nCacuNum1;         //�������
extern int g_nCacuNum2;
extern int g_nFirstData;//��һ��������������
extern long  g_nAvgStop1; //ÿn����������ƽ��ֵ
extern long g_nAvgStop2; //ÿn�����½���ƽ��ֵ
extern long g_nSumStop1; //ÿn���������صĺ�
extern long g_nSumStop2; //ÿn�����½��صĺ�
extern int g_nReviseNum;  //ÿ�������������
extern int g_nInterpNum;  //������ϵ������С
extern int g_datax[arrayXYsize];//��ȡ��Ͻ���������
extern int g_datay[arrayXYsize];
extern int g_nXY[100][2];  //��ʱ�������
extern int g_nXYlen;     //����ĳ���

extern CArray<int, int> g_array1; //�����������
extern CArray<int, int> g_array2; //��������׼��ֵ

/*
�����¶Ȳ�ѯ��ɱ�־
*/
u8 g_u8LoadParamDone = 0;
u8 g_u8LoadParamDoneFlag = 0;
int g_nRecvNum = 0;
int g_nDrawNum = 0;
int g_nRecvNum1 = 0;
int g_nRecvNum2 = 0;
int g_nRecvNum3 = 0;
int g_recv1len = 0;
int g_recv2len = 0;
int g_recv3len = 0;
char g_recvother[10000];
int  g_recvotherlen;
int avgdata(0), avgdata1(0), avgdata2(0);

int g_nCount_scanf = 0;
int g_nRecvNum_scanf = 0;          //���հ���
int g_nSaveNum_scanf = 0;          //�������

//���Ӳ���
bool g_bConnectting = false;    //�ж��Ƿ���������
int g_nConnecttingCount = 0;     //�ж��Զ���������

int HeartBeatCount = 0;  //��������ͳ�ƣ���������

//�Զ����
bool g_bAutoMoniterRunning = false;    //�Զ���ر�־
bool g_bMoniterFaultedLost = false;    //��ص���֡�쳣�ı�־
bool g_bMoniterFaultedMotor = false;    //��ص�����쳣�ı�־
bool g_bMoniterFaultedWave = false;    //��ص������쳣�ı�־
int g_nFaultWaveCount = 0;     //���ϲ��δ���
int g_nLostWaveCount = 0; //��֡ͳ�ƴ���
int g_nSendWaveNum = 0;   //���ͼ���
int g_nFaultMSCount = 0;  //���ת���쳣����
float g_nCurrentMotorSpeed = 0; //��ǰ���ת��
int d_landmark[500] = { 0 };

int flag1 = 0;
u8 SystemLoader = 3; //�޸���������޸�Ĭ�ϵ�¼��Աѡ�0λĬ�ϣ�3ΪĬ���з���Ա��¼
DWORD WINAPI ThreadFuct_WriteLogAll(void *data);

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWndEx)

const int  iMaxUserToolbars = 10;
const UINT uiFirstUserToolBarId = AFX_IDW_CONTROLBAR_FIRST + 40;
const UINT uiLastUserToolBarId = uiFirstUserToolBarId + iMaxUserToolbars - 1;

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWndEx)
	ON_MESSAGE(WM_NET_RECV, OnRecvNetData)
	ON_MESSAGE(WM_NET_INI, OnINI)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_CUSTOMIZE, &CMainFrame::OnViewCustomize)
	ON_REGISTERED_MESSAGE(AFX_WM_CREATETOOLBAR, &CMainFrame::OnToolbarCreateNew)
	ON_COMMAND_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnApplicationLook)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_APPLOOK_WIN_2000, ID_VIEW_APPLOOK_WINDOWS_7, &CMainFrame::OnUpdateApplicationLook)
	ON_COMMAND(ID_CONNECT, &CMainFrame::OnConnect)
	ON_UPDATE_COMMAND_UI(ID_CONNECT, &CMainFrame::OnUpdateConnect)

	ON_MESSAGE(WM_SENDCMD, OnNetSendCmd)
	ON_MESSAGE(WM_SENDCMD_TIMER, OnNetSendCmdTimer)
	ON_MESSAGE(WM_CONNECT7070, OnConnect7070)

	ON_WM_CLOSE()
	ON_COMMAND(ID_UDP_BROD, &CMainFrame::OnUdpBrod)
	ON_UPDATE_COMMAND_UI(ID_UDP_BROD, &CMainFrame::OnUpdateUdpBrod)
	//ON_COMMAND(ID_LOAD, &CMainFrame::OnLoad)
	//	ON_UPDATE_COMMAND_UI(ID_LOAD, &CMainFrame::OnUpdateLoad)
	ON_COMMAND(ID_ServerIP, &CMainFrame::OnServerip)
END_MESSAGE_MAP()


BEGIN_EASYSIZE_MAP(CMainFrame)
	EASYSIZE(IDC_TAB_2DGRAPH, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP

static UINT indicators[] =
{
	ID_SEPARATOR,           // ״̬��ָʾ��
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/����


//#define _USER_DEBUG

#ifdef _USER_DEBUG
void OpenConsole()
{
	AllocConsole();
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle,_O_TEXT);

	FILE * hf = _fdopen(hCrt,"w");
	*stdout = *hf;
}

#endif 



CMainFrame::CMainFrame()
{
	EnableActiveAccessibility();
	// TODO: �ڴ���ӳ�Ա��ʼ������
	theApp.m_nAppLook = theApp.GetInt(_T("ApplicationLook"), ID_VIEW_APPLOOK_VS_2008);
	m_wndSplitter_OK = false;

#ifdef _USER_DEBUG
	OpenConsole();
#endif



}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	CheckLicense();
	if (!bJiaoyan)
	{

		return -1;
	}

	if (CFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;

	BOOL bNameValid;
	// ���ڳ־�ֵ�����Ӿ�����������ʽ
	OnApplicationLook(theApp.m_nAppLook);


	if (!m_wndMenuBar.Create(this))
	{
		TRACE0("δ�ܴ����˵���\n");
		return -1;      // δ�ܴ���
	}
	m_wndMenuBar.SetPaneStyle(m_wndMenuBar.GetPaneStyle() | CBRS_SIZE_DYNAMIC | CBRS_TOOLTIPS | CBRS_FLYBY);
	// ��ֹ�˵����ڼ���ʱ��ý���
	CMFCPopupMenu::SetForceMenuFocus(FALSE);

	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(theApp.m_bHiColorIcons ? IDR_MAINFRAME_256 : IDR_MAINFRAME))
	{
		TRACE0("δ�ܴ���������\n");
		return -1;      // δ�ܴ���
	}

	CString strToolBarName;
	bNameValid = strToolBarName.LoadString(IDS_TOOLBAR_STANDARD);
	ASSERT(bNameValid);
	m_wndToolBar.SetWindowText(strToolBarName);

	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);
	m_wndToolBar.EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);

	// �����û�����Ĺ���������:
	//InitUserToolbars(NULL, uiFirstUserToolBarId, uiLastUserToolBarId);

	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("δ�ܴ���״̬��\n");
		return -1;      // δ�ܴ���
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT));

	// TODO: �������ϣ���������Ͳ˵�����ͣ������ɾ��������
	m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockPane(&m_wndMenuBar);
	DockPane(&m_wndToolBar);

	// ���� Visual Studio 2005 ��ʽͣ��������Ϊ
	CDockingManager::SetDockingMode(DT_SMART);
	// ���� Visual Studio 2005 ��ʽͣ�������Զ�������Ϊ
	EnableAutoHidePanes(CBRS_ALIGN_ANY);

	// ���ù�������ͣ�����ڲ˵��滻
	EnablePaneMenu(TRUE, ID_VIEW_CUSTOMIZE, strCustomize, ID_VIEW_TOOLBAR);

	// ���ÿ���(��ס Alt �϶�)�������Զ���
	CMFCToolBar::EnableQuickCustomization();

	if (CMFCToolBar::GetUserImages() == NULL)
	{
		// �����û�����Ĺ�����ͼ��
		if (m_UserImages.Load(_T(".\\UserImages.bmp")))
		{
			CMFCToolBar::SetUserImages(&m_UserImages);
		}
	}

	// ���ò˵����Ի�(���ʹ�õ�����)
	// TODO: �������Լ��Ļ������ȷ��ÿ�������˵�������һ���������
	CList<UINT, UINT> lstBasicCommands;

	lstBasicCommands.AddTail(ID_FILE_NEW);
	lstBasicCommands.AddTail(ID_FILE_OPEN);
	lstBasicCommands.AddTail(ID_FILE_SAVE);
	lstBasicCommands.AddTail(ID_FILE_PRINT);
	lstBasicCommands.AddTail(ID_APP_EXIT);
	lstBasicCommands.AddTail(ID_EDIT_CUT);
	lstBasicCommands.AddTail(ID_EDIT_PASTE);
	lstBasicCommands.AddTail(ID_EDIT_UNDO);
	lstBasicCommands.AddTail(ID_APP_ABOUT);
	lstBasicCommands.AddTail(ID_VIEW_STATUS_BAR);
	lstBasicCommands.AddTail(ID_VIEW_TOOLBAR);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2003);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_VS_2005);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLUE);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_SILVER);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_BLACK);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_OFF_2007_AQUA);
	lstBasicCommands.AddTail(ID_VIEW_APPLOOK_WINDOWS_7);

	CMFCToolBar::SetBasicCommands(lstBasicCommands);


	CRect rs;
	//this->GetClientRect(&rs);

	//rs.top += 25;
	//   rs.bottom -= 4;
	//   rs.left += 1;
	//   rs.right -= 2;
	//
	// m_scanf2ddlg.ShowWindow(true);	

	//m_sBroadCastDlg.Create(IDD_BROADDLG,this);
	////m_sBroadCastDlg.MoveWindow(&rs);
	//m_sBroadCastDlg.ShowWindow(true);
	//m_sSystemLoadDlg.Create(IDD_SYSTEMLOADDLG,this);
	//m_sSystemLoadDlg.MoveWindow(&rs);
	//m_sSystemLoadDlg.ShowWindow(true);
	g_bMenuLoadClicked = false;
	g_bMenuBroadClicked = false;
	////////////////////////////////////////////////////////////20190610 plΪ���ó�ʼ�����ַΪ192.168.0.2��
	DWORD dwIP = ntohl(inet_addr("192.168.0.2"));
	m_NetConnection.dwServerIP = dwIP;
	m_NetConnection.unServerPort = 2110;

	char pBuf[MAX_PATH];//���·���ı���
	GetCurrentDirectory(MAX_PATH, pBuf);//��ȡ����ĵ�ǰĿ¼
	g_strCurDir.Format("%s", pBuf);
	//g_strLogDir = g_strCurDir + "\\Log_TCP\\";
	m_sBroadCastDlg.Create(IDD_BROADDLG, this);
	return 0;
}
u8 g_u8SystemStart = 0;
BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{

	//return m_wndSplitter.Create(this,
	//	2, 2,               // TODO: ��������������
	//	CSize(10, 10),      // TODO: ������С�����С
	//	pContext);
	//��һ�ξ�̬�з�CreateStatic��һ������
	//�ڶ��ξ�̬�з֣�����һ���зֺ�ĵڶ����ٷ�Ϊ2*2�������е�����ͼ������CreateView��������

	//if (//!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CControlForm), CSize(100, 100), pContext) ||
	// !m_wndSplitter.CreateStatic(this,2,2,WS_CHILD|WS_VISIBLE)||  
	//// !m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(TopLView), CSize(350, 240), pContext) ||
	// !m_wndSplitter1.CreateStatic(&m_wndSplitter,1,2,WS_CHILD|WS_VISIBLE,m_wndSplitter.IdFromRowCol(0, 0))|| 
	// !m_wndSplitter1.CreateView(0, 0, RUNTIME_CLASS(CControlForm), CSize(100, 100), pContext) ||
	// !m_wndSplitter1.CreateView(0, 1, RUNTIME_CLASS(TopLView), CSize(100, 100), pContext) ||
	// !m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(BottomLView), CSize(350, 240), pContext) ||
	// !m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(TopRView), CSize(350, 240), pContext) ||
	// !m_wndSplitter.CreateView(1, 1, RUNTIME_CLASS(BottomRView), CSize(350, 240),  pContext))
	// {
	//  m_wndSplitter.DestroyWindow();
	//  return FALSE;
	// }

	m_NetConnection.Init();
	CRect rect;
	GetClientRect(rect);
	m_wndLeftSplitter.CreateStatic(this, 1, 2);

	if (//!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CControlForm), CSize(100, 100), pContext) ||
		!m_wndSplitter.CreateStatic(&(m_wndLeftSplitter), 2, 1, WS_CHILD | WS_VISIBLE, m_wndLeftSplitter.IdFromRowCol(0, 1)) ||
		// !m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(TopLView), CSize(350, 240), pContext) ||
		!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(CSetFormView), CSize(0, 0), pContext) ||
		!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CGraphForm2dState), CSize(0, 0), pContext) ||
		//!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CGraphForm2dState), CSize(0, 0), pContext) ||
		!m_wndLeftSplitter.CreateView(0, 0, RUNTIME_CLASS(GraphForm2d), CSize(0, 0), pContext))
	{
		m_wndSplitter.DestroyWindow();
		m_wndLeftSplitter.DestroyWindow();
		return FALSE;
	}

	//m_NetConnection.Init();
	//if (//!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(CControlForm), CSize(100, 100), pContext) ||
	//	!m_wndSplitter.CreateStatic(this,2,2,WS_CHILD|WS_VISIBLE)||  
	//	// !m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(TopLView), CSize(350, 240), pContext) ||
	//	!m_wndSplitter.CreateView(0, 0, RUNTIME_CLASS(GraphForm2d), CSize(0, 0), pContext) ||
	//	!m_wndSplitter.CreateView(1, 0, RUNTIME_CLASS(GraphForm3d), CSize(0, 0), pContext) ||
	//	!m_wndSplitter.CreateView(0, 1, RUNTIME_CLASS(CGraphForm2dState), CSize(0, 0), pContext) ||
	//	!m_wndSplitter.CreateView(1, 1, RUNTIME_CLASS(CSetFormView), CSize(0, 0),  pContext))
	//{
	//	m_wndSplitter.DestroyWindow();
	//	return FALSE;
	//}

	m_wndLeftSplitter.SetColumnInfo(0, rect.Width() / 2, 10);
	m_wndLeftSplitter.SetColumnInfo(1, rect.Width() / 2, 10);

	m_wndSplitter.SetRowInfo(0, rect.Height() / 2, 10);
	m_wndSplitter.SetRowInfo(1, rect.Height() / 2, 10);
	m_wndSplitter_OK = true;

	this->SetWindowText("�������⴫����20190711�������棩");
	DWORD thread1;
	//CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(ThreadFuct_MyWriteLog), this, 0, &thread1);
	UpdateData(false);

	return true;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CFrameWndEx::PreCreateWindow(cs))
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
		| WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_MAXIMIZE | WS_SYSMENU;
	//  cs.style   =~(WS_MAXIMIZEBOX|WS_MINIMIZEBOX);      
	//cs.style   &=~WS_THICKFRAME;  
	cs.style &= ~FWS_ADDTOTITLE;
	cs.lpszName = _T("ɨ��ʽ���⴫����WLR-P711-0001-20170929-δ��½");

	return TRUE;
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{

	CFrameWndEx::OnSize(nType, cx, cy);
	m_wndSplitter.m_hWnd;
	if (m_wndSplitter_OK)
	{
		CRect rect;
		GetClientRect(rect);

		m_wndLeftSplitter.SetColumnInfo(0, rect.Width() / 2, 10);
		m_wndLeftSplitter.SetColumnInfo(1, rect.Width() / 2, 10);

		m_wndSplitter.SetRowInfo(0, rect.Height() / 2, 10);
		m_wndSplitter.SetRowInfo(1, rect.Height() / 2, 10);
		m_wndSplitter_OK = false;
	}

}

// CMainFrame ���

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWndEx::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CMainFrame ��Ϣ�������

void CMainFrame::OnViewCustomize()
{
	CMFCToolBarsCustomizeDialog* pDlgCust = new CMFCToolBarsCustomizeDialog(this, TRUE /* ɨ��˵�*/);
	pDlgCust->EnableUserDefinedToolbars();
	pDlgCust->Create();
}

LRESULT CMainFrame::OnToolbarCreateNew(WPARAM wp, LPARAM lp)
{
	LRESULT lres = CFrameWndEx::OnToolbarCreateNew(wp, lp);
	if (lres == 0)
	{
		return 0;
	}

	CMFCToolBar* pUserToolbar = (CMFCToolBar*)lres;
	ASSERT_VALID(pUserToolbar);

	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
	return lres;
}

void CMainFrame::OnApplicationLook(UINT id)
{
	CWaitCursor wait;

	theApp.m_nAppLook = id;

	switch (theApp.m_nAppLook)
	{
	case ID_VIEW_APPLOOK_WIN_2000:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManager));
		break;

	case ID_VIEW_APPLOOK_OFF_XP:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOfficeXP));
		break;

	case ID_VIEW_APPLOOK_WIN_XP:
		CMFCVisualManagerWindows::m_b3DTabsXPTheme = TRUE;
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
		break;

	case ID_VIEW_APPLOOK_OFF_2003:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2003));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2005:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2005));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_VS_2008:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerVS2008));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	case ID_VIEW_APPLOOK_WINDOWS_7:
		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows7));
		CDockingManager::SetDockingMode(DT_SMART);
		break;

	default:
		switch (theApp.m_nAppLook)
		{
		case ID_VIEW_APPLOOK_OFF_2007_BLUE:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_LunaBlue);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_BLACK:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_ObsidianBlack);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_SILVER:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Silver);
			break;

		case ID_VIEW_APPLOOK_OFF_2007_AQUA:
			CMFCVisualManagerOffice2007::SetStyle(CMFCVisualManagerOffice2007::Office2007_Aqua);
			break;
		}

		CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));
		CDockingManager::SetDockingMode(DT_SMART);
	}

	RedrawWindow(NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE | RDW_UPDATENOW | RDW_FRAME | RDW_ERASE);

	theApp.WriteInt(_T("ApplicationLook"), theApp.m_nAppLook);
}

void CMainFrame::OnUpdateApplicationLook(CCmdUI* pCmdUI)
{
	pCmdUI->SetRadio(theApp.m_nAppLook == pCmdUI->m_nID);
}

BOOL CMainFrame::LoadFrame(UINT nIDResource, DWORD dwDefaultStyle, CWnd* pParentWnd, CCreateContext* pContext)
{
	// ���ཫִ�������Ĺ���

	if (!CFrameWndEx::LoadFrame(nIDResource, dwDefaultStyle, pParentWnd, pContext))
	{
		return FALSE;
	}


	// Ϊ�����û������������Զ��尴ť
	BOOL bNameValid;
	CString strCustomize;
	bNameValid = strCustomize.LoadString(IDS_TOOLBAR_CUSTOMIZE);
	ASSERT(bNameValid);

	for (int i = 0; i < iMaxUserToolbars; i++)
	{
		CMFCToolBar* pUserToolbar = GetUserToolBarByIndex(i);
		if (pUserToolbar != NULL)
		{
			pUserToolbar->EnableCustomizeButton(TRUE, ID_VIEW_CUSTOMIZE, strCustomize);
		}
	}
	return TRUE;
}
u8 g_u8timer0cnt;
void CMainFrame::OnConnect()
{
	// TODO: �ڴ���������������
	NetConnect();
	GraphForm2d * pView;
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pView = (GraphForm2d*)pMainFrame->m_wndLeftSplitter.GetPane(0, 0);

	pView->s_u8MaxMinFlag = 0;
	//CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	//GraphForm3d *pView3d;
	//pView3d = (GraphForm3d*)pMainFrame->m_wndSplitter.GetPane(1,0);
	//pView3d->PostMessage(WM_NETCMD_DRAW3D,(WPARAM)&g_sNetData,NULL);
}
void CMainFrame::NetConnect()
{
	CString str;
	//BYTE UDPaddr[4];
	//GetDlgItem(ID_CONNECT)->GetWindowText(str);
	//GetDlgItem(ID_CONNECT)->EnableWindow(FALSE);
	m_test = 100;
	g_nRecvNum = 0;
	g_nDrawNum = 0;
	g_nRecvNum1 = 0;
	g_nRecvNum2 = 0;
	g_nRecvNum3 = 0;
	g_recv1len = 0;
	g_recv2len = 0;
	g_recv3len = 0;

	//CSetFormView *pSetView;
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	//pSetView = (CSetFormView*) pMainFrame->m_wndSplitter.GetPane(1,0);
	CGraphForm2dState *pView1;
	pView1 = (CGraphForm2dState*)pMainFrame->m_wndSplitter.GetPane(0, 0);

	KillTimer(0);
	KillTimer(1);
	g_u8LoadParamDone = 0;
	g_u8LoadParamDoneFlag = 0;
	pView1->KillTimer(0);
	pView1->KillTimer(1);
	pView1->KillTimer(2);
	pView1->KillTimer(3);
	//Sleep(2000);
	g_u8timer0cnt = 0;
	g_u8LoadParamDone = 0;

	g_bConnectting = true;
	pThread1 = ::AfxBeginThread(ConnectThreadProc, this);

}

void CMainFrame::OnUpdateConnect(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	//pCmdUI->SetText(_T("�Ͽ�"));
	//pCmdUI->Enable(false);
	if (pCmdUI->m_nID == ID_CONNECT)
	{
		if (!m_NetConnection.bConnected)
		{
			if (g_bConnectting == true)
			{
				CString strTemp = "";
				strTemp.Format("�����У�������%d��", g_nConnecttingCount);
				pCmdUI->SetText(_T(strTemp));
			}
			else
			{
				pCmdUI->SetText(_T("����"));
			}
		}
		else
		{
			pCmdUI->SetText(_T("�Ͽ�"));
		}
	}
}
LRESULT CMainFrame::OnINI(WPARAM wParam, LPARAM lParam){
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CSetFormView *pSetView;
	pSetView = (CSetFormView*)pMainFrame->m_wndSplitter.GetPane(1, 0);
	CGraphForm2dState *pView1;
	pView1 = (CGraphForm2dState*)pMainFrame->m_wndSplitter.GetPane(0, 0);

	//pSetView->m_SetAppDlg.GetDlgItem(IDC_BTN_NetWork_WR)->EnableWindow(false);
	pSetView->m_SetProdDlg.GetDlgItem(IDC_BTN_NetWork_WR)->EnableWindow(false);
	pSetView->m_SetProdDlg.m_btnAPDPibWr.EnableWindow(false);
	pSetView->m_SetProdDlg.m_btnPIDDn1.EnableWindow(false);
	pSetView->m_SetProdDlg.m_btnAPDTestD1.EnableWindow(false);
	pSetView->m_SetProdDlg.m_CIPAddSubMaskSer.SetWindowText(_T(""));
	pSetView->m_SetProdDlg.m_CIPAddGetWaySer.SetWindowText(_T(""));
	pSetView->m_SetProdDlg.m_CIPAddIPSer.SetWindowText(_T(""));
	pSetView->m_SetProdDlg.GetDlgItem(IDC_EDIT_EQUIPMAC)->SetWindowText(_T(""));
	pSetView->m_SetProdDlg.GetDlgItem(IDC_EDIT_APDHVVALUE)->SetWindowText("0");
	pSetView->m_SetProdDlg.GetDlgItem(IDC_EDIT_APDHV_OP_RATIO)->SetWindowText("0");
	pSetView->m_SetProdDlg.GetDlgItem(IDC_EDIT_PORT_SER)->SetWindowText("0");
	pSetView->m_SetProdDlg.GetDlgItem(IDC_EDIT_APDTEMPERVALUE)->SetWindowText("0");
	pSetView->m_SetProdDlg.GetDlgItem(IDC_EDIT_PID_P1)->SetWindowText("0");
	pSetView->m_SetProdDlg.GetDlgItem(IDC_EDIT_PID_I1)->SetWindowText("0");
	pSetView->m_SetProdDlg.GetDlgItem(IDC_EDIT_PID_D1)->SetWindowText("0");
	pSetView->m_SetProdDlg.GetDlgItem(IDC_EDIT_MotoSpeed)->SetWindowText("0");
	pSetView->m_SetProdDlg.m_CombMotoDir.SetCurSel(-1);
	pSetView->m_SetProdDlg.m_CombMotoLoop.SetCurSel(-1);
	/*pSetView->m_SetProdDlg.GetDlgItem(IDC_COMBO_MOTODIR)->SetWindowText(_T(""));
	pSetView->m_SetProdDlg.GetDlgItem(IDC_COMBO_MOTOLOOP)->SetWindowText(_T(""));*/
	pSetView->m_SetProdDlg.GetDlgItem(IDC_EDIT_APDTEST_APDHVTEST)->SetWindowText("0");
	pSetView->m_SetProdDlg.GetDlgItem(IDC_SLIDER_APDTEST)->EnableWindow(FALSE);
	pSetView->m_SetProdDlg.GetDlgItem(IDC_SLIDER_APDTEST)->EnableWindow(FALSE);
	//pSetView->m_SetAppDlg.GetDlgItem(IDC_BTN_BASICSTATE_WRITE)->EnableWindow(FALSE);

	//���Ϻ������
	SetTimer(0, 500, NULL);    //ѭ����״̬��ѯָ��
	SetTimer(1, 5000, NULL);    //��������
	pView1->SetTimer(1, 500, 0);

	return 0;
}
extern int g_nNum;
extern int g_PXnNum;
extern BOOL g_flag;
extern BOOL g_PXflag;
extern HANDLE g_evnet;
extern HANDLE g_PXevnet;
extern HANDLE g_PXDownevnet;
extern HANDLE g_PXAngevent;
extern HANDLE g_PXMap;
extern HANDLE g_MNPXMap;
extern HANDLE g_FPGAevnet;
extern int LaserTimestap;

extern unsigned int nTime1;
extern unsigned int nTime2;
extern unsigned char firstData;
CString displaystr;
LRESULT CMainFrame::OnRecvNetData(WPARAM wParam, LPARAM lParam)
{
	int nBufID = (int)wParam;
	int nChannel;

	int nDlThred = 0;
	int nSglThred = 0;
	CString m_sData = "";
	long int nSum = 0L;//ÿ֡�ĺ�

	GraphForm2d * pView;
	CGraphForm2dState *pView1;
	CSetFormView *pSetView;
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pView = (GraphForm2d*)pMainFrame->m_wndLeftSplitter.GetPane(0, 0);
	pView1 = (CGraphForm2dState*)pMainFrame->m_wndSplitter.GetPane(0, 0);
	pSetView = (CSetFormView*)pMainFrame->m_wndSplitter.GetPane(1, 0);

	int l_nInterval = pView->m_single2ddlg.m_sSi2dCfgDlg.m_nInterval;
	int l_nOneChartNum = pView->m_single2ddlg.m_sSi2dCfgDlg.m_nOneChartNum;
	int l_nCaCuNum = pView->m_single2ddlg.m_sSi2dCfgDlg.m_nCaCuNum;

	if (g_u8NetStateCnt < 100 && g_u8NetStateCnt >= 0)
	{
		g_u8NetStateCnt++;
	}
	else
	{
		g_u8NetStateCnt = 0;
	}

	if (g_cNetRecvBuf[nBufID][0] == (char)0x02 &&
		g_cNetRecvBuf[nBufID][1] == (char)0x02 &&
		g_cNetRecvBuf[nBufID][2] == (char)0x02 &&
		g_cNetRecvBuf[nBufID][3] == (char)0x03)     //sick����,02ɾ�������������³������С���²����߼�����
	{
		if ((g_cNetRecvBuf[nBufID][9] == (char)0x52) || (g_cNetRecvBuf[nBufID][9] == (char)0x53))//���ݻظ�֡?����
		{
			pView->m_scanf2ddlg.m_sScan2dCfgDlg.UpdateData(true);
			g_nRecvNum_scanf++;
			pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nRecvNum = g_nRecvNum_scanf;    //����+1

			//�����-��֡
			nTime1 = nTime2;
			nTime2 = (((g_cNetRecvBuf[nBufID][42] & 0xff) << 24) | ((g_cNetRecvBuf[nBufID][43] & 0xff) << 16) | ((g_cNetRecvBuf[nBufID][44] & 0xff) << 8) | ((g_cNetRecvBuf[nBufID][45] & 0xff)));//g_sNetData.m_pcData[51];
			int tmp = nTime2 - nTime1;
			tmp = abs(tmp);
			if ((tmp / 1000) > (21) && firstData == 1)
			{
				printf("���ʱ��%d\n", (tmp / 1000));
				pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_n32LostDataNum++;
				pView->m_scanf2ddlg.UpdateData(false);
			}
			firstData = 1;


			if (g_nRecvNum_scanf%pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nInterval == 0)          //ÿ��m_nInterval�����ݰ���ʾһ��
			{
				if (g_nCount_scanf >= pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nOneChartNum)       //һ��ͼֻ��ʾm_nOneChartNum����              
				{
					g_nCount_scanf = 0;
					pView->PostMessage(WM_NETCMD_CLEARCHARTSC, g_nNetRecvInd, 0);
				}
				g_nCount_scanf++;
				//g_sNetData.m_pcData = g_cNetRecvBuf[nBufID];
				g_sNetData.m_n32Len = g_nNetRecvSize[nBufID];
				g_sNetData.m_n32BufCnt = 1;
				g_sNetData.m_n32Channel = 0;

				//nTime1 = nTime2;
				//nTime2 =(((g_cNetRecvBuf[nBufID][42]&0xff) << 24) |((g_cNetRecvBuf[nBufID][43]&0xff)  << 16)|((g_cNetRecvBuf[nBufID][44]&0xff)  << 8)|((g_cNetRecvBuf[nBufID][45]&0xff) ));//g_sNetData.m_pcData[51];
				//int tmp = nTime2-nTime1;
				//tmp = abs(tmp);
				//if((tmp/1000) > (30*pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nInterval) && firstData == 1)
				//{
				//	printf("���ʱ��%d\n",(tmp/1000));
				//	pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_n32LostDataNum++;
				//	pView->m_scanf2ddlg.UpdateData(false);
				//}
				//firstData = 1;
				pView->PostMessage(WM_NETCMD_DRAWCHARTSC, (WPARAM)&g_sNetData, NULL);
			}

			//g_sNetData.m_pcData = g_cNetRecvBuf[nBufID];
			g_sNetData.m_n32Len = g_nNetRecvSize[nBufID];
			g_sNetData.m_n32BufCnt = 1;
			g_sNetData.m_n32Channel = 0;


			pView->m_scanf2ddlg.m_sScan2dCfgDlg.UpdateData(false);
		}
		else {

		}
	}
	else if (g_cNetRecvBuf[nBufID][0] == (char)0xFF &&
		g_cNetRecvBuf[nBufID][1] == (char)0xAA && g_cNetRecvBuf[nBufID][12] == (char)0x00 &&
		g_cNetRecvBuf[nBufID][13] == (char)0x07 && g_cNetRecvBuf[nBufID][11] == 0x02)   //����������
	{
		if (g_cNetRecvBuf[nBufID][22] == (char)0x01)//������ţ�����ģʽ����ͨ��
		{
			switch ((unsigned char)(g_cNetRecvBuf[nBufID][23]))
			{
			case (char)0x02://������������
				pView->m_single2ddlg.m_sSi2dCfgDlg.GetDlgItem(IDC_BUTTON_REVISE)->EnableWindow(TRUE);
				switch (g_cNetRecvBuf[nBufID][24])
				{
				case (char)0x01:
					nChannel = 1;
					g_nRecvNum++;
					g_nRecvNum1++;
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_nRecvNum = g_nRecvNum;
					pView->m_single2ddlg.m_sSi2dCfgDlg.UpdateData(false);
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, _T("������������  0x01"));
					if (g_nRecvNum1%l_nInterval == 0)
					{
						if (g_nRecvNum1 % (l_nInterval * 3) == 0)
						{
							avgdata1 = 0;
							for (int k = 268; k < 308; k = k + 4)
							{
								avgdata1 += ((g_cNetRecvBuf[nBufID][k + 3] & 0xFF) << 24) + ((g_cNetRecvBuf[nBufID][k + 2] & 0xFF) << 16) + ((g_cNetRecvBuf[nBufID][k + 1] & 0xFF) << 8) + (g_cNetRecvBuf[nBufID][k] & 0xFF);
							}
							pView->m_single2ddlg.m_sSi2dCfgDlg.m_nsubdata = (avgdata2 - avgdata1) / 10;
							//printf("avgdata1 is %d,avgdata2 is %d,sub is %d\n",avgdata1,avgdata2,pView->m_single2ddlg.m_sSi2dCfgDlg.m_nsubdata);
							pView->m_single2ddlg.m_sSi2dCfgDlg.UpdateData(false);
						}
						if (g_nRecvNum1 % (l_nOneChartNum*l_nInterval) == 0)
						{

							pView->SendMessage(WM_NETCMD_CLEARCHARTSI, g_nNetRecvInd, nChannel);
						}

						g_nDrawNum++;
						pView->m_single2ddlg.m_sSi2dCfgDlg.m_nDrawNum = g_nDrawNum;
						pView->m_single2ddlg.m_sSi2dCfgDlg.UpdateData(false);
						//g_sNetData.m_pcData = g_cNetRecvBuf[nBufID];
						memcpy(g_sNetData.m_pcData, g_cNetRecvBuf[nBufID], sizeof(g_cNetRecvBuf[nBufID]));
						g_sNetData.m_n32Len = g_nNetRecvSize[nBufID];
						g_sNetData.m_n32BufCnt = 1;
						g_sNetData.m_n32Channel = nChannel;

						pView->SendMessage(WM_NETCMD_DRAWCHARTSI, (WPARAM)&g_sNetData, NULL);

						if (TRUE == pView->m_single2ddlg.m_sSi2dCfgDlg.m_bCacuFlag)
						{
							//   nSum = FD_UDP_DataSum(g_cNetRecvBuf[nBufID],8);
							nSum = FD_Cal_DataSum(g_sNetData.m_pcData, 28);
							g_nSumStop1 = g_nSumStop1 + nSum;
							g_nCacuNum1++;
							if (g_nCacuNum1 == pView->m_single2ddlg.m_sSi2dCfgDlg.m_nCaCuNum)
							{
								// g_nAvgStop2 = g_nSumStop2 / m_nCacuNum / nFrameDataNum;
								g_nAvgStop1 = g_nSumStop1 / pView->m_single2ddlg.m_sSi2dCfgDlg.m_nCaCuNum;
							}

						}

					}
					break;
				case (char)0x02:
					nChannel = 2;
					g_nRecvNum++;
					g_nRecvNum2++;
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_nRecvNum = g_nRecvNum;
					pView->m_single2ddlg.m_sSi2dCfgDlg.UpdateData(false);
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, _T("������������  0x00    0x02"));
					if (g_nRecvNum2%l_nInterval == 0)
					{
						if (g_nRecvNum2 % (l_nInterval * 3) == 0)
						{
							avgdata2 = 0;
							for (int k = 268; k < 308; k = k + 4)
							{
								avgdata2 += ((g_cNetRecvBuf[nBufID][k + 3] & 0xFF) << 24) + ((g_cNetRecvBuf[nBufID][k + 2] & 0xFF) << 16) + ((g_cNetRecvBuf[nBufID][k + 1] & 0xFF) << 8) + (g_cNetRecvBuf[nBufID][k] & 0xFF);
							}
							pView->m_single2ddlg.m_sSi2dCfgDlg.m_nsubdata = (avgdata2 - avgdata1) / 10;
							//printf("avgdata1 is %d,avgdata2 is %d,sub is %d\n",avgdata1,avgdata2,pView->m_single2ddlg.m_sSi2dCfgDlg.m_nsubdata);
							pView->m_single2ddlg.m_sSi2dCfgDlg.UpdateData(false);
						}
						if (g_nRecvNum2 % (l_nOneChartNum*l_nInterval) == 0)
						{

							pView->SendMessage(WM_NETCMD_CLEARCHARTSI, g_nNetRecvInd, nChannel);
						}

						g_nDrawNum++;
						pView->m_single2ddlg.m_sSi2dCfgDlg.m_nDrawNum = g_nDrawNum;
						pView->m_single2ddlg.m_sSi2dCfgDlg.UpdateData(false);

						//g_sNetData.m_pcData = g_cNetRecvBuf[nBufID];
						memcpy(g_sNetData.m_pcData, g_cNetRecvBuf[nBufID], sizeof(g_cNetRecvBuf[nBufID]));
						g_sNetData.m_n32Len = g_nNetRecvSize[nBufID];
						g_sNetData.m_n32BufCnt = 1;
						g_sNetData.m_n32Channel = nChannel;

						pView->SendMessage(WM_NETCMD_DRAWCHARTSI, (WPARAM)&g_sNetData, NULL);

						if (TRUE == pView->m_single2ddlg.m_sSi2dCfgDlg.m_bCacuFlag)
						{
							//   nSum = FD_UDP_DataSum(g_cNetRecvBuf[nBufID],8);
							nSum = FD_Cal_DataSum(g_sNetData.m_pcData, 28);
							g_nSumStop2 = g_nSumStop2 + nSum;
							g_nCacuNum2++;
							if (g_nCacuNum2 == pView->m_single2ddlg.m_sSi2dCfgDlg.m_nCaCuNum)
							{
								// g_nAvgStop2 = g_nSumStop2 / m_nCacuNum / nFrameDataNum;
								g_nAvgStop2 = g_nSumStop2 / pView->m_single2ddlg.m_sSi2dCfgDlg.m_nCaCuNum;
							}

						}

					}
					break;
				case (char)0x03:
					PauseSendCount++;
					nChannel = 3;
					g_nRecvNum++;
					g_nRecvNum3++;
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_nRecvNum = g_nRecvNum;
					pView->m_single2ddlg.m_sSi2dCfgDlg.UpdateData(false);
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, _T("������������  0x00    0x02    0x03"));
					if (g_nRecvNum3 != g_nRecvNum2 || g_nRecvNum3 != g_nRecvNum2 || g_nRecvNum1 != g_nRecvNum3)
					{
						nChannel = 3;
					}
					if (g_nRecvNum3%l_nInterval == 0)
					{
						if (g_nRecvNum3 % (l_nOneChartNum*l_nInterval) == 0)
						{

							pView->SendMessage(WM_NETCMD_CLEARCHARTSI, g_nNetRecvInd, nChannel);
						}
						g_nDrawNum++;
						pView->m_single2ddlg.m_sSi2dCfgDlg.m_nDrawNum = g_nDrawNum;
						pView->m_single2ddlg.m_sSi2dCfgDlg.UpdateData(false);
						//g_sNetData.m_pcData = g_cNetRecvBuf[nBufID];
						memcpy(g_sNetData.m_pcData, g_cNetRecvBuf[nBufID], sizeof(g_cNetRecvBuf[nBufID]));
						g_sNetData.m_n32Len = g_nNetRecvSize[nBufID];
						g_sNetData.m_n32BufCnt = 1;
						g_sNetData.m_n32Channel = nChannel;
						pView->SendMessage(WM_NETCMD_DRAWCHARTSI, (WPARAM)&g_sNetData, NULL);

					}
					break;
				}
				break;
			}
			if (g_nCacuNum1 >= pView->m_single2ddlg.m_sSi2dCfgDlg.m_nCaCuNum && g_nCacuNum2 >= pView->m_single2ddlg.m_sSi2dCfgDlg.m_nCaCuNum)
			{
				if (g_nAvgStop1 > 1 && g_nAvgStop2 > 5)
				{
					if (g_nReviseNum == 0)
					{
						g_nFirstData = g_nAvgStop1;
					}
					nDlThred = g_nAvgStop2 - g_nAvgStop1;
					nSglThred = g_nFirstData - g_nAvgStop1;
					m_sData.Format("%ld", g_nAvgStop2);
					m_sData = "����:" + m_sData;
					m_sData += "\r\n";
					//pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog,m_sData);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.SetSel(-1, -1);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.ReplaceSel(m_sData);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

					m_sData.Format("%ld", g_nAvgStop1);
					m_sData = "����:" + m_sData;
					m_sData += "\r\n";
					//pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog,m_sData);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.SetSel(-1, -1);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.ReplaceSel(m_sData);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

					m_sData.Format("%d", g_nFirstData);
					m_sData = "��׼��" + m_sData;
					m_sData += "\r\n";
					//pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog,m_sData);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.SetSel(-1, -1);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.ReplaceSel(m_sData);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

					g_array1.SetAtGrow(g_nReviseNum, nDlThred);
					g_array2.SetAtGrow(g_nReviseNum, nSglThred);

					m_sData.Format("%d", nDlThred);
					m_sData = "����" + m_sData;
					m_sData += "\r\n";
					//pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog,m_sData);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.SetSel(-1, -1);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.ReplaceSel(m_sData);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

					m_sData.Format("%d", nSglThred);
					m_sData = "��׼-���أ�" + m_sData;
					m_sData += "\r\n";
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.SetSel(-1, -1);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.ReplaceSel(m_sData);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

					m_sData = "---------------------";
					m_sData += "\r\n";
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.SetSel(-1, -1);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.ReplaceSel(m_sData);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

					g_nReviseNum++;
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_bCacuFlag = FALSE;
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_btnCaculate.EnableWindow(TRUE);
					//pView->m_single2ddlg.m_sSi2dCfgDlg.m_edtCaculate.EnableWindow(TRUE);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_btnCaculate.SetWindowText("����");
					//pView->m_single2ddlg.m_sSi2dCfgDlg.m_btnOpenFile.EnableWindow(TRUE);
					//pView->m_single2ddlg.m_sSi2dCfgDlg.m_btnFitting.EnableWindow(TRUE);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_btnCacuEnable = TRUE;
					g_nCacuNum1 = 0;
					g_nCacuNum2 = 0;
					g_nXY[g_nXYlen][0] = nDlThred;
					g_nXY[g_nXYlen][1] = nSglThred;
					g_datax[g_nInterpNum] = nDlThred;
					g_datay[g_nInterpNum] = nSglThred;
					g_nInterpNum++;
					g_nXYlen++;
				}
				else if (g_nAvgStop2 <= 5 || g_nAvgStop1 <= 5)
				{
					m_sData = "���ݴ����޷�����";
					m_sData += "\r\n";
					//pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog,m_sData);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.SetSel(-1, -1);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.ReplaceSel(m_sData);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_recDataLog.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

					pView->m_single2ddlg.m_sSi2dCfgDlg.m_bCacuFlag = FALSE;
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_btnCaculate.EnableWindow(TRUE);
					//pView->m_single2ddlg.m_sSi2dCfgDlg.m_edtCaculate.EnableWindow(TRUE);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_btnCaculate.SetWindowText("����");
					//pView->m_single2ddlg.m_sSi2dCfgDlg.m_btnOpenFile.EnableWindow(TRUE);
					//pView->m_single2ddlg.m_sSi2dCfgDlg.m_btnFitting.EnableWindow(TRUE);
					pView->m_single2ddlg.m_sSi2dCfgDlg.m_btnCacuEnable = TRUE;
					g_nCacuNum1 = 0;
					g_nCacuNum2 = 0;
				}
			}
		}
		else if (g_cNetRecvBuf[nBufID][22] == (char)0x02)//ɨ��ģʽ������ͨ��
		{
			pView->m_scanf2ddlg.m_sScan2dCfgDlg.UpdateData(true);
			g_nRecvNum_scanf++;
			pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nRecvNum = g_nRecvNum_scanf;    //����+1
			int quanNo = (byte)g_cNetRecvBuf[nBufID][81];   //Ȧ��
			int jiaoduNo = g_cNetRecvBuf[nBufID][82];  //��Ȧ����
			static int pre_quanNO = 1;
			if (g_cNetRecvBuf[nBufID][80] == 0x00)   //12��
			{
				if (jiaoduNo == 0)
				{
					pre_quanNO = quanNo;
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[0], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 1)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[1200], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 2)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[2400], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 3)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[3600], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 4)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[4800], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 5)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[6000], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 6)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[7200], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 7)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[8400], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 8)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[9600], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 9)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[10800], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 10)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[12000], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 11)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					//pView->PostMessage(WM_NETCMD_CLEARCHARTSC,g_nNetRecvInd,0);
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[13200], &g_cNetRecvBuf[nBufID][86], 1200);
					if (quanNo%pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nInterval == 0)
					{
						g_nCount_scanf++;
						if (g_nCount_scanf >= pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nOneChartNum)       //һ��ͼֻ��ʾm_nOneChartNum����              
						{
							g_nCount_scanf = 0;
							pView->PostMessage(WM_NETCMD_CLEARCHARTSC, g_nNetRecvInd, 0);
						}
						g_sNetData.m_n32Len = 7200;
						g_sNetData.m_n32BufCnt = 1;
						g_sNetData.m_n32Channel = 0;
						//memset(g_sNetData.m_pcData,0x10,8000);
						pView->PostMessage(WM_NETCMD_DRAWCHARTSC, (WPARAM)&g_sNetData, NULL);
						g_nSendWaveNum = 0;
						g_bMoniterFaultedLost = false;
					}
				}
			}
			else if (g_cNetRecvBuf[nBufID][80] == 0x01)   //6��
			{
				if (jiaoduNo == 0)
				{
					pre_quanNO = quanNo;
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[0], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 1)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[1200], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 2)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[2400], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 3)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[3600], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 4)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[4800], &g_cNetRecvBuf[nBufID][86], 1200);
				}
				else if (jiaoduNo == 5)
				{
					if (pre_quanNO != quanNo)
					{
						memset(&g_sNetData, 0, sizeof(g_sNetData));
						printf("pre_quanNO != quanNo\n");
						return 0;
					}
					pView->PostMessage(WM_NETCMD_CLEARCHARTSC, g_nNetRecvInd, 0);
					//g_cNetRecvBuf[nBufID][83] = 0x1C;
					//g_cNetRecvBuf[nBufID][84] = 0x20;
					memcpy(&g_sNetData.m_pcData[6000], &g_cNetRecvBuf[nBufID][86], 1200);
					if (quanNo%pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nInterval == 0)
					{
						g_nCount_scanf++;
						if (g_nCount_scanf >= pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nOneChartNum)       //һ��ͼֻ��ʾm_nOneChartNum����              
						{
							g_nCount_scanf = 0;
							pView->PostMessage(WM_NETCMD_CLEARCHARTSC, g_nNetRecvInd, 0);
						}
						g_sNetData.m_n32Len = 3600;
						g_sNetData.m_n32BufCnt = 1;
						g_sNetData.m_n32Channel = 0;
						//memset(g_sNetData.m_pcData,0x10,8000);
						pView->PostMessage(WM_NETCMD_DRAWCHARTSC, (WPARAM)&g_sNetData, NULL);
						g_nSendWaveNum = 0;
						g_bMoniterFaultedLost = false;
					}
				}
			}

			if (0)    //ͳ�ƶ�֡
			{
				if (g_nCount_scanf >= pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nOneChartNum)       //һ��ͼֻ��ʾm_nOneChartNum����              
				{
					g_nCount_scanf = 0;
					pView->PostMessage(WM_NETCMD_CLEARCHARTSC, g_nNetRecvInd, 0);
					//F_ClearChart();
					//UpdateData(FALSE);
				}
				g_nCount_scanf++;
				//g_sNetData.m_pcData = g_cNetRecvBuf[nBufID];; yjun20160608
				g_sNetData.m_n32Len = g_nNetRecvSize[nBufID];
				g_sNetData.m_n32BufCnt = 1;
				g_sNetData.m_n32Channel = 0;

				//�����ж�
				nTime1 = nTime2;
				nTime2 = (((g_cNetRecvBuf[nBufID][42] & 0xff) << 24) | ((g_cNetRecvBuf[nBufID][43] & 0xff) << 16) | ((g_cNetRecvBuf[nBufID][44] & 0xff) << 8) | ((g_cNetRecvBuf[nBufID][45] & 0xff)));//g_sNetData.m_pcData[51];
				int tmp = nTime2 - nTime1;
				tmp = abs(tmp);

				//if(tmp != pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nInterval && firstData == 1)
				if ((tmp / 1000) > (30 * pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nInterval) && firstData == 1)
				{
					printf("���ʱ��%d\n", (tmp / 1000));
					pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_n32LostDataNum++;
					pView->m_scanf2ddlg.UpdateData(false);
				}
				firstData = 1;
				//				pView->PostMessage(WM_NETCMD_DRAWCHARTSC,(WPARAM)&g_sNetData,NULL);
				//FD_TCP_DrawWave_Fast(g_cNetRecvBuf[nBufID],g_nNetRecvSize[nBufID]);     //TCP�������ݻ���ͼ��	
			}
			//if (pView3d->m_scanf3ddlg.GetSafeHwnd() != NULL)
			//{
			//	//g_sNetData.m_pcData = g_cNetRecvBuf[nBufID];; yjun20160608
			//	/*g_sNetData.m_n32Len = g_nNetRecvSize[nBufID];
			//	g_sNetData.m_n32BufCnt = 1;
			//	g_sNetData.m_n32Channel = 0;
			//	pView3d->PostMessage(WM_NETCMD_DRAW3D,(WPARAM)&g_sNetData,NULL);*/
			//	//pView3d->OnDraw3dChartSc((WPARAM)&g_sNetData,NULL);
			//	//FD_TCP_Draw3D(g_cNetRecvBuf[nBufID], g_nNetRecvSize[nBufID]);
			//}
			pView->m_scanf2ddlg.m_sScan2dCfgDlg.UpdateData(false);
		}
		else if (g_cNetRecvBuf[nBufID][22] == (char)0x03)//��������Կͨ�����
		{
			g_recvotherlen = g_nNetRecvSize[nBufID];
			switch ((unsigned char)(g_cNetRecvBuf[nBufID][23]))
			{
			case (char)0x01://��ȡоƬID
				memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
				SetEvent(GETFACTORYID_EVENT_NET_rs);
				break;
			case (char)0x02://�滻��Կ
				memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
				SetEvent(REPLACESKEY_EVENT_NET_rs);
				break;
			case (char)0x03://���������
				if (g_cNetRecvBuf[nBufID][26] == 0x01)//����ɹ�
				{
					memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
					SetEvent(CALCSENDRANDS_EVENT_NET_rs);
				}
				break;
			case (char)0x04://��ȡҳ������	
				memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
				SetEvent(READPAGE_EVENT_NET_rs);
				break;
			case (char)0x05://����дMemeroy����
				memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
				SetEvent(WRITEMEMO_EVENT_NET_rs);
			case (char)0x06://��ѯ��Կ״̬
				memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
				SetEvent(CHECKKEYSTATE_EVENT_NET_rs);
				break;
			}

		}
		else if (g_cNetRecvBuf[nBufID][22] == (char)0x04)//������״̬
		{
			g_recvotherlen = (g_cNetRecvBuf[nBufID][2] << 8) + g_cNetRecvBuf[nBufID][3] + 4;
			switch ((unsigned char)(g_cNetRecvBuf[nBufID][23]))
			{
			case (char)0x01://Ӳ�����汾��
				memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
				pView1->SendMessage(WM_VERSION_READ, (WPARAM)g_recvother, g_recvotherlen);
				break;
			case (char)0x02://����������״̬
				memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
				pView1->SendMessage(WM_MAIN2GR2DST_READ, (WPARAM)g_recvother, g_recvotherlen);
				break;
			case (char)0x03://��ѯ��λ��Ϣ
				memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
				pView1->SendMessage(WM_SETAPP_RESETNUM_U, (WPARAM)g_recvother, g_recvotherlen);
				break;
			case (char)0x04://�����λ��Ϣ
				AfxMessageBox(_T("��λ��Ϣ����ɹ���"));
				break;
			case (char)0x05://�����豸
				AfxMessageBox(_T("�����������ɹ���"));
				::AfxBeginThread(ConnectThreadProc, pMainFrame);
				break;
			}

		}
		else if (g_cNetRecvBuf[nBufID][22] == (char)0x05)//�������������ָ��
		{
			g_recvotherlen = (g_cNetRecvBuf[nBufID][2] << 8) + g_cNetRecvBuf[nBufID][3] + 4;
			switch ((unsigned char)(g_cNetRecvBuf[nBufID][23]))
			{
			case (char)0x01://��ȡ�豸��������
				memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
				pSetView->m_SetAppDlg.SendMessage(WM_SETAPP_BASICPIB_U, (WPARAM)g_recvother, g_recvotherlen);
				break;
			case (char)0x02://��ȡ�豸�������
				memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
				pSetView->m_SetProdDlg.SendMessage(WM_SETPROD_NetWork_Read, (WPARAM)g_recvother, g_recvotherlen);
				break;
			case (char)0x03://�����������
				AfxMessageBox(_T("���سɹ���"));
				::AfxBeginThread(ConnectThreadProc, pMainFrame);
				break;
			case (char)0x04://��ȡAPD��������
				memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
				pSetView->m_SetProdDlg.SendMessage(WM_SETPROD_APDPIB_U, (WPARAM)g_recvother, g_recvotherlen);
				break;
			case (char)0x05://����APD��������
				AfxMessageBox(_T("���سɹ���"));
				break;
			case (char)0x06://���������ȡ
				memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
				pSetView->m_SetProdDlg.SendMessage(WM_SETRD_MOTOPID_READ, (WPARAM)g_recvother, g_recvotherlen);
				break;
			case (char)0x07://�����������
				if (f_SetProDialog)
				{
					AfxMessageBox(_T("���سɹ���"));
					f_SetProDialog = false;
				}
				else
				{
					SetEvent(g_PXDownevnet);
				}
				break;
			case (char)0x08://���ת�ٲ��Կ���
				memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
				pView->m_SetRdDlg.SendMessage(WM_SETRD_MOTOSPEED_TEST, (WPARAM)g_recvother, g_recvotherlen);
				break;
				//case (char)0x09://���⹦�ʲ�������
				//	AfxMessageBox(_T("���سɹ���"));
				//	break;
			case (char)0x09://���ת�ٲ���
				memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
				pView->m_SetRdDlg.SendMessage(WM_MOTOSPEED_TEST, (WPARAM)g_recvother, g_recvotherlen);
				break;
			case (char)0x0A://APD����
				memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
				pSetView->m_SetProdDlg.SendMessage(WM_SETRD_APDTEST_BACK, (WPARAM)g_recvother, g_recvotherlen);
				break;
			case (char)0x0B://APD��������
				AfxMessageBox(_T("���سɹ���"));
				break;
			case (char)0x0C://��������������
				if (g_cNetRecvBuf[nBufID][24] == 0x01)//����ֵ
				{
					if (g_cNetRecvBuf[nBufID][29] == 0x01)
					{
						g_nNum = (g_cNetRecvBuf[nBufID][26] << 8) + g_cNetRecvBuf[nBufID][27];
						g_flag = TRUE;
						SetEvent(g_evnet);
						displaystr.Format("%d", g_nNum);
						pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "�յ��ظ�������ֵ��" + displaystr + "�����ͳɹ�\r\n");
					}
					else if (g_cNetRecvBuf[nBufID][29] == 0x00)
					{
						g_nNum = (g_cNetRecvBuf[nBufID][26] << 8) + g_cNetRecvBuf[nBufID][27];
						g_flag = FALSE;
						SetEvent(g_evnet);
						displaystr.Format("%d", g_nNum);
						pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "�յ��ظ�������ֵ��" + displaystr + "������ʧ��\r\n");
					}
				}
				else if (g_cNetRecvBuf[nBufID][24] == 0x02){
					if (g_cNetRecvBuf[nBufID][29] == 0x01)
					{
						g_nNum = (g_cNetRecvBuf[nBufID][26] << 8) + g_cNetRecvBuf[nBufID][27];
						g_flag = TRUE;
						SetEvent(g_evnet);
						displaystr.Format("%d", g_nNum);
						pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "�յ��ظ�������ֵ��" + displaystr + "�����ͳɹ�\r\n");
					}
					else if (g_cNetRecvBuf[nBufID][29] == 0x02)
					{
						g_nNum = (g_cNetRecvBuf[nBufID][26] << 8) + g_cNetRecvBuf[nBufID][27];
						g_flag = FALSE;
						SetEvent(g_evnet);
						displaystr.Format("%d", g_nNum);
						pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "�յ��ظ�������ֵ��" + displaystr + "������ʧ��\r\n");
					}
				}
				break;
			case (char)0x0D://������д������
				if (g_cNetRecvBuf[nBufID][29] == 0x01)
				{
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "��������д�ɹ���");
					//pView->m_single2ddlg.SendMessage(WM_CHECK_MAIKUAN,(WPARAM)g_recvother,g_recvotherlen);
				}
				else if (g_cNetRecvBuf[nBufID][29] == 0x02){
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "��������дʧ�ܡ�");
				}
				break;
			case (char)0x0E://��ѯ������
				if (g_cNetRecvBuf[nBufID][26] == 0x01)//?
				{
					short nTmp_h = 0;
					CString strTemp_h = "�յ��ظ�����������ǰ10���������£�\n";
					CString strTmp_h;
					for (int k = 0; k < 10; k++)
					{
						nTmp_h = (((g_cNetRecvBuf[nBufID][29 + k * 2]) << 8) & 0xff00) + (g_cNetRecvBuf[nBufID][30 + k * 2] & 0xff);
						strTmp_h.Format("%d\t ", nTmp_h);
						strTemp_h += strTmp_h;
					}
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, strTemp_h);
				}
				else if (g_cNetRecvBuf[nBufID][26] == 0x02)
				{
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "�յ��ظ�����������");
				}
				break;
			case (char)0x0F://�����豸��������
				if (g_cNetRecvBuf[nBufID][29] == 0x01)
				{
					AfxMessageBox(_T("���سɹ���"));
					if (pSetView->m_SetAppDlg.m_Heart.GetCurSel() == 1)
					{
						SetTimer(1, 5000, NULL);    //��������
					}
					//::AfxBeginThread(ConnectThreadProc,pMainFrame);
				}
				else{
					AfxMessageBox(_T("����ʧ�ܣ�"));
				}
				break;
			case (char)0x10:  //����
				m_NetConnection.pcSendDataBuf = (char *)LaserHeart_zhiling;
				m_NetConnection.nSendDataBufSize = LaserHeart_zhilingSize;
				if (m_NetConnection.bConnected)
					SendData(&m_NetConnection);
				HeartBeatCount++;
				break;
			case (char)0x11://�������ط����ʱ�
				if (g_cNetRecvBuf[nBufID][29] == 0x01)
				{
					g_nNum = (g_cNetRecvBuf[nBufID][26] << 8) + g_cNetRecvBuf[nBufID][27];
					g_flag = TRUE;
					SetEvent(g_evnet);
					displaystr.Format("%d", g_nNum);
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "�յ��ظ��������ʱ��" + displaystr + "�����ͳɹ�\r\n");
				}
				else if (g_cNetRecvBuf[nBufID][29] == 0x02)
				{
					g_nNum = (g_cNetRecvBuf[nBufID][26] << 8) + g_cNetRecvBuf[nBufID][27];
					g_flag = FALSE;
					SetEvent(g_evnet);
					displaystr.Format("%d", g_nNum);
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "�յ��ظ��������ʱ��" + displaystr + "������ʧ��\r\n");
				}
				break;
			case (char)0x12://������д�����ʱ�
				if (g_cNetRecvBuf[nBufID][29] == 0x01)
				{
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "�����ʱ���д�ɹ���");
					//pView->m_single2ddlg.SendMessage(WM_CHECK_MAIKUAN,(WPARAM)g_recvother,g_recvotherlen);
				}
				else if (g_cNetRecvBuf[nBufID][29] == 0x02){
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "�����ʱ���дʧ�ܡ�");
				}
				break;
			case (char)0x13://��ѯ�����ʱ�
				if (g_cNetRecvBuf[nBufID][26] == 0x01)//
				{
					short nTmp_h = 0;
					CString strTemp_h = "�յ��ظ�����д�����ʱ�ɹ���ǰ10���������£�\n";
					CString strTmp_h;
					for (int k = 0; k < 10; k++)
					{
						nTmp_h = (((g_cNetRecvBuf[nBufID][29 + k * 2]) << 8) & 0xff00) + (g_cNetRecvBuf[nBufID][30 + k * 2] & 0xff);
						strTmp_h.Format("%d\t ", nTmp_h);
						strTemp_h += strTmp_h;
					}
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, strTemp_h);
				}
				else if (g_cNetRecvBuf[nBufID][26] == 0x02)
				{
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "�յ��ظ�����д�����ʱ���ȷ��");
				}
				else
				{
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "�յ�����ȷ�ظ���");
				}
				break;
			case (char)0x16://������������ƫ�ı�
				if (g_cNetRecvBuf[nBufID][29] == 0x01)
				{
					g_nNum = (g_cNetRecvBuf[nBufID][26] << 8) + g_cNetRecvBuf[nBufID][27];
					g_PXnNum = (g_cNetRecvBuf[nBufID][26] << 8) + g_cNetRecvBuf[nBufID][27];
					g_flag = TRUE;
					g_PXflag = TRUE;
					SetEvent(g_evnet);
					SetEvent(g_PXevnet);
					displaystr.Format("%d", g_nNum);
					if (f_PXAtSi2dCfg)
					{
						pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "�յ��ظ�������ƫ�ı��" + displaystr + "�����ͳɹ�\r\n");
					}
					else
					{
						pView->m_AGVTestDlg.m_AGVTestCfg.WriteToRichEdit("�յ��ظ�������ƫ�ı��" + displaystr + "�����ͳɹ�");//19 02 26Ϊ������ƫ���������
					}
				}
				else if (g_cNetRecvBuf[nBufID][29] == 0x02)
				{
					g_nNum = (g_cNetRecvBuf[nBufID][26] << 8) + g_cNetRecvBuf[nBufID][27];
					g_PXnNum = (g_cNetRecvBuf[nBufID][26] << 8) + g_cNetRecvBuf[nBufID][27];
					g_flag = FALSE;
					g_PXflag = FALSE;
					SetEvent(g_evnet);
					SetEvent(g_PXevnet);
					displaystr.Format("%d", g_nNum);
					if (f_PXAtSi2dCfg)
					{
						pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "�յ��ظ�������ƫ�ı��" + displaystr + "������ʧ��\r\n");
					}
					else
					{
						pView->m_AGVTestDlg.m_AGVTestCfg.WriteToRichEdit("�յ��ظ�������ƫ�ı��" + displaystr + "������ʧ��,��������������ƫ�ı�");//19 02 26Ϊ������ƫ���������
					}
					AfxMessageBox("����ƫ�ı���ʧ�ܣ���������������ƫ�ı�");
				}
				break;
			case (char)0x17://������д����ƫ�ı�
				if (g_cNetRecvBuf[nBufID][29] == 0x01)
				{
					if (f_PXAtSi2dCfg)
					{
						pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "����ƫ�ı���д�ɹ���");
					}
					else
					{
						pView->m_AGVTestDlg.m_AGVTestCfg.WriteToRichEdit("����ƫ�ı���д�ɹ���");
						pView->m_AGVTestDlg.m_AGVTestCfg.SendOpenPX();//ƫ�ı����سɹ��ʹ�ƫ������
						if (AfterNAVCount > 0)
						{
							pView->m_AGVTestDlg.m_AGVTestCfg.OnBnClickedOk();//������֤
						}
					}
				}
				else if (g_cNetRecvBuf[nBufID][29] == 0x02)
				{
					if (f_PXAtSi2dCfg)
					{
						pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "����ƫ�ı���дʧ�ܡ�");
					}
					else
					{
						pView->m_AGVTestDlg.m_AGVTestCfg.WriteToRichEdit("����ƫ�ı���дʧ��,��������������ƫ�ı�");
					}
					AfxMessageBox("����ƫ�ı���дʧ�ܣ���������������ƫ�ı�");
				}
				break;
			case (char)0x18://��ѯ����ƫ���ʱ�
				if (g_cNetRecvBuf[nBufID][26] == 0x01)//
				{
					short nTmp_h = 0;
					CString strTemp_h = "�յ��ظ�����д����ƫ�ı�ɹ���ǰ10���������£�\n";
					CString strTmp_h;
					for (int k = 0; k < 10; k++)
					{
						nTmp_h = (g_cNetRecvBuf[nBufID][29 + k] & 0xff);
						strTmp_h.Format("%d\t ", nTmp_h);
						strTemp_h += strTmp_h;
					}
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, strTemp_h);
				}
				else if (g_cNetRecvBuf[nBufID][26] == 0x02)
				{
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "�յ��ظ�����д����ƫ�ı���ȷ��");
				}
				else
				{
					pView->m_single2ddlg.m_sSi2dCfgDlg.WriteLog(&pView->m_single2ddlg.m_sSi2dCfgDlg.m_recOperLog, "�յ�����ȷ�ظ���");
				}
				break;
			case (char)0x1B://�����������
				if (g_cNetRecvBuf[nBufID][29] == 0x01)
				{
					AfxMessageBox(_T("����ɹ���"));
				}
				break;
			case (char)0x14://FPGA��������
			case (char)0x15://boot��������
			{
								CRichEditCtrl *p_rich = (CRichEditCtrl *)pSetView->m_FPGADownloadDlg.GetDlgItem(IDC_RICHEDIT_FPGADN);
								CString str_rich = "";
								if (g_cNetRecvBuf[nBufID][29] == 0x01)//
								{
									int nTmp = (((g_cNetRecvBuf[nBufID][26]) << 8) & 0xff00) + (g_cNetRecvBuf[nBufID][27] & 0xff);
									if (nTmp == FPGAfile_index)
									{
										str_rich.Format("%d", FPGAfile_index);
										str_rich = "��д�� " + str_rich + " ������ɹ�\n";
										SetEvent(Event_SUCCEED);
									}
									else
									{
										str_rich.Format("�ظ�Ϊ�� %d ������д�� %d ���������������д����\n", nTmp, FPGAfile_index);
										SetEvent(Event_ERROR);
									}
								}
								else if (g_cNetRecvBuf[nBufID][29] == 0x00)
								{
									str_rich.Format("%d", FPGAfile_index);
									str_rich = "��д�� " + str_rich + " ���������������д����\n";
									SetEvent(Event_ERROR);
								}
								else
								{
									str_rich = "�յ�����Ļظ�\n";
								}
								p_rich->SetSel(-1, -1);
								p_rich->ReplaceSel(str_rich);
			}
				break;
			case (char)0x1A://ƫ������ʱ����ȡ�б���Ϣ
			{
								if (f_AGVTest)
								{
									memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
									pView->m_AGVTestDlg.m_AGVTestCfg.SendMessage(WM_LMAngleDeal, (WPARAM)g_recvother, g_recvotherlen);
									SetEvent(g_PXAngevent);
								}
			}
				break;
			}
		}
		else if (g_cNetRecvBuf[nBufID][22] == (char)0x06)//������Ӧ�����ָ��
		{
			switch ((unsigned char)(g_cNetRecvBuf[nBufID][23]))
			{
			case (char)0x03:
			{
							   if ((BYTE)g_cNetRecvBuf[nBufID][27] == 2)
							   {
								   if ((BYTE)g_cNetRecvBuf[nBufID][29] == 1)
								   {
									   f_FirDataLMSet = true;
								   }
							   }
							   if ((BYTE)g_cNetRecvBuf[nBufID][27] == 1)
							   {
								   if ((BYTE)g_cNetRecvBuf[nBufID][29] == 1)
								   {
									   if (f_AGVTest&&!f_SimuAGV)
									   {
										   pView->m_AGVTestDlg.m_AGVTestCfg.SendMessage(WM_LMSettingInfo, 0, 1);//�б����óɹ���������Ϣ�л�������ģʽ
									   }
									   else if (f_SimuAGV&&!f_AGVTest)
									   {
										   pView->m_SIMUAGVTESTDLG.SimuAGV.SendMessage(WM_MNLMSettingInfo, 0, 1);//�б����óɹ���������Ϣ�л�������ģʽ
									   }
								   }
								   else
								   {
									   if (f_AGVTest&&!f_SimuAGV)
									   {
										   pView->m_AGVTestDlg.m_AGVTestCfg.SendMessage(WM_LMSettingInfo, 0, 0);//�б�����ʧ��
									   }
									   else if (f_SimuAGV&&!f_AGVTest)
									   {
										   pView->m_SIMUAGVTESTDLG.SimuAGV.SendMessage(WM_MNLMSettingInfo, 0, 0);//�б�����ʧ��
									   }
								   }
							   }
			}
				break;
			case (char)0x04:
			{
							   if ((BYTE)g_cNetRecvBuf[nBufID][29] == 1)
							   {
								   if (f_nav && (BYTE)g_cNetRecvBuf[nBufID][29] == 1)
								   {
									   /*  pView->m_AGVTestDlg.m_AGVTestCfg.WriteToRichEdit("����ģʽ���óɹ���");
										 f_nav = false;*/
								   }
								   if (f_nav && (BYTE)g_cNetRecvBuf[nBufID][29] == 0)
								   {
									   /* pView->m_AGVTestDlg.m_AGVTestCfg.WriteToRichEdit("����ģʽ����ʧ�ܣ�");
										f_nav = false;*/
								   }
							   }
			}
				break;
			case (char)0x05://�б��ȡ
			{
								SetEvent(g_PXMap);
								SetEvent(g_MNPXMap);
								int l_LocalID = 0, l_GX_Pos = 0, l_GY_Pos = 0, l_Shape = 0, l_diameter = 0;
								int num_Target = (BYTE)g_cNetRecvBuf[nBufID][31];
								int Curr_Package = ((g_cNetRecvBuf[nBufID][28] & 0xFF) << 8) + (BYTE)(g_cNetRecvBuf[nBufID][29]);
								if (num_Target > 0)
								{
									for (int i = 0; i < num_Target; i++)
									{
										l_LocalID = (BYTE)(g_cNetRecvBuf[nBufID][32 + i * 20]);//����ID
										l_GX_Pos = ((g_cNetRecvBuf[nBufID][33 + i * 20] & 0xFF) << 24) + ((g_cNetRecvBuf[nBufID][34 + i * 20] & 0xFF) << 16) +
											((g_cNetRecvBuf[nBufID][35 + i * 20] & 0xFF) << 8) + (BYTE)(g_cNetRecvBuf[nBufID][36 + i * 20]);
										l_GY_Pos = ((g_cNetRecvBuf[nBufID][37 + i * 20] & 0xFF) << 24) + ((g_cNetRecvBuf[nBufID][38 + i * 20] & 0xFF) << 16) +
											((g_cNetRecvBuf[nBufID][39 + i * 20] & 0xFF) << 8) + (BYTE)(g_cNetRecvBuf[nBufID][40 + i * 20]);//��������&&�������ݴ�1120
										l_Shape = (BYTE)(g_cNetRecvBuf[nBufID][42 + i * 20]);//��״
										l_diameter = (BYTE)(g_cNetRecvBuf[nBufID][43 + i * 20]);//�ߴ�
										int l_Kind = (BYTE)g_cNetRecvBuf[nBufID][41];
										int s_dlandmark = 5 * i;
										d_landmark[s_dlandmark] = l_LocalID;
										d_landmark[s_dlandmark + 1] = l_GX_Pos;
										d_landmark[s_dlandmark + 2] = l_GY_Pos;
										d_landmark[s_dlandmark + 3] = l_Shape;
										d_landmark[s_dlandmark + 4] = l_diameter;
									}
									if (f_AGVTest&&!f_SimuAGV)
									{
										pView->m_AGVTestDlg.m_AGVTestCfg.SendMessage(WM_LMInfoView, num_Target, LPARAM(d_landmark));
									}
									else if (f_SimuAGV&&!f_AGVTest)
									{
										pView->m_SIMUAGVTESTDLG.SimuAGV.SendMessage(WM_MNLMInfoView, num_Target, LPARAM(d_landmark));
									}
								}

								if (Curr_Package <= 20)//Ĭ��20��
								{
									CTime time = CTime::GetCurrentTime();
									char Hour = time.GetHour();
									char Miu = time.GetMinute();
									char Sec = time.GetSecond();
									unsigned char pn[14] = { 0x00, 0x1E, 0x00, 0x00, 0x00, Hour, Miu, Sec, 0x01, 0x01, 0x00, 0x07, 0x06, 0x05 };
									ZhiLing(LMGetting_zhiling, pn, 0);
									LMGetting_zhiling[26] = 0;
									LMGetting_zhiling[27] = Curr_Package;
									LMGetting_zhiling[28] = 0;
									LMGetting_zhiling[29] = 1;
									AfxGetMainWnd()->PostMessage(WM_SENDCMD, LMGetting_zhilingSize, (LPARAM)LMGetting_zhiling);
									if (Curr_Package == 20)
									{
										if (num_Target == 0)
										{
											if (f_AGVTest&&!f_SimuAGV)
											{
												pView->m_AGVTestDlg.m_AGVTestCfg.WriteToRichEdit("����ɰб��ȡ����ȡ�İб����Ϊ0");
											}
											else if (f_SimuAGV&&!f_AGVTest)
											{
												pView->m_SIMUAGVTESTDLG.SimuAGV.MNWriteToRichEdit("����ɰб��ȡ����ȡ�İб����Ϊ0");
											}
										}
									}
									else
									{
										CString str_CurrCount = "";
										str_CurrCount.Format("%d", Curr_Package);
										if (f_AGVTest&&!f_SimuAGV)
										{
											pView->m_AGVTestDlg.m_AGVTestCfg.WriteToRichEdit("�����" + str_CurrCount + "��ƽ��");
										}
										else if (f_SimuAGV&&!f_AGVTest)
										{
											pView->m_SIMUAGVTESTDLG.SimuAGV.MNWriteToRichEdit("�����" + str_CurrCount + "��ƽ��");
										}
									}
								}
			}
				break;
			case (char)0x18://�ָ���������
				if (g_cNetRecvBuf[nBufID][29] == 0x01)
				{
					AfxMessageBox(_T("�ָ��ɹ��������²�ѯ��������"));
				}
				break;
			}
		}
	}
	else if (g_cNetRecvBuf[nBufID][0] == (char)0x02 && g_cNetRecvBuf[nBufID][1] != (char)0x02)   //���������� sickЭ�� �жϵ���02��֡ͷ
	{
		g_recvotherlen = g_nNetRecvSize[nBufID];
		memcpy(g_recvother, g_cNetRecvBuf[nBufID], g_recvotherlen);
		if (g_recvother[12] == 84 && g_recvother[13] == 105)//Ti ���㼤����ʱ���
		{
			CString str_timp = g_recvother;
			int t_Space = 0, s_Timestap_index = 0;
			for (int i = 0; i < str_timp.GetLength(); i++)
			{
				if (str_timp[i] == ' ')
				{
					t_Space++;
					if (t_Space == 3)
					{
						s_Timestap_index = i + 1;
					}
				}
			}
			CString t_str = str_timp.Mid(s_Timestap_index, (str_timp.GetLength() - s_Timestap_index - 1));
			LaserTimestap = strtoul(t_str, NULL, 16);
		}
		else
		{
			if (f_AGVTest&&!f_SimuAGV)
			{
				pView->m_AGVTestDlg.SendMessage(WM_SICK_POSEREAD, (WPARAM)g_recvother, g_recvotherlen);
			}
			else if (f_SimuAGV&&!f_AGVTest)
			{
				pView->m_SIMUAGVTESTDLG.SendMessage(WM_SICK_MNPOSEREAD, (WPARAM)g_recvother, g_recvotherlen);
			}
		}
	}
	else
	{

	}
	return 0;
}

LONG CMainFrame::OnNetSendCmd(WPARAM wParam, LPARAM lParam)       //��Ӧ����ҳ�еĲ�ѯ�豸��������
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CGraphForm2dState *pView1 = (CGraphForm2dState*)pMainFrame->m_wndSplitter.GetPane(0, 0);

	m_nSendBufDataSize = (int)wParam;
	memcpy(m_cSendBufData, (char *)lParam, m_nSendBufDataSize);
	m_NetConnection.pcSendDataBuf = m_cSendBufData;
	m_NetConnection.nSendDataBufSize = m_nSendBufDataSize;
	pView1->SendMessage(WM_STATEDLG_STOPTIMER, 0, 0);
	Sleep(1);
	if (m_NetConnection.bConnected)
		SendData(&m_NetConnection);
	Sleep(1);
	//	pView1->SendMessage(WM_STATEDLG_STARTTIMER,0,0);

	return 0;
}

LONG CMainFrame::OnNetSendCmdTimer(WPARAM wParam, LPARAM lParam)       //��Ӧ����ҳ�еĲ�ѯ�豸��������
{
	m_nSendBufDataSize = (int)wParam;
	memcpy(m_cSendBufData, (char *)lParam, m_nSendBufDataSize);

	m_NetConnection.pcSendDataBuf = m_cSendBufData;
	m_NetConnection.nSendDataBufSize = m_nSendBufDataSize;
	if (m_NetConnection.bConnected)
		SendData(&m_NetConnection);

	return 0;
}

void CMainFrame::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	GraphForm2d * pView;
	CGraphForm2dState *pView1;
	CSetFormView *pSetView;

	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pView = (GraphForm2d*)pMainFrame->m_wndLeftSplitter.GetPane(0, 0);
	pView1 = (CGraphForm2dState*)pMainFrame->m_wndSplitter.GetPane(0, 0);
	pSetView = (CSetFormView*)pMainFrame->m_wndSplitter.GetPane(1, 0);
	pView->SendMessage(WM_NETCMD_CHARTCLOSE, 0, 0);
	TerminateProcess(pMainFrame, 0);//�رս���
	CFrameWndEx::OnClose();
}


void CMainFrame::OnUdpBrod()
{
	// TODO: �ڴ���������������
	if (!m_NetConnection.bConnected)
	{
		if (g_bMenuBroadClicked == false)
		{
			g_bMenuBroadClicked = true;
			m_sBroadCastDlg.ShowWindow(true);
			g_bMenuBroadClicked = false;
		}
		else
		{
			g_bMenuBroadClicked = false;
		}
	}
	else
	{
		AfxMessageBox(_T("����������"));
	}
}


void CMainFrame::OnUpdateUdpBrod(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	if (g_bMenuBroadClicked == true)
	{
		pCmdUI->Enable(false);
	}
	else
	{
		pCmdUI->Enable(true);
	}
}


void CMainFrame::OnLoad()
{
	// TODO: �ڴ���������������	
	if (g_bMenuLoadClicked == false)
	{
		m_sSystemLoadDlg.DoModal();
		if (SystemLoader != 0)
		{
			g_bMenuLoadClicked = true;

			switch (SystemLoader)
			{
			case 1:this->SetWindowText("ɨ��ʽ���⴫����WLR-P711-0001-20170929-��ͨ�û�"); break;
			case 2:this->SetWindowText("ɨ��ʽ���⴫����WLR-P711-0001-20170929-������Ա"); break;
			case 3:this->SetWindowText("ɨ��ʽ���⴫����WLR-P711-0001-20170929-�з���Ա"); break;
			default:this->SetWindowText("ɨ��ʽ���⴫����WLR-P711-0001-20171024"); break;
			}
		}
	}
	else
	{
		g_bMenuLoadClicked = false;
		this->SetWindowText("ɨ��ʽ���⴫����WLR-P711-0001-20170929-δ��½");
		SystemLoader = 3;
	}
}


void CMainFrame::OnUpdateLoad(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	if (g_bMenuLoadClicked == false)
	{
		pCmdUI->SetText("��½");
		this->SetWindowText("ɨ��ʽ���⴫����WLR-P711-0001-20170929-δ��½");
	}
	else
	{
		pCmdUI->SetText("ע��");

	}
}

void CMainFrame::OnTimer(UINT nIDEvent)
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	CGraphForm2dState *pView1;
	CSetFormView *pSetView;
	pSetView = (CSetFormView*)pMainFrame->m_wndSplitter.GetPane(1, 0);
	pView1 = (CGraphForm2dState*)pMainFrame->m_wndSplitter.GetPane(0, 0);

	switch (nIDEvent)
	{

	case 0:
		if ((g_u8LoadParamDone & 0x01) != 0x01)
		{
			//���Ͳ�ѯ����״̬
			AfxGetMainWnd()->SendMessage(WM_SENDCMD, LaserState_zhilingSize, (LPARAM)LaserState_zhiling);
		}
		break;
		/*Sleep(5);
		if((g_u8LoadParamDone & 0x02) != 0x02)
		{

		AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Prod_QueryAPDPibReadLen, (LPARAM)g_acProd_QueryAPDPibReadFrameBuf);
		}*/

		//Sleep(5);
		//if((g_u8LoadParamDone & 0x04) != 0x04)
		//{
		//	AfxGetMainWnd()->PostMessage(WM_SENDCMD,g_n32Prod_ConfigAPDTestULen, (LPARAM)g_acProd_ConfigAPDTestUFrameBuf);

		//}
		//Sleep(5);
		//if((g_u8LoadParamDone & 0x08) != 0x08)
		//{
		//	AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Prod_QueryBasicPibReadLen, (LPARAM)g_acProd_QueryBasicPibReadFrameBuf);
		//}
		//Sleep(5);
		//if((g_u8LoadParamDone & 0x10) != 0x10)
		//{
		//	AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32Prod_ConfigPwTestULen, (LPARAM)g_acProd_ConfigPwTestUFrameBuf);
		//}
		//Sleep(5);
		//if((g_u8LoadParamDone & 0x20) != 0x20)
		//{
		//	AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32RD_ConfigReadLen, (LPARAM)g_acRD_ConfigReadFrameBuf);
		//}
		//Sleep(5);
		//if((g_u8LoadParamDone & 0x40) != 0x40)
		//{
		//	AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32RD_MotorPIDReadLen, (LPARAM)g_acRD_MotorPIDReadFrameBuf);
		//}
		//Sleep(5);
		//g_u8timer0cnt++;
		//if((g_u8LoadParamDone) == 0x7f || g_u8timer0cnt > 10)
		//{
		//	g_u8timer0cnt = 0;
		//	g_u8LoadParamDoneFlag = 1;
		//	KillTimer(0);
		//	Sleep(100);
		/*CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		CGraphForm2dState *pView1;
		pView1 = (CGraphForm2dState*) pMainFrame->m_wndSplitter.GetPane(0,0);*/
		//pView1->SetTimer(0,1000,00);
		/*pView1->SetTimer(1,500,0);*/
		//pView1->SetTimer(2,1000,00);
		//pView1->SetTimer(3,1000,00);
		//}
	case 1:
		if (pView1->m_strHeartState == "����")
		if (pSetView->m_SetAppDlg.m_Heart.GetCurSel() == 1)
		{
			if (HeartBeatCount != 0)
			{
				HeartBeatCount = 0;
			}
			else
			{
				//�Ͽ�
				NetConnect();
				Sleep(1000);   //Ϊ100ms�Ļ�����������
				//����
				NetConnect();
			}
		}
		else
		{
			KillTimer(1);
		}
		break;
	}
}

long CMainFrame::FD_Cal_DataSum(char *pcDataBuf, int nFirstByte)
{
	long nSum = 0L;
	int i = 0;
	int j = 0;
	int nDataNum = 0;
	int l_n32OkCnt = 0;
	int l_n32SumTmp = 0;
	nDataNum = (pcDataBuf[26] << 8) + (pcDataBuf[27]);//���ݵ����
	i = nFirstByte;
	for (j = 1; j <= nDataNum; j++)
	{
		l_n32SumTmp = ((pcDataBuf[i + 3] & 0xFF) << 24) + ((pcDataBuf[i + 2] & 0xFF) << 16) + ((pcDataBuf[i + 1] & 0xFF) << 8) + (pcDataBuf[i] & 0xFF);
		if (l_n32SumTmp > 5)
		{
			nSum += l_n32SumTmp;
			l_n32OkCnt++;
		}

		i = i + 4;
	}

	if (l_n32OkCnt > 0)
	{
		return nSum / l_n32OkCnt;
	}
	else
	{
		return l_n32OkCnt;
	}

}

void CMainFrame::OnServerip()
{
	// TODO: �ڴ���������������
	CServerIPDlg dlg = new CServerIPDlg(m_NetConnection);
	int i = dlg.DoModal();
	{
		if (IDOK == i)
		{

		}
	}
}
LONG CMainFrame::OnConnect7070(WPARAM wParam, LPARAM lParam)
{
	CSetFormView *pSetView;
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pSetView = (CSetFormView*)pMainFrame->m_wndSplitter.GetPane(1, 0);
	CGraphForm2dState *pView1;
	//CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pView1 = (CGraphForm2dState*)pMainFrame->m_wndSplitter.GetPane(0, 0);
	memcpy(&m_NetConnection7070, &m_NetConnection, sizeof(m_NetConnection));
	m_NetConnection7070.unServerPort = 7070;
	pThread1 = ::AfxBeginThread(ConnectThread7070, this);
	return 1;
}

CString CMainFrame::GetMacByGetAdaptersAddresses(PIP_ADAPTER_INFO pAdapter)
{
	//PIP_ADAPTER_INFO pAdapterInfo;
	//PIP_ADAPTER_INFO pAdapter = NULL;
	//DWORD dwRetVal = 0;
	//ULONG ulOutBufLen;
	//pAdapterInfo=(PIP_ADAPTER_INFO)malloc(sizeof(IP_ADAPTER_INFO));
	//ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	CString TheSerialNumber = "";
	CString TheSerialNumber_temp = "";
	byte TheSerialNum[6] = { 0 };
	byte temp[6] = { 0 };
	//CString MACaddress = "";//MAC��ַд��־��10-08
	//	dwRetVal = GetAdaptersInfo( pAdapterInfo, &ulOutBufLen);

	// ��һ�ε���GetAdapterInfo��ȡulOutBufLen��С
	//if (dwRetVal == ERROR_BUFFER_OVERFLOW)
	//{
	//	free(pAdapterInfo);
	//	pAdapterInfo = (IP_ADAPTER_INFO *) malloc (ulOutBufLen); 
	//}

	//	if (dwRetVal == NO_ERROR) 
	//	{
	//		pAdapter = pAdapterInfo;

	//MAC��ַд��־��10-08
	//MACaddress.Format("MAC��ַ��%02x-%02x-%02x-%02x-%02x-%02x",pAdapter->Address[0],pAdapter->Address[1],pAdapter->Address[2],pAdapter->Address[3],pAdapter->Address[4],pAdapter->Address[5]);
	//::SendMessage(hwnd,WM_WRITELOG,0,(LPARAM)(LPCTSTR)MACaddress);
	//MACaddress.ReleaseBuffer();

	//�����㷨
	TheSerialNum[0] = pAdapter->Address[0] ^ 'y';
	TheSerialNum[1] = pAdapter->Address[1] ^ 'u';
	TheSerialNum[2] = pAdapter->Address[2] ^ 'n';
	TheSerialNum[3] = pAdapter->Address[3] ^ 'n';
	TheSerialNum[4] = pAdapter->Address[4] ^ 'a';
	TheSerialNum[5] = pAdapter->Address[5] ^ 't'; //n

	temp[0] = TheSerialNum[0] << 1;
	temp[1] = TheSerialNum[1] << 1;
	temp[2] = TheSerialNum[2] << 1;
	temp[3] = TheSerialNum[3] << 1;
	temp[4] = TheSerialNum[4] << 1;
	temp[5] = TheSerialNum[5] << 1;

	unsigned short crcjiaoyan = CRC16((unsigned char *)(temp), 6);
	TheSerialNumber_temp.Format("%02x%02x", crcjiaoyan & 0xFF, (crcjiaoyan >> 8) & 0xFF);
	TheSerialNumber = TheSerialNumber + TheSerialNumber_temp;

	TheSerialNum[0] = temp[1] ^ 'w';
	TheSerialNum[1] = temp[2] ^ 'a';
	TheSerialNum[2] = temp[3] ^ 'n';
	TheSerialNum[3] = temp[4] ^ 'j';
	TheSerialNum[4] = temp[5] ^ 'i';
	TheSerialNum[5] = temp[0] ^ 'i';

	temp[0] = TheSerialNum[0] << 1;
	temp[1] = TheSerialNum[1] << 1;
	temp[2] = TheSerialNum[2] << 1;
	temp[3] = TheSerialNum[3] << 1;
	temp[4] = TheSerialNum[4] << 1;
	temp[5] = TheSerialNum[5] << 1;

	crcjiaoyan = CRC16((unsigned char *)(temp), 6);
	TheSerialNumber_temp.Format("%02x%02x", crcjiaoyan & 0xFF, (crcjiaoyan >> 8) & 0xFF);
	TheSerialNumber = TheSerialNumber + TheSerialNumber_temp;

	TheSerialNum[0] = temp[2] ^ 's';
	TheSerialNum[1] = temp[3] ^ 'h';
	TheSerialNum[2] = temp[4] ^ 'i';
	TheSerialNum[3] = temp[5] ^ 'n';
	TheSerialNum[4] = temp[0] ^ 'a';
	TheSerialNum[5] = temp[1] ^ 'n';

	temp[0] = TheSerialNum[0] << 1;
	temp[1] = TheSerialNum[1] << 1;
	temp[2] = TheSerialNum[2] << 1;
	temp[3] = TheSerialNum[3] << 1;
	temp[4] = TheSerialNum[4] << 1;
	temp[5] = TheSerialNum[5] << 1;

	crcjiaoyan = CRC16((unsigned char *)(temp), 6);
	TheSerialNumber_temp.Format("%02x%02x", crcjiaoyan & 0xFF, (crcjiaoyan >> 8) & 0xFF);
	TheSerialNumber = TheSerialNumber + TheSerialNumber_temp;

	TheSerialNum[0] = temp[3] ^ 'l';
	TheSerialNum[1] = temp[4] ^ 'i';
	TheSerialNum[2] = temp[5] ^ 'k';
	TheSerialNum[3] = temp[0] ^ 'a';
	TheSerialNum[4] = temp[1] ^ 'n';
	TheSerialNum[5] = temp[2] ^ 'g';

	temp[0] = TheSerialNum[0] << 1;
	temp[1] = TheSerialNum[1] << 1;
	temp[2] = TheSerialNum[2] << 1;
	temp[3] = TheSerialNum[3] << 1;
	temp[4] = TheSerialNum[4] << 1;
	temp[5] = TheSerialNum[5] << 1;

	crcjiaoyan = CRC16((unsigned char *)(temp), 6);
	TheSerialNumber_temp.Format("%02x%02x", crcjiaoyan & 0xFF, (crcjiaoyan >> 8) & 0xFF);
	TheSerialNumber = TheSerialNumber + TheSerialNumber_temp;

	TheSerialNum[0] = temp[4] ^ 'y';
	TheSerialNum[1] = temp[5] ^ 'u';
	TheSerialNum[2] = temp[0] ^ 'd';
	TheSerialNum[3] = temp[1] ^ 'e';
	TheSerialNum[4] = temp[2] ^ 'n';
	TheSerialNum[5] = temp[3] ^ 'g';

	temp[0] = TheSerialNum[0] << 1;
	temp[1] = TheSerialNum[1] << 1;
	temp[2] = TheSerialNum[2] << 1;
	temp[3] = TheSerialNum[3] << 1;
	temp[4] = TheSerialNum[4] << 1;
	temp[5] = TheSerialNum[5] << 1;

	crcjiaoyan = CRC16((unsigned char *)(temp), 6);
	TheSerialNumber_temp.Format("%02x%02x", crcjiaoyan & 0xFF, (crcjiaoyan >> 8) & 0xFF);
	TheSerialNumber = TheSerialNumber + TheSerialNumber_temp;

	TheSerialNum[0] = temp[5] ^ 'y';
	TheSerialNum[1] = temp[0] ^ 'a';
	TheSerialNum[2] = temp[1] ^ 'n';
	TheSerialNum[3] = temp[2] ^ 'g';
	TheSerialNum[4] = temp[3] ^ 'y';
	TheSerialNum[5] = temp[4] ^ 'o';

	temp[0] = TheSerialNum[0] << 1;
	temp[1] = TheSerialNum[1] << 1;
	temp[2] = TheSerialNum[2] << 1;
	temp[3] = TheSerialNum[3] << 1;
	temp[4] = TheSerialNum[4] << 1;
	temp[5] = TheSerialNum[5] << 1;

	crcjiaoyan = CRC16((unsigned char *)(temp), 6);
	TheSerialNumber_temp.Format("%02x%02x", crcjiaoyan & 0xFF, (crcjiaoyan >> 8) & 0xFF);
	TheSerialNumber = TheSerialNumber + TheSerialNumber_temp;

	//	return TheSerialNumber;
	//}
	//else
	//{

	//}
	return TheSerialNumber;
}
unsigned short CMainFrame::CRC16(unsigned char *puchMsg, unsigned short usDataLen)
{
	unsigned char uchCRCHi = 0xFF;
	unsigned char uchCRCLo = 0xFF;
	unsigned uIndex;
	while (usDataLen--)
	{
		uIndex = uchCRCHi ^ *puchMsg++;
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex];
	}
	//	return (uchCRCHi << 8 | uchCRCLo) ;
	return (uchCRCLo << 8 | uchCRCHi);
}
void CMainFrame::CheckLicense()
{
	PIP_ADAPTER_INFO pAdapterInfo;
	PIP_ADAPTER_INFO pAdapter = NULL;
	DWORD dwRetVal = 0;
	ULONG ulOutBufLen;
	pAdapterInfo = (PIP_ADAPTER_INFO)malloc(sizeof(IP_ADAPTER_INFO));
	ulOutBufLen = sizeof(IP_ADAPTER_INFO);
	char lpPath[MAX_PATH];
	CString license;
	CString adno;
	LPTSTR l_getLicense = new char[50];
	dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);

	////��ȡ����������Ϣ
	//DWORD pdwNegotiatedVersion;
	//HANDLE phClientHandle;
	//PWLAN_INTERFACE_INFO_LIST wiiList;
	//WlanOpenHandle (1,NULL,&pdwNegotiatedVersion,&phClientHandle);
	//WlanEnumInterfaces(phClientHandle,NULL,&wiiList);


	// ��һ�ε���GetAdapterInfo��ȡulOutBufLen��С
	if (dwRetVal == ERROR_BUFFER_OVERFLOW)
	{
		free(pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc(ulOutBufLen);
	}
	dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen);
	if (dwRetVal == NO_ERROR)
	{
		while (pAdapterInfo)
		{
			if (pAdapterInfo->Type == 6 && (strstr(pAdapterInfo->Description, "Wireless") == 0))
			{
				pAdapter = pAdapterInfo;
				adno.Format(_T("%02x-%02x-%02x-%02x-%02x-%02x"), pAdapter->Address[0], pAdapter->Address[1], pAdapter->Address[2],
					pAdapter->Address[3], pAdapter->Address[4], pAdapter->Address[5]);
				break;
			}
			pAdapterInfo = pAdapterInfo->Next;
		}
	}
	if (pAdapter == NULL)
	{
		MessageBox("δ�ҵ�����MAC��Ϣ,������������״̬", "��ʾ", MB_OK);
		bJiaoyan = FALSE;
		return;
	}

	GetCurrentDirectory(MAX_PATH, lpPath);
	strcat(lpPath, "\\setup.ini");
	fstream _file;
	_file.open(lpPath, ios::in);

	if (!_file)
	{
		//m_bVersionRight = FALSE;
		MessageBox("δ�ҵ������ļ������ȷ��ʹ�÷ǹ����棡\n���������÷�����\n(1)�����룺" + adno + "�����з���ȡ���кţ�\n(2)�ڳ���Ŀ¼�´���setup.ini�ļ������ݸ�ʽΪ��\n[License]\nSerialLicense=License\n���õ������к��滻�����License���������б�����", "��ʾ", MB_OK);
		bJiaoyan = FALSE;
	}
	else
	{
		memset(l_getLicense, 0, sizeof(l_getLicense));
		license = GetMacByGetAdaptersAddresses(pAdapter);
		GetPrivateProfileString("License", "SerialLicense", "", l_getLicense, 50, lpPath);
		if (license != l_getLicense)
		{
			//m_bVersionRight = FALSE;
			MessageBox("���к�У�����\n���������÷�����\n(1)�����룺" + adno + "�����з���ȡ���кţ�\n(2)�޸�setup.ini�ļ������õ������к��滻ԭ�����кź��������б�����", "��ʾ", MB_OK);
			bJiaoyan = FALSE;
		}
		else
		{
			//m_bVersionRight = TRUE;
			//MessageBox("���к�У��ɹ������ȷ��ʹ�ù����棡","��ʾ",MB_OK);
		}
	}
	delete[] l_getLicense;
	_file.close();
}