// Scan2dCfg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "Scan2dCfg.h"
#include "afxdialogex.h"
#include "NetParam.h"
#include "UserMsg.h"
#include "MainFrm.h"
#include "FileUtils.h"
#include "CAxis.h"
#include "CAxes.h"
#include "CLegend.h"
#include "CSeries.h"
#include "math.h"
#include "CPointSeries.h"
#include "CPointer.h"
#include "CPen0.h"
#include "CTitles.h"
#include "CStrings.h"
#include "MyWriteLog.h"
#include "InputDlg.h"
#include "GraphForm3d.h"
typedef struct TagStructMeasureInfo
{
	int m_mesureinterval;
	int m_nRealDisc;
	int m_nangle_num;
	int m_nMaxDis;
	int m_nMinDis;
	int m_nAVGDis;
	bool m_dealnegative;
	unsigned int m_u32in;
	int m_editValRef;
	int m_editDispersion;
	float m_ediProbability;
}StructMeasureInfo;


StrRecordInfo g_sRecordInfo;
StructMeasureInfo g_sMeasureInfo[1000];
unsigned int g_u32MeasureInfoIndex;
// CScan2dCfg �Ի���
extern int nAVGPos;
TCHAR szFilePath[MAX_PATH + 1];  //����ļ�Ŀ¼
IMPLEMENT_DYNAMIC(CScan2dCfg, CDialog)
	//д��־�߳�
	HANDLE hthread_WriteLog_Sc;
DWORD WINAPI ThreadFuct_WriteLog_Sc(void *data);
CString g_strLogfile_Sc;
CString g_strMeasureDisfile_Sc;  

typedef struct TagStructExcel
{
	CString m_strName;
	u32 m_u32Row;
	u32 m_u32Col;
}StructExcel;
StructExcel g_sExcelInfo;  
//��־�ļ�ָ��
FILE *g_pfLogfile_Sc;  
FILE *g_pfMeasureDisfile_Sc;
STRUCT_LOG structLog_Sc;
//��ǰӦ�ó���Ŀ¼
CString g_strCurDir_Sc = _T("");
//��־�ļ�Ŀ¼
CString g_strLogDir_Sc = _T("");
// CScanf2dDlg �Ի���
CScan2dCfg::CScan2dCfg(CWnd* pParent /*=NULL*/)
	: CDialog(CScan2dCfg::IDD, pParent)
	, m_editValRef(0)
	, m_editDispersion(0)
	, m_ediProbability(0)
	, m_nSigDataCmdCnt(0)
{
	m_dealnegative = false;//190826�� ���ӵĻ���release��Ĭ��Ϊtrue �ͻᵼ���к�Ĳ��ο������쳣
	m_nangle_num = 1;
	m_nDrawNum = 1;
	m_nFrameID = 1;
	m_nInterval = 1;
	//  m_nMaxDistance = 0;
	//  m_nOneChartDistance = 0;
	//  m_RecvNum = 0;
	m_nRecvNum = 0;
	m_nSaveNum = 0;
	m_nZeroX = 180;
	//  m_nMaxDis = 0;
	m_nMaxDistance = 200000;
	m_nMaxDis = 0;
	m_nMinDis = 0;
	m_mesureinterval = 100;
	m_nAVGDis = 0;
	m_nOneChartNum = 1;
	m_nNetorFile = 0;
	m_n32LostDataNum = 0;
	m_nRealDisc = 0;
	m_fjiaodu = 1.0f;
	m_JianGe_Point = 0;
}

CScan2dCfg::~CScan2dCfg()
{
}

void CScan2dCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//  DDX_Check(pDX, IDC_CHECK1, m_dealnegative);
	//  DDX_Check(pDX, IDC_CHECK2, m_mesurectl);
	DDX_Text(pDX, IDC_EDIT_AngleNum, m_nangle_num);
	DDX_Text(pDX, IDC_EDIT_DRAWNUM, m_nDrawNum);
	DDX_Text(pDX, IDC_EDIT_FRAME_ID, m_nFrameID);
	DDX_Text(pDX, IDC_EDIT_INTERVAL, m_nInterval);
	//  DDX_Text(pDX, IDC_EDIT_MAXDISTANCE, m_nMaxDistance);
	//  DDX_Text(pDX, IDC_EDIT_ONECHART, m_nOneChartDistance);
	//  DDX_Text(pDX, IDC_EDIT_RECVNUM, m_RecvNum);
	DDX_Text(pDX, IDC_EDIT_RECVNUM, m_nRecvNum);
	DDX_Text(pDX, IDC_EDIT_SAVENUM, m_nSaveNum);
	DDX_Text(pDX, IDC_EDIT_ZEROX, m_nZeroX);
	//  DDX_Text(pDX, IDC_EDIT_MAXDISTANCE, m_nMaxDis);
	DDX_Text(pDX, IDC_EDIT_MAXDISTANCE, m_nMaxDistance);
	DDX_Text(pDX, IDC_MAXDISTANCE, m_nMaxDis);
	DDX_Text(pDX, IDC_MINDISTANCE, m_nMinDis);
	DDX_Text(pDX, IDC_mesureinterval, m_mesureinterval);
	DDX_Text(pDX, IDC_AVERAGEDISTANCE, m_nAVGDis);
	DDX_Text(pDX, IDC_EDIT_ONECHART, m_nOneChartNum);
	DDX_Control(pDX, IDC_RICHEDIT_OPERATEMESSAGE_SC, m_recOperLogSc);
	DDX_Control(pDX, IDC_CHECK2, m_mesurectl);
	DDX_Check(pDX, IDC_CHECK1, m_dealnegative);
	DDX_Text(pDX, IDC_EDIT_LOSTDATANUM, m_n32LostDataNum);
	DDX_Text(pDX, IDC_REALDISC, m_nRealDisc);
	DDX_Text(pDX, IDC_JIAODUREAL, m_fjiaodu);
	DDX_Control(pDX, IDC_BUTTON_RECORD, m_btnRecord);
	DDX_Control(pDX, IDC_BUTTON_RECORD_DEL, m_btnRecordDel);
	DDX_Control(pDX, IDC_BUTTON_SAVEMEASUREDIS, m_btnSaveExcel);
	DDX_Text(pDX, IDC_EDIT_VALREF, m_editValRef);
	DDX_Text(pDX, IDC_EDIT_DISPERSION, m_editDispersion);
	DDX_Text(pDX, IDC_EDIT_PROBABILITY, m_ediProbability);
	DDX_Control(pDX, IDC_BUTTON_SAVE, m_btnSave);
	DDX_Text(pDX, IDC_EDIT_SIGDATACMD, m_nSigDataCmdCnt);
	DDX_Control(pDX, IDC_BUTTON_SICK_ONEFRAME, m_btn_sick_oneframe);
	DDX_Control(pDX, IDC_BUTTON_SICKCONTINUEFRAME, m_btn_sick_continueframe);
	DDX_Text(pDX, IDC_EDIT_JianGe_Point, m_JianGe_Point);
	DDX_Control(pDX, IDC_COMBO_WORKMODESEL, m_WorkModeSel);
}


BEGIN_MESSAGE_MAP(CScan2dCfg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_SICK_ONEFRAME, &CScan2dCfg::OnBnClickedButtonSickOneframe)
	ON_BN_CLICKED(IDC_BUTTON_SICKCONTINUEFRAME, &CScan2dCfg::OnBnClickedButtonSickcontinueframe)
//	ON_EN_CHANGE(IDC_mesureinterval, &CScan2dCfg::OnEnChangemesureinterval)
	ON_BN_CLICKED(IDC_BTN_HT_SELECT, &CScan2dCfg::OnBnClickedBtnHtSelect)
	ON_EN_CHANGE(IDC_EDIT_ONECHART, &CScan2dCfg::OnEnChangeEditOnechart)
	ON_BN_CLICKED(IDC_CHECK2, &CScan2dCfg::OnBnClickedCheck2)
