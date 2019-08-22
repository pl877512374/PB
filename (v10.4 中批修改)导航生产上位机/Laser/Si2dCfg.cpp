// Si2dCfg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "Si2dCfg.h"
#include "afxdialogex.h"
#include "NetParam.h"
#include "UserMsg.h"
#include "MainFrm.h"
#include "DInterpDlg.h"
#include "FileUtils.h"

// CSi2dCfg �Ի���
// CMainFrame
//д��־�߳�
 HANDLE hthread_WriteLog;
 DWORD WINAPI ThreadFuct_WriteLog(void *data);
 CString g_strLogfile;
 CString g_strMeasureDisfile;  
//��־�ļ�ָ��
 FILE *g_pfLogfile;  
 FILE *g_pfMeasureDisfile;
 STRUCT_LOG structLog;
 //��ǰӦ�ó���Ŀ¼
CString g_strCurDir = _T("");
//��־�ļ�Ŀ¼
CString g_strLogDir = _T("");

//�ж������ȡ�����Ƿ�ɹ�
bool PauseSendJudgeRuning=false;
int PauseSendCount = 0;
CMainFrame* pMainFrameForSi2dCfg; 
bool f_PXAtSi2dCfg = false;
#define  arrayXYsize  100
int  g_nfilelen = 0 ;     //Ҫ���͵��ļ��ܳ���
int g_nRevcNum1 = 0 ;  //�յ���1�Ű����� 
int g_nRevcNum2 = 0 ;  //�յ���2�Ű�����
int g_nRevcNum3 = 0 ;  //�յ���3�Ű�����
short bccW_packs=0;

int g_nFirstData = 0;//��һ��������������
long  g_nAvgStop1 = 0L; //ÿn����������ƽ��ֵ
long g_nAvgStop2 = 0; //ÿn�����½���ƽ��ֵ
long g_nSumStop1 = 0; //ÿn���������صĺ�
long g_nSumStop2 = 0; //ÿn�����½��صĺ�
int g_nReviseNum = 0;  //ÿ�������������
int g_nInterpNum = 0;  //������ϵ������С
int g_datax[arrayXYsize] = {0};//��ȡ��Ͻ���������
int g_datay[arrayXYsize] = {0};
int g_nXY[100][2] = {0};  //��ʱ�������
int g_nXYlen = 0;     //����ĳ���

CArray<int,int> g_array1; //�����������
CArray<int,int> g_array2; //��������׼��ֵ

IMPLEMENT_DYNAMIC(CSi2dCfg, CDialog)

CSi2dCfg::CSi2dCfg(CWnd* pParent /*=NULL*/)
	: CDialog(CSi2dCfg::IDD, pParent)
	, m_ReadAddress(0)
{

	m_nInterval = 0;
	m_nOneChartNum = 0;
	m_nCaCuNum = 0;
	m_nRecvNum = 0;
	m_nSaveNum = 0;
	m_nDrawNum = 0;
	m_nsubdata = 0;
	m_cacpath = _T("");
	m_bCacuFlag = FALSE;
}

CSi2dCfg::~CSi2dCfg()
{
}

void CSi2dCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_INTERVAL, m_nInterval);
	DDX_Text(pDX, IDC_EDIT_ONECHART, m_nOneChartNum);
	DDX_Text(pDX, IDC_EDIT_CACULATE, m_nCaCuNum);
	DDX_Control(pDX, IDC_RICHEDIT_OPERATEMESSAGE, m_recOperLog);
	DDX_Text(pDX, IDC_EDIT_RECVNUM, m_nRecvNum);
	DDX_Text(pDX, IDC_EDIT_SAVENUM, m_nSaveNum);
	DDX_Text(pDX, IDC_EDIT_DRAWNUM, m_nDrawNum);
	DDX_Text(pDX, IDC_EDITSUB, m_nsubdata);
	DDX_Control(pDX, IDC_BUTTON_PAUSE_SEND, m_btnPauseSend);
	DDX_Text(pDX, IDC_EDIT_FILE_PATH, m_cacpath);
	DDX_Control(pDX, IDC_RICHEDIT_DATA, m_recDataLog);
	DDX_Control(pDX, IDC_BUTTON_CACULATE, m_btnCaculate);
	DDX_Control(pDX, IDC_CHSEL, m_CombChSel);
	DDX_Control(pDX, IDC_COMBO_DATATYPE, m_DataType);
	DDX_Text(pDX, IDC_EDIT_READADDRESS, m_ReadAddress);
}


BEGIN_MESSAGE_MAP(CSi2dCfg, CDialog)
	
	ON_MESSAGE(WM_CHECK_MAIKUAN,OnCheckSend)
	ON_EN_CHANGE(IDC_EDIT_INTERVAL, &CSi2dCfg::OnEnChangeEditInterval)
	ON_EN_CHANGE(IDC_EDIT_ONECHART, &CSi2dCfg::OnEnChangeEditOnechart)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE_SEND, &CSi2dCfg::OnBnClickedButtonPauseSend)
	ON_BN_CLICKED(IDOK7, &CSi2dCfg::OnBnClickedOk7)
	ON_BN_CLICKED(IDC_BUTTON_REVISE, &CSi2dCfg::OnBnClickedButtonRevise)
	ON_BN_CLICKED(IDOK4, &CSi2dCfg::OnBnClickedOk4)
	ON_BN_CLICKED(IDC_BUTTON_CACULATE, &CSi2dCfg::OnBnClickedButtonCaculate)
	ON_BN_CLICKED(IDC_BUTTON_FITTING, &CSi2dCfg::OnBnClickedButtonFitting)
	ON_BN_CLICKED(IDOK8, &CSi2dCfg::OnBnClickedOk8)
	ON_BN_CLICKED(IDOK2, &CSi2dCfg::OnBnClickedOk2)
	ON_BN_CLICKED(IDC_BUTTON_SEND_DATA, &CSi2dCfg::OnBnClickedButtonSendData)
	ON_BN_CLICKED(IDC_BUTTON_QUERY, &CSi2dCfg::OnBnClickedButtonQuery)
	ON_EN_CHANGE(IDC_EDIT_CACULATE, &CSi2dCfg::OnEnChangeEditCaculate)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CSi2dCfg::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_CLEAN, &CSi2dCfg::OnBnClickedButtonClean)
	ON_EN_CHANGE(IDC_RICHEDIT_OPERATEMESSAGE, &CSi2dCfg::OnEnChangeRicheditOperatemessage)
	ON_MESSAGE(WM_WRITELOG, &CSi2dCfg::OnWritelog)
	ON_CBN_SELCHANGE(IDC_COMBO_DATATYPE, &CSi2dCfg::OnSelchangeComboDatatype)
	ON_EN_CHANGE(IDC_EDIT_READADDRESS, &CSi2dCfg::OnChangeEditReadaddress)
