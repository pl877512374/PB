// GraphForm2d.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "GraphForm2d.h"
#include "NetParam.h"
#include "CAxis.h"
#include "CAxes.h"
#include "CLegend.h"
#include "CSeries.h"
#include "math.h"
#include "CPointSeries.h"
#include "CPointer.h"
#include "CPen0.h"
#include "MainFrm.h"
// GraphForm2d
int g_nDrawNum_scanf = 0;          //绘制包数
int maxline = 0;
int g_nseriesnum = 0; //极坐标线数量
int cBufAVG[1024] = {0};
int nAVGPos = 0;
const int c_n32MaxAngle = 360;
double YValuesTmp[50000];
double XValuesTmp[50000];

double Y1ValuesTmp[50000];
double X1ValuesTmp[50000];
CSeries g_asChartJi[1000],g_asChartZhi[1000];
int ScanPointCountMode = 0;   //0:3600 ; 1:7200
int LastScanPointCountMode = 0;
int g_nAngle;
extern u8 g_u8CurClkmode;
extern int g_nCount_scanf;
IMPLEMENT_DYNCREATE(GraphForm2d, CFormView)

GraphForm2d::GraphForm2d()
	: CFormView(GraphForm2d::IDD)
{
	m_bFisrtEasySize =true;
	
}

GraphForm2d::~GraphForm2d()
{
}

void GraphForm2d::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_2DGRAPH, m_TabGraphSet);
}

BEGIN_MESSAGE_MAP(GraphForm2d, CFormView)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_MESSAGE(WM_NETCMD_CLEARCHARTSI,OnClearChart)
	ON_MESSAGE(WM_NETCMD_DRAWCHARTSI,OnDrawChartSi)
	ON_MESSAGE(WM_NETCMD_DRAWCHARTSC,OnDrawChartSc)
	ON_MESSAGE(WM_NETCMD_CLEARCHARTSC,OnClearChartSc)
	ON_MESSAGE(WM_NETCMD_CHARTCLOSE,OnMyChartClose)
	ON_MESSAGE(WM_DI_UART_U,OnUartDataShow)
	ON_MESSAGE(WM_NETCMD_UARTDATA,OnUartDataClear)
	ON_MESSAGE(WM_TEMPERRECORD_U,OnTemperRecordShow)
	ON_MESSAGE(WM_TEMPERRECORD_U,OnDianJiRecordShow)
	
	
	
	
//	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_2DGRAPH, &GraphForm2d::OnTcnSelchangeTab2dgraph)
ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_2DGRAPH, &GraphForm2d::OnTcnSelchangeTab2dgraph)
ON_WM_CLOSE()
END_MESSAGE_MAP()



BEGIN_EASYSIZE_MAP(GraphForm2d)
    EASYSIZE(IDC_TAB_2DGRAPH,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	//EASYSIZE(IDD_SCANF2DPROPDLG,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)	
END_EASYSIZE_MAP

// GraphForm2d 诊断

#ifdef _DEBUG
void GraphForm2d::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void GraphForm2d::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// GraphForm2d 消息处理程序


//CPropertySheet propertySheet ;
void GraphForm2d::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	//INIT_EASYSIZE;
	m_TabGraphSet.InsertItem(0, _T("扫描波形"));
	m_TabGraphSet.InsertItem(1, _T("单点波形"));
	//m_TabGraphSet.InsertItem(3, _T("串口波形"));
	//m_TabGraphSet.InsertItem(1, _T("全年温度波形"));
	m_TabGraphSet.InsertItem(2, _T("电机转速测试"));
	m_TabGraphSet.InsertItem(3, _T("码盘偏心修正"));
	m_TabGraphSet.InsertItem(4, _T("模拟AGV测试"));

	m_scanf2ddlg.Create(IDD_SCANF2DDLG,GetDlgItem(IDC_TAB_2DGRAPH));
	m_single2ddlg.Create(IDD_SINGLE2DDLG,GetDlgItem(IDC_TAB_2DGRAPH));
	//m_UartDatadlg.Create(IDD_UARTDLG,GetDlgItem(IDC_TAB_2DGRAPH));
	//m_TemperRecordDlg.Create(IDD_TEMPER_DLG,GetDlgItem(IDC_TAB_2DGRAPH));
	m_SetRdDlg.Create(IDD_SET_RDDLG,GetDlgItem(IDC_TAB_2DGRAPH));
	m_AGVTestDlg.Create(IDD_AGVTESTDLG,GetDlgItem(IDC_TAB_2DGRAPH));
	m_SIMUAGVTESTDLG.Create(IDD_SIMUAGVDLG, GetDlgItem(IDC_TAB_2DGRAPH));

	((CAxis)((CAxes)m_single2ddlg.m_ChartSi.get_Axis()).get_Bottom()).put_Maximum(72000);
	((CAxis)((CAxes)m_single2ddlg.m_ChartSi.get_Axis()).get_Bottom()).put_Automatic(false);
	((CAxis)((CAxes)m_single2ddlg.m_ChartSi.get_Axis()).get_Left()).put_Automatic(true);
	((CSeries)m_single2ddlg.m_ChartSi.Series(0)).Clear();
	((CSeries)m_single2ddlg.m_ChartSi.Series(1)).Clear();
	((CSeries)m_single2ddlg.m_ChartSi.Series(2)).Clear();

	

	((CAxis)((CAxes)m_scanf2ddlg.m_ChartJi.get_Axis()).get_Bottom()).put_Minimum(0);
	((CAxis)((CAxes)m_scanf2ddlg.m_ChartJi.get_Axis()).get_Bottom()).put_Maximum(3600);

	((CAxis)((CAxes)m_scanf2ddlg.m_ChartJi.get_Axis()).get_Bottom()).put_Automatic(false);
    ((CAxis)((CAxes)m_scanf2ddlg.m_ChartJi.get_Axis()).get_Left()).put_Automatic(true);

	/*((CAxis)((CAxes)m_UartDatadlg.m_ChartUart.get_Axis()).get_Bottom()).put_Maximum(720);
	((CAxis)((CAxes)m_UartDatadlg.m_ChartUart.get_Axis()).get_Bottom()).put_Minimum(0);
	((CAxis)((CAxes)m_UartDatadlg.m_ChartUart.get_Axis()).get_Bottom()).put_Automatic(false);
	((CAxis)((CAxes)m_UartDatadlg.m_ChartUart.get_Axis()).get_Left()).put_Automatic(true);
	((CSeries)m_UartDatadlg.m_ChartUart.Series(0)).Clear();*/
   //((CAxis)m_single2ddlg.m_ChartSi.get_Axis()).put_Maximum(10000);
   //l_sSeries.Clear();
   //((CAxis)((CAxes)l_sSeries).get_Left()).put_Automatic(TRUE);
   //((CAxis)((CAxes)l_sSeries).get_Bottom()).put_Automatic(FALSE);
	CRect rs;
	m_TabGraphSet.GetClientRect(&rs);

	rs.top += 25;
    rs.bottom -= 4;
    rs.left += 1;
    rs.right -= 2;
	//
   // m_scanf2ddlg.ShowWindow(true);	
	m_scanf2ddlg.MoveWindow(&rs);
	m_scanf2ddlg.ShowWindow(true);	
	SendMessage(WM_SIZE);


	//propertySheet.AddPage(&m_Scanf2dProp); 

	//m_TabGraphSet.SetCurSel(0);
	//propertySheet.Create(this);
	//propertySheet.ShowWindow(true);
	// TODO: 在此添加专用代码和/或调用基类
}