//	ON_EN_CHANGE(IDC_EDIT_AngleNum, &CScan2dCfg::OnEnChangeEditAnglenum)
	ON_BN_CLICKED(IDC_BUTTON_SAVEMEASUREDIS, &CScan2dCfg::OnBnClickedButtonSavemeasuredis)
	ON_EN_CHANGE(IDC_EDIT_INTERVAL, &CScan2dCfg::OnEnChangeEditInterval)
	ON_EN_CHANGE(IDC_EDIT_MAXDISTANCE, &CScan2dCfg::OnEnChangeEditMaxdistance)
	ON_EN_CHANGE(IDC_EDIT_ZEROX, &CScan2dCfg::OnEnChangeEditZerox)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CScan2dCfg::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_SAVENETDATA, &CScan2dCfg::OnBnClickedButtonSavenetdata)
	ON_BN_CLICKED(IDC_BTN_HT_PREV, &CScan2dCfg::OnBnClickedBtnHtPrev)
	ON_BN_CLICKED(IDC_BTN_HT_NEXT, &CScan2dCfg::OnBnClickedBtnHtNext)
	ON_BN_CLICKED(IDC_BUTTON_FILE_PLAY, &CScan2dCfg::OnBnClickedButtonFilePlay)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_OPEN_ONE_FRAME, &CScan2dCfg::OnBnClickedButtonOpenOneFrame)
	ON_BN_CLICKED(IDC_BUTTON_RECORD, &CScan2dCfg::OnBnClickedButtonRecord)
	//ON_EN_CHANGE(IDC_AVERAGEDISTANCE2, &CScan2dCfg::OnEnChangeAveragedistance2)
	ON_BN_CLICKED(IDC_BUTTON_RECORD_DEL, &CScan2dCfg::OnBnClickedButtonRecordDel)
	ON_BN_CLICKED(IDC_CHECK1, &CScan2dCfg::OnBnClickedCheck1)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CScan2dCfg::OnBnClickedButtonSave)
//	ON_EN_CHANGE(IDC_EDIT_DISPERSION, &CScan2dCfg::OnEnChangeEditDispersion)
	ON_EN_CHANGE(IDC_EDIT_VALREF, &CScan2dCfg::OnEnChangeEditValref)
	ON_BN_CLICKED(IDC_BUTTON_CLEARCNT, &CScan2dCfg::OnBnClickedButtonClearcnt)
	ON_BN_CLICKED(IDC_CHECK_20msSIGDATACMD, &CScan2dCfg::OnBnClickedCheck20mssigdatacmd)
	ON_BN_CLICKED(IDC_BUTTON_WORKMODESET, &CScan2dCfg::OnBnClickedButtonWorkmodeset)
	ON_EN_KILLFOCUS(IDC_REALDISC, &CScan2dCfg::OnKillfocusRealdisc)
	ON_EN_KILLFOCUS(IDC_EDIT_AngleNum, &CScan2dCfg::OnKillfocusEditAnglenum)
	ON_EN_KILLFOCUS(IDC_mesureinterval, &CScan2dCfg::OnKillfocusMesureinterval)
	ON_EN_KILLFOCUS(IDC_EDIT_DISPERSION, &CScan2dCfg::OnKillfocusEditDispersion)
END_MESSAGE_MAP()


// CScan2dCfg ��Ϣ�������


void CScan2dCfg::OnBnClickedButtonSickOneframe()
{
	AfxGetMainWnd()->PostMessage(WM_SENDCMD, nGetSickOneFrameBufSize, (LPARAM)acGetSickOneFrameBuf);
	WriteLog(&m_recOperLogSc,"���͵�֡ȡ������");
	UpdateData(false);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CScan2dCfg::OnBnClickedButtonSickcontinueframe()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	//int nBufSize = 25;
	m_nNetorFile = 1;
	CString  str;
	StructLogFile l_sMyLog;
	GetDlgItem(IDC_BUTTON_SICKCONTINUEFRAME)->GetWindowText(str);
	if (str == "ֹͣ��ȡ")   //ֹͣ������ȡ����
	{
		KillTimer(6);
		/*AfxGetMainWnd()->SendMessage(WM_SENDCMD, nStopGetSickContinueFrameBufSize, (LPARAM)acStopGetSickContinueFrameBuf);*/
		GetDlgItem(IDC_BUTTON_SICKCONTINUEFRAME)->SetWindowText("������ȡ");

		//l_sMyLog.m_strLog = "����ֹͣ��ȡ����";
		//l_sMyLog.m_n32id = IDC_RICHEDIT_OPERATEMESSAGE_SC;
		//((CRichEditCtrl *)(GetDlgItem(l_sMyLog.m_n32id)))->SetSel(-1, -1); 
		//	((CRichEditCtrl *)(GetDlgItem(l_sMyLog.m_n32id)))->ReplaceSel(l_sMyLog.m_strLog); 
		//	((CRichEditCtrl *)(GetDlgItem(l_sMyLog.m_n32id)))->PostMessage(WM_VSCROLL, SB_BOTTOM,0);
		//WriteMyLog(l_sMyLog);
		WriteLog(&m_recOperLogSc,"����ֹͣ��ȡ����");
	} 
	else					 //������ȡ����
	{	
		GraphForm2d * pView	;
		CString l_strBtnName;
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		pView = (GraphForm2d*) pMainFrame->m_wndLeftSplitter.GetPane(0,0);
		pView->m_single2ddlg.m_sSi2dCfgDlg.m_btnPauseSend.GetWindowText(l_strBtnName);
		if (l_strBtnName == "��ͣ����")
		{
			pView->m_single2ddlg.m_sSi2dCfgDlg.OnBnClickedButtonPauseSend();
		}

		SetTimer(6, 130, NULL);
		/*AfxGetMainWnd()->SendMessage(WM_SENDCMD, nGetSickContinueFrameBufSize, (LPARAM)acGetSickContinueFrameBuf);*/	
		GetDlgItem(IDC_BUTTON_SICKCONTINUEFRAME)->SetWindowText("ֹͣ��ȡ");	
		WriteLog(&m_recOperLogSc,"����������ȡ����");
	}
}

void CScan2dCfg::OnKillfocusMesureinterval()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL is_valid;
	GetDlgItemInt(IDC_mesureinterval,&is_valid,0);
	if(is_valid)
	{	
		UpdateData(TRUE);
		if(m_mesureinterval <= 0)
			m_mesureinterval = 1;
		else if (m_mesureinterval > 1000 )
			m_mesureinterval = 1000;
		UpdateData(false);
	}
	else
	{
		SetDlgItemInt(IDC_mesureinterval,1,0);
		UpdateData(TRUE);
	}
	nAVGPos = 0;
}

//�ص����� ���ϻ�
int CALLBACK BroweCallProc(HWND hwnd, UINT msg,LPARAM lp,LPARAM pData)
{
	if(msg == BFFM_INITIALIZED)
	{
		::SendMessage(hwnd ,BFFM_SETSELECTION,TRUE,pData);
	}
	return 0;
}

int g_nRevcNum = 0;          //���հ���
extern int  createflag; //�����Ի����־
extern int g_nCount_scanf ;
extern int maxline;
extern int g_nseriesnum; //������������
extern CSeries g_asChartJi[50],g_asChartZhi[50];
extern int cBufAVG[1024];
extern int getData(int buf[],int len,int *AVG,int *MAX, int *MIN);
//extern const int c_n32MaxAngle;

extern double YValuesTmp[5000];
extern double XValuesTmp[5000];