END_MESSAGE_MAP()


// CSi2dCfg ��Ϣ�������


BOOL CSi2dCfg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_nCaCuNum = 100;
	m_nInterval = 5;
	m_nOneChartNum = 100;
	m_CombChSel.SetCurSel(1);
	m_DataType.SetCurSel(0);
	UpdateData(false);

	pMainFrameForSi2dCfg = (CMainFrame*)AfxGetMainWnd();

	if (hthread_WriteLog <= 0)
	{
		DWORD thread1;
		hthread_WriteLog = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(ThreadFuct_WriteLog), this, 0, &thread1);
	}
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CSi2dCfg::OnEnChangeEditInterval()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	UpdateData(TRUE);
	if (m_nInterval <= 0 || m_nInterval >= 100)
	{ 
		m_nInterval = 5;
	}		
	UpdateData(FALSE);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CSi2dCfg::OnEnChangeEditOnechart()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
			UpdateData(TRUE);
		if (m_nOneChartNum <= 0 || m_nOneChartNum > 999)
		{
		  m_nOneChartNum = 999;
		
		}
		UpdateData(FALSE);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

afx_msg LRESULT CSi2dCfg::OnWritelog(WPARAM wParam, LPARAM lParam)
{
	CString TxtLog = (CString)((BSTR)wParam);
	SysFreeString((BSTR)wParam);

	CString str=(char*)lParam; 

	if (TxtLog == "")
	{
		return 0;
	}

	if(m_recOperLog.GetLineCount()>2000)
	{
		m_recOperLog.SetSel(0, -1);		
		m_recOperLog.ReplaceSel(_T(""));
	}
	m_recOperLog.SetWindowText(TxtLog);

	//CString getData = "";
	//CTime t=CTime::GetCurrentTime(); 
	//getData=t.Format("%y-%m-%d %H:%M:%S	") + str;
	//if(m_pMyLog)	//д����־
	//	m_pMyLog->SetNotify(getData.GetBuffer(getData.GetLength()));
	//getData.ReleaseBuffer();
	////ReleaseBuffer
	//CString strWndText ="";
	//m_TraceWnd.GetWindowText(strWndText);


	//if(m_TraceWnd.GetLineCount()>100)//��ʾ100��
	//{
	//	strWndText=strWndText.Right( strWndText.GetLength()-strWndText.Find(("\r\n"),1) );
	//}

	//strWndText = strWndText+"\r\n" +getData;
	//m_TraceWnd.SetWindowText(strWndText);
	//m_TraceWnd.LineScroll(MAXINT_PTR);

	//::SendMessage(hwnd,WM_RECVDATA,0,(LPARAM)(LPCTSTR)getData);
	return 0;
}


void CSi2dCfg::WriteLog(CRichEditCtrl *pREC, CString sLog, unsigned char *pbuf, unsigned short u16datalen)
{
	if (sLog != "")
	{
		structLog.strLogInfo[structLog.in] = sLog;	
	}
	else
	{
		structLog.strLogInfo[structLog.in] = "����ʾ��Ϣ";	
	}
	
	if (pbuf != NULL)
	{
		memcpy(&structLog.au8buf[structLog.in][0],pbuf,u16datalen);
		structLog.au16buflen[structLog.in] = u16datalen;
	}
	else
	{
		//memcpy(&strLog_Com.au8buf[strLog_Com.in][0],pbuf,u16datalen);
		structLog.au16buflen[structLog.in] = 0;
	}
	if((pbuf != NULL)||(sLog != ""))
	{
		structLog.in ++ ;
		structLog.in %= MAXLOGINFO;	
	}
	
	if(pREC->GetLineCount()>2000)
	{
		pREC->SetSel(0, -1);		
		pREC->ReplaceSel(_T(""));
	}
}

DWORD WINAPI PauseSendJudgeThreadProc(LPVOID lpParam)
{
	Sleep(1000);
	while (PauseSendJudgeRuning == true)
	{
		if (PauseSendCount == 0)
		{
			CTime time=CTime::GetCurrentTime();
			char Hour= time.GetHour();
			char Miu= time.GetMinute();
			char Sec= time.GetSecond();
			unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x01,0x02};
			ZhiLing(Single_zhiling,pn,0);
			Single_zhiling[29]=0x01;


			pMainFrameForSi2dCfg->SendMessage(WM_SENDCMD, Single_zhilingSize, (LPARAM)Single_zhiling);
			
		}
		else
		{
			PauseSendCount = 0;
		}
		Sleep(1000);
	}
	return 0;
}

void CSi2dCfg::OnBnClickedButtonPauseSend()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	CString l_strBtnName;
	m_btnPauseSend.GetWindowText(l_strBtnName);
	
	if (l_strBtnName == "��ͣ����")
	{
		PauseSendJudgeRuning = false;
		CTime time=CTime::GetCurrentTime();
		char Hour= time.GetHour();
		char Miu= time.GetMinute();
		char Sec= time.GetSecond();
		unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x01,0x02};
		ZhiLing(Single_zhiling,pn,0);
		Single_zhiling[29]=0x02;
		AfxGetMainWnd()->PostMessage(WM_SENDCMD, Single_zhilingSize, (LPARAM)Single_zhiling);

		m_btnPauseSend.SetWindowText("��ʼ����");
	}
	else
	{
		GraphForm2d * pView	;
		CString l_strBtnName;
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		pView = (GraphForm2d*) pMainFrame->m_wndLeftSplitter.GetPane(0,0);
		pView->m_scanf2ddlg.m_sScan2dCfgDlg.GetDlgItem(IDC_BUTTON_SICKCONTINUEFRAME)->GetWindowText(l_strBtnName);
		if (l_strBtnName == "ֹͣ��ȡ")
		{
			pView->m_scanf2ddlg.m_sScan2dCfgDlg.OnBnClickedButtonSickcontinueframe();
		}

		CTime time=CTime::GetCurrentTime();
		char Hour= time.GetHour();
		char Miu= time.GetMinute();
		char Sec= time.GetSecond();
		unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x01,0x02};
		ZhiLing(Single_zhiling,pn,0);
		Single_zhiling[29]=0x01;
	
		AfxGetMainWnd()->PostMessage(WM_SENDCMD, Single_zhilingSize, (LPARAM)Single_zhiling);
		//AfxGetMainWnd()->PostMessage(WM_CONNECT7070);
		m_btnPauseSend.SetWindowText("��ͣ����");
		
		PauseSendJudgeRuning=true;
		DWORD ThreadID;
		HANDLE hThreadPauseSendJudge;
		PauseSendCount = 0;
		hThreadPauseSendJudge = CreateThread(NULL,0,PauseSendJudgeThreadProc,this,NULL,&ThreadID);
	}
}


