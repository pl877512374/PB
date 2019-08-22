// InputDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "InputDlg.h"
#include "afxdialogex.h"


// InputDlg 对话框

IMPLEMENT_DYNAMIC(InputDlg, CDialogEx)

InputDlg::InputDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(InputDlg::IDD, pParent)
{

}

InputDlg::~InputDlg()
{
}

void InputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(InputDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &InputDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &InputDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// InputDlg 消息处理程序


void InputDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	((CEdit*)GetDlgItem(IDC_EDIT1))->GetWindowText(strText);
	CDialogEx::OnOK();
}
CString InputDlg::GetInputText()
{
	return strText;
}

void InputDlg::SetStaticText(CString& str)
{
	strStaticText = str;
}

void InputDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
	CDialogEx::OnCancel();
}


BOOL InputDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	GetDlgItem(IDC_STATIC)->SetWindowText(strStaticText);
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