extern double Y1ValuesTmp[50000];
extern double X1ValuesTmp[50000];
int g_nSendNum_sigle = 0;          //��֡������
//���Ʋ���
bool CScan2dCfg::DrawWave(char *pcDataBuf,int nDataBufSize,int nBufCount)
{
	int i,j;
	int nDataJi=0,nDataZhiX=0,nDataZhiY = 0;
	int nDataNum;
	int nPreData = 0;
	int nNum = 0;
	int nAngle = m_nZeroX;
	UpdateData(TRUE);
	int nAVG,nMAX,nMIN;
	maxline = m_nOneChartNum;
	if (m_nOneChartNum>999)
	{
		m_nOneChartNum = 999;
		maxline = 999;
		return 0;
	}
	GraphForm2d * pView	;
	CGraphForm2dState *pView1;

	//GraphForm3d *pView3d;
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pView = (GraphForm2d*)pMainFrame->m_wndLeftSplitter.GetPane(0,0);

	if (g_nseriesnum<50)
	{
		pView->m_scanf2ddlg.m_ChartJi.AddSeries(0);
		pView->m_scanf2ddlg.m_ChartZhi.AddSeries(4);
		g_asChartJi[g_nseriesnum] = ((CSeries)(pView->m_scanf2ddlg.m_ChartJi.Series(g_nseriesnum)));
		g_asChartZhi[g_nseriesnum] = ((CSeries)(pView->m_scanf2ddlg.m_ChartZhi.Series(g_nseriesnum)));
		g_asChartJi[g_nseriesnum].put_Color(RGB(255,0,0));
		CPointSeries point = (CPointSeries)(g_asChartZhi[g_nseriesnum].get_asPoint());
		((CPointer)point.get_Pointer()).put_Style(6);
		((CPen0)(((CPointer)point.get_Pointer()).get_Pen())).put_Color(RGB(255,0,0));
		// m_ChartZhi.AddSeries(0);
	}

	if(pcDataBuf[0] == (char)0x02 && pcDataBuf[1] == (char)0x02
		&& pcDataBuf[2] == (char)0x02 && pcDataBuf[3] == (char)0x02)//TCP����
	{

		i = 83;
		nDataNum = (pcDataBuf[i]<<8) + (BYTE)pcDataBuf[i+1];
		i += 2;			   
		//for(j = (nBufCount-1)*nDataNum;j<nBufCount*nDataNum;j++)
		for(j = 0;j<nDataNum;j++)
		{

			nDataJi = 0;
			nDataJi = ((pcDataBuf[i]&0xFF)<<8) + (BYTE)pcDataBuf[i+1];
			i += 2;
			//if (m_dealnegative) //����
			//{
			//	if(nDataJi>=0 && nDataJi<=10)
			//	{
			//		nDataJi = nPreData;
			//	}
			//}
				   			 
			if(nDataNum == 361)
			{
				((CAxis)((CAxes)pView->m_scanf2ddlg.m_ChartJi.get_Axis()).get_Bottom()).put_Maximum(360);

				//pView->m_scanf2ddlg.m_sScan2dCfgDlg.UpdateData(true);
				//float jiaodu =  pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_fjiaodu;
				//if(jiaodu <= 0)
				//	continue;
				nDataZhiX = (int)(nDataJi*sin((((double)(j - nAngle))/360)*3.1416));
				nDataZhiY = (int)(nDataJi*cos((((double)(j - nAngle))/360)*3.1416));

				if (m_dealnegative) //����
				{
					if(nDataJi>=0 && nDataJi<=10)
					{
						//nDataZhiX = (int)(nDataJi*sin((((double)(j - nAngle))/360)*3.1416));
						if(j!= 360)
						{
						nDataZhiY = nPreData;
						nDataJi = nDataZhiY/cos((((double)(j - nAngle))/360)*3.1416);
						nDataZhiX = nDataZhiY*tan((((double)(j - nAngle))/360)*3.1416);
						}
						else
						{
							nDataZhiY = nPreData;
							nDataJi = 0;
							//nDataZhiX = 0;
						}
					}
				}
				if(nDataJi > m_nMaxDistance)
				{
					nDataJi = m_nMaxDistance;
				}		
				nPreData  = nDataZhiY;
			}
			else if(nDataNum == 721)
			{
				nDataZhiX = (int)(nDataJi*sin((((double)(j - nAngle))/360/2)*3.1416));
				nDataZhiY = (int)(nDataJi*cos((((double)(j - nAngle))/360/2)*3.1416));
			}

			nDataZhiY = abs(nDataZhiY);

			YValuesTmp[j] = nDataZhiY;
			XValuesTmp[j] = nDataZhiX;
			Y1ValuesTmp[j] = nDataJi;
			X1ValuesTmp[j] = j;

			if (m_mesurectl) 
			{
				if(j == m_nangle_num)
				{
					cBufAVG[nAVGPos++] = nDataJi;
				}
				if((nAVGPos != 0)&&( nAVGPos % m_mesureinterval == 0) )
				{
					int ret = getData(cBufAVG,nAVGPos,&nAVG,&nMAX,&nMIN);
					if(ret == 0)
					{
						m_nMaxDis = nMAX;
						m_nMinDis = nMIN;
						m_nAVGDis = nAVG;
						m_editValRef = m_nAVGDis;
						UpdateData(false);
						nAVGPos = 0;
					}
				}
			}
			//nPreData = nDataJi;

		}
		if(nDataNum == 721)
		{
			((CAxis)((CAxes)pView->m_scanf2ddlg.m_ChartJi.get_Axis()).get_Bottom()).put_Maximum(720);
			pView->DrawLine((LPVOID)(&g_asChartZhi[g_nseriesnum%maxline]),XValuesTmp,YValuesTmp,nDataNum);
			pView->DrawLine((LPVOID)(&g_asChartJi[g_nseriesnum%maxline]),X1ValuesTmp,Y1ValuesTmp,nDataNum);
		}
		else if(nDataNum == 361)
		{
			((CAxis)((CAxes)pView->m_scanf2ddlg.m_ChartJi.get_Axis()).get_Bottom()).put_Maximum(360);
			pView->DrawLine((LPVOID)(&g_asChartZhi[g_nseriesnum%maxline]),XValuesTmp,YValuesTmp,nDataNum);
			pView->DrawLine((LPVOID)(&g_asChartJi[g_nseriesnum%maxline]),X1ValuesTmp,Y1ValuesTmp,nDataNum);
		}
		else
		{
			return true;
		}

		g_nseriesnum++;
		return TRUE;
	}
	else if(pcDataBuf[0] == (char)0xFF && pcDataBuf[1] == (char)0xFF)//UDP����
	{

		i = 6;
		nDataNum = (pcDataBuf[i+1]<<8) + (pcDataBuf[i]);
		i += 2;
		//for(j=(nBufCount-1)*nDataNum;j<nBufCount*nDataNum;j++)
		for(j=0;j<nDataNum;j++)
		{
			nDataJi = 0;
			nDataJi = ((pcDataBuf[i+3]&0xFF)<<24) + ((pcDataBuf[i+2]&0xFF)<<16) + ((pcDataBuf[i+1]&0xFF)<<8) + (pcDataBuf[i+1]&0xFF);
			i += 4;
			nDataZhiX = (int)(nDataJi*sin((((double)(nNum - nAngle))/360)*3.1416));
			nDataZhiY = (int)(nDataJi*cos((((double)(nNum - nAngle))/360)*3.1416));
			nDataZhiY = abs(nDataZhiY);
			//m_ChartJi.Series(0).Add(nDataJi,"",0);  
			//pView->m_scanf2ddlg.m_ChartJi.Series(g_nseriesnum%maxline).Add((double)nDataJi,"",RGB(255,0,0));
			//pView->m_scanf2ddlg.m_ChartZhi.Series(0).AddXY(nDataZhiX, nDataZhiY, NULL, 0);    

			nNum++;
		}
		g_nseriesnum++;
		return TRUE;
	}
	else 
	{
		nDataNum = nDataBufSize/2;
		for(j=0;j<nDataNum;j++)
		{
			Y1ValuesTmp[j] = ((pcDataBuf[j*2]&0xFF)<<8) + (pcDataBuf[j*2+1]&0xFF);
			X1ValuesTmp[j] = j;
		}   
		((CAxis)((CAxes)pView->m_scanf2ddlg.m_ChartJi.get_Axis()).get_Bottom()).put_Maximum(nDataNum);
		pView->DrawLine((LPVOID)(&g_asChartJi[g_nseriesnum%maxline]),X1ValuesTmp,Y1ValuesTmp,nDataNum);

		g_nseriesnum++;
		return TRUE;
		//	   MessageBox("���ݸ�ʽ���ԣ���ȡʧ�ܣ�",MB_OK);
		//	   Sleep(100);
		//return FALSE;
	}
	UpdateData(FALSE);
	pView->UpdateData(FALSE);
}