void GraphForm2d::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy); 

	SetScrollSizes(MM_TEXT,CSize(0,0));
	 if (IsWindowVisible()&&GetWindow(GW_CHILD)!=NULL)
	 {
		 if (m_TabGraphSet.GetSafeHwnd() == NULL)
		 {
			 return;
		 }
             SetScrollSizes(MM_TEXT,CSize(0,0));
             UPDATE_EASYSIZE;
			 //重绘m_TabGraphSet中子控件
			 CRect rs;
			 m_TabGraphSet.GetClientRect(&rs);
			 rs.top += 25;
			 rs.bottom -= 5;
			 rs.left += 1;
			 rs.right -= 2;
			 //
			int CurSel = m_TabGraphSet.GetCurSel();
			switch(CurSel)
			{
				case 0:
					m_scanf2ddlg.MoveWindow(&rs);
					m_scanf2ddlg.ShowWindow(true);
					m_SetRdDlg.ShowWindow(false);
					m_single2ddlg.ShowWindow(false);
					m_AGVTestDlg.ShowWindow(false);
					m_SIMUAGVTESTDLG.ShowWindow(false);
					break;	
				case 1:
					m_single2ddlg.MoveWindow(&rs);
					m_single2ddlg.ShowWindow(true);
					m_scanf2ddlg.ShowWindow(false);
					m_SetRdDlg.ShowWindow(false);
					m_AGVTestDlg.ShowWindow(false);
					m_SIMUAGVTESTDLG.ShowWindow(false);
					break;
				case 2:
					m_SetRdDlg.MoveWindow(&rs);
					m_SetRdDlg.ShowWindow(true);
					m_scanf2ddlg.ShowWindow(false);
					m_single2ddlg.ShowWindow(false);
					m_AGVTestDlg.ShowWindow(false);
					m_SIMUAGVTESTDLG.ShowWindow(false);
					break;		
				case 3:
					m_AGVTestDlg.MoveWindow(&rs);
					m_AGVTestDlg.ShowWindow(true);
					m_SetRdDlg.ShowWindow(false);
					m_scanf2ddlg.ShowWindow(false);
					m_single2ddlg.ShowWindow(false);
					m_SIMUAGVTESTDLG.ShowWindow(false);
				case 4:
					m_SIMUAGVTESTDLG.MoveWindow(&rs);
					m_SIMUAGVTESTDLG.ShowWindow(true);
					m_SetRdDlg.ShowWindow(false);
					m_scanf2ddlg.ShowWindow(false);
					m_single2ddlg.ShowWindow(false);
					m_AGVTestDlg.ShowWindow(false);
					break;	
			}
      }
	
	 // OnSize(nType, cx, cy); 
	// TODO: 在此处添加消息处理程序代码
}


void GraphForm2d::OnSizing(UINT fwSide, LPRECT pRect)
{
	CFormView::OnSizing(fwSide, pRect);
	EASYSIZE_MINSIZE(280,250,fwSide,pRect);
	// TODO: 在此处添加消息处理程序代码
}


//void GraphForm2d::OnTcnSelchangeTab2dgraph(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	// TODO: 在此添加控件通知处理程序代码
//	*pResult = 0;
//}


BOOL GraphForm2d::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此添加专用代码和/或调用基类

	return CFormView::PreCreateWindow(cs);
}


