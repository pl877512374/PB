// Scan2dCfg.cpp : 实现文件
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
// CScan2dCfg 对话框
extern int nAVGPos;
TCHAR szFilePath[MAX_PATH + 1];  //存放文件目录
IMPLEMENT_DYNAMIC(CScan2dCfg, CDialog)
	//写日志线程
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
//日志文件指针
FILE *g_pfLogfile_Sc;  
FILE *g_pfMeasureDisfile_Sc;
STRUCT_LOG structLog_Sc;
//当前应用程序目录
CString g_strCurDir_Sc = _T("");
//日志文件目录
CString g_strLogDir_Sc = _T("");
// CScanf2dDlg 对话框
CScan2dCfg::CScan2dCfg(CWnd* pParent /*=NULL*/)
	: CDialog(CScan2dCfg::IDD, pParent)
	, m_editValRef(0)
	, m_editDispersion(0)
	, m_ediProbability(0)
	, m_nSigDataCmdCnt(0)
{
	m_dealnegative = false;//190826加 不加的话在release下默认为true 就会导致切后的波形看起来异常
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


// CScan2dCfg 消息处理程序


void CScan2dCfg::OnBnClickedButtonSickOneframe()
{
	AfxGetMainWnd()->PostMessage(WM_SENDCMD, nGetSickOneFrameBufSize, (LPARAM)acGetSickOneFrameBuf);
	WriteLog(&m_recOperLogSc,"发送单帧取数命令");
	UpdateData(false);
	// TODO: 在此添加控件通知处理程序代码
}


void CScan2dCfg::OnBnClickedButtonSickcontinueframe()
{
	// TODO: 在此添加控件通知处理程序代码
	//int nBufSize = 25;
	m_nNetorFile = 1;
	CString  str;
	StructLogFile l_sMyLog;
	GetDlgItem(IDC_BUTTON_SICKCONTINUEFRAME)->GetWindowText(str);
	if (str == "停止获取")   //停止连续获取命令
	{
		KillTimer(6);
		/*AfxGetMainWnd()->SendMessage(WM_SENDCMD, nStopGetSickContinueFrameBufSize, (LPARAM)acStopGetSickContinueFrameBuf);*/
		GetDlgItem(IDC_BUTTON_SICKCONTINUEFRAME)->SetWindowText("连续获取");

		//l_sMyLog.m_strLog = "发送停止获取命令";
		//l_sMyLog.m_n32id = IDC_RICHEDIT_OPERATEMESSAGE_SC;
		//((CRichEditCtrl *)(GetDlgItem(l_sMyLog.m_n32id)))->SetSel(-1, -1); 
		//	((CRichEditCtrl *)(GetDlgItem(l_sMyLog.m_n32id)))->ReplaceSel(l_sMyLog.m_strLog); 
		//	((CRichEditCtrl *)(GetDlgItem(l_sMyLog.m_n32id)))->PostMessage(WM_VSCROLL, SB_BOTTOM,0);
		//WriteMyLog(l_sMyLog);
		WriteLog(&m_recOperLogSc,"发送停止获取命令");
	} 
	else					 //连续获取命令
	{	
		GraphForm2d * pView	;
		CString l_strBtnName;
		CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
		pView = (GraphForm2d*) pMainFrame->m_wndLeftSplitter.GetPane(0,0);
		pView->m_single2ddlg.m_sSi2dCfgDlg.m_btnPauseSend.GetWindowText(l_strBtnName);
		if (l_strBtnName == "暂停发数")
		{
			pView->m_single2ddlg.m_sSi2dCfgDlg.OnBnClickedButtonPauseSend();
		}

		SetTimer(6, 130, NULL);
		/*AfxGetMainWnd()->SendMessage(WM_SENDCMD, nGetSickContinueFrameBufSize, (LPARAM)acGetSickContinueFrameBuf);*/	
		GetDlgItem(IDC_BUTTON_SICKCONTINUEFRAME)->SetWindowText("停止获取");	
		WriteLog(&m_recOperLogSc,"发送连续获取命令");
	}
}

void CScan2dCfg::OnKillfocusMesureinterval()
{
	// TODO: 在此添加控件通知处理程序代码
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
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

//回调函数 胡孟虎
int CALLBACK BroweCallProc(HWND hwnd, UINT msg,LPARAM lp,LPARAM pData)
{
	if(msg == BFFM_INITIALIZED)
	{
		::SendMessage(hwnd ,BFFM_SETSELECTION,TRUE,pData);
	}
	return 0;
}

int g_nRevcNum = 0;          //接收包数
extern int  createflag; //创建对话框标志
extern int g_nCount_scanf ;
extern int maxline;
extern int g_nseriesnum; //极坐标线数量
extern CSeries g_asChartJi[50],g_asChartZhi[50];
extern int cBufAVG[1024];
extern int getData(int buf[],int len,int *AVG,int *MAX, int *MIN);
//extern const int c_n32MaxAngle;

extern double YValuesTmp[5000];
extern double XValuesTmp[5000];

extern double Y1ValuesTmp[50000];
extern double X1ValuesTmp[50000];
int g_nSendNum_sigle = 0;          //单帧命令数
//绘制波形
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
		&& pcDataBuf[2] == (char)0x02 && pcDataBuf[3] == (char)0x02)//TCP数据
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
			//if (m_dealnegative) //修正
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

				if (m_dealnegative) //修正
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
	else if(pcDataBuf[0] == (char)0xFF && pcDataBuf[1] == (char)0xFF)//UDP数据
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
		//	   MessageBox("数据格式不对，读取失败！",MB_OK);
		//	   Sleep(100);
		//return FALSE;
	}
	UpdateData(FALSE);
	pView->UpdateData(FALSE);
}

