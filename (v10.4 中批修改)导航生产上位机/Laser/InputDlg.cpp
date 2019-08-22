// InputDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "InputDlg.h"
#include "afxdialogex.h"


// InputDlg �Ի���

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


// InputDlg ��Ϣ�������


void InputDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}


BOOL InputDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	GetDlgItem(IDC_STATIC)->SetWindowText(strStaticText);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