void GraphForm2d::OnTcnSelchangeTab2dgraph(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	CRect rs;
	m_TabGraphSet.GetClientRect(&rs);
	rs.top += 25;
	rs.bottom -= 5;
	rs.left += 1;
	rs.right -= 2;
	int CurSel = m_TabGraphSet.GetCurSel();
	switch(CurSel)
	{
	case 0:
		m_scanf2ddlg.MoveWindow(&rs);
		m_scanf2ddlg.ShowWindow(true);
		m_single2ddlg.ShowWindow(false);
		m_SetRdDlg.ShowWindow(false);
		m_AGVTestDlg.ShowWindow(false);
		m_SIMUAGVTESTDLG.ShowWindow(false);
		break;
	case 1:
		if(SystemLoader != 2&&SystemLoader != 3)
		{
			AfxMessageBox(_T("请以研发人员身份登陆！"));
			return;
		}
		m_single2ddlg.MoveWindow(&rs);
		m_single2ddlg.ShowWindow(true);
		m_scanf2ddlg.ShowWindow(false);
		m_SetRdDlg.ShowWindow(false);
		m_AGVTestDlg.ShowWindow(false);
		m_SIMUAGVTESTDLG.ShowWindow(false);
		break;
	case 2:
		m_SetRdDlg.MoveWindow(&rs);
		m_SetRdDlg.ShowWindow(true);
		m_single2ddlg.ShowWindow(false);
		m_scanf2ddlg.ShowWindow(false);
		m_AGVTestDlg.ShowWindow(false);
		m_SIMUAGVTESTDLG.ShowWindow(false);
		break;	
	case 3:
		m_AGVTestDlg.MoveWindow(&rs);
		m_AGVTestDlg.ShowWindow(true);
		m_SetRdDlg.ShowWindow(false);
		m_scanf2ddlg.ShowWindow(false);
		m_single2ddlg.ShowWindow(false);
		m_SIMUAGVTESTDLG.ShowWindow(false);
		break;
	case 4:
		m_SIMUAGVTESTDLG.MoveWindow(&rs);
		m_SIMUAGVTESTDLG.ShowWindow(true);
		m_SetRdDlg.ShowWindow(false);
		m_scanf2ddlg.ShowWindow(false);
		m_single2ddlg.ShowWindow(false);
		m_AGVTestDlg.ShowWindow(false);
		break;
	}
}

LRESULT GraphForm2d::OnClearChart(WPARAM wParam,LPARAM lParam)
{
	int nChannel = (int)lParam;

	if(nChannel < 1 || nChannel > 3)
		return 0;
	for(int i =1;i<=3;i++)
	{
		CSeries l_sSeries = (CSeries)m_single2ddlg.m_ChartSi.Series(nChannel-1);
		l_sSeries.Clear();
	}
    ((CAxis)((CAxes)m_single2ddlg.m_ChartSi.get_Axis()).get_Bottom()).put_Maximum(72000);
    ((CAxis)((CAxes)m_single2ddlg.m_ChartSi.get_Axis()).get_Bottom()).put_Automatic(false);
    ((CAxis)((CAxes)m_single2ddlg.m_ChartSi.get_Axis()).get_Left()).put_Automatic(true);


	
	return 1;
}

int aa1 = 0;
int bb1 = 0;
int cc1 = 0;
int dd1 = 0;
int test1=0xffff;
CString GetCurTick(CString text)
{
	static long pret1;
	CString str1;
	long t1=GetTickCount();//程序段开始前取得系统运行时间(ms)
	
	CString text1 = text;
	CString	str;
	str.Format("%d\n",t1);//前后之差即 程序运行时间
	text1 += str;
    str1.Format("%d\n",t1-pret1);
	FILE *fp;
	if((fp=fopen("d:\\tick.txt","at+"))!=NULL)
	{
			fwrite(text1,strlen(text1),1,fp);
			text1 = "耗时：";
			text1 +=str1;
			fwrite(text1,strlen(text1),1,fp);
			if(text == "结束绘图tick：")
			{
				text1 = "----------------------\n";
				fwrite(text1,strlen(text1),1,fp);
			}
			fclose(fp);
	}
	pret1 = t1;
	return str;
}
LRESULT GraphForm2d::OnDrawChartSi(WPARAM wParam,LPARAM lParam)
{
	NetDataStruct *l_psNetData;
	l_psNetData =  (NetDataStruct *)wParam;
	CSeries l_Chart =  ((CSeries)m_single2ddlg.m_ChartSi.Series(l_psNetData->m_n32Channel-1));
	 //((CSeries)m_single2ddlg.m_ChartSi.Series()).Add(l_n32DataSi,_T(""),0);
	int l_n32i,l_n32j;
	int l_n32DataNum;
	int l_n32DataSi;
	u8 l_u8flag = 0;
	dd1++;
	GetCurTick("开始");

	l_n32DataNum = (l_psNetData->m_pcData[26]*256)+(BYTE)(l_psNetData->m_pcData[27]);
	if(l_psNetData->m_n32BufCnt != 720)

	//GetCurTick("开始计算");
	l_n32i = 28;
	int l_nNum = 0;
	for(l_n32j = (l_psNetData->m_n32BufCnt - 1)*l_n32DataNum;l_n32j<l_psNetData->m_n32BufCnt *l_n32DataNum;l_n32j++)
	{
		l_n32DataSi = ((l_psNetData->m_pcData[l_n32i+3]&0xff) << 24)
			         +((l_psNetData->m_pcData[l_n32i+2]&0xff) << 16)
					 +((l_psNetData->m_pcData[l_n32i+1]&0xff) << 8)
					 +((l_psNetData->m_pcData[l_n32i+0]&0xff) << 0);
		l_n32i += 4;
		
		if(l_psNetData->m_n32Channel<1 || l_psNetData->m_n32Channel > 3)
			return 0;
		if(test1 == l_n32DataSi)
		{
			//break;
		}
		test1 = l_n32DataSi;

		l_Chart.Add(l_n32DataSi,_T(""),0);
		//if(l_u8flag == 1)
		//{
		//	fseek(fp,0,SEEK_END);
		//	CString str;
		//	str.Format("%d ",l_n32DataSi);
		//	fwrite(str,strlen(str),1,fp);            
		//	fclose(g_pfLogfile);
		//}
	   //((CSeries)m_single2ddlg.m_ChartSi.Series(l_psNetData->m_n32Channel-1)).Add(l_n32DataSi,_T(""),0);
		//((CSeries)m_single2ddlg.m_ChartSi.Series(0)).Add(0,_T(""),0);
		//((CSeries)m_single2ddlg.m_ChartSi.Series(0)).Clear();
	}
	//GetCurTick("结束");
	//((CSeries)m_single2ddlg.m_ChartSi.Series(0)).Clear();
	return 1;
}