void CScan2dCfg::OnBnClickedBtnHtSelect()
{
	// TODO: 在此添加控件通知处理程序代码
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
	m_sFilePath = fileUtils.OpenDirDlg(this->GetSafeHwnd(),"选择数据文件夹",fileUtils.GetCurDir());*/
	m_sFilePath += "\\";
	if (MessageBox(m_sFilePath,"所选文件夹",MB_YESNO) == IDNO)
	{
		return ;
	}
	m_sLog = "界面操作 点击\'选择\'按钮,并选择了文件夹：";
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

		res = fileFind.FindFile(m_sFilePath + "*.txt");//开始查找txt文件
		if (!res)
		{
			res = fileFind.FindFile(m_sFilePath + "*.lmd");//开始查找lmd文件
		}		
		if (!res)
		{
			res = fileFind.FindFile(m_sFilePath + "*.wj"); //开始查找wj文件
		}	
		if(!res)
		{
			MessageBox("没有查找到数据文件，请重新查找!","提示",MB_OK);
			return;
		}		
		m_slFileName.RemoveAll();
		while(res)
		{
			res = fileFind.FindNextFile();
			if(!fileFind.IsDirectory() && !fileFind.IsDots())
			{
				sFileName = fileFind.GetFileName();//把文件放到CStringList
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
			m_sLog.Format("选择数据文件夹，共有%d条数据",m_slFileName.GetCount());
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

			nSeriesNumZhi = pView->m_scanf2ddlg.m_ChartZhi.get_SeriesCount();     //清空极坐标图像
			for (int i=0; i<nSeriesNumZhi; i++)
				((CSeries) pView->m_scanf2ddlg.m_ChartZhi.Series(i)).Clear();

			nSeriesNumJi =  pView->m_scanf2ddlg.m_ChartJi.get_SeriesCount();   //清空直角坐标图像
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
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	BOOL is_valid;
	GetDlgItemInt(IDC_EDIT_ONECHART,&is_valid,0);
	if(is_valid)
	{
		UpdateData(TRUE);
		if (m_nInterval == 0)
		{
			m_nInterval = 1;
		}
		if (m_nNetorFile)  //网络收数
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
	// TODO: 在此添加控件通知处理程序代码
	int n1 = 0;
	CString szGetName;
	n1 =  ((CButton*)(GetDlgItem(IDC_CHECK2)))->GetCheck();
	g_u32MeasureInfoIndex = 0; 
	//InitExcel();
	//初始化excel相关工作

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

	//bool bRet = m_OperExcel.OpenExcelFile("扫描试激光传感器711检测模板误修改.xlsx");
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
	//		AfxMessageBox("初始化excel失败！");
	//		return ;
	//	}
	//	TCHAR szPath[MAX_PATH];   
	//	GetModuleFileName(NULL, szPath, MAX_PATH);
	//	CString PathName(szPath);
	//	CString strExcelDemoPath = PathName.Left(PathName.ReverseFind(_T('\\')) + 1);
	//	strExcelDemoPath += "扫描试激光传感器711检测模板误修改.xlsx";

	//	bool bRet = m_OperExcel.OpenExcelFile(strExcelDemoPath);
	//	CString strSheetName = m_OperExcel.GetSheetName(1);
	//	bool bLoad = m_OperExcel.LoadSheet(strSheetName);*/

	//	g_sExcelInfo.m_strName = "";
	//	g_sExcelInfo.m_u32Col = 2;
	//	g_sExcelInfo.m_u32Row = 1;
	//	CFileDialog *lpszOpenFile; //定义一个CfileDialog对象
	//	lpszOpenFile = new CFileDialog(FALSE,"",CTime::GetCurrentTime().Format("激光测距_%Y-%m-%d-%H-%M-%S.xlsx"),OFN_FILEMUSTEXIST |OFN_HIDEREADONLY , "文件类型(*.xls)|*.xlsx||");//生成一个对话框
	//	if(lpszOpenFile->DoModal() == IDOK)//假如点击对话框确定按钮
	//	{
	//		szGetName = lpszOpenFile->GetPathName();
	//		g_sExcelInfo.m_strName = szGetName;//;
	//		/*m_sLog.Format("激光测距   间隔： 测量点： 最大值： 最小值： 平均值： 修正状态");
	//		WriteLog(&m_recOperLogSc,m_sLog);*/

	//	}
	//	delete lpszOpenFile;//释放分配的对话框
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
		structLog_Sc.strLogInfo[structLog_Sc.in] = "无提示信息";	
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
	// TODO: 在此添加控件通知处理程序代码
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
	dlg.SetStaticText(CString("请输入测试人员名"));
	if(dlg.DoModal() == IDOK)
	{
		strDecName = dlg.GetInputText();
	}

	if(strDecName == "")
	{
		pDlg->MessageBox("保存失败！\r\n请输入测试人员名","提示", MB_ICONINFORMATION);
		pDlg->GetDlgItem(IDC_BUTTON_SAVEMEASUREDIS)->EnableWindow(TRUE);
		return 1;
	}

	g_sExcelInfo.m_strName = "";
	g_sExcelInfo.m_u32Col = 1;
	g_sExcelInfo.m_u32Row = 10;
	CFileDialog *lpszOpenFile; //定义一个CfileDialog对象
	lpszOpenFile = new CFileDialog(FALSE,"",CTime::GetCurrentTime().Format("激光测距_%Y-%m-%d-%H-%M-%S.xlsx"),OFN_FILEMUSTEXIST |OFN_HIDEREADONLY , "文件类型(*.xls)|*.xlsx||");//生成一个对话框
	if(lpszOpenFile->DoModal() == IDOK)//假如点击对话框确定按钮
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
		AfxMessageBox("已达到最大记录数据");
		pDlg->GetDlgItem(IDC_BUTTON_SAVEMEASUREDIS)->EnableWindow(TRUE);
		return 1;
	}

	bool bInit = pDlg->m_OperExcel.InitExcel();
	if(!bInit)
	{
		AfxMessageBox("初始化excel失败！");
		pDlg->GetDlgItem(IDC_BUTTON_SAVEMEASUREDIS)->EnableWindow(TRUE);
		return 1;
	}
	TCHAR szPath[MAX_PATH];   
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString PathName(szPath);
	CString strExcelDemoPath = PathName.Left(PathName.ReverseFind(_T('\\')) + 1);
	strExcelDemoPath += "加滤光片定位焦距后测距数据2015.05.11.xlsx";

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
		pDlg->m_OperExcel.SetCellString_Cells("G7",1,1,str);  //记录总条数

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
	pDlg->MessageBox("保存成功！","提示", MB_ICONINFORMATION);
	pDlg->GetDlgItem(IDC_BUTTON_SAVEMEASUREDIS)->EnableWindow(TRUE);
	return 1;
}
void CScan2dCfg::OnBnClickedButtonSavemeasuredis()
{
	// TODO: 在此添加控件通知处理程序代码
	CString szGetName;
	UpdateData(FALSE);
	DWORD dwThreadID = 0;
	HANDLE hThread=CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadSaveExcel,this,NULL,&dwThreadID);
	//m_sLog.Format("激光测距 %d %d %d %d %d %d",m_mesureinterval, m_nangle_num, m_nMaxDis, m_nMinDis, m_nAVGDis, m_dealnegative);

	//bool bRet = m_OperExcel.OpenExcelFile("扫描试激光传感器711检测模板误修改.xlsx");
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
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
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
		if (m_nNetorFile)  //网络收数
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
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
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
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
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
		// 			g_strLogfile_Com = g_strLogfile_Com + time.Format("Log_Com_%Y%m%d%H%M%S.txt");		m_single2ddlg.m_ChartSi	CXX0017: 错误: 没有找到符号“m_single2ddlg”	

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
			if (structLog_Sc.strLogInfo[structLog_Sc.out].Left(9) == _T("激光测距 "))
			{
				l_strInfoTxt = CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S__"));
				l_strInfoTxt += structLog_Sc.strLogInfo[structLog_Sc.out] + _T("\r\n");
				if (g_strMeasureDisfile_Sc == _T(""))
				{
					g_strMeasureDisfile_Sc =g_strLogDir_Sc +  CTime::GetCurrentTime().Format(_T("激光测距_%Y-%m-%d-%H.txt"));
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

		if ((_T("数据发送成功！") != structLog_Sc.strLogInfo[structLog_Sc.out])&&(_T("接收到数据：") != structLog_Sc.strLogInfo[structLog_Sc.out]))
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
		//if ("UDP网络连接成功" == structLog.strLogInfo[structLog.out])
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
	// TODO:  在此添加额外的初始化



	m_bSaveFlag = false;
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
extern unsigned char firstData;

void CScan2dCfg::OnBnClickedButtonClear()
{
	// TODO: 在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	GraphForm2d * pView	;
	firstData = 0;
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pView = (GraphForm2d*) pMainFrame->m_wndLeftSplitter.GetPane(0,0);
	// TODO: 在此添加控件通知处理程序代码
	m_n32LostDataNum = 0;
	pView->SendMessage(WM_NETCMD_CLEARCHARTSC,g_nNetRecvInd,1);

}


void CScan2dCfg::OnBnClickedButtonSavenetdata()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_bSaveFlag == FALSE)
	{
		m_sLog = "开始保存";

		m_bSaveFlag = TRUE;
		//SetTimer(4,6000,NULL); //启动定时器用来恢复网络
		//	StartTimer(m_unSaveTimer,1,5000);
		GetDlgItem(IDC_BUTTON_SAVENETDATA)->SetWindowText("暂停保存");

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
				MessageBox("文件夹创建失败！","提示");
				return; 
			}		
		}
	}
	else if (m_bSaveFlag == TRUE)
	{
		m_sLog = "暂停保存";
		m_bSaveFlag = FALSE;
		KillTimer(4); //关闭定时器
		GetDlgItem(IDC_BUTTON_SAVENETDATA)->SetWindowText("开始保存");
	}	

	WriteLog(&m_recOperLogSc,m_sLog);
}