void CScan2dCfg::OnBnClickedBtnHtSelect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetModuleFileName(NULL, szFilePath, MAX_PATH);   
	(_tcsrchr(szFilePath, _T('\\')))[1] = 0;		

	m_strFiles.Format("%s",szFilePath);

	m_strFiles += "NetDataFiles";
	BROWSEINFO bi;
	bi.hwndOwner = NULL;
	bi.pidlRoot = 0;
	bi.pszDisplayName = 0;
	bi.ulFlags = BIF_EDITBOX;
	bi.lpszTitle = "select";
	bi.lParam = (long)(m_strFiles.GetBuffer(m_strFiles.GetLength()));
	bi.lpfn = BroweCallProc;
	LPITEMIDLIST pIDList = SHBrowseForFolder(&bi);
	if (pIDList != NULL)
	{
		SHGetPathFromIDList(pIDList,szFilePath);
		m_sFilePath = szFilePath;
	}
	else
		return ;
	//	MessageBox(pIDList);
	/*
	BROWSEINFO bi;
	ZeroMemory(&bi,sizeof(bi));
	bi.hwndOwner = m_hWnd;
	bi.lpszTitle = _T("select file path");
	bi.ulFlags = 0x0040;
	LPITEMIDLIST lpdist = dirpidl("D:\\data_multifile_double");
	bi.pidlRoot = lpdist;
	bi.pszDisplayName = szFilePath;
	LPITEMIDLIST pITem = SHBrowseForFolder(&bi);


	CFileUtils fileUtils;
	m_sFilePath = fileUtils.OpenDirDlg(this->GetSafeHwnd(),"ѡ�������ļ���",fileUtils.GetCurDir());*/
	m_sFilePath += "\\";
	if (MessageBox(m_sFilePath,"��ѡ�ļ���",MB_YESNO) == IDNO)
	{
		return ;
	}
	m_sLog = "������� ���\'ѡ��\'��ť,��ѡ�����ļ��У�";
	m_sLog += m_sFilePath;
	m_sLog += ".";
	WriteLog(&m_recOperLogSc,m_sLog);

	if(m_sFilePath != "")
	{
		CFileFind fileFind;
		BOOL res;	
		CString sFileName;
		CString sTmp;
		bool bFlag;

		res = fileFind.FindFile(m_sFilePath + "*.txt");//��ʼ����txt�ļ�
		if (!res)
		{
			res = fileFind.FindFile(m_sFilePath + "*.lmd");//��ʼ����lmd�ļ�
		}		
		if (!res)
		{
			res = fileFind.FindFile(m_sFilePath + "*.wj"); //��ʼ����wj�ļ�
		}	
		if(!res)
		{
			MessageBox("û�в��ҵ������ļ��������²���!","��ʾ",MB_OK);
			return;
		}		
		m_slFileName.RemoveAll();
		while(res)
		{
			res = fileFind.FindNextFile();
			if(!fileFind.IsDirectory() && !fileFind.IsDots())
			{
				sFileName = fileFind.GetFileName();//���ļ��ŵ�CStringList
				m_slFileName.AddTail(sFileName);
			}
		}
		fileFind.Close();
		m_nCurFileInd = 0;
		bFlag = AnalyLWDFile(m_sFilePath+m_slFileName.GetAt(m_slFileName.FindIndex(m_nCurFileInd)));
		if(bFlag)
		{
			m_nFrameID = m_nCurFileInd + 1;
			UpdateData(FALSE);
			m_sLog.Format("ѡ�������ļ��У�����%d������",m_slFileName.GetCount());
			WriteLog(&m_recOperLogSc,m_sLog);
		}
		else
		{
			OnBnClickedButtonFilePlay();
		}

	}
	m_nNetorFile = 0; 
	m_nNetorFile = 0;
}

 NetDataStruct g_sNetData3d;
 extern int g_nAngle;
bool CScan2dCfg::AnalyLWDFile(CString sFileName)
{
	CFileUtils fileUtils;
	char *pFileData;
	int nFileLen;
	bool bFlag = false;
	int nSeriesNumZhi;
	int nSeriesNumJi;

	GraphForm2d * pView	;
	CGraphForm2dState *pView1;

	//GraphForm3d *pView3d;
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pView = (GraphForm2d*) pMainFrame->m_wndLeftSplitter.GetPane(0,0);

	//pView3d = (GraphForm3d*) pMainFrame->m_wndSplitter.GetPane(1,0);

	UpdateData(TRUE);
	fileUtils.ReadDatafile(sFileName,FALSE);
	nFileLen = fileUtils.GetDatafileLength();
	pFileData = (char*)malloc(nFileLen);
	fileUtils.ReadDatafileData(pFileData,nFileLen);

	fileUtils.CloseDatafile();
	if (g_nRevcNum%m_nInterval == 0)  
	{   
		if (g_nCount_scanf >= m_nOneChartNum)                  
		{
			g_nCount_scanf = 0;
			//F_ClearChart();
			//pView->PostMessage(WM_NETCMD_CLEARCHARTSC,g_nNetRecvInd,0);
			// pView->DrawLine((LPVOID)(&g_asChartZhi[g_nseriesnum%maxline]),XValuesTmp,YValuesTmp,nDataNum);

			nSeriesNumZhi = pView->m_scanf2ddlg.m_ChartZhi.get_SeriesCount();     //��ռ�����ͼ��
			for (int i=0; i<nSeriesNumZhi; i++)
				((CSeries) pView->m_scanf2ddlg.m_ChartZhi.Series(i)).Clear();

			nSeriesNumJi =  pView->m_scanf2ddlg.m_ChartJi.get_SeriesCount();   //���ֱ������ͼ��
			for (int i=0; i<nSeriesNumJi; i++)
				((CSeries)pView->m_scanf2ddlg.m_ChartJi.Series(i)).Clear();

			// ((CAxis)((CAxes)m_scanf2ddlg.m_ChartZhi.get_Axis()).get_Bottom()).put_Maximum(360);

			((CAxis)((CAxes)pView->m_scanf2ddlg.m_ChartZhi.get_Axis()).get_Bottom()).put_Automatic(false);
			((CAxis)((CAxes)pView->m_scanf2ddlg.m_ChartZhi.get_Axis()).get_Left()).put_Automatic(true);

			((CAxis)((CAxes)pView->m_scanf2ddlg.m_ChartJi.get_Axis()).get_Bottom()).put_Automatic(false);
			((CAxis)((CAxes)pView->m_scanf2ddlg.m_ChartJi.get_Axis()).get_Left()).put_Automatic(true);
			UpdateData(FALSE);
		}
		g_nCount_scanf++;
		//pView->PostMessage(WM_NETCMD_DRAWCHARTSC,(WPARAM)&g_sNetData,NULL);
		//DrawLine(LPVOID pAddr,double *pX,double *pY,long nNum)
		bFlag = DrawWave(pFileData,nFileLen,g_nCount_scanf); 
//		g_sNetData3d.m_pcData = pFileData;
		g_sNetData3d.m_n32Len = nFileLen;
		g_sNetData3d.m_n32BufCnt = 1;
		g_sNetData3d.m_n32Channel = 0;
		g_nAngle = pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nZeroX;
		//pView3d->SendMessage(WM_NETCMD_DRAW3D,(WPARAM)&g_sNetData3d,NULL);
		if (bFlag == 0)
		{
			free(pFileData);
			return FALSE;
		}
	}
	g_nRevcNum++;
	if (createflag == 1)
	{
		//FD_TCP_Draw3D(pFileData, nFileLen);
	}

	if(bFlag)
	{
		CTitles title = pView->m_scanf2ddlg.m_ChartJi.get_Header();
		title.put_Caption(fileUtils.GetFileName(sFileName));
		//title
		//title.   .SetCaption(fileUtils.GetFileName(sFileName));
		free(pFileData);
		return TRUE;
	}
	else
		return FALSE;

	free(pFileData);
	UpdateData(FALSE);

}
void CScan2dCfg::OnEnChangeEditOnechart()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL is_valid;
	GetDlgItemInt(IDC_EDIT_ONECHART,&is_valid,0);
	if(is_valid)
	{
		UpdateData(TRUE);
		if (m_nInterval == 0)
		{
			m_nInterval = 1;
		}
		if (m_nNetorFile)  //��������
		{		

			if ((m_nOneChartNum/m_nInterval)>19)
			{
				m_nOneChartNum = 19*m_nInterval;
			}
			m_nOneChartNum = (m_nOneChartNum>50)?50:m_nOneChartNum;
		}
		if (m_nOneChartNum < 1)
		{
			m_nOneChartNum = 1;
		}

		UpdateData(FALSE);
		//	m_ChartJi.GetAxis().GetBottom().SetMinMax(0,m_nOneChartNum*320);		
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_ONECHART,1,0);
		UpdateData(TRUE);
	}
}


