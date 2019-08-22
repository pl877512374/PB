// SetFormView.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "SetFormView.h"
#include "NetParam.h"


// CSetFormView

IMPLEMENT_DYNCREATE(CSetFormView, CFormView)

CSetFormView::CSetFormView()
	: CFormView(CSetFormView::IDD)
{

}

CSetFormView::~CSetFormView()
{
}

void CSetFormView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_DATASET, m_TabDataSet);
}

BEGIN_MESSAGE_MAP(CSetFormView, CFormView)
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_DATASET, &CSetFormView::OnTcnSelchangeTabDataset)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CSetFormView)
   // EASYSIZE(IDC_TAB_DATASET,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	// EASYSIZE(IDC_TAB_DATASET,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	//EASYSIZE(IDD_SCANF2DPROPDLG,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)	
END_EASYSIZE_MAP
// CSetFormView 诊断

#ifdef _DEBUG
void CSetFormView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CSetFormView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CSetFormView 消息处理程序


void CSetFormView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	//	INIT_EASYSIZE;
	m_TabDataSet.InsertItem(0, _T("应用设置"));
	m_TabDataSet.InsertItem(1, _T("生产设置"));
	//m_TabDataSet.InsertItem(2, _T("电机转速"));
	//m_TabDataSet.InsertItem(1, _T("故障诊断"));
	m_TabDataSet.InsertItem(2, _T("密钥下载"));
	m_TabDataSet.InsertItem(3, _T("主控程序下载"));
	m_TabDataSet.InsertItem(4, _T("FPGA和BOOT程序下载"));
	//m_TabDataSet.InsertItem(4, _T("灰尘检测"));
	//m_TabDataSet.InsertItem(5, _T("ARM串口"));
	

	m_SetAppDlg.Create(IDD_SET_APPDLG,GetDlgItem(IDC_TAB_DATASET));
	m_SetProdDlg.Create(IDD_SET_PRODDLG,GetDlgItem(IDC_TAB_DATASET));
	//m_SetRdDlg.Create(IDD_SET_RDDLG,GetDlgItem(IDC_TAB_DATASET));
	//m_DiagnosisDlg.Create(IDD_DIAGNOSISDLG,GetDlgItem(IDC_TAB_DATASET));
	m_IssueDlg.Create(IDD_ISSUEDLG,GetDlgItem(IDC_TAB_DATASET));
	m_ProgramDlg.Create(IDD_PROGRAMDOWNDLG,GetDlgItem(IDC_TAB_DATASET));
	m_FPGADownloadDlg.Create(IDD_FPGADOWNLOAD,GetDlgItem(IDC_TAB_DATASET));
	//m_DustDlg.Create(IDD_DUSTDLG,GetDlgItem(IDC_TAB_DATASET));
	//m_ArmUartDlg.Create(IDD_ARMUARTDLG,GetDlgItem(IDC_TAB_DATASET));
	CRect rs,rs1;
	m_TabDataSet.GetClientRect(&rs);

	


	//m_TabDataSet.GetClientRect(rs1);
	//m_TabDataSet.MoveWindow(&rs);

	rs.top += 25;
    rs.bottom -= 4;
    rs.left += 1;
    rs.right -= 2;
	this->GetClientRect(&rs1);
	rs1.top += 10;
    rs1.bottom -= 4;
    rs1.left += 1;
    rs1.right -= 4;
	//m_TabDataSet.MoveWindow(&rs1);
	//
	m_SetAppDlg.MoveWindow(&rs);
    m_SetAppDlg.ShowWindow(true);


	SendMessage(WM_SIZE);
	// TODO: 在此添加专用代码和/或调用基类
}


void CSetFormView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	//SetScrollSizes(MM_TEXT,CSize(0,0));
	//UPDATE_EASYSIZE;
  // UPDATE_EASYSIZE; 
      //if (m_bFisrtEasySize && IsWindowVisible()&& GetWindow(GW_CHILD)!=NULL)
      //{
      //      // INIT_EASYSIZE;
      //       m_bFisrtEasySize = FALSE;
      //       SetScrollSizes(MM_TEXT,CSize(0,0));
      //       UPDATE_EASYSIZE;
      //}else if (!m_bFisrtEasySize){
	 if (IsWindowVisible()&& GetWindow(GW_CHILD)!=NULL)
	 {
             //SetScrollSizes(MM_TEXT,CSize(0,1));
             //UPDATE_EASYSIZE;

			// CRect rs,rs1;
			// m_TabDataSet.GetClientRect(&rs);
			// rs1 = rs;
			// rs.top += 25;
			// rs.bottom -= 2;
			// rs.left += 1;
			// rs.right -= 2;
			// ////this->GetClientRect(rs1);
			// ////if(rs1.bottom < rs.bottom)
			//	////	rs1.bottom = rs.bottom;
			// ////if(rs1.left < rs.left)
			//	////	rs1.left = rs.left;
			// ////if(rs1.right < rs.right)
			//	////	rs1.right = rs.right;
			// ////if(rs1.top < rs.top)
			//	////	rs1.top = rs.top;
			// //m_TabDataSet.MoveWindow(&rs);
			//// //
			// 	int CurSel = m_TabDataSet.GetCurSel();
			//	switch(CurSel)
			//	{
			//		case 0:
			//			m_SetAppDlg.MoveWindow(&rs);
			//			m_SetAppDlg.ShowWindow(true);
			//			m_SetProdDlg.ShowWindow(false);
			//			m_SetRdDlg.ShowWindow(false);
			//			break;
			//		case 1:
			//			m_SetProdDlg.MoveWindow(&rs);
			//			m_SetProdDlg.ShowWindow(true);
			//			m_SetAppDlg.ShowWindow(false);
			//			m_SetRdDlg.ShowWindow(false);
			//			break;
			//		case 2:
			//			m_SetRdDlg.MoveWindow(&rs);
			//			m_SetRdDlg.ShowWindow(true);
			//			m_SetAppDlg.ShowWindow(false);
			//			m_SetProdDlg.ShowWindow(false);
			//			break;
			//	}
			 //重绘m_TabGraphSet中子控件

      }
	// TODO: 在此处添加消息处理程序代码
}