void CScan2dCfg::OnBnClickedBtnHtPrev()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_slFileName.GetCount() > 0)
	{
		if(m_nCurFileInd  < 1)
		{
			AfxMessageBox("当前已经是最后一个！");
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
	// TODO: 在此添加控件通知处理程序代码
	if(m_slFileName.GetCount() > 0)
	{
		if(m_nCurFileInd >= m_slFileName.GetCount()-1)
		{
			AfxMessageBox("当前已经是最后一个！");
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
	// TODO: 在此添加控件通知处理程序代码
	if(m_slFileName.GetCount() > 0)
	{
		BOOL bTimer;
		if(m_unFilePlayTimer == NULL)
		{
			bTimer = FALSE;
			StartTimer(m_unFilePlayTimer,1,30);
			GetDlgItem(IDC_BTN_HT_SELECT)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_FILE_PLAY)->SetWindowText("停止");
			m_sLog = "界面操作 点击\'播放\'按钮.";
			WriteLog(&m_recOperLogSc,m_sLog);
		}
		else
		{
			bTimer = TRUE;
			StopTimer(m_unFilePlayTimer);
			GetDlgItem(IDC_BTN_HT_SELECT)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_FILE_PLAY)->SetWindowText("播放");
			m_sLog = "界面操作 点击\'停止\'按钮.";
			WriteLog(&m_recOperLogSc,m_sLog);
		}
		GetDlgItem(IDC_BTN_HT_NEXT)->EnableWindow(bTimer);
		GetDlgItem(IDC_BTN_HT_PREV)->EnableWindow(bTimer);
		GetDlgItem(IDC_BUTTON_OPEN_ONE_FRAME)->EnableWindow(bTimer);
	}
	else
	{
		MessageBox("请先打开数据文件","提示",MB_OK);
		return;
	}
}
//停止计时器
void CScan2dCfg::StopTimer(UINT &nTimer)
{
	if(nTimer)
	{
		KillTimer(nTimer);
		nTimer = NULL;
	}
}

//开始计时器
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
	// TODO: 在此添加控件通知处理程序代码
	if (m_slFileName.GetCount() > 0)
	{
		UpdateData(TRUE);
		if(m_nFrameID > m_slFileName.GetCount() || m_nFrameID <1)
		{
			m_sLog.Format("界面操作 点击\'跳转\'按钮，现有帧数%d,输入帧数超过范围，请重新输入！",m_slFileName.GetCount());
			AfxMessageBox(m_sLog);
			WriteLog(&m_recOperLogSc,m_sLog);
			return;
		}
		m_nCurFileInd = m_nFrameID - 1;
		AnalyLWDFile(m_sFilePath+m_slFileName.GetAt(m_slFileName.FindIndex(m_nCurFileInd)));
		m_sLog.Format("界面操作 点击\'跳转\'按钮，跳转至帧号为%d的数据文件.",m_nCurFileInd);
	}
	else
	{
		m_sLog = "界面操作 点击\'跳转\'按钮，当前未选择文件夹，或选择的文件夹中无数据文件.";
	}

	WriteLog(&m_recOperLogSc,m_sLog);
}