void CSi2dCfg::OnBnClickedOk7()
{
	GraphForm2d * pView	;
    CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pView = (GraphForm2d*) pMainFrame->m_wndLeftSplitter.GetPane(0,0);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	pView->SendMessage(WM_NETCMD_CLEARCHARTSI,g_nNetRecvInd,1);
	pView->SendMessage(WM_NETCMD_CLEARCHARTSI,g_nNetRecvInd,2);
	pView->SendMessage(WM_NETCMD_CLEARCHARTSI,g_nNetRecvInd,3);
}

DWORD WINAPI ThreadFuct_WriteLog(void *data)
{
// 	WIN32_FIND_DATA fileInfo;
// 	HANDLE hFind;
// 	DWORD fileSize;
	structLog.in = 0;	
	structLog.out = 0;
	const char *fileName = "123";
	unsigned short i;
	CString strTmp; 
	CSi2dCfg *pDlg = (CSi2dCfg *)data;
	while(1)
	{
// 		fileName = "LOG.txt";
// 		hFind = FindFirstFile(fileName ,&fileInfo);
// 		if(hFind != INVALID_HANDLE_VALUE)
// 			fileSize = fileInfo.nFileSizeLow;
// 		FindClose(hFind);
// 		if (fileSize >= LOGFILEMAXSIZE)
// 		{
// 			g_strLogfile_Com = g_strLogfile_Com.Left(g_strLogfile_Com.GetLength() - 26);
// 			
// 			CTime time = CTime::GetCurrentTime();
// 			g_strLogfile_Com = g_strLogfile_Com + time.Format("Log_Com_%Y%m%d%H%M%S.txt");
// 			
// 		}
		if (structLog.in == structLog.out)
		{
			Sleep(10);
			continue;
		}
		
		CString l_strInfoTxt;
		//l_strInfoTxt.Format("%s", MSystemTime());
		CString strLog_Time = CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S\r\n"));
		l_strInfoTxt = strLog_Time;
		g_strLogfile = CTime::GetCurrentTime().Format(_T("%Y-%m-%d-%H.txt"));
		if (structLog.strLogInfo[structLog.out] != "")
		{
			l_strInfoTxt += structLog.strLogInfo[structLog.out] + _T("\r\n");	
			if (structLog.strLogInfo[structLog.out].Left(9) == _T("������ "))
			{
				l_strInfoTxt = CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S__"));
				l_strInfoTxt += structLog.strLogInfo[structLog.out] + _T("\r\n");
				if (g_strMeasureDisfile == _T(""))
				{
					g_strMeasureDisfile =g_strLogDir +  CTime::GetCurrentTime().Format(_T("������_%Y-%m-%d-%H.txt"));
				}
				if((g_pfMeasureDisfile=fopen(g_strMeasureDisfile,_T("at+")))==NULL)
				{
					structLog.out ++;
					structLog.out %= MAXLOGINFO;
					continue;
				}
				else
				{
					fseek(g_pfMeasureDisfile,0,SEEK_END);
					fwrite(l_strInfoTxt,strlen(l_strInfoTxt),1,g_pfMeasureDisfile);            
					fclose(g_pfMeasureDisfile);
				}
				structLog.out ++;
				structLog.out %= MAXLOGINFO;
				continue;
			}

		}
		
		if (structLog.au16buflen[structLog.out] != 0)
		{
			for ( i = 0;  i < structLog.au16buflen[structLog.out];  i++)
			{
				strTmp.Format("%02X ", structLog.au8buf[structLog.out][i]);
				l_strInfoTxt += strTmp;
			}
			l_strInfoTxt += "\r\n";
		}

		if ((_T("���ݷ��ͳɹ���") != structLog.strLogInfo[structLog.out])&&(_T("���յ����ݣ�") != structLog.strLogInfo[structLog.out]))
		{
			pDlg->m_recOperLog.SetSel(-1, -1);        
			pDlg->m_recOperLog.ReplaceSel(l_strInfoTxt);        
			pDlg->m_recOperLog.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
		}
		else
		{
// 			if (pDlg->m_bComShowSendOrRecvInfo == TRUE)
			{
				pDlg->m_recOperLog.SetSel(-1, -1);        
				pDlg->m_recOperLog.ReplaceSel(l_strInfoTxt);        
				pDlg->m_recOperLog.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
			}	
		}
		
		if((g_pfLogfile=fopen(g_strLogDir + g_strLogfile,"at+"))==NULL)
		{
			continue;
		}
		else
		{
			fseek(g_pfLogfile,0,SEEK_END);
			fwrite(l_strInfoTxt,strlen(l_strInfoTxt),1,g_pfLogfile);            
			fclose(g_pfLogfile);
		}
		//if ("UDP�������ӳɹ�" == structLog.strLogInfo[structLog.out])
		//{
		//	AfxGetMainWnd()->PostMessage(WM_INQUIREZERO, nBeatFrameSize, (LPARAM)acBeatFrameBuf);
		//}
		structLog.out ++;
		structLog.out %= MAXLOGINFO;
	}
	return 0;
}