unsigned int nTime1 = 0;
unsigned int nTime2 = 0;
unsigned char firstData = 0;
LRESULT GraphForm2d::OnClearChartSc(WPARAM wParam,LPARAM lParam)
{
	int nSeriesNumJi, nSeriesNumZhi;
	//nSeriesNumZhi = m_scanf2ddlg.m_ChartZhi.get_SeriesCount();     //清空极坐标图像
	//for (int i=0; i<nSeriesNumZhi; i++)
	//	 ((CSeries)m_scanf2ddlg.m_ChartZhi.Series(i)).Clear();
	
	nSeriesNumJi = m_scanf2ddlg.m_ChartJi.get_SeriesCount();   //清空直角坐标图像
	for (int i=0; i<nSeriesNumJi; i++)
		((CSeries)m_scanf2ddlg.m_ChartJi.Series(i)).Clear();

	// ((CAxis)((CAxes)m_scanf2ddlg.m_ChartZhi.get_Axis()).get_Bottom()).put_Maximum(360);
	/*if(g_u8TcpSendClkMode == 0)
	{
		if(((CAxis)((CAxes)m_scanf2ddlg.m_ChartJi.get_Axis()).get_Bottom()).get_Maximum() != 3600)
			((CAxis)((CAxes)m_scanf2ddlg.m_ChartJi.get_Axis()).get_Bottom()).put_Maximum(3600);
	}
	else
	{
		if(((CAxis)((CAxes)m_scanf2ddlg.m_ChartJi.get_Axis()).get_Bottom()).get_Maximum() != 7200)
			((CAxis)((CAxes)m_scanf2ddlg.m_ChartJi.get_Axis()).get_Bottom()).put_Maximum(7200);
	}*/
    //((CAxis)((CAxes)m_scanf2ddlg.m_ChartZhi.get_Axis()).get_Bottom()).put_Automatic(false);
    //((CAxis)((CAxes)m_scanf2ddlg.m_ChartZhi.get_Axis()).get_Left()).put_Automatic(true);

    ((CAxis)((CAxes)m_scanf2ddlg.m_ChartJi.get_Axis()).get_Bottom()).put_Automatic(false);
    ((CAxis)((CAxes)m_scanf2ddlg.m_ChartJi.get_Axis()).get_Left()).put_Automatic(true);

	 ((CAxes)m_scanf2ddlg.m_ChartJi.get_Axis()).put_FastCalc(true);
	 //((CAxes)m_scanf2ddlg.m_ChartZhi.get_Axis()).put_FastCalc(true);
	return 1;
}

int getData(int buf[],int len,int *AVG,int *MAX, int *MIN)
{
	int i;
	int sum = 0;
	if(len<=0)
		return -1;
	*AVG = buf[0];
	*MAX = buf[0];
	*MIN = buf[0];
	sum = buf[0];
	for(i=1;i<len;i++)
	{
		if(buf[i]>(*MAX))
		{
			*MAX = buf[i];
		}
		
		if(buf[i]<(*MIN))
		{
			*MIN = buf[i];
		}
		sum += buf[i];
		
	}

	*AVG = sum/len;
	return 0;
}