void CScan2dCfg::OnBnClickedCheck2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int n1 = 0;
	CString szGetName;
	n1 =  ((CButton*)(GetDlgItem(IDC_CHECK2)))->GetCheck();
	g_u32MeasureInfoIndex = 0; 
	//InitExcel();
	//��ʼ��excel��ع���

	if(n1 == 1)
	{
		m_btnRecord.EnableWindow(true);
		m_btnRecordDel.EnableWindow(true);
		m_btnSaveExcel.EnableWindow(true);
		m_btnSave.EnableWindow(true);
	}
	else
	{
		m_btnRecord.EnableWindow(false);
		m_btnRecordDel.EnableWindow(false);
		m_btnSaveExcel.EnableWindow(false);
		m_btnSave.EnableWindow(false);
	}

	//bool bRet = m_OperExcel.OpenExcelFile("ɨ���Լ��⴫����711���ģ�����޸�.xlsx");
	//CString strSheetName = m_OperExcel.GetSheetName(1);
	//bool bLoad = m_OperExcel.LoadSheet(strSheetName);
	//int nRow = m_OperExcel.GetRowCount();
	//int nCol = m_OperExcel.GetColumnCount();
	//for (int i=1; i<=nRow; ++i)
	//{
	//	for (int j=1; j<=nCol; ++j)
	//	{
	//		m_OperExcel.SetCellString(i,j,"hello");
	//	}
	//}

	//m_OperExcel.SaveasXSLFile("D:\\demo1.xlsx");
	//m_OperExcel.CloseExcelFile(0);
	//m_OperExcel.ReleaseExcel();

	//if (n1 == 1)
	//{
	//	//memset(&g_sExcelInfo,0,sizeof(g_sExcelInfo));
	//	/*bool bInit = m_OperExcel.InitExcel();
	//	if(!bInit)
	//	{
	//		AfxMessageBox("��ʼ��excelʧ�ܣ�");
	//		return ;
	//	}
	//	TCHAR szPath[MAX_PATH];   
	//	GetModuleFileName(NULL, szPath, MAX_PATH);
	//	CString PathName(szPath);
	//	CString strExcelDemoPath = PathName.Left(PathName.ReverseFind(_T('\\')) + 1);
	//	strExcelDemoPath += "ɨ���Լ��⴫����711���ģ�����޸�.xlsx";

	//	bool bRet = m_OperExcel.OpenExcelFile(strExcelDemoPath);
	//	CString strSheetName = m_OperExcel.GetSheetName(1);
	//	bool bLoad = m_OperExcel.LoadSheet(strSheetName);*/

	//	g_sExcelInfo.m_strName = "";
	//	g_sExcelInfo.m_u32Col = 2;
	//	g_sExcelInfo.m_u32Row = 1;
	//	CFileDialog *lpszOpenFile; //����һ��CfileDialog����
	//	lpszOpenFile = new CFileDialog(FALSE,"",CTime::GetCurrentTime().Format("������_%Y-%m-%d-%H-%M-%S.xlsx"),OFN_FILEMUSTEXIST |OFN_HIDEREADONLY , "�ļ�����(*.xls)|*.xlsx||");//����һ���Ի���
	//	if(lpszOpenFile->DoModal() == IDOK)//�������Ի���ȷ����ť
	//	{
	//		szGetName = lpszOpenFile->GetPathName();
	//		g_sExcelInfo.m_strName = szGetName;//;
	//		/*m_sLog.Format("������   ����� �����㣺 ���ֵ�� ��Сֵ�� ƽ��ֵ�� ����״̬");
	//		WriteLog(&m_recOperLogSc,m_sLog);*/

	//	}
	//	delete lpszOpenFile;//�ͷŷ���ĶԻ���
	//}
}
void CScan2dCfg::WriteLog(CRichEditCtrl *pREC, CString sLog, unsigned char *pbuf, unsigned short u16datalen)
{
	if (sLog != "")
	{
		structLog_Sc.strLogInfo[structLog_Sc.in] = sLog;	
	}
	else
	{
		structLog_Sc.strLogInfo[structLog_Sc.in] = "����ʾ��Ϣ";	
	}

	if (pbuf != NULL)
	{
		memcpy(&structLog_Sc.au8buf[structLog_Sc.in][0],pbuf,u16datalen);
		structLog_Sc.au16buflen[structLog_Sc.in] = u16datalen;
	}
	else
	{
		//memcpy(&strLog_Com.au8buf[strLog_Com.in][0],pbuf,u16datalen);
		structLog_Sc.au16buflen[structLog_Sc.in] = 0;
	}
	if((pbuf != NULL)||(sLog != ""))
	{
		structLog_Sc.in ++ ;
		structLog_Sc.in %= MAXLOGINFO;	
	}

	if(pREC->GetLineCount()>2000)
	{
		pREC->SetSel(0, -1);		
		pREC->ReplaceSel("");
	}
	//	OnButtonClearLogprant();
}


void CScan2dCfg::OnKillfocusEditAnglenum()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL is_valid;
	GetDlgItemInt(IDC_EDIT_AngleNum,&is_valid,0);
	if(is_valid)
	{	
		UpdateData(TRUE);
		if(m_nangle_num<0 )
			m_nangle_num = 0;
		UpdateData(false);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_AngleNum,1,0);
		UpdateData(TRUE);
	}
}

DWORD CScan2dCfg::ThreadSaveExcel(LPVOID pParam)
{
	CScan2dCfg *pDlg = (CScan2dCfg *)pParam;
	CString szGetName,str;
	InputDlg dlg;
	CString strDecName =NULL;

	pDlg->GetDlgItem(IDC_BUTTON_SAVEMEASUREDIS)->EnableWindow(FALSE);
	dlg.SetStaticText(CString("�����������Ա��"));
	if(dlg.DoModal() == IDOK)
	{
		strDecName = dlg.GetInputText();
	}

	if(strDecName == "")
	{
		pDlg->MessageBox("����ʧ�ܣ�\r\n�����������Ա��","��ʾ", MB_ICONINFORMATION);
		pDlg->GetDlgItem(IDC_BUTTON_SAVEMEASUREDIS)->EnableWindow(TRUE);
		return 1;
	}

	g_sExcelInfo.m_strName = "";
	g_sExcelInfo.m_u32Col = 1;
	g_sExcelInfo.m_u32Row = 10;
	CFileDialog *lpszOpenFile; //����һ��CfileDialog����
	lpszOpenFile = new CFileDialog(FALSE,"",CTime::GetCurrentTime().Format("������_%Y-%m-%d-%H-%M-%S.xlsx"),OFN_FILEMUSTEXIST |OFN_HIDEREADONLY , "�ļ�����(*.xls)|*.xlsx||");//����һ���Ի���
	if(lpszOpenFile->DoModal() == IDOK)//�������Ի���ȷ����ť
	{
		szGetName = lpszOpenFile->GetPathName();
		g_sExcelInfo.m_strName = szGetName;
	}
	else
	{
		pDlg->GetDlgItem(IDC_BUTTON_SAVEMEASUREDIS)->EnableWindow(TRUE);
		return 1;
	}
	if(g_u32MeasureInfoIndex >= 1000)
	{
		AfxMessageBox("�Ѵﵽ����¼����");
		pDlg->GetDlgItem(IDC_BUTTON_SAVEMEASUREDIS)->EnableWindow(TRUE);
		return 1;
	}

	bool bInit = pDlg->m_OperExcel.InitExcel();
	if(!bInit)
	{
		AfxMessageBox("��ʼ��excelʧ�ܣ�");
		pDlg->GetDlgItem(IDC_BUTTON_SAVEMEASUREDIS)->EnableWindow(TRUE);
		return 1;
	}
	TCHAR szPath[MAX_PATH];   
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString PathName(szPath);
	CString strExcelDemoPath = PathName.Left(PathName.ReverseFind(_T('\\')) + 1);
	strExcelDemoPath += "���˹�Ƭ��λ�����������2015.05.11.xlsx";

	bool bRet = pDlg->m_OperExcel.OpenExcelFile(strExcelDemoPath);
	CString strSheetName = pDlg->m_OperExcel.GetSheetName(1);
	bool bLoad = pDlg->m_OperExcel.LoadSheet(strSheetName);

	g_sRecordInfo.m_strName = strDecName;
	pDlg->m_OperExcel.SetCellString_Cells("A2",1,2,CTime::GetCurrentTime().Format("%Y-%m-%d-%H-%M-%S"));
	pDlg->m_OperExcel.SetCellString_Cells("A2",1,4,g_sRecordInfo.m_strAPDBV);
	pDlg->m_OperExcel.SetCellString_Cells("G2",1,2,g_sRecordInfo.m_strEquipNo);
	pDlg->m_OperExcel.SetCellString_Cells("G2",1,4,g_sRecordInfo.m_strInter);
	pDlg->m_OperExcel.SetCellString_Cells("A2",2,2,g_sRecordInfo.m_strName );
	pDlg->m_OperExcel.SetCellString_Cells("A2",2,4,g_sRecordInfo.m_strAPDHV);
	pDlg->m_OperExcel.SetCellString_Cells("G2",2,2,g_sRecordInfo.m_strAPDTemper);
	pDlg->m_OperExcel.SetCellString_Cells("G2",2,4,g_sRecordInfo.m_strAPDRatio);

	//pDlg->m_OperExcel.SetCellString(3,2,g_sExcelInfo.m_strName);
	g_sExcelInfo.m_u32Row = 2;
	for(int i = 0;i < g_u32MeasureInfoIndex;i++)
	{
		str.Format("%d",g_sMeasureInfo[i].m_nRealDisc);
		pDlg->m_OperExcel.SetCellString_Cells("A7",g_sExcelInfo.m_u32Row,1,str);

		str.Format("%d",g_sMeasureInfo[i].m_nMaxDis);
		pDlg->m_OperExcel.SetCellString_Cells("A7",g_sExcelInfo.m_u32Row,2,str);

		str.Format("%d",g_sMeasureInfo[i].m_nMinDis);
		pDlg->m_OperExcel.SetCellString_Cells("A7",g_sExcelInfo.m_u32Row,3,str);

		str.Format("%d",g_sMeasureInfo[i].m_nAVGDis);
		pDlg->m_OperExcel.SetCellString_Cells("A7",g_sExcelInfo.m_u32Row,4,str);
		g_sExcelInfo.m_u32Row++;
	}

	   str.Format("%d",g_u32MeasureInfoIndex);
		pDlg->m_OperExcel.SetCellString_Cells("G7",1,1,str);  //��¼������

	int n32biaozhun,n32shiji;
	if(g_u32MeasureInfoIndex >= 1)
	{
		n32biaozhun = g_sMeasureInfo[(g_u32MeasureInfoIndex-1)/2].m_nRealDisc;
		n32shiji = g_sMeasureInfo[(g_u32MeasureInfoIndex-1)/2].m_nAVGDis;
	}
	else
	{
		n32biaozhun = g_sMeasureInfo[0].m_nRealDisc;
		n32shiji = g_sMeasureInfo[0].m_nAVGDis;
	}
	g_sExcelInfo.m_u32Row = 2;
	pDlg->m_OperExcel.SaveasXSLFile(g_sExcelInfo.m_strName);
	pDlg->m_OperExcel.CloseExcelFile(0);
	pDlg->m_OperExcel.ReleaseExcel();
	pDlg->MessageBox("����ɹ���","��ʾ", MB_ICONINFORMATION);
	pDlg->GetDlgItem(IDC_BUTTON_SAVEMEASUREDIS)->EnableWindow(TRUE);
	return 1;
}
void CScan2dCfg::OnBnClickedButtonSavemeasuredis()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString szGetName;
	UpdateData(FALSE);
	DWORD dwThreadID = 0;
	HANDLE hThread=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadSaveExcel,this,NULL,&dwThreadID);
	//m_sLog.Format("������ %d %d %d %d %d %d",m_mesureinterval, m_nangle_num, m_nMaxDis, m_nMinDis, m_nAVGDis, m_dealnegative);

	//bool bRet = m_OperExcel.OpenExcelFile("ɨ���Լ��⴫����711���ģ�����޸�.xlsx");
	//CString strSheetName = m_OperExcel.GetSheetName(1);
	//bool bLoad = m_OperExcel.LoadSheet(strSheetName);
	//int nRow = m_OperExcel.GetRowCount();
	//int nCol = m_OperExcel.GetColumnCount();
	//for (int i=1; i<=nRow; ++i)
	//{
	//	for (int j=1; j<=nCol; ++j)
	//	{
	//		m_OperExcel.SetCellString(i,j,"hello");
	//	}
	//}

	//m_OperExcel.SaveasXSLFile("D:\\demo1.xlsx");
	//m_OperExcel.CloseExcelFile(0);
	//m_OperExcel.ReleaseExcel();




}


