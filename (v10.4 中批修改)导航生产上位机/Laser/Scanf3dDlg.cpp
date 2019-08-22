// Scanf3dDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "Scanf3dDlg.h"
#include "afxdialogex.h"


// CScanf3dDlg �Ի���

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


// CScanf3dDlg ��Ϣ�������


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
	// TODO: �ڴ˴������Ϣ����������
}
BEGIN_EVENTSINK_MAP(CScanf3dDlg, CDialogEx)
	ON_EVENT(CScanf3dDlg, IDC_TCHART_3D, 8, CScanf3dDlg::OnDblClickTchart3d, VTS_NONE)
END_EVENTSINK_MAP()


void CScanf3dDlg::OnDblClickTchart3d()
{
	// TODO: �ڴ˴������Ϣ����������
	m_sScan3dCfgDlg.ShowWindow(true);
}


BOOL CScanf3dDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_sScan3dCfgDlg.Create(IDD_SCANF3DCFG);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
