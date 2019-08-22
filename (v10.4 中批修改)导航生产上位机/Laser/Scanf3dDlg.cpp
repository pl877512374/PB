// Scanf3dDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "Scanf3dDlg.h"
#include "afxdialogex.h"


// CScanf3dDlg 对话框

IMPLEMENT_DYNAMIC(CScanf3dDlg, CDialogEx)

CScanf3dDlg::CScanf3dDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScanf3dDlg::IDD, pParent)
{

}

CScanf3dDlg::~CScanf3dDlg()
{
}

void CScanf3dDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TCHART_3D, m_draw3d);
}


BEGIN_MESSAGE_MAP(CScanf3dDlg, CDialogEx)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CScanf3dDlg 消息处理程序


void CScanf3dDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	 CRect rs,rs3d;
	if (IsWindowVisible()&& GetWindow(GW_CHILD)!=NULL)
	 {
		 this->GetClientRect(&rs);
		 rs.top += 10;
		 rs.bottom -= 5;
		 rs.left += 1;
		 rs.right -= 2;
		 rs3d.top = rs.top;
		 rs3d.bottom = rs.bottom/2-5;
		 rs3d.left = rs.left;
		 rs3d.right = rs.right;
		 m_draw3d.MoveWindow(&rs);
	 }
	// TODO: 在此处添加消息处理程序代码
}
BEGIN_EVENTSINK_MAP(CScanf3dDlg, CDialogEx)
	ON_EVENT(CScanf3dDlg, IDC_TCHART_3D, 8, CScanf3dDlg::OnDblClickTchart3d, VTS_NONE)
END_EVENTSINK_MAP()


void CScanf3dDlg::OnDblClickTchart3d()
{
	// TODO: 在此处添加消息处理程序代码
	m_sScan3dCfgDlg.ShowWindow(true);
}


BOOL CScanf3dDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_sScan3dCfgDlg.Create(IDD_SCANF3DCFG);
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