void CScan2dCfg::OnEnChangeEditInterval()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL is_valid;
	GetDlgItemInt(IDC_EDIT_INTERVAL,&is_valid,0);
	if(is_valid)
	{	
		UpdateData(TRUE);
		if (m_nInterval <= 0)
		{
			m_nInterval = 1;
		}
		if (m_nInterval >= 100)
		{
			m_nInterval = 100;
		}
		if (m_nNetorFile)  //��������
		{
			if ((m_nOneChartNum/m_nInterval)>19)
			{
				m_nInterval = m_nOneChartNum/19+1;	
			}
		}
		UpdateData(FALSE);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_INTERVAL,1,0);
		UpdateData(TRUE);
	}	
}


void CScan2dCfg::OnEnChangeEditMaxdistance()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL is_valid;
	GetDlgItemInt(IDC_EDIT_MAXDISTANCE,&is_valid,0);
	if(is_valid)
	{	
		UpdateData(TRUE);
		if (m_nMaxDistance <= 0)
		{
			m_nMaxDistance = 1;
		}
		if (m_nMaxDistance >= 1000000)
		{
			m_nMaxDistance = 1000000;
		}
		UpdateData(FALSE);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_MAXDISTANCE,20000,0);
		UpdateData(TRUE);
	}
}


void CScan2dCfg::OnEnChangeEditZerox()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL is_valid;
	GetDlgItemInt(IDC_EDIT_ZEROX,&is_valid,0);
	if(is_valid)
	{	
		UpdateData(TRUE);
		if (m_nZeroX <= 0)
		{
			m_nZeroX = 0;
		}
		if (m_nZeroX >= 721)
		{
			m_nZeroX = 721;
		}
		UpdateData(FALSE);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_ZEROX,1,0);
		UpdateData(TRUE);
	}	
}

DWORD WINAPI ThreadFuct_WriteLog_Sc(void *data)
{
	// 	WIN32_FIND_DATA fileInfo;
	// 	HANDLE hFind;
	// 	DWORD fileSize;
	structLog_Sc.in = 0;	
	structLog_Sc.out = 0;
	const char *fileName = "123";
	unsigned short i;
	CString strTmp; 
	CScan2dCfg *pDlg = (CScan2dCfg *)data;
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
		// 			g_strLogfile_Com = g_strLogfile_Com + time.Format("Log_Com_%Y%m%d%H%M%S.txt");		m_single2ddlg.m_ChartSi	CXX0017: ����: û���ҵ����š�m_single2ddlg��	

		// 			
		// 		}
		if (structLog_Sc.in == structLog_Sc.out)
		{
			Sleep(10);
			continue;
		}

		CString l_strInfoTxt;
		//l_strInfoTxt.Format("%s", MSystemTime());
		CString strLog_Time = CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S\r\n"));
		l_strInfoTxt = strLog_Time;
		g_strLogfile_Sc = CTime::GetCurrentTime().Format(_T("%Y-%m-%d-%H.txt"));
		if (structLog_Sc.strLogInfo[structLog_Sc.out] != "")
		{
			l_strInfoTxt += structLog_Sc.strLogInfo[structLog_Sc.out] + _T("\r\n");	
			if (structLog_Sc.strLogInfo[structLog_Sc.out].Left(9) == _T("������ "))
			{
				l_strInfoTxt = CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S__"));
				l_strInfoTxt += structLog_Sc.strLogInfo[structLog_Sc.out] + _T("\r\n");
				if (g_strMeasureDisfile_Sc == _T(""))
				{
					g_strMeasureDisfile_Sc =g_strLogDir_Sc +  CTime::GetCurrentTime().Format(_T("������_%Y-%m-%d-%H.txt"));
				}
				///FILE *l_pfMeasureDisfile_Sc;
				if((g_pfMeasureDisfile_Sc=fopen(g_strMeasureDisfile_Sc,_T("at+")))==NULL)
				{
					structLog_Sc.out ++;
					structLog_Sc.out %= MAXLOGINFO;
					continue;
				}
				else
				{
					fseek(g_pfMeasureDisfile_Sc,0,SEEK_END);
					fwrite(l_strInfoTxt,strlen(l_strInfoTxt),1,g_pfMeasureDisfile_Sc);            
					fclose(g_pfMeasureDisfile_Sc);
				}
				structLog_Sc.out ++;
				structLog_Sc.out %= MAXLOGINFO;
				continue;
			}

		}

		if (structLog_Sc.au16buflen[structLog_Sc.out] != 0)
		{
			for ( i = 0;  i < structLog_Sc.au16buflen[structLog_Sc.out];  i++)
			{
				strTmp.Format("%02X ", structLog.au8buf[structLog_Sc.out][i]);
				l_strInfoTxt += strTmp;
			}
			l_strInfoTxt += "\r\n";
		}

		if ((_T("���ݷ��ͳɹ���") != structLog_Sc.strLogInfo[structLog_Sc.out])&&(_T("���յ����ݣ�") != structLog_Sc.strLogInfo[structLog_Sc.out]))
		{
			pDlg->m_recOperLogSc.SetSel(-1, -1);        
			pDlg->m_recOperLogSc.ReplaceSel(l_strInfoTxt);        
			pDlg->m_recOperLogSc.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
		}
		else
		{
			// 			if (pDlg->m_bComShowSendOrRecvInfo == TRUE)
			{
				pDlg->m_recOperLogSc.SetSel(-1, -1);        
				pDlg->m_recOperLogSc.ReplaceSel(l_strInfoTxt);        
				pDlg->m_recOperLogSc.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
			}	
		}

		if((g_pfLogfile_Sc=fopen(g_strLogDir_Sc + g_strLogfile_Sc,"at+"))==NULL)
		{
			continue;
		}
		else
		{
			fseek(g_pfLogfile_Sc,0,SEEK_END);
			fwrite(l_strInfoTxt,strlen(l_strInfoTxt),1,g_pfLogfile_Sc);            
			fclose(g_pfLogfile_Sc);
		}
		//if ("UDP�������ӳɹ�" == structLog.strLogInfo[structLog.out])
		//{
		//	AfxGetMainWnd()->PostMessage(WM_INQUIREZERO, nBeatFrameSize, (LPARAM)acBeatFrameBuf);
		//}
		structLog_Sc.out ++;
		structLog_Sc.out %= MAXLOGINFO;
	}
	return 0;
}