u8 g_u8DrawScFlag = 0xff;
extern int g_nSaveNum;          //保存包数
LRESULT GraphForm2d::OnDrawChartSc(WPARAM wParam,LPARAM lParam)
{
	g_u8DrawScFlag = 1;
	int nDataJi, nDataZhiX, nDataZhiY;
	int nPreData = 0;
	int nAngle = m_scanf2ddlg.m_sScan2dCfgDlg.m_nZeroX;    //直角坐标系零点横坐标
	g_nAngle = nAngle;
	int nAVG,nMAX,nMIN,nErrorCount = 0;

	CString l_strLog_Time,str;
	SYSTEMTIME lTime;
	GraphForm2d * pView	;

	//GraphForm3d *pView3d;
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pView = (GraphForm2d*)pMainFrame->m_wndLeftSplitter.GetPane(0,0);
	NetDataStruct *l_psNetData;
	l_psNetData =  (NetDataStruct *)wParam;

	//对波形进行判断
	bool NothingFault=false;
	if (g_bAutoMoniterRunning == true)
	{
		int total = l_psNetData->m_n32Len/600;
		for (int i=0;i<total;i++)
		{
			for(int j=0;i<20;j++)
			{
				nDataJi = ((l_psNetData->m_pcData[j+i*600]&0xFF)<<8) + (BYTE)l_psNetData->m_pcData[j+i*600+1];
				if (nDataJi != 4)
				{
					NothingFault=true;
					break;
				}
			}
			if (NothingFault == true)
			{
				break;
			}
		}
	}
	if (NothingFault == false)
	{
		g_bMoniterFaultedWave = true;
		g_nFaultWaveCount++;
	}
	else
	{
		if (g_bMoniterFaultedWave == true)
		{
			g_bMoniterFaultedWave = false;
		}
	}

	if( l_psNetData->m_n32Len == 3600)  //l_psNetData->m_n32Len == 831 || l_psNetData->m_n32Len ==835 ||
	{
		if (ScanPointCountMode != 0)
		{
			((CAxis)((CAxes)m_scanf2ddlg.m_ChartJi.get_Axis()).get_Bottom()).put_Maximum(3600);
		}
		ScanPointCountMode = 0;
		g_u8CurClkmode = 0;
	}
	else if( l_psNetData->m_n32Len == 7200)    //l_psNetData->m_n32Len == 1661 ||
	{
		if (ScanPointCountMode != 1)
		{
			((CAxis)((CAxes)m_scanf2ddlg.m_ChartJi.get_Axis()).get_Bottom()).put_Maximum(7200);
		}
		ScanPointCountMode = 1;
		g_u8CurClkmode = 1;
	}

	//((CSeries)m_single2ddlg.m_ChartSi.Series()).Add(l_n32DataSi,_T(""),0);
	int l_n32i,l_n32j;
	int l_n32DataNum;
	int l_n32DataSi;
	dd1++;
	if(l_psNetData->m_n32Channel == 0x01)
	{
		aa1++;
	}
	if(l_psNetData->m_n32Channel == 0x02)
	{
		bb1++;
	}
	if(l_psNetData->m_n32Channel == 0x03)
	{
		cc1++;
		if(aa1 != bb1)
		{
			l_psNetData->m_n32Channel = 3;
		}
	}

	g_nDrawNum_scanf++;        //绘制包数
	m_scanf2ddlg.m_sScan2dCfgDlg.m_nDrawNum = g_nDrawNum_scanf;

	l_n32i = 0;//83;	
	//l_n32DataNum = (l_psNetData->m_pcData[l_n32i]*256)+(BYTE)(l_psNetData->m_pcData[l_n32i+1]);
	l_n32DataNum =l_psNetData->m_n32Len;
	//l_n32i += 2;

	//添加删除波形，用于屏显数量的变化
	if (maxline < m_scanf2ddlg.m_sScan2dCfgDlg.m_nOneChartNum && (m_scanf2ddlg.m_sScan2dCfgDlg.m_nOneChartNum <= 50))
	{
		for (int i=0;i<(m_scanf2ddlg.m_sScan2dCfgDlg.m_nOneChartNum-maxline);i++)
		{
			m_scanf2ddlg.m_ChartJi.AddSeries(0);
			m_scanf2ddlg.m_ChartZhi.AddSeries(4);
			g_asChartJi[g_nCount_scanf] = ((CSeries)m_scanf2ddlg.m_ChartJi.Series(g_nCount_scanf));
			g_asChartZhi[g_nCount_scanf] = ((CSeries)m_scanf2ddlg.m_ChartZhi.Series(g_nCount_scanf));
			g_asChartJi[g_nCount_scanf].put_Color(RGB(255,0,0));
			CPointSeries point = (CPointSeries)(g_asChartZhi[g_nCount_scanf].get_asPoint());
			((CPointer)point.get_Pointer()).put_Style(6);
			((CPen0)(((CPointer)point.get_Pointer()).get_Pen())).put_Color(RGB(255,0,0));
		}
	}
	else if(maxline > m_scanf2ddlg.m_sScan2dCfgDlg.m_nOneChartNum)
	{
		int SeriesJiCount = m_scanf2ddlg.m_ChartJi.get_SeriesCount();
		for (int i=0;i<(maxline - m_scanf2ddlg.m_sScan2dCfgDlg.m_nOneChartNum);i++)
		{
			SeriesJiCount--;
			m_scanf2ddlg.m_ChartJi.RemoveSeries(SeriesJiCount);
		}
	}

	maxline = ( m_scanf2ddlg.m_sScan2dCfgDlg.m_nOneChartNum > 50) ? 50 :  m_scanf2ddlg.m_sScan2dCfgDlg.m_nOneChartNum ;
	for (l_n32j=0;l_n32j<l_n32DataNum;l_n32j++)
	{
		nDataJi = 0;
		nDataJi = ((l_psNetData->m_pcData[l_n32i]&0xFF)<<8) + (BYTE)l_psNetData->m_pcData[l_n32i+1];
		l_n32i += 2;

		if (nDataJi<0)
		{
			nDataJi = 0;
		}

		if (m_scanf2ddlg.m_sScan2dCfgDlg.m_dealnegative) //修正
		{
			if(nDataJi>=0 && nDataJi<=10)
			{
				nDataJi = nPreData;
			}
		}
		if(nDataJi > m_scanf2ddlg.m_sScan2dCfgDlg.m_nMaxDistance)
		{
			nDataJi = m_scanf2ddlg.m_sScan2dCfgDlg.m_nMaxDistance;
		}

		if(l_psNetData->m_n32Len == 831|| l_psNetData->m_n32Len ==835)
		{;
			nDataZhiX = (int)(nDataJi*sin((((double)(l_n32j - nAngle))/c_n32MaxAngle)*3.1416));
			nDataZhiY = (int)(nDataJi*cos((((double)(l_n32j - nAngle))/c_n32MaxAngle)*3.1416));
		}
		else if(l_psNetData->m_n32Len == 1661)
		{
			/*if(l_n32j == 360)
			nDataZhiX = nDataJi;*/
			nDataZhiX = (int)(nDataJi*sin((((double)(l_n32j - nAngle))/c_n32MaxAngle/2)*3.1416));
			nDataZhiY = (int)(nDataJi*cos((((double)(l_n32j - nAngle))/c_n32MaxAngle/2)*3.1416));
		}
		else
		{
			nDataZhiX=0;
			nDataZhiY=0;
		}

		nDataZhiY = abs(nDataZhiY);

		YValuesTmp[l_n32j] = nDataZhiY;
		XValuesTmp[l_n32j] = nDataZhiX;
		Y1ValuesTmp[l_n32j] = nDataJi;
		X1ValuesTmp[l_n32j] = l_n32j;

		//	nDataZhiX = abs(nDataZhiX);
		//	m_ChartJi.Series(0).Add((double)nDataJi,"",0);

		//	m_ChartJi.Series(g_nseriesnum%maxline).Add((double)nDataJi,"",RGB(255,0,0));
		//((CSeries)m_scanf2ddlg.m_ChartZhi.Series(0)).AddXY(nDataZhiX, nDataZhiY, NULL, 0); 

		if (m_scanf2ddlg.m_sScan2dCfgDlg.m_mesurectl.GetCheck()) 
		{
			if(l_n32j == m_scanf2ddlg.m_sScan2dCfgDlg.m_nangle_num)
			{
				cBufAVG[nAVGPos++] = nDataJi;
			}

			if((nAVGPos != 0)&&( nAVGPos % m_scanf2ddlg.m_sScan2dCfgDlg.m_mesureinterval == 0) )
			{
				int ret = getData(cBufAVG,nAVGPos,&nAVG,&nMAX,&nMIN);
				for(int i = 0;i<nAVGPos;i++)
				{
					if((cBufAVG[i] < (m_scanf2ddlg.m_sScan2dCfgDlg.m_editValRef-m_scanf2ddlg.m_sScan2dCfgDlg.m_editDispersion)) || (cBufAVG[i] > (m_scanf2ddlg.m_sScan2dCfgDlg.m_editValRef+m_scanf2ddlg.m_sScan2dCfgDlg.m_editDispersion)))
					{
						nErrorCount ++;
					}
				}

				//m_scanf2ddlg.m_sScan2dCfgDlg.m_ediProbability = (nAVGPos - nErrorCount)*1000/nAVGPos;
				m_scanf2ddlg.m_sScan2dCfgDlg.m_ediProbability = (nAVGPos-nErrorCount)*1000/nAVGPos;
				m_scanf2ddlg.m_sScan2dCfgDlg.m_ediProbability = m_scanf2ddlg.m_sScan2dCfgDlg.m_ediProbability/10;
				if(ret == 0)
				{
					m_scanf2ddlg.m_sScan2dCfgDlg.m_nMaxDis = nMAX;
					m_scanf2ddlg.m_sScan2dCfgDlg.m_nMinDis = nMIN;
					m_scanf2ddlg.m_sScan2dCfgDlg.m_nAVGDis = nAVG;
					m_scanf2ddlg.m_sScan2dCfgDlg.m_editValRef = m_scanf2ddlg.m_sScan2dCfgDlg.m_nAVGDis;
					m_scanf2ddlg.m_sScan2dCfgDlg.UpdateData(false);
					nAVGPos = 0;
					nErrorCount = 0;
				}
			}
		}
		nPreData = nDataJi;	

		//		
	}
	DrawLine((LPVOID)(&g_asChartJi[g_nCount_scanf%maxline]),X1ValuesTmp,Y1ValuesTmp,l_n32DataNum);

	if (pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_bSaveFlag == TRUE)
	{
		g_nSaveNum++;
		pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nSaveNum = g_nSaveNum;
		pView->m_scanf2ddlg.m_sScan2dCfgDlg.UpdateData(false);
		SYSTEMTIME sys;
		GetLocalTime(&sys);                           //得到系统时间：年，月，日，小时，分，秒，毫秒
		pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_strNetFileName.Format("%d_%.2d_%.2d_%.2d_%.2d_%.2d_%.3d",sys.wYear,sys.wMonth,sys.wDay,
			sys.wHour,sys.wMinute,sys.wSecond,sys.wMilliseconds);
		pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_strNetFileName = pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_strFiles + pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_strNetFileName + ".wj";
		if(!pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_NetDataFile.Open(pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_strNetFileName, CFile::modeCreate | CFile::modeWrite))
			return 0;
		char tmp[15000];
		memcpy(tmp,l_psNetData->m_pcData,l_psNetData->m_n32Len*2);
		//printf("tmp[0] = %d\n",tmp[0]);
		pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_NetDataFile.Write(tmp,l_psNetData->m_n32Len*2);
		pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_NetDataFile.Close();
	}

	//	nTime1 = nTime2;//t1.wMilliseconds;
	////	GetLocalTime(&t1); 
	//	nTime2 =l_psNetData->m_pcData[51];//t1.wMilliseconds;
	//	unsigned char tmp = nTime2-nTime1;
	//	//m_scanf2ddlg.m_sScan2dCfgDlg.UpdateData(true);
	//	if(tmp != m_scanf2ddlg.m_sScan2dCfgDlg.m_nInterval && firstData == 1)
	//	{
	//		m_scanf2ddlg.m_sScan2dCfgDlg.m_n32LostDataNum++;
	//	}
	//	firstData = 1;

	m_scanf2ddlg.m_sScan2dCfgDlg.UpdateData(false);
	//int ret11 = s_asChartZhi.GetAxis().GetBottom().GetMaximum();
	//g_nseriesnum++;
	return 1;
}

