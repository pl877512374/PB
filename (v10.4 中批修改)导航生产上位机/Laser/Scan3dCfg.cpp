// Scan3dCfg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "Scan3dCfg.h"
#include "afxdialogex.h"
int  createflag =0; //创建对话框标志

// CScan3dCfg 对话框

IMPLEMENT_DYNAMIC(CScan3dCfg, CDialogEx)

CScan3dCfg::CScan3dCfg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CScan3dCfg::IDD, pParent)
{

	m_ymax = 500;
	m_ymin = 500;
	m_high = 7500;
	m_xmin = -16000;
	m_xmax = 0;
	m_drawflag = 0;
	m_n32jiaoduSta = 50;
	m_n32jiaoduEnd = 300;
}

CScan3dCfg::~CScan3dCfg()
{
}

void CScan3dCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_HIGH, m_high);
	//  DDX_Text(pDX, IDC_EDIT_XMAX, m_xmax);
	DDX_Text(pDX, IDC_EDIT_XMIN, m_xmin);
	DDX_Text(pDX, IDC_EDIT_XMAX, m_xmax);
	//  DDX_Text(pDX, IDC_EDIT_YMAX, m_ymax);
	//  DDX_Text(pDX, IDC_EDIT_YMAX, m_ymax);
	DDX_Text(pDX, IDC_EDIT_YMAX, m_ymax);
	DDX_Text(pDX, IDC_EDIT_YMIN, m_ymin);
	DDX_Text(pDX, IDC_EDIT_JIAODUSTA, m_n32jiaoduSta);
	DDX_Text(pDX, IDC_EDIT_JIAODUEND, m_n32jiaoduEnd);
}


BEGIN_MESSAGE_MAP(CScan3dCfg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_DRAW, &CScan3dCfg::OnBnClickedButtonDraw)
END_MESSAGE_MAP()


// CScan3dCfg 消息处理程序


BOOL CScan3dCfg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CScan3dCfg::OnBnClickedButtonDraw()
{
	// TODO: 在此添加控件通知处理程序代码
		// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	CString str;
	GetDlgItem(IDC_BUTTON_DRAW)->GetWindowText(str);
	if (str=="开始")
	{
		if (m_xmin > m_xmax)
		{
			return ;
		}
		GetDlgItem(IDC_BUTTON_DRAW)->SetWindowText("停止");
				m_drawflag = 1;
				createflag = 1;
	}
	else
	{
		GetDlgItem(IDC_BUTTON_DRAW)->SetWindowText("开始");
			  m_drawflag = 0;
			  createflag = 0;
	}
}