BOOL CScan2dCfg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if (hthread_WriteLog_Sc <= 0)
	{
		DWORD thread1;
		hthread_WriteLog_Sc = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(ThreadFuct_WriteLog_Sc), this, 0, &thread1);
	}
	m_unFilePlayTimer = NULL;
	//CFileFind finder;
	//m_sIniFilePath = _T("Scan2dCfg.ini");
	//BOOL ifFind = finder.FindFile(m_sIniFilePath);
	//if(!ifFind)
	//{
	//	::WritePrivateProfileString(_T("Set Info"),_T("ZeroX"),_T("180)",m_sIniFilePath));
	//}
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��



	m_bSaveFlag = false;
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
extern unsigned char firstData;

void CScan2dCfg::OnBnClickedButtonClear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GraphForm2d * pView	;
	firstData = 0;
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pView = (GraphForm2d*) pMainFrame->m_wndLeftSplitter.GetPane(0,0);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_n32LostDataNum = 0;
	pView->SendMessage(WM_NETCMD_CLEARCHARTSC,g_nNetRecvInd,1);

}


void CScan2dCfg::OnBnClickedButtonSavenetdata()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_bSaveFlag == FALSE)
	{
		m_sLog = "��ʼ����";

		m_bSaveFlag = TRUE;
		//SetTimer(4,6000,NULL); //������ʱ�������ָ�����
		//	StartTimer(m_unSaveTimer,1,5000);
		GetDlgItem(IDC_BUTTON_SAVENETDATA)->SetWindowText("��ͣ����");

		//	TCHAR szFilePath[MAX_PATH + 1];

		GetModuleFileName(NULL, szFilePath, MAX_PATH);      

		(_tcsrchr(szFilePath, _T('\\')))[1] = 0;		

		m_strFiles.Format("%s",szFilePath);		
		m_strFiles += "NetDataFiles\\";
		m_strFiles.Replace("\\","\\\\");

		if (!PathFileExists(m_strFiles))
		{
			if(!CreateDirectory(m_strFiles,NULL)) 
			{
				MessageBox("�ļ��д���ʧ�ܣ�","��ʾ");
				return; 
			}		
		}
	}
	else if (m_bSaveFlag == TRUE)
	{
		m_sLog = "��ͣ����";
		m_bSaveFlag = FALSE;
		KillTimer(4); //�رն�ʱ��
		GetDlgItem(IDC_BUTTON_SAVENETDATA)->SetWindowText("��ʼ����");
	}	

	WriteLog(&m_recOperLogSc,m_sLog);
}


void CScan2dCfg::OnBnClickedBtnHtPrev()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_slFileName.GetCount() > 0)
	{
		if(m_nCurFileInd  < 1)
		{
			AfxMessageBox("��ǰ�Ѿ������һ����");
			return;
		}
		m_nCurFileInd--;
		AnalyLWDFile(m_sFilePath + m_slFileName.GetAt(m_slFileName.FindIndex(m_nCurFileInd)));
		m_nFrameID = m_nCurFileInd + 1;
		UpdateData(FALSE);
	}
}


void CScan2dCfg::OnBnClickedBtnHtNext()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_slFileName.GetCount() > 0)
	{
		if(m_nCurFileInd >= m_slFileName.GetCount()-1)
		{
			AfxMessageBox("��ǰ�Ѿ������һ����");
			return;
		}
		m_nCurFileInd++;
		AnalyLWDFile(m_sFilePath + m_slFileName.GetAt(m_slFileName.FindIndex(m_nCurFileInd)));
		m_nFrameID = m_nCurFileInd + 1;
		UpdateData(FALSE);
	}
}


void CScan2dCfg::OnBnClickedButtonFilePlay()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(m_slFileName.GetCount() > 0)
	{
		BOOL bTimer;
		if(m_unFilePlayTimer == NULL)
		{
			bTimer = FALSE;
			StartTimer(m_unFilePlayTimer,1,30);
			GetDlgItem(IDC_BTN_HT_SELECT)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_FILE_PLAY)->SetWindowText("ֹͣ");
			m_sLog = "������� ���\'����\'��ť.";
			WriteLog(&m_recOperLogSc,m_sLog);
		}
		else
		{
			bTimer = TRUE;
			StopTimer(m_unFilePlayTimer);
			GetDlgItem(IDC_BTN_HT_SELECT)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_FILE_PLAY)->SetWindowText("����");
			m_sLog = "������� ���\'ֹͣ\'��ť.";
			WriteLog(&m_recOperLogSc,m_sLog);
		}
		GetDlgItem(IDC_BTN_HT_NEXT)->EnableWindow(bTimer);
		GetDlgItem(IDC_BTN_HT_PREV)->EnableWindow(bTimer);
		GetDlgItem(IDC_BUTTON_OPEN_ONE_FRAME)->EnableWindow(bTimer);
	}
	else
	{
		MessageBox("���ȴ������ļ�","��ʾ",MB_OK);
		return;
	}
}
//ֹͣ��ʱ��
void CScan2dCfg::StopTimer(UINT &nTimer)
{
	if(nTimer)
	{
		KillTimer(nTimer);
		nTimer = NULL;
	}
}

//��ʼ��ʱ��
void CScan2dCfg::StartTimer(UINT &nTimer,UINT nIDEvent,UINT nElapse)
{
	if(nTimer)
	{
		StopTimer(nTimer);
	}
	nTimer = SetTimer(nIDEvent,nElapse,0);
}

void CScan2dCfg::OnTimer(UINT nIDEvent) 
{
	switch (nIDEvent)   
    {   
    case 1:   
       if(m_nCurFileInd < m_slFileName.GetCount()-1)
			OnBnClickedBtnHtNext();
		else
			OnBnClickedButtonFilePlay();   
        break;   
    case 6:   
		if (g_nSendWaveNum>=5)
		{
			g_bMoniterFaultedLost=true;
			g_nLostWaveCount++;
			g_nSendWaveNum = 0;
		}
        AfxGetMainWnd()->PostMessage(WM_SENDCMD, nGetSickOneFrameBufSize, (LPARAM)acGetSickOneFrameBuf);
		g_nSendWaveNum++;
		g_nSendNum_sigle++;
		m_nSigDataCmdCnt = g_nSendNum_sigle;
		UpdateData(false);
		break; 
    default:   
        break;   
    }
}

void CScan2dCfg::OnBnClickedButtonOpenOneFrame()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_slFileName.GetCount() > 0)
	{
		UpdateData(TRUE);
		if(m_nFrameID > m_slFileName.GetCount() || m_nFrameID <1)
		{
			m_sLog.Format("������� ���\'��ת\'��ť������֡��%d,����֡��������Χ�����������룡",m_slFileName.GetCount());
			AfxMessageBox(m_sLog);
			WriteLog(&m_recOperLogSc,m_sLog);
			return;
		}
		m_nCurFileInd = m_nFrameID - 1;
		AnalyLWDFile(m_sFilePath+m_slFileName.GetAt(m_slFileName.FindIndex(m_nCurFileInd)));
		m_sLog.Format("������� ���\'��ת\'��ť����ת��֡��Ϊ%d�������ļ�.",m_nCurFileInd);
	}
	else
	{
		m_sLog = "������� ���\'��ת\'��ť����ǰδѡ���ļ��У���ѡ����ļ������������ļ�.";
	}

	WriteLog(&m_recOperLogSc,m_sLog);
}


