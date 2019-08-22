// SystemLoad.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "SystemLoad.h"
#include "afxdialogex.h"
#include "NetParam.h"

// CSystemLoad 对话框

IMPLEMENT_DYNAMIC(CSystemLoad, CDialogEx)

CSystemLoad::CSystemLoad(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSystemLoad::IDD, pParent)
{

	m_strKey = _T("");
}

CSystemLoad::~CSystemLoad()
{
}

void CSystemLoad::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMB_USER, m_CombUser);
	DDX_Text(pDX, IDC_EDIT_KEY, m_strKey);
}


BEGIN_MESSAGE_MAP(CSystemLoad, CDialogEx)
	ON_BN_CLICKED(IDOK, &CSystemLoad::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CSystemLoad::OnBnClickedCancel)
END_MESSAGE_MAP()


// CSystemLoad 消息处理程序


void CSystemLoad::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(true);
	if(m_CombUser.GetCurSel() == 0)
	{
		if(m_strKey == "wanjiyingyong")
		{
			SystemLoader = 1;
			AfxMessageBox("登录成功！");
		}
		else
		{
			AfxMessageBox("登录失败,密码错误！");
		}
	}
	else if(m_CombUser.GetCurSel() == 1)
	{
		if(m_strKey == "wanjishengchan")
		{
			SystemLoader = 2;
			AfxMessageBox("登录成功！");
		}
		else
		{
			AfxMessageBox("登录失败,密码错误！");
		}
	}
	else if(m_CombUser.GetCurSel() == 2)
	{
		if(m_strKey == "wanjiyanfa")
		{
			SystemLoader = 3;
			AfxMessageBox("登录成功！");
		}
		else
		{
			AfxMessageBox("登录失败,密码错误！");
		}
	}
	else
	{
		AfxMessageBox("登录失败！");;
	}
	m_strKey = "";
	UpdateData(false);
	CDialogEx::OnOK();
}


void CSystemLoad::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	m_strKey = "";
	UpdateData(false);
	CDialogEx::OnCancel();
}