void CSi2dCfg::OnBnClickedButtonRevise()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
		//����������λ��
	CFileDialog dlg(FALSE);
	dlg.m_ofn.lpstrDefExt = _T("txt");
	dlg.m_ofn.lpstrFilter = _T("All Files (*.*)\0*.*\0")  
	_T("H Files (*.txt_H)\0*.txt_H\0")
	_T("L Files (*.txt_L)\0*.txt_L\0");
	if(m_CombChSel.GetCurSel() == 0)
	{
		dlg.m_ofn.lpstrFilter =_T("H Files (*.txt_H)\0*.txt_H\0");
	}
	else
	{
		dlg.m_ofn.lpstrFilter =_T("L Files (*.txt_L)\0*.txt_L\0");
	}
	if (dlg.DoModal())
	{
		m_cacpath = dlg.GetPathName();
	}
	
	CString m_string ="";
	GetDlgItemText(IDC_BUTTON_REVISE,m_string);
	g_nFirstData = 0;
	if (m_string== "����")
	{
		m_string =  CTime::GetCurrentTime().Format("%m-%d %H:%M:%S");
		WriteLog(&m_recDataLog,m_string);
		m_string = "----��ʼ����----\r\n";

		m_recDataLog.SetSel(-1, -1);        
		m_recDataLog.ReplaceSel(m_string);        
		m_recDataLog.PostMessage(WM_VSCROLL, SB_BOTTOM,0);

		//WriteLog(&m_recDataLog,m_string);
		m_btnCaculate.EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_REVISE)->SetWindowText("��������");
		g_array1.SetSize(30,-1);
		g_array2.SetSize(30,-1);
	}	

	if (m_string =="��������")
	{
	    int nResult = MessageBox("�Ƿ�Ҫ��������!","��ʾ",MB_ICONINFORMATION | MB_OKCANCEL);
		if (IDOK == nResult)
		{		  
		   g_nReviseNum = 0;
		   m_string =  CTime::GetCurrentTime().Format("%m-%d %H:%M:%S");
		   WriteLog(&m_recDataLog,m_string);
		   m_string = "----���¿�ʼ����----\r\n";
		   m_recDataLog.SetSel(-1, -1);        
		   m_recDataLog.ReplaceSel(m_string);        
		   m_recDataLog.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
		  // WriteLog(&m_recDataLog,m_string);
		   //�����㰴ťû�лظ�������״̬�����������ָ������״̬
		   if (m_btnCacuEnable == FALSE)
		   {
			   m_btnCacuEnable = TRUE;
			   m_btnCaculate.EnableWindow(TRUE);	
			   //m_edtCaculate.EnableWindow(TRUE);
			   m_btnCaculate.SetWindowText("����");
		   }
		   g_array1.RemoveAll();
	       g_array2.RemoveAll();

		   g_array1.SetSize(30,-1);
	       g_array2.SetSize(30,-1);
		   
		}
	    if (IDCANCEL == nResult)
		{
		   return;
		}  
	}	
}


void CSi2dCfg::OnBnClickedOk4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//	CFileUtils fileUtils;   //2016

	CString strRec = "";     //��ʾ��
	CString strRec_x = "";   
	CString strRec_y = "";
	CString hustring = "";   //�����ʱ��һ������
	CString test;
	int i = 0;	
	GetDlgItem(IDC_BUTTON_FITTING)->EnableWindow(TRUE);
	//CFileDialog dlg(TRUE);
	//dlg.m_ofn.lpstrDefExt = _T("txt");
	//dlg.m_ofn.lpstrFilter = _T("*.txt");
	//if (dlg.DoModal())
	//{
		//m_sFilePath = dlg.GetPathName();
		if(m_CombChSel.GetCurSel() == 0)
		{
			/*m_sFilePath = CFileUtils::OpenFileDlg(TRUE,_T("H Files (*.txt_H)\0*.txt_H\0"),NULL,NULL,NULL);*/
			m_sFilePath = CFileUtils::OpenFileDlg(TRUE,_T("H Files (*.txt_H)|*.txt_H||"),NULL,NULL,NULL);
		}
		else if(m_CombChSel.GetCurSel() == 1)
		{
			m_sFilePath = CFileUtils::OpenFileDlg(TRUE,_T("L Files (*.txt_L)|*.txt_L||"),NULL,NULL,NULL);
		}
	//}
	CStdioFile hufile;
	if (hufile.Open(m_sFilePath,CFile::modeRead))
	{   
		hustring ="��-��                ��-��\r\n";
		m_recDataLog.SetSel(-1, -1);        
		m_recDataLog.ReplaceSel(hustring);        
		m_recDataLog.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
//		UpdateData(FALSE);
		memset(g_datax, 0,arrayXYsize);
		memset(g_datay, 0,arrayXYsize);
		while (hufile.ReadString(hustring))
		{
			hustring += "\r\n";
			m_recDataLog.SetSel(-1, -1);        
			m_recDataLog.ReplaceSel(hustring);        
			m_recDataLog.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
		    hustring.TrimLeft();
			if (hustring == "")
			{
				break;
			}
			sscanf(hustring,"%d %d",&g_datax[i],&g_datay[i]);
			i++;
		}
		g_nInterpNum = i;
		hufile.Close();	
// 		strRec_x.Format("%d",i);
// 		MessageBox(strRec_x);
	}
	else
	{
		MessageBox("���ļ�ʧ�ܣ�");
		return ;
	}
	hustring ="**********�ָ���***********\r\n";
	m_recDataLog.SetSel(-1, -1);        
	m_recDataLog.ReplaceSel(hustring);        
	m_recDataLog.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
	 //WriteLog(&m_recDataLog,hustring);

/*	m_sFilePath = fileUtils.OpenDirDlg(this->GetSafeHwnd(),"ѡ�������ļ���",fileUtils.GetCurDir());




	if (m_sFilePath != "")
	{
		CFileFind fileFind;
		BOOL res;	
		CString sFileName;
		CString sTmp;
		res = fileFind.FindFile(m_sFilePath+"*.fit"); //txt
		m_slFileName.RemoveAll();
		while (res)
		{
			res = fileFind.FindNextFile();
			if (!fileFind.IsDirectory() && !fileFind.IsDots())
			{
				sFileName = fileFind.GetFileName();
				m_slFileName.AddTail(sFileName);
			}			
		}
		fileFind.Close();
		int i = 0;
		for (i = 0;i<m_slFileName.GetCount();i++)
		{
			ReadDatatoArray(m_sFilePath+m_slFileName.GetAt(m_slFileName.FindIndex(i)));
		}
		strRec = "     ��-��       ��-��";
		WriteData(&m_recDataDis,strRec);
		for (i=0;i<g_nCount;i++)
		{
		  strRec_x.Format("%5d",g_datax[i]);		  
		  strRec_y.Format("%4d",g_datay[i]);
		  strRec = "     "+strRec_x + "     "+strRec_y;
		  WriteData(&m_recDataDis,strRec);		  
		}
		strRec = "-----��ȡ���----";
		WriteData(&m_recDataDis,strRec);
	}*/
	bFILEFitting = TRUE;//���ļ��������	
}
static int s_nCount = 0;     //���Ʊ���

int g_nSaveNum = 0;          //�������
int g_nCacuNum1 = 0;         //�������
int g_nCacuNum2 = 0;

void CSi2dCfg::OnBnClickedButtonCaculate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
		m_bCacuFlag = TRUE;
		m_btnCaculate.EnableWindow(FALSE);
		//m_edtCaculate.EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_FITTING)->EnableWindow(TRUE);
		m_btnCaculate.SetWindowText("������...");
		m_btnCacuEnable = FALSE;//��ֹ��ťһֱ���ڲ�����״̬
		//��ʼ������
		g_nSumStop1 = 0;
		g_nSumStop2 = 0;
		g_nCacuNum1 = 0;
    	g_nCacuNum2 = 0;
		g_nAvgStop1 = 0;
		g_nAvgStop2 = 0;

		g_nCacuNum1 = 0;  //���յ���·����
		g_nCacuNum2 = 0;


}


void CSi2dCfg::OnBnClickedButtonFitting()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDInterpDlg *pinterp = new CDInterpDlg(this);
	pinterp->Create(IDD_DIALOG_INTERP,this);
	pinterp->ShowWindow(SW_SHOW);
}

//���ݷ��ͱ���
int DataTableType = 0;  // 0Ϊ������1Ϊ�����ʱ�2Ϊƫ�ı�
int g_nNum = 0;
BOOL g_flag = FALSE;
HANDLE g_evnet = CreateEvent(NULL,TRUE,FALSE,NULL);
#define readbuflen  50000   //��д����������
int g_nReadbuf[readbuflen] ;  //��д����
int g_nReadbuflen = 0; //ʵ�ʳ���
//���ݷ��ͱ���

void CSi2dCfg::OnBnClickedOk8()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	DataTableType=m_DataType.GetCurSel();
	if (DataTableType==0)  //������
	{
		if(m_CombChSel.GetCurSel() == 0)
		{
			/*m_sFilePath = CFileUtils::OpenFileDlg(TRUE,_T("H Files (*.txt_H)\0*.txt_H\0"),NULL,NULL,NULL);*/
			m_sFilePath = CFileUtils::OpenFileDlg(TRUE,_T("H Files (*.txt_MK)|*.txt_MK||"),NULL,NULL,NULL);
		}
		else if(m_CombChSel.GetCurSel() == 1)
		{
			m_sFilePath = CFileUtils::OpenFileDlg(TRUE,_T("L Files (*.txt_MK)|*.txt_MK||"),NULL,NULL,NULL);
		}
	} 
	else if(DataTableType==1)  //�����ʱ�
	{
		m_sFilePath = CFileUtils::OpenFileDlg(TRUE,_T("Files (*.txt)|*.txt||"),NULL,NULL,NULL);
	}
	else if(DataTableType==2)  //����ƫ�ı�
	{
		m_sFilePath = CFileUtils::OpenFileDlg(TRUE,_T("PX Files (*.txt_PX)|*.txt_PX||"),NULL,NULL,NULL);
	}
	
	CStdioFile hufile;
 // //  sendnum = 1;
	g_nNum = 0;
	g_flag = FALSE;
	ResetEvent(g_evnet);
	CString l_sNameTmp;//���ļ��ĺ�׺��
	l_sNameTmp = m_sFilePath.Right(m_sFilePath.GetLength()-m_sFilePath.ReverseFind('.')-1);
	
	if (DataTableType==0)  //������
	{
		if (m_CombChSel.GetCurSel() == 0)
		{
			if(l_sNameTmp != "txt_MK")
			{
				WriteLog(&m_recOperLog,"���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ��txt_MK�ļ���");
				GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
				MessageBox("��ѡ���ļ���");
				return;
			}
		}
		else if (m_CombChSel.GetCurSel() == 1)
		{
			if(l_sNameTmp != "txt_MK")
			{		
				WriteLog(&m_recOperLog,"���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ��txt_MK�ļ���");
				GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
				MessageBox("��ѡ���ļ���");
				return;
			}
		}
	} 
	else if(DataTableType==1)  //�����ʱ�
	{
		if(l_sNameTmp != "txt")
		{		
			WriteLog(&m_recOperLog,"���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ��txt�ļ���");
			GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
			MessageBox("��ѡ�����ʱ��ļ���");
			return;
		}
	}
	else if(DataTableType==2)  //����ƫ�ı�
	{
		if(l_sNameTmp != "txt_PX")
		{		
			WriteLog(&m_recOperLog,"���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ��txt_PX�ļ���");
			GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
			MessageBox("��ѡ������ƫ�ı��ļ���");
			return;
		}
	}
	
	if (hufile.Open(m_sFilePath,CFile::modeRead))
	{
		GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(TRUE);
	}	
}


void CSi2dCfg::OnBnClickedOk2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CStdioFile hufile;
	int i;
	CString  strXY;
	CString l_sSavePath;
	l_sSavePath = m_cacpath;

	if (hufile.Open(l_sSavePath,CFile::modeReadWrite|CFile::modeNoTruncate|CFile::modeCreate))
	{
		for (i=0;i<g_nXYlen;i++)
		{
			strXY.Format("%d      %d\n",g_nXY[i][0],g_nXY[i][1]);			
			hufile.WriteString(strXY);
		}
		hufile.Close(); 
		
		strXY.Format("������%d������ \n",g_nXYlen);
		g_nXYlen = 0;
		/*m_recDataDis.SetSel(-1,-1);
		m_recDataDis.ReplaceSel(strXY);*/
		m_recDataLog.SetSel(-1, -1);        
		   m_recDataLog.ReplaceSel(strXY);        
		   m_recDataLog.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
	}	
	else
	{
		//m_recDataDis.SetSel(-1,-1);
		//m_recDataDis.ReplaceSel("�޷����ļ�������ʧ��\r\n");
				m_recDataLog.SetSel(-1, -1);        
		   //m_recDataLog.ReplaceSel(strXY);        
		   m_recDataLog.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
	}
}
#define Frame_02020202 1