void GraphForm2d::DrawLine(LPVOID pAddr,double *pX,double *pY,long nNum)
{
	//{
	COleSafeArray XValues;
	COleSafeArray YValues;
	CSeries *Chart = (CSeries *)pAddr;
	long i(0);

	DWORD wLength = nNum;
	XValues.Create(VT_R8,1,&wLength);
	YValues.Create(VT_R8,1,&wLength);

	for (i = 0; i< nNum; i++)
	{
		XValues.PutElement(&i,pX+i);
		YValues.PutElement(&i,pY+i);

	}
//	Chart->Clear();
	Chart->AddArray(nNum,YValues,XValues);
}
//void GraphForm2d::DrawLine(CSeries *Chart,double *pX,double *pY,long nNum)
//{
//	COleSafeArray XValues;
//	COleSafeArray YValues;
//
//	long i(0);
//
//	DWORD wLength = nNum;
//	XValues.Create(VT_R8,1,&wLength);
//	YValues.Create(VT_R8,1,&wLength);
//
//	for (i = 0; i< nNum; i++)
//	{
//		XValues.PutElement(&i,pX+i);
//		YValues.PutElement(&i,pY+i);
//
//	}
//	//Chart->Clear();
//	Chart->AddArray(nNum,YValues,XValues);
//} 

void GraphForm2d::OnClose()
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值

	CFormView::OnClose();
}

LRESULT GraphForm2d::OnMyChartClose(WPARAM wParam,LPARAM lParam)
{
	for(int i = 0;i<50;i++)
	{
		g_asChartJi[i].ReleaseDispatch();
		g_asChartZhi[i].ReleaseDispatch();
	}
	return 1;
}