void CScan2dCfg::OnBnClickedButtonRecord()
{
	// TODO: 在此添加控件通知处理程序代码
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
	l_strInfoTxt = "记录第"+str+"组数据\n";
	str.Format("%d",m_nRealDisc);
	l_strInfoTxt = l_strInfoTxt+"实际距离："+ str + "\n";


	str.Format("%d",m_nMaxDis);
	l_strInfoTxt = l_strInfoTxt + "最大值："+ str + "\n";

	str.Format("%d",m_nMinDis);
	l_strInfoTxt = l_strInfoTxt + "最小值："+ str + "\n";

	str.Format("%d",m_nAVGDis);
	l_strInfoTxt = l_strInfoTxt + "平均值："+ str + "\n";

	/*str.Format("%d",m_editValRef);
	l_strInfoTxt = l_strInfoTxt + "参考值："+ str + "\n";*/
	str.Format("%d",m_nAVGDis);
	l_strInfoTxt = l_strInfoTxt + "参考值："+ str + "\n";

	str.Format("%d",m_editDispersion);
	l_strInfoTxt = l_strInfoTxt + "离散度：±"+ str + "\n";

	str.Format("%f",m_ediProbability);
	l_strInfoTxt = l_strInfoTxt + "概率："+ str +"%"+ "\n";


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
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}