int SetPacket(char index, char* sendpacket, int* datapacket, int len, int num)  //Դ���ݵĳ��� ����
{
#if Frame_02020202
	bool calcuBCC = TRUE;
	static int currentNum=-1;
	if (num != currentNum)
	{
		currentNum = num;
		calcuBCC = TRUE;
	}
	else
	{
		calcuBCC = FALSE;
	}
	char acTmp[2000];
	int i= 0 ;
	int j =512*(num-1);//���Ŵ�0��ʼ
	int dlen;
	int plen = (len%512==0)?(len/512):(len/512+1);
	CString tm;
	tm.Format("%d,%d",len,plen);
	TRACE(tm);
	if (j>=len)
	{
		return 0;
	}

	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();

	if (DataTableType == 0)
	{
		if(index==0)
		{
			unsigned char zhiling[26]={0xFF,0xAA,0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x0C,0x01,0x00};//����ֵ
			memcpy(acTmp,zhiling,26);
		}
		else
		{
			unsigned char zhiling[26]={0xFF,0xAA,0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x0C,0x02,0x00};//����ֵ
			memcpy(acTmp,zhiling,26);
		}
	}
	else if (DataTableType == 1)
	{
		unsigned char zhiling[26]={0xFF,0xAA,0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x11,0x00,0x00};//������
		memcpy(acTmp,zhiling,26);
	}
	else if (DataTableType == 2)
	{
		unsigned char zhiling[26]={0xFF,0xAA,0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x16,0x00,0x00};//����ƫ�ı�
		memcpy(acTmp,zhiling,26);
	}
	
// 	sendpacket[0] = (char)0xFF;//֡ͷ
// 	sendpacket[1] = (char)0xFF;
// 	sendpacket[2] = (char)0x58;//����
// 	sendpacket[3] = (char)num;//����
// 	sendpacket[6] = (char)plen;    //�ܰ���
// 	sendpacket[7] = (char)plen>>8;

//	acTmp[9] = (char)0x58;//����
	
	
	acTmp[26] = ((char)plen>>8);    //�ܰ���
	acTmp[27] = ((char)plen & 0xff);
	acTmp[28] = ((char)num>>8);//����
	acTmp[29] = ((char)num & 0xff);
	int packlen_cur=0;
	if (DataTableType == 2)    //����ƫ�ı�
	{
		for (i = 0; i<512 && (j+i)<len ;i++)
		{
			acTmp[32+i] = (char) (datapacket[j+i]);
			packlen_cur++;
		}
		j=packlen_cur;
		if (calcuBCC)
		{
			char bccTemp = Xor_download((char)(bccW_packs & 0xff), &acTmp[32], packlen_cur);
			bccW_packs = bccTemp & 0x00ff;
		}
	}
	else
	{
		for (i = 0; i<512 && (j+i)<len ;i++)
		{

			acTmp[33+i*2] = (char) (datapacket[j+i]>>8); 
			acTmp[32+i*2] = (char) (datapacket[j+i]);
			packlen_cur++;
		}
		j=packlen_cur*2;
		if (calcuBCC)
		{
			bccW_packs = XorMaiKuan_download(bccW_packs, (short*)(&acTmp[32]), packlen_cur);
		}
	}
	
	acTmp[30] = (char)(j>>8);//�������ݳ���
	acTmp[31] = (char)(j & 0xff);
	int lenth=j+32;
	acTmp[2]=(char)(lenth>>8);
	acTmp[3]=(char)(lenth& 0xff);

	
memcpy(sendpacket,acTmp,(j+36));
	return (j+36);

#else
	int i= 0 ;
	int j =512*(num-1);//���Ŵ�0��ʼ
	int dlen;
	int plen = (len%512==0)?(len/512):(len/512+1);
	CString tm;
	tm.Format("%d,%d",len,plen);
	TRACE(tm);
	if (j>len)
	{
		return 0;
	}	
	sendpacket[0] = (char)0xFF;//֡ͷ
	sendpacket[1] = (char)0xFF;
	sendpacket[2] = (char)0x58;//����
	sendpacket[3] = (char)num;//����
	sendpacket[6] = (char)plen;    //�ܰ���
	sendpacket[7] = (char)plen>>8;
	
	for (i = 0; i<512 && (j+i)<len ;i++)
	{
		sendpacket[12+i*2] =(char) (datapacket[j+i]);
		sendpacket[13+i*2] = (char) (datapacket[j+i]>>8); 
	}
    j = i*2+14;
	dlen = i*2;
	sendpacket[4] = (j&0xff);//������
	sendpacket[5] = (j>>8)&0xFF;
	sendpacket[8] = (char)0x55;
	sendpacket[9] = (char)0xaa;
	sendpacket[10] = (j-14)&0xff;
	sendpacket[11] = ((j-14)>>8)&0xff;
	sendpacket[j-2] = (char)0xFF;
	sendpacket[j-1] = (char)0xFF;
	return j;
#endif

} 

