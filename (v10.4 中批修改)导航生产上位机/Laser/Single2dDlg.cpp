// Single2dDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "Single2dDlg.h"
#include "afxdialogex.h"
#include "Si2dCfg.h"
// CSingle2dDlg �Ի���

IMPLEMENT_DYNAMIC(CSingle2dDlg, CDialog)

CSingle2dDlg::CSingle2dDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSingle2dDlg::IDD, pParent)
{

}

CSingle2dDlg::~CSingle2dDlg()
{
}

void CSingle2dDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TCHART_SINGLE, m_ChartSi);
}


BEGIN_MESSAGE_MAP(CSingle2dDlg, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CSingle2dDlg)
    EASYSIZE(IDC_TCHART_SINGLE,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	//EASYSIZE(IDD_SCANF2DPROPDLG,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)	
END_EASYSIZE_MAP
// CSingle2dDlg ��Ϣ�������


BOOL CSingle2dDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_bFisrtEasySize = true;
	m_sSi2dCfgDlg.Create(IDD_SI2DCFG);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	INIT_EASYSIZE;
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CSingle2dDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
	 //if (m_bFisrtEasySize && IsWindowVisible()&& GetWindow(GW_CHILD)!=NULL)
  //    {
  //           INIT_EASYSIZE;
  //           m_bFisrtEasySize = FALSE;
  //           UPDATE_EASYSIZE;
  //    }else if (!m_bFisrtEasySize){
  //           UPDATE_EASYSIZE;
  //    }
	// TODO: �ڴ˴������Ϣ����������
}
BEGIN_EVENTSINK_MAP(CSingle2dDlg, CDialog)
	ON_EVENT(CSingle2dDlg, IDC_TCHART_SINGLE, 8, CSingle2dDlg::OnDblClickTchartSingle, VTS_NONE)
END_EVENTSINK_MAP()


void CSingle2dDlg::OnDblClickTchartSingle()
{
	// TODO: �ڴ˴������Ϣ����������
	//m_sSi2dCfgDlg.Create(IDD_SI2DCFG,this->GetParent()->GetParent()->GetParent());
	//m_sSi2dCfgDlg.Create(IDD_SI2DCFG,GetDlgItem(IDD_SINGLE2DDLG));
	m_sSi2dCfgDlg.ShowWindow(true);
}