LRESULT GraphForm2d::OnUartDataShow(WPARAM wParam,LPARAM lParam)
{
	NetDataStruct *l_psNetData;
	l_psNetData =  (NetDataStruct *)wParam;
	//CSeries l_Chart =  ((CSeries)m_single2ddlg.m_ChartSi.Series(l_psNetData->m_n32Channel-1));
	 //((CSeries)m_single2ddlg.m_ChartSi.Series()).Add(l_n32DataSi,_T(""),0);
	int l_n32i,l_n32j;
	int l_n32DataNum;
	int l_n32DataSi;
	static u32 s_u32cnt;
	s_u32cnt++;

	char *pcBuf = (char *)wParam;
	int nBufSize = (int)lParam;

	u16 l_u16len;//数据部分长度，除去前8个字节
	l_u16len = nBufSize - 8;
	u8 *pu8buf = new u8[nBufSize];
	u8 l_u8Bcc_Value;

	memcpy(pu8buf,&pcBuf[8],l_u16len);//去掉前8个字节的头
	l_u8Bcc_Value	= *(pu8buf+l_u16len-1);
	if (l_u8Bcc_Value != BCC_CHECK(pu8buf,l_u16len))
	{
		//m_sLog = "接收 应用界面状态失败(校验错误！):";
		//WriteLog(&m_recOperLog,m_sLog,(unsigned char *)pcBuf,nBufSize);	
		return 0;
	}

	u8 *pu8Buftmp = (u8*)(&pu8buf[16]);
	if(s_u32cnt >= 720)
	{
		s_u32cnt= 0;
		((CSeries)m_UartDatadlg.m_ChartUart.Series(0)).Clear();
		((CSeries)m_UartDatadlg.m_ChartUart.Series(1)).Clear();
		((CSeries)m_UartDatadlg.m_ChartUart.Series(2)).Clear();
		((CSeries)m_UartDatadlg.m_ChartUart.Series(3)).Clear();
		((CSeries)m_UartDatadlg.m_ChartUart.Series(4)).Clear();

		((CAxis)((CAxes)m_UartDatadlg.m_ChartUart.get_Axis()).get_Bottom()).put_Maximum(720);
		((CAxis)((CAxes)m_UartDatadlg.m_ChartUart.get_Axis()).get_Bottom()).put_Minimum(0);
		((CAxis)((CAxes)m_UartDatadlg.m_ChartUart.get_Axis()).get_Bottom()).put_Automatic(false);
		((CAxis)((CAxes)m_UartDatadlg.m_ChartUart.get_Axis()).get_Left()).put_Automatic(true);
	}
	CSeries  l_Chart;
	l_n32DataNum = (pu8Buftmp[0]*256)+(BYTE)(pu8Buftmp[1]);
	l_Chart =  ((CSeries)m_UartDatadlg.m_ChartUart.Series(0));
	m_UartDatadlg.m_sUartCfgDlg.m_fStart = l_n32DataNum;
	l_Chart.Add(l_n32DataNum,_T(""),0);

	l_n32DataNum = (pu8Buftmp[2]*256*256)+(pu8Buftmp[3]*256)+(BYTE)(pu8Buftmp[4]);
	l_Chart =  ((CSeries)m_UartDatadlg.m_ChartUart.Series(1));
	m_UartDatadlg.m_sUartCfgDlg.m_fDATAHUp = l_n32DataNum;  
	l_Chart.Add(l_n32DataNum,_T(""),0);

	l_n32DataNum = (pu8Buftmp[11]*256*256)+(pu8Buftmp[12]*256)+(BYTE)(pu8Buftmp[13]);
	l_Chart =  ((CSeries)m_UartDatadlg.m_ChartUart.Series(2));
	m_UartDatadlg.m_sUartCfgDlg.m_fDataHDn = l_n32DataNum;
	l_Chart.Add(l_n32DataNum,_T(""),0);

	l_n32DataNum = (pu8Buftmp[8]*256*256)+(pu8Buftmp[9]*256)+(BYTE)(pu8Buftmp[10]);
	l_Chart =  ((CSeries)m_UartDatadlg.m_ChartUart.Series(3));
	m_UartDatadlg.m_sUartCfgDlg.m_fDataLUp= l_n32DataNum;
	l_Chart.Add(l_n32DataNum,_T(""),0);

	l_n32DataNum = (pu8Buftmp[5]*256*256)+(pu8Buftmp[6]*256)+(BYTE)(pu8Buftmp[7]);
	l_Chart =  ((CSeries)m_UartDatadlg.m_ChartUart.Series(4));
	m_UartDatadlg.m_sUartCfgDlg.m_fDataLDn = l_n32DataNum;
	l_Chart.Add(l_n32DataNum,_T(""),0);

	m_UartDatadlg.m_sUartCfgDlg.m_fSetH = ((pu8Buftmp[14]*256)+(BYTE)(pu8Buftmp[15]))/1000.0;
	m_UartDatadlg.m_sUartCfgDlg.m_fSetL = ((pu8Buftmp[16]*256)+(BYTE)(pu8Buftmp[17]))/1000.0;
	m_UartDatadlg.m_sUartCfgDlg.m_fAPDHv = ((pu8Buftmp[18]*256)+(BYTE)(pu8Buftmp[19]))/100.0;
	m_UartDatadlg.m_sUartCfgDlg.m_fLaserHv = ((pu8Buftmp[20]*256)+(BYTE)(pu8Buftmp[21]))/100.0;

	//m_UartDatadlg.m_sUartCfgDlg.m_fHumm = (pu8Buftmp[22]*256)+(BYTE)(pu8Buftmp[23]);
	m_UartDatadlg.m_sUartCfgDlg.m_fScanfSta = ((pu8Buftmp[24]*256)+(BYTE)(pu8Buftmp[25]))/4;
	m_UartDatadlg.m_sUartCfgDlg.m_fScanfEnd = ((pu8Buftmp[26]*256)+(BYTE)(pu8Buftmp[27]))/4;
	m_UartDatadlg.m_sUartCfgDlg.m_fMotorDa = (pu8Buftmp[28]*256)+(BYTE)(pu8Buftmp[29]);

	m_UartDatadlg.m_sUartCfgDlg.m_fPID_P = ((pu8Buftmp[30]*256)+(BYTE)(pu8Buftmp[31]))/1024.0;
	m_UartDatadlg.m_sUartCfgDlg.m_fPID_I = ((pu8Buftmp[32]*256)+(BYTE)(pu8Buftmp[33]));
	m_UartDatadlg.m_sUartCfgDlg.m_fPID_D = ((pu8Buftmp[34]*256)+(BYTE)(pu8Buftmp[35]));
	m_UartDatadlg.m_sUartCfgDlg.m_fZeroPluseNum = (pu8Buftmp[36]*256)+(BYTE)(pu8Buftmp[37]);

	short int tmp = (short int)((pu8Buftmp[38]*256)+(BYTE)(pu8Buftmp[39]));

	m_UartDatadlg.m_sUartCfgDlg.m_fUartMotorTemper = tmp/100.0;

	tmp = (short int)((pu8Buftmp[40]*256)+(BYTE)(pu8Buftmp[41]));
	m_UartDatadlg.m_sUartCfgDlg.m_fUartApdTemper = tmp/100.0;

	UpdateData(false);
	m_UartDatadlg.m_sUartCfgDlg.UpdateData(false);



	//((CSeries)m_single2ddlg.m_ChartSi.Series(0)).Clear();
	return 1;
}