void CScan2dCfg::OnBnClickedButtonRecordDel()
{
	// TODO: 在此添加控件通知处理程序代码
	if(g_u32MeasureInfoIndex > 0)
	{
		g_u32MeasureInfoIndex = (g_u32MeasureInfoIndex-1);
		CString l_strInfoTxt,str;	
		str.Format("%d",g_u32MeasureInfoIndex);
		l_strInfoTxt = "删除第"+str+"组数据\n";

		m_recOperLogSc.SetSel(-1, -1);        
		m_recOperLogSc.ReplaceSel(l_strInfoTxt);        
		m_recOperLogSc.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
	}
	else
	{
		CString l_strInfoTxt;
		l_strInfoTxt = "已为第0组数据\n";

		m_recOperLogSc.SetSel(-1, -1);        
		m_recOperLogSc.ReplaceSel(l_strInfoTxt);        
		m_recOperLogSc.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
	}
}


void CScan2dCfg::OnBnClickedCheck1()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CScan2dCfg::OnBnClickedButtonSave()
{
	// TODO: 在此添加控件通知处理程序代码
	

	//CString strFileName = "d:\\adasdfsa.txt";
	CString strFileName;
	CFileDialog *lpszOpenFile; //定义一个CfileDialog对象
	lpszOpenFile = new CFileDialog(FALSE,"",CTime::GetCurrentTime().Format("激光测距_%Y-%m-%d-%H-%M-%S.txt"),OFN_HIDEREADONLY |OFN_OVERWRITEPROMPT ,_T( "文件类型(*.txt)|*.txt||"));//生成一个对话框
	if(lpszOpenFile->DoModal() == IDOK)//假如点击对话框确定按钮
	{
		strFileName = lpszOpenFile->GetPathName();
	}
	
	CFile file;
	file.Open(strFileName,CFile::modeCreate|CFile::modeReadWrite);
	CString str;
	char strTitle[100] = "参考值      离散度      概率\r\n";
	file.Write((LPCTSTR)strTitle,strlen(strTitle));
	for(int i = 0;i < g_u32MeasureInfoIndex;i++)
	{
		CString strTmp;
		strTmp.Format(" %d         %d      %f\r\n",g_sMeasureInfo[i].m_editValRef,g_sMeasureInfo[i].m_editDispersion,g_sMeasureInfo[i].m_ediProbability);
		str += strTmp;
	}
	file.Write((LPCTSTR)str,str.GetLength());
	file.Close();
	MessageBox(_T("文件保存成功！"));
}

