// SystemLoad.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "SystemLoad.h"
#include "afxdialogex.h"
#include "NetParam.h"

// CSystemLoad �Ի���

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


// CSystemLoad ��Ϣ�������


void CSystemLoad::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	if(m_CombUser.GetCurSel() == 0)
	{
		if(m_strKey == "wanjiyingyong")
		{
			SystemLoader = 1;
			AfxMessageBox("��¼�ɹ���");
		}
		else
		{
			AfxMessageBox("��¼ʧ��,�������");
		}
	}
	else if(m_CombUser.GetCurSel() == 1)
	{
		if(m_strKey == "wanjishengchan")
		{
			SystemLoader = 2;
			AfxMessageBox("��¼�ɹ���");
		}
		else
		{
			AfxMessageBox("��¼ʧ��,�������");
		}
	}
	else if(m_CombUser.GetCurSel() == 2)
	{
		if(m_strKey == "wanjiyanfa")
		{
			SystemLoader = 3;
			AfxMessageBox("��¼�ɹ���");
		}
		else
		{
			AfxMessageBox("��¼ʧ��,�������");
		}
	}
	else
	{
		AfxMessageBox("��¼ʧ�ܣ�");;
	}
	m_strKey = "";
	UpdateData(false);
	CDialogEx::OnOK();
}


void CSystemLoad::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_strKey = "";
	UpdateData(false);
	CDialogEx::OnCancel();
}