//��д�����̺߳���
UINT SendDataThreadProc(LPVOID lpParm)
{
	bccW_packs=0;
	CSi2dCfg* pDlg = (CSi2dCfg*)lpParm;
	CFileUtils fileutils;
    const int nFrameLen = 1036;//֡��
	const int nDataLen = 1024; //���ݳ���
	const int nMaxFileData = 512*100;//�ļ�����������
// 	char *pSendData; //�������ݻ���
// 	char* pStr;
//	char* pTemp;
//	int* pArray;	
	int /*i,j,*/k=0;  
	int nIndex1 = 0;
	int nIndex2 = 0;
//     DWORD nLen;
	DWORD dw ;
	CString hustring;
	int packetlen;//udp���ĳ���
	int cfnum=0;//�ط�����
	int g_nReadbuf[readbuflen] = {0};  //��д����
	int g_nReadbuflen = 0; //ʵ�ʳ���
	char psenddata[2000];
	int sendnum =1;
	CString str;
	DWORD biao;
    if (pDlg->m_sFilePath == "")
    {
		pDlg->MessageBox("���ȶ�ȡ�����ļ���",MB_OK);
		return -1;
    }
    pDlg->GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
	CStdioFile hufile;
	if (hufile.Open(pDlg->m_sFilePath,CFile::modeRead))
	{	
		while(hufile.ReadString(hustring))
		{
			hustring.TrimLeft();
			hustring.TrimRight();
			if (hustring == "")
			{
				break;
			}
			biao = sscanf(hustring,"%d %d %d %d %d",&g_nReadbuf[k],&g_nReadbuf[k+1],&g_nReadbuf[k+2],&g_nReadbuf[k+3],&g_nReadbuf[k+4]);
			if (biao != 0)
			{
				k += biao;
				if (biao!=5)
				{
					break;
				}
			}
		}
		hufile.Close();
	}//�������ݳɹ�
	//SOCKET sockSrv
//	hustring.Format("%d",k);
//	pDlg->MessageBox(hustring);
	if( 0 == k)
	{
		pDlg->MessageBox("�ļ��������ݣ�",MB_OK);
		return 0;
	}
	//DWORD dwIP;
	//pDlg->m_CtrlIPAddrTarget.GetAddress(dwIP);	
	//SOCKADDR_IN addrTo;
	//addrTo.sin_family=AF_INET;
	//addrTo.sin_port=htons(pDlg->m_editNetPortTarget);
	//addrTo.sin_addr.S_un.S_addr=htonl(dwIP);
	while (1)
	{
	/*	str.Format("%d",k);
		pDlg->MessageBox(str);*/
		packetlen =SetPacket(pDlg->m_CombChSel.GetCurSel(), psenddata, g_nReadbuf, k, sendnum);//�ӵ�1������
		if (packetlen != 0)  //����
		{			
//			pDlg->m_NetConnection.pcSendDataBuf = psenddata;
//			pDlg->m_NetConnection.nSendDataBufSize = packetlen;
//			str.Format("%d",packetlen);
////			pDlg->MessageBox("packetlen = "+str);
//
//
//			if (pDlg->m_comboNetProtocol.GetCurSel() == 0)//yjun ���tcp����
//				SendData(&(pDlg->m_NetConnection));
//			else
//				UDP_SendData(&(pDlg->m_NetConnection), &addrTo);  

			CMainFrame *pMainDlg;
			pMainDlg = (CMainFrame *)AfxGetApp()->m_pMainWnd;

			pMainDlg->m_NetConnection.pcSendDataBuf = psenddata;
			pMainDlg->m_NetConnection.nSendDataBufSize = packetlen;
			//pView1->SendMessage(WM_STATEDLG_STOPTIMER,0,0);
			//Sleep(10);
			if (pMainDlg->m_NetConnection.bConnected)
			SendData(&pMainDlg->m_NetConnection);
			
		}
		else
		{			
			break;  //������ ����
		}
		dw = WaitForSingleObject(g_evnet,10000); //���¼�  ������ INFINITE 
		ResetEvent(g_evnet);

		switch(dw)
		{
		case WAIT_OBJECT_0:
			//�ȵ��¼�			
				if (g_flag == TRUE && g_nNum == sendnum)  //���ͳɹ�
				{
					sendnum ++;
					g_flag = FALSE;
					cfnum = 0;
				}
				else                      //����ʧ��
				{
				//	sendnum = 1;
				    str.Format("%d",g_nNum);
					//pDlg->MessageBox("��" + str +"��������дʧ�ܣ���������д��",MB_OK);
					if (cfnum > 2)
					{
						pDlg->MessageBox("��" + str +"��������дʧ�ܣ���������д��",MB_OK);
						pDlg->GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(TRUE);
						pDlg->GetDlgItem(IDC_BUTTON_SEND_DATA)->SetWindowText("����"); 
						return 0;
					}
					//pDlg->GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(TRUE);
					//pDlg->GetDlgItem(IDC_BUTTON_SEND_DATA)->SetWindowText("����"); 
					cfnum ++;
				}			
			break;
		case WAIT_TIMEOUT:				
			//��ʱδ�յ��ظ�		
			if (cfnum > 2)
			{
				str.Format("%d",sendnum);
				pDlg->WriteLog(&(pDlg->m_recOperLog),"��ʱδ�յ��ظ�");
				pDlg->MessageBox("��ʱδ�յ���"+str+"���ظ�",MB_OK);

				pDlg->GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
				pDlg->GetDlgItem(IDC_BUTTON_SEND_DATA)->SetWindowText("����"); 
				sendnum = 1;
				return 0;
			}
			else
			{
				str.Format("%d",sendnum);
				pDlg->WriteLog(&(pDlg->m_recOperLog),"��ʱδ�յ��ظ����ط�");
				pDlg->MessageBox("��ʱδ�յ���"+str+"���ظ����ط�",MB_OK);
				cfnum ++;
			}
			break;
		case WAIT_FAILED:
			return 0;
			break;
		}			
		
		Sleep(500);

	}//while
	//pDlg->MessageBox("������д���ͳɹ�",MB_OK);
	//AfxGetMainWnd()->SendMessage(WM_CHECK_MAIKUAN,OnCheckSend);
	//����������֮���飬ͨ��֮�����д
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	if (DataTableType == 0)   //������
	{
		unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x0D};
		ZhiLing(ShaoXie_zhiling,pn,0);
		CString l_sNameTmp;
		l_sNameTmp = pDlg->m_sFilePath.Right(pDlg->m_sFilePath.GetLength()- (pDlg->m_sFilePath.ReverseFind('.'))-1);	
		if(l_sNameTmp == "txt_H")
		{
			ShaoXie_zhiling[24]=0x01;
		}
		else
		{
			ShaoXie_zhiling[24]=0x02;
		}
	} 
	else if(DataTableType == 1)  //�����ʱ�
	{
		unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x12};
		ZhiLing(ShaoXie_zhiling,pn,0);
	}
	else if(DataTableType == 2)  //����ƫ�ı�
	{
		unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x17};
		ZhiLing(ShaoXie_zhiling,pn,0);
	}

	ShaoXie_zhiling[26]=(bccW_packs>>8);
	ShaoXie_zhiling[27]=bccW_packs;
	AfxGetMainWnd()->PostMessage(WM_SENDCMD,ShaoXie_zhilingSize, (LPARAM)ShaoXie_zhiling);
	//pDlg->GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(TRUE);
	pDlg->GetDlgItem(IDC_BUTTON_SEND_DATA)->SetWindowText("����");
	return 0;
}

