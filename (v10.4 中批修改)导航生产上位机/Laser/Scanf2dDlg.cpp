// Scanf2dDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "Scanf2dDlg.h"
#include "afxdialogex.h"
#include "stdio.h"


IMPLEMENT_DYNAMIC(CScanf2dDlg, CDialogEx)

CScanf2dDlg::CScanf2dDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScanf2dDlg::IDD, pParent)
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif

}

CScanf2dDlg::~CScanf2dDlg()
{
}

void CScanf2dDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TCHART_JIZUOBIAO, m_ChartJi);
	DDX_Control(pDX, IDC_TCHART_ZHIJIAO, m_ChartZhi);
}


BEGIN_MESSAGE_MAP(CScanf2dDlg, CDialog)
	ON_WM_SIZE()
	ON_WM_DROPFILES()
	//ON_MESSAGE(WM_SETAPP_BASICPIB_S,OnShow)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CScanf2dDlg)
    EASYSIZE(IDC_TCHART1,ES_BORDER,ES_BORDER,ES_BORDER,ES_KEEPSIZE,0)
	EASYSIZE(IDC_TCHART2,ES_BORDER,ES_KEEPSIZE,ES_BORDER,ES_BORDER,0)
	
	//EASYSIZE(IDC_TCHART_SINGLE,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	//EASYSIZE(IDD_SCANF2DPROPDLG,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)	
END_EASYSIZE_MAP

// CScanf2dDlg 消息处理程序


void CScanf2dDlg::OnSize(UINT nType, int cx, int cy)
{
	 CDialogEx::OnSize(nType, cx, cy);
	 CRect rs,rsZhi,rsJi;
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
	if (IsWindowVisible()&& GetWindow(GW_CHILD)!=NULL)
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
	 }

     //m_scanf2ddlg.ShowWindow(true)
	//SetScrollSizes(MM_TEXT,CSize(0,0));
   // UPDATE_EASYSIZE;
	// TODO: 在此处添加消息处理程序代码
}


BOOL CScanf2dDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_sScan2dCfgDlg.Create(IDD_SCAN2DCFG);
	INIT_EASYSIZE;
	
	//DockPane(&m_wndMenuBar);
	// TODO:  在此添加额外的初始化
	//AfxGetMainWnd()->SendMessage(WM_SIZE,SIZE_MAXSHOW,0);
	return TRUE;  // return TRUE unless you set the focus to a control
	//m_sSi2dCfgDlg.Create(IDD_SI2DCFG);
	// 异常: OCX 属性页应返回 FALSE
}
BEGIN_EVENTSINK_MAP(CScanf2dDlg, CDialogEx)
	ON_EVENT(CScanf2dDlg, IDC_TCHART_ZHIJIAO, 8, CScanf2dDlg::OnDblClickTchartZhi, VTS_NONE)
	ON_EVENT(CScanf2dDlg, IDC_TCHART_JIZUOBIAO, 8, CScanf2dDlg::OnDblClickTchartJi, VTS_NONE)
END_EVENTSINK_MAP()


void CScanf2dDlg::OnDblClickTchartZhi()
{
	// TODO: 在此处添加消息处理程序代码
    m_sScan2dCfgDlg.ShowWindow(TRUE);
	//m_sScan2dCfgDlg.GetDlgItem(IDC_EDIT_JianGe_Point)->ShowWindow(SW_SHOW);
	//m_sScan2dCfgDlg.GetDlgItem(IDC_STATIC_JIANBGE)->ShowWindow(SW_SHOW);
	
}
 LONG CScanf2dDlg::OnShow(WPARAM wParam, LPARAM lParam)
{
	return 0L;
}

void CScanf2dDlg::OnDblClickTchartJi()
{
	// TODO: 在此处添加消息处理程序代码
	m_sScan2dCfgDlg.ShowWindow(TRUE);
}


void CScanf2dDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	WCHAR wcStr[MAX_PATH];
	int DropCount = DragQueryFile(hDropInfo,-1,NULL,0);
	//for(int i = 0;i<DropCount;i++)
	//{
	//	
	//	DragQueryFile(hDropInfo,i,(LPSTR)wcStr,MAX_PATH);
	//}
	DragQueryFile(hDropInfo,0,(LPSTR)wcStr,MAX_PATH);
	DragFinish(hDropInfo);

	//m_sScan2dCfgDlg.m_nCurFileInd = 0;
	 m_sScan2dCfgDlg.AnalyLWDFile((LPSTR)wcStr);

	CDialogEx::OnDropFiles(hDropInfo);
}