void CScan2dCfg::OnKillfocusEditDispersion()
{
	// TODO: 在此添加控件通知处理程序代码
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
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
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialog::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
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
	// TODO: 在此添加控件通知处理程序代码
	g_nSendNum_sigle = 0;
	g_nRecvNum_scanf = 0;
	m_nSigDataCmdCnt = 0;
	m_nRecvNum = 0;
	UpdateData(false);
}


void CScan2dCfg::OnBnClickedCheck20mssigdatacmd()
{
	// TODO: 在此添加控件通知处理程序代码
	int n1 = 0;
	n1 =  ((CButton*)(GetDlgItem(IDC_CHECK_20msSIGDATACMD)))->GetCheck();

	if(n1 == 1)
	{
		m_btn_sick_oneframe.EnableWindow(false);
		m_btn_sick_continueframe.EnableWindow(false);
		SetTimer(6, 10, NULL);
		WriteLog(&m_recOperLogSc,"20ms定时发送单帧取数命令");
	}
	else
	{
		m_btn_sick_oneframe.EnableWindow(true);
		m_btn_sick_continueframe.EnableWindow(true);
		KillTimer(6);
		WriteLog(&m_recOperLogSc,"停止发送单帧取数命令");
	}
}


void CScan2dCfg::OnBnClickedButtonWorkmodeset()
{
	// TODO: 在此添加控件通知处理程序代码
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
	WriteLog(&m_recOperLogSc,"发送设置工作模式命令");
	UpdateData(false);
}






