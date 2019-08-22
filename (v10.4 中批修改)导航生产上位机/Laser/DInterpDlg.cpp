// DInterpDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "DInterpDlg.h"
#include "afxdialogex.h"
#include "Si2dCfg.h"
#include "Interp.h"
#include "CAxis.h"
#include "CAxes.h"
#include "CLegend.h"
#include "CSeries.h"
#include "math.h"
#include "CPointSeries.h"
#include "CPointer.h"
#include "CPen0.h"
#define  arrayXYsize  100
// CDInterpDlg �Ի���


extern int g_nInterpNum;  //������ϵ������С
extern int g_datax[arrayXYsize];//��ȡ��Ͻ���������
extern int g_datay[arrayXYsize];
extern int g_nXY[100][2];  //��ʱ�������

extern int g_nCount = 0;

IMPLEMENT_DYNAMIC(CDInterpDlg, CDialogEx)

CDInterpDlg::CDInterpDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CDInterpDlg::IDD, pParent)
{

	m_start = 0;
	m_savepath = _T("d:\\xy.txt_H");
	m_interval = 32;
	m_end = 200000;
	m_movedata = 20000;
}

CDInterpDlg::~CDInterpDlg()
{
}

void CDInterpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_START, m_start);
	DDX_Text(pDX, IDC_EDIT_PATH, m_savepath);
	DDX_Text(pDX, IDC_EDIT_INTERVAL, m_interval);
	DDX_Text(pDX, IDC_EDIT_END, m_end);
	DDX_Text(pDX, IDC_EDIT_DIS, m_movedata);
	DDX_Control(pDX, IDC_TCHART2, m_tchartinterp);
}


BEGIN_MESSAGE_MAP(CDInterpDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_PATH, &CDInterpDlg::OnBnClickedButtonPath)
	ON_BN_CLICKED(IDC_BUTTON_START, &CDInterpDlg::OnBnClickedButtonStart)
END_MESSAGE_MAP()


// CDInterpDlg ��Ϣ�������


void CDInterpDlg::OnBnClickedButtonPath()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CSi2dCfg *pSi2dCfg;
	pSi2dCfg = (CSi2dCfg *)(this->GetParent());
    //pSi2dCfg->m_CombChSel.GetCurSel();


		UpdateData(TRUE);
	CFileDialog dlg(false); 
// 	dlg.m_ofn.lpstrDefExt = _T("txt");
// 	dlg.m_ofn.lpstrFilter = _T("All Files (*.*)\0*.*\0")  
// 	_T("H Files (*.txt_H)\0*.txt_H\0")  
// 	_T("L Files (*.txt_L)\0*.txt_L\0");
	
	CString l_strExt;
	//dlg.m_ofn.lpstrDefExt = _T("txt");
	if ( pSi2dCfg->m_CombChSel.GetCurSel() == 0)
	{
		dlg.m_ofn.lpstrFilter = _T("H Files (*.txt_H)\0*.txt_H\0");
		dlg.m_ofn.lpstrDefExt = _T("txt_H");
		dlg.m_ofn.hwndOwner = AfxGetMainWnd()->GetSafeHwnd();
		l_strExt = "txt_H";
		
	}
	else if ( pSi2dCfg->m_CombChSel.GetCurSel() == 1)
	{
		dlg.m_ofn.lpstrFilter = _T("L Files (*.txt_L)\0*.txt_L\0");
		dlg.m_ofn.lpstrDefExt = _T("txt_L");
		l_strExt = "txt_L";
	}
	
	if (dlg.DoModal())
	{
		m_savepath = dlg.GetPathName();
		CString l_sNameTmp =m_savepath.Right(m_savepath.GetLength()-m_savepath.ReverseFind('.')-1);
		if (l_strExt != l_sNameTmp)
		{
			MessageBox("��ѡ����ȷ���ļ���");
			return;
		}
	}


	int index =  pSi2dCfg->m_CombChSel.GetCurSel();
	CString l_sNameTmp;
	l_sNameTmp = m_savepath.Right(m_savepath.GetLength()-m_savepath.ReverseFind('.')-1);
	if (index == 0)
	{
		if(l_sNameTmp != "txt_H")
		{
			MessageBox("���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ�����ֵ�ļ���");
			//pLaserDlg->WriteLog(&pLaserDlg->m_recOperLog,"���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ�����ֵ�ļ���");
			//pLaserDlg->GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
			return;
		}
		
		
	}
	else if (index == 1)
	{
		if(l_sNameTmp != "txt_L")
		{
				MessageBox("���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ�����ֵ�ļ���");
// 			pLaserDlg->WriteLog(&pLaserDlg->m_recOperLog,"���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ�����ֵ�ļ���");
// 			pLaserDlg->GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
			return;
		}		
		
	}
	UpdateData(FALSE);

	
}