LRESULT GraphForm2d::OnUartDataClear(WPARAM wParam,LPARAM lParam)
{
		//m_UartDatadlg.UpdateData(true);
		((CSeries)m_UartDatadlg.m_ChartUart.Series(0)).Clear();
		((CSeries)m_UartDatadlg.m_ChartUart.Series(1)).Clear();
		((CSeries)m_UartDatadlg.m_ChartUart.Series(2)).Clear();
		((CSeries)m_UartDatadlg.m_ChartUart.Series(3)).Clear();
		((CSeries)m_UartDatadlg.m_ChartUart.Series(4)).Clear();

		((CAxis)((CAxes)m_UartDatadlg.m_ChartUart.get_Axis()).get_Bottom()).put_Maximum(720);
	((CAxis)((CAxes)m_UartDatadlg.m_ChartUart.get_Axis()).get_Bottom()).put_Minimum(0);
	((CAxis)((CAxes)m_UartDatadlg.m_ChartUart.get_Axis()).get_Bottom()).put_Automatic(false);
	((CAxis)((CAxes)m_UartDatadlg.m_ChartUart.get_Axis()).get_Left()).put_Automatic(true);

	//m_UartDatadlg.m_sUartCfgDlg.m_fSetH = 0;
	//m_UartDatadlg.m_sUartCfgDlg.m_fSetL = 0;
	//m_UartDatadlg.m_sUartCfgDlg.m_fAPDHv = 0;
	//m_UartDatadlg.m_sUartCfgDlg.m_fLaserHv = 0;

	//m_UartDatadlg.m_sUartCfgDlg.m_fHumm = 0;
	//m_UartDatadlg.m_sUartCfgDlg.m_fScanfSta = 0;
	//m_UartDatadlg.m_sUartCfgDlg.m_fScanfEnd = 0;
	//m_UartDatadlg.m_sUartCfgDlg.m_fMotorDa = 0;

	//m_UartDatadlg.m_sUartCfgDlg.m_fPID_P = 0;
	//m_UartDatadlg.m_sUartCfgDlg.m_fPID_I =0;
	//m_UartDatadlg.m_sUartCfgDlg.m_fPID_D = 0;
	//m_UartDatadlg.m_sUartCfgDlg.m_fZeroPluseNum =0;

	//m_UartDatadlg.UpdateData(false);
	//UpdateData(false);
	return 1;
}
LRESULT GraphForm2d::OnDianJiRecordShow(WPARAM wParam,LPARAM lParam){
	return 0;
}
LRESULT GraphForm2d::OnTemperRecordShow(WPARAM wParam,LPARAM lParam)
{
	NetDataStruct *l_psNetData;
	l_psNetData =  (NetDataStruct *)wParam;
	//CSeries l_Chart =  ((CSeries)m_single2ddlg.m_ChartSi.Series(l_psNetData->m_n32Channel-1));
	 //((CSeries)m_single2ddlg.m_ChartSi.Series()).Add(l_n32DataSi,_T(""),0);
	int l_n32i,l_n32j;
	int l_n32DataNum;
	int l_n32DataSi;
	static u32 s_u32cnt;
	s_u32cnt++;

	char *pcBuf = (char *)wParam;
	int nBufSize = (int)lParam;

	u16 l_u16len;//数据部分长度，除去前8个字节
	l_u16len = nBufSize - 8;
	u8 *pu8buf = new u8[nBufSize];
	u8 l_u8Bcc_Value;
	u16 *pu16Buf = (u16*)(&pu8buf[16]);
	memcpy(pu8buf,&pcBuf[8],l_u16len);//去掉前8个字节的头
	l_u8Bcc_Value	= *(pu8buf+l_u16len-1);
	if (l_u8Bcc_Value != BCC_CHECK(pu8buf,l_u16len))
	{
		//m_sLog = "接收 应用界面状态失败(校验错误！):";
		//WriteLog(&m_recOperLog,m_sLog,(unsigned char *)pcBuf,nBufSize);	
		return 0;
	}
	((CSeries)m_TemperRecordDlg.m_ChartTemper.Series(0)).Clear(); 

	((CAxis)((CAxes)m_TemperRecordDlg.m_ChartTemper.get_Axis()).get_Bottom()).put_Maximum(1460);
	((CAxis)((CAxes)m_TemperRecordDlg.m_ChartTemper.get_Axis()).get_Bottom()).put_Minimum(0);
	((CAxis)((CAxes)m_TemperRecordDlg.m_ChartTemper.get_Axis()).get_Bottom()).put_Automatic(false);
	((CAxis)((CAxes)m_TemperRecordDlg.m_ChartTemper.get_Axis()).get_Left()).put_Automatic(true);

	CSeries l_Chart =  ((CSeries)m_TemperRecordDlg.m_ChartTemper.Series(0));

	double l_d32DataNum;
	for(int i = 0;i<1460;i++)
	{
		l_d32DataNum = (short int)pu16Buf[i]/100.0;
		l_Chart.Add(l_d32DataNum,_T(""),0);
		if(i == 1450)
		{
			i = 1450;
		}
	}
}