void CSetFormView::OnTcnSelchangeTabDataset(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CRect rs;
	m_TabDataSet.GetClientRect(&rs);
	rs.top += 25;
	rs.bottom -= 5;
	rs.left += 1;
	rs.right -= 2;
	int CurSel = m_TabDataSet.GetCurSel();
	switch(CurSel)
	{
		case 0:		
			m_SetAppDlg.MoveWindow(&rs);
			m_SetAppDlg.ShowWindow(true);
			m_SetProdDlg.ShowWindow(false);
			//m_SetRdDlg.ShowWindow(false);
			//m_DiagnosisDlg.ShowWindow(false);
			m_IssueDlg.ShowWindow(false);
			m_ProgramDlg.ShowWindow(false);
			//m_DustDlg.ShowWindow(false);
			m_FPGADownloadDlg.ShowWindow(false);
			//m_ArmUartDlg.ShowWindow(false);
			break;
		case 1:
			if(SystemLoader != 3 && SystemLoader !=2)
			{
				AfxMessageBox(_T("请以生产或研发人员身份登陆！"));
				return;
			}
			m_SetProdDlg.MoveWindow(&rs);
			m_SetProdDlg.ShowWindow(true);
			m_SetAppDlg.ShowWindow(false);
			//m_SetRdDlg.ShowWindow(false);	
			//m_DiagnosisDlg.ShowWindow(false);
			m_IssueDlg.ShowWindow(false);
			m_ProgramDlg.ShowWindow(false);
			//m_DustDlg.ShowWindow(false);
			m_FPGADownloadDlg.ShowWindow(false);
			//m_ArmUartDlg.ShowWindow(false);
			break;
		//case 2:
		//	if(SystemLoader != 3)
		//	{
		//		AfxMessageBox(_T("请以研发人员身份登陆！"));
		//		return;
		//	}
		//	m_SetRdDlg.MoveWindow(&rs);
		//	m_SetRdDlg.ShowWindow(true);
		//	m_SetProdDlg.ShowWindow(false);
		//	m_SetAppDlg.ShowWindow(false);	
		//	//m_DiagnosisDlg.ShowWindow(false);
		//	m_IssueDlg.ShowWindow(false);
		//	m_ProgramDlg.ShowWindow(false);
		//	m_DustDlg.ShowWindow(false);
		//	//m_ArmUartDlg.ShowWindow(false);
		//	break;
		case 2:	
			m_IssueDlg.MoveWindow(&rs);			
			m_IssueDlg.ShowWindow(true);
			m_ProgramDlg.ShowWindow(false);
			//m_DiagnosisDlg.ShowWindow(false);
			//m_SetRdDlg.ShowWindow(false);
			m_SetProdDlg.ShowWindow(false);
			m_SetAppDlg.ShowWindow(false);	
			//m_DustDlg.ShowWindow(false);
			m_FPGADownloadDlg.ShowWindow(false);
			//m_ArmUartDlg.ShowWindow(false);
			break;
		case 3:	
			m_ProgramDlg.MoveWindow(&rs);
			//m_DustDlg.ShowWindow(false);
			m_ProgramDlg.ShowWindow(true);
			m_IssueDlg.ShowWindow(false);
			//m_DiagnosisDlg.ShowWindow(false);
			//m_SetRdDlg.ShowWindow(false);
			m_SetProdDlg.ShowWindow(false);
			m_SetAppDlg.ShowWindow(false);	
			m_FPGADownloadDlg.ShowWindow(false);
			//m_ArmUartDlg.ShowWindow(false);
			break;
		case 4:	
			m_FPGADownloadDlg.MoveWindow(&rs);
			m_FPGADownloadDlg.ShowWindow(true);
			//m_ArmUartDlg.ShowWindow(false);
			m_ProgramDlg.ShowWindow(false);
			m_IssueDlg.ShowWindow(false);
			//m_DiagnosisDlg.ShowWindow(false);
			//m_SetRdDlg.ShowWindow(false);
			m_SetProdDlg.ShowWindow(false);
			m_SetAppDlg.ShowWindow(false);	
			//m_DustDlg.ShowWindow(false);
			
			break;
	}
}