void CDInterpDlg::OnBnClickedButtonStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CSi2dCfg *pSi2dCfg;
	pSi2dCfg = (CSi2dCfg *)(this->GetParent());
	int index =pSi2dCfg->m_CombChSel.GetCurSel();

	CString l_sNameTmp;
	l_sNameTmp = m_savepath.Right(m_savepath.GetLength()-m_savepath.ReverseFind('.')-1);
	if (index == 0)
	{
		if(l_sNameTmp != "txt_H")
		{
			MessageBox("���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ�����ֵ�ļ���");
			//pLaserDlg->WriteLog(&pLaserDlg->m_recOperLog,"���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ�����ֵ�ļ���");
			//pLaserDlg->GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
			return;
		}
		
		
	}
	else if (index == 1)
	{
		if(l_sNameTmp != "txt_L")
		{
			MessageBox("���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ�����ֵ�ļ���");
			// 			pLaserDlg->WriteLog(&pLaserDlg->m_recOperLog,"���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ�����ֵ�ļ���");
			// 			pLaserDlg->GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
			return;
		}
		
		
	}
	UpdateData(TRUE);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	CFile sfile;
	BOOL pfile;
	int  i;
	static int j=0; //���int���� yjun
	double dx,dy = 0;
	CString inttostr = " ";
	CString temp;
	double pdx[arrayXYsize] = {0}; 
	double pdy[arrayXYsize] = {0};
	((CAxis)((CAxes)m_tchartinterp.get_Axis()).get_Bottom()).put_Maximum(72000);
	for (i=0;i<g_nInterpNum;i++)
	{
		pdx[i] = (double) g_datax[i];
		pdy[i] = (double) g_datay[i];
		((CSeries)m_tchartinterp.Series(1)).AddXY(pdx[i],pdy[i],NULL,RGB(255,0,0));
		((CSeries)m_tchartinterp.Series(0)).AddXY(pdx[i],pdy[i],NULL,RGB(255,255,0));
	}

	Interp hmh(pdx,pdy,g_nInterpNum);
	for (i = m_start;i<m_end;i+=m_interval)
	{
		dx = (double)i;
		if (hmh.GetYByX(dx,dy))
		{
			((CSeries)m_tchartinterp.Series(0)).AddXY(dx,dy,NULL,RGB(255,255,0));
			inttostr.Format("%d\t\t",(int)(dy+m_movedata));
			temp += inttostr;
			j++;
			if (j>4)
			{
				temp += "\r\n";
				j = 0;
			}
		}
		else
		{
			MessageBox("����Ĳ�ֵ����Դ����ȷ�����ʧ�ܣ�������ѡ������!");		
			PostNcDestroy();
			return ;
		}

	}
	//�����ļ�
	pfile = sfile.Open(m_savepath,CFile::modeReadWrite|CFile::modeCreate);
	if (pfile == NULL)
	{
		MessageBox("open file failure");
		return ;
	}
	sfile.SeekToEnd();
	sfile.Write(temp,temp.GetLength());
	sfile.Close();
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
}