LONG CSi2dCfg::OnCheckSend(WPARAM wParam, LPARAM lParam)
{
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x20,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x04,0x05,0x05};
	ZhiLing(ShaoXie_zhiling,pn,0);
	AfxGetMainWnd()->PostMessage(WM_SENDCMD,ShaoXie_zhilingSize, (LPARAM)ShaoXie_zhiling);
  return 1;
}

void CSi2dCfg::OnBnClickedButtonSendData()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString l_sNameTmp;//���ļ��ĺ�׺��
	l_sNameTmp = m_sFilePath.Right(m_sFilePath.GetLength()-m_sFilePath.ReverseFind('.')-1);
	
	if (DataTableType==0)  //������
	{
		if (m_CombChSel.GetCurSel() == 0)
		{
			if(l_sNameTmp != "txt_MK")
			{
				WriteLog(&m_recOperLog,"���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ��txt_MK�ļ���");
				GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
				return;
			}
		}
		else if (m_CombChSel.GetCurSel() == 1)
		{
			if(l_sNameTmp != "txt_MK")
			{		
				WriteLog(&m_recOperLog,"���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ��txt_MK�ļ���");
				GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
				return;
			}
		}
	} 
	else if(DataTableType==1)  //�����ʱ�
	{
		if(l_sNameTmp != "txt")
		{		
			WriteLog(&m_recOperLog,"���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ�������ļ���");
			GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
			return;
		}
	}
	else if(DataTableType==2)  //����ƫ�ı�
	{
		f_PXAtSi2dCfg = true;
		if(l_sNameTmp != "txt_PX")
		{	
			WriteLog(&m_recOperLog,"���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ������ƫ�ı��ļ���");
			GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
			return;
		}
	}
	
	memset(g_nReadbuf,0,readbuflen);
    g_nReadbuflen = 0; //ʵ�ʳ���
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if (pMainFrame->m_NetConnection.bConnected)
	{
		if (m_sFilePath != "")
		{
			GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_SEND_DATA)->SetWindowText("������");
			CWinThread* pThread;
			pThread = AfxBeginThread(SendDataThreadProc,this);
		// 2013-04-26 �̲߳���
//    		CloseHandle(pThread);
//			CloseHandle(g_evnet);
			GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_SEND_DATA)->SetWindowText("������");
			WriteLog(&m_recOperLog,"������");
//			MessageBox("������д�������",MB_OK);
		} 
		else
		{
			MessageBox("���ȴ��ļ���",MB_OK);
			return;
		}
	}
	else
	{
		MessageBox("�����ѶϿ������������磡",MB_OK);
		return;
	}   
}


void CSi2dCfg::OnBnClickedButtonQuery()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
//	AfxGetMainWnd()->SendMessage(WM_SENDCMD,g_n32DI_StartLen, (LPARAM)g_acDI_StartFrameBuf);
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if (!pMainFrame->m_NetConnection.bConnected)
	{
		MessageBox("�����ѶϿ������������磡",MB_OK);
		return;
	}	
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();

	if (m_DataType.GetCurSel() == 0)
	{
		/*unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x0E};
		ZhiLing(HYuZhiSearch_zhiling,pn,0);
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,HYuZhiSearch_zhilingSize, (LPARAM)HYuZhiSearch_zhiling);*/
		UpdateData(true);
		XiuZhengSearch_zhiling[26]=(m_ReadAddress>>8);
		XiuZhengSearch_zhiling[27]=(m_ReadAddress& 0xff);
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,XiuZhengSearch_zhilingSize, (LPARAM)XiuZhengSearch_zhiling);
	} 
	else if(m_DataType.GetCurSel() == 1)
	{
		UpdateData(true);
		FanSheLvSearch_zhiling[26]=(m_ReadAddress>>8);
		FanSheLvSearch_zhiling[27]=(m_ReadAddress& 0xff);
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,FanSheLvSearch_zhilingSize, (LPARAM)FanSheLvSearch_zhiling);
	}
	else if(m_DataType.GetCurSel() == 2)
	{
		UpdateData(true);
		EncoderSearch_zhiling[26]=(m_ReadAddress>>8);
		EncoderSearch_zhiling[27]=(m_ReadAddress& 0xff);
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,EncoderSearch_zhilingSize, (LPARAM)EncoderSearch_zhiling);
	}
	
	/*if(0 == m_CombChSel.GetCurSel())	
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,HYuZhiSearch_zhilingSize, (LPARAM)HYuZhiSearch_zhiling);
	else if (1 == m_CombChSel.GetCurSel())
		AfxGetMainWnd()->SendMessage(WM_SENDCMD,LYuZhiSearch_zhilingSize, (LPARAM)LYuZhiSearch_zhiling);*/
}


void CSi2dCfg::OnEnChangeEditCaculate()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	UpdateData(TRUE);
	if (m_nCaCuNum <= 0 || m_nCaCuNum > 100)
	{
		m_nCaCuNum = 100;
		
	}
	UpdateData(FALSE);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CSi2dCfg::OnBnClickedButtonDelete()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_recDataLog.Clear();
	m_recDataLog.SetSel(-1,-1);
	g_nInterpNum = 0;
	CString hustring;
    if (g_nXYlen>=1)
    {
		g_nXYlen--;
		hustring.Format("��ɾ����%d  ,  %d\n",g_nXY[g_nXYlen][0],g_nXY[g_nXYlen][1]);		
		m_recDataLog.ReplaceSel(hustring);
    }
}


void CSi2dCfg::OnBnClickedButtonClean()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	m_recDataLog.SetWindowText(" ");
}


void CSi2dCfg::OnEnChangeRicheditOperatemessage()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

void CSi2dCfg::OnSelchangeComboDatatype()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	/*if (m_DataType.GetCurSel()==1)
	{
		GetDlgItem(IDC_EDIT_READADDRESS)->EnableWindow(true);
	}
	else if(m_DataType.GetCurSel()==0)
	{
		GetDlgItem(IDC_EDIT_READADDRESS)->EnableWindow(false);
	}*/
}


void CSi2dCfg::OnChangeEditReadaddress()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	if (m_ReadAddress < 0 || m_ReadAddress > 50000)
	{
		m_ReadAddress = 50000;
	}
	UpdateData(FALSE);
}