void CScan2dCfg::OnBnClickedButtonRecord()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(true);
	CString str;
	//for(int i = 0;i<g_u32MeasureInfoIndex;i++)
	//	g_u32MeasureInfoIndex = 0;
	//str.Format("%d",m_mesureinterval);
	//m_OperExcel.SetCellString(g_sExcelInfo.m_u32Row,1,str);
	g_sMeasureInfo[g_u32MeasureInfoIndex].m_mesureinterval = m_mesureinterval;
	g_sMeasureInfo[g_u32MeasureInfoIndex].m_nRealDisc = m_nRealDisc;
	//str.Format("%d",m_nangle_num);
	//m_OperExcel.SetCellString(g_sExcelInfo.m_u32Row,2,str);
	g_sMeasureInfo[g_u32MeasureInfoIndex].m_nangle_num = m_nangle_num;
	//str.Format("%d",m_nMaxDis);
	//m_OperExcel.SetCellString(g_sExcelInfo.m_u32Row,3,str);
	g_sMeasureInfo[g_u32MeasureInfoIndex].m_nMaxDis = m_nMaxDis;
	//str.Format("%d",m_nMinDis);
	//m_OperExcel.SetCellString(g_sExcelInfo.m_u32Row,4,str);
	g_sMeasureInfo[g_u32MeasureInfoIndex].m_nMinDis = m_nMinDis;
	//str.Format("%d",m_nAVGDis);
	//m_OperExcel.SetCellString(g_sExcelInfo.m_u32Row,5,str);
	g_sMeasureInfo[g_u32MeasureInfoIndex].m_nAVGDis = m_nAVGDis;
	//str.Format("%d",m_dealnegative);
	//m_OperExcel.SetCellString(g_sExcelInfo.m_u32Row,6,str);
	g_sMeasureInfo[g_u32MeasureInfoIndex].m_dealnegative = m_dealnegative;

	g_sMeasureInfo[g_u32MeasureInfoIndex].m_editValRef = m_nAVGDis;
	g_sMeasureInfo[g_u32MeasureInfoIndex].m_editDispersion = m_editDispersion;
	g_sMeasureInfo[g_u32MeasureInfoIndex].m_ediProbability = m_ediProbability;


	str.Format("%d",m_mesureinterval);
	g_sRecordInfo.m_strInter = str;

	CString l_strInfoTxt;
	str.Format("%d",g_u32MeasureInfoIndex);
	l_strInfoTxt = "��¼��"+str+"������\n";
	str.Format("%d",m_nRealDisc);
	l_strInfoTxt = l_strInfoTxt+"ʵ�ʾ��룺"+ str + "\n";


	str.Format("%d",m_nMaxDis);
	l_strInfoTxt = l_strInfoTxt + "���ֵ��"+ str + "\n";

	str.Format("%d",m_nMinDis);
	l_strInfoTxt = l_strInfoTxt + "��Сֵ��"+ str + "\n";

	str.Format("%d",m_nAVGDis);
	l_strInfoTxt = l_strInfoTxt + "ƽ��ֵ��"+ str + "\n";

	/*str.Format("%d",m_editValRef);
	l_strInfoTxt = l_strInfoTxt + "�ο�ֵ��"+ str + "\n";*/
	str.Format("%d",m_nAVGDis);
	l_strInfoTxt = l_strInfoTxt + "�ο�ֵ��"+ str + "\n";

	str.Format("%d",m_editDispersion);
	l_strInfoTxt = l_strInfoTxt + "��ɢ�ȣ���"+ str + "\n";

	str.Format("%f",m_ediProbability);
	l_strInfoTxt = l_strInfoTxt + "���ʣ�"+ str +"%"+ "\n";


	m_recOperLogSc.SetSel(-1, -1);        
	m_recOperLogSc.ReplaceSel(l_strInfoTxt);        
	m_recOperLogSc.PostMessage(WM_VSCROLL, SB_BOTTOM,0);

	g_u32MeasureInfoIndex = (g_u32MeasureInfoIndex+1);
	UpdateData(false);
	//g_sExcelInfo.m_u32Row++;
	//m_OperExcel.SaveasXSLFile(g_sExcelInfo.m_strName);
	//m_OperExcel.CloseExcelFile(0);
	//m_OperExcel.ReleaseExcel();
}


void CScan2dCfg::OnEnChangeAveragedistance2()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}


void CScan2dCfg::OnBnClickedButtonRecordDel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(g_u32MeasureInfoIndex > 0)
	{
		g_u32MeasureInfoIndex = (g_u32MeasureInfoIndex-1);
		CString l_strInfoTxt,str;	
		str.Format("%d",g_u32MeasureInfoIndex);
		l_strInfoTxt = "ɾ����"+str+"������\n";

		m_recOperLogSc.SetSel(-1, -1);        
		m_recOperLogSc.ReplaceSel(l_strInfoTxt);        
		m_recOperLogSc.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
	}
	else
	{
		CString l_strInfoTxt;
		l_strInfoTxt = "��Ϊ��0������\n";

		m_recOperLogSc.SetSel(-1, -1);        
		m_recOperLogSc.ReplaceSel(l_strInfoTxt);        
		m_recOperLogSc.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
	}
}


void CScan2dCfg::OnBnClickedCheck1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CScan2dCfg::OnBnClickedButtonSave()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	

	//CString strFileName = "d:\\adasdfsa.txt";
	CString strFileName;
	CFileDialog *lpszOpenFile; //����һ��CfileDialog����
	lpszOpenFile = new CFileDialog(FALSE,"",CTime::GetCurrentTime().Format("������_%Y-%m-%d-%H-%M-%S.txt"),OFN_HIDEREADONLY |OFN_OVERWRITEPROMPT ,_T( "�ļ�����(*.txt)|*.txt||"));//����һ���Ի���
	if(lpszOpenFile->DoModal() == IDOK)//�������Ի���ȷ����ť
	{
		strFileName = lpszOpenFile->GetPathName();
	}
	
	CFile file;
	file.Open(strFileName,CFile::modeCreate|CFile::modeReadWrite);
	CString str;
	char strTitle[100] = "�ο�ֵ      ��ɢ��      ����\r\n";
	file.Write((LPCTSTR)strTitle,strlen(strTitle));
	for(int i = 0;i < g_u32MeasureInfoIndex;i++)
	{
		CString strTmp;
		strTmp.Format(" %d         %d      %f\r\n",g_sMeasureInfo[i].m_editValRef,g_sMeasureInfo[i].m_editDispersion,g_sMeasureInfo[i].m_ediProbability);
		str += strTmp;
	}
	file.Write((LPCTSTR)str,str.GetLength());
	file.Close();
	MessageBox(_T("�ļ�����ɹ���"));
}

void CScan2dCfg::OnKillfocusEditDispersion()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL is_valid;
	GetDlgItemInt(IDC_EDIT_DISPERSION,&is_valid,0);
	if(is_valid)
	{	
		UpdateData(TRUE);
		if(m_editDispersion<0 )
			m_editDispersion = 0;
		UpdateData(false);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_DISPERSION,1,0);
		UpdateData(TRUE);
	}
}

void CScan2dCfg::OnEnChangeEditValref()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	BOOL is_valid;
	GetDlgItemInt(IDC_EDIT_VALREF,&is_valid,0);
	if(is_valid)
	{	
		UpdateData(TRUE);
		if(m_editValRef<0 )
			m_editValRef = 0;
		UpdateData(false);
	}
	else
	{
		SetDlgItemInt(IDC_EDIT_VALREF,1,0);
		UpdateData(TRUE);
	}
}

void CScan2dCfg::OnKillfocusRealdisc()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	BOOL is_valid;
	GetDlgItemInt(IDC_REALDISC ,&is_valid,0);
	if(is_valid)
	{	
		UpdateData(TRUE);
		if(m_nRealDisc<0 )
			m_nRealDisc = 0;
		UpdateData(false);
	}
	else
	{
		SetDlgItemInt(IDC_REALDISC ,1,0);
		UpdateData(TRUE);
	}
}

void CScan2dCfg::OnBnClickedButtonClearcnt()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g_nSendNum_sigle = 0;
	g_nRecvNum_scanf = 0;
	m_nSigDataCmdCnt = 0;
	m_nRecvNum = 0;
	UpdateData(false);
}


void CScan2dCfg::OnBnClickedCheck20mssigdatacmd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int n1 = 0;
	n1 =  ((CButton*)(GetDlgItem(IDC_CHECK_20msSIGDATACMD)))->GetCheck();

	if(n1 == 1)
	{
		m_btn_sick_oneframe.EnableWindow(false);
		m_btn_sick_continueframe.EnableWindow(false);
		SetTimer(6, 10, NULL);
		WriteLog(&m_recOperLogSc,"20ms��ʱ���͵�֡ȡ������");
	}
	else
	{
		m_btn_sick_oneframe.EnableWindow(true);
		m_btn_sick_continueframe.EnableWindow(true);
		KillTimer(6);
		WriteLog(&m_recOperLogSc,"ֹͣ���͵�֡ȡ������");
	}
}


void CScan2dCfg::OnBnClickedButtonWorkmodeset()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	int WorkMode=m_WorkModeSel.GetCurSel();
	if (WorkMode==0)
	{
		AfxGetMainWnd()->PostMessage(WM_SENDCMD, LandmarkModeSet_zhilingSize, (LPARAM)LandmarkModeSet_zhiling);
	} 
	else if (WorkMode==1)
	{
		AfxGetMainWnd()->PostMessage(WM_SENDCMD, MappingModeSet_zhilingSize, (LPARAM)MappingModeSet_zhiling);
	}
	else if (WorkMode==2)
	{
		AfxGetMainWnd()->PostMessage(WM_SENDCMD, NaviModeSet_zhilingSize, (LPARAM)NaviModeSet_zhiling);
	}
	WriteLog(&m_recOperLogSc,"�������ù���ģʽ����");
	UpdateData(false);
}






