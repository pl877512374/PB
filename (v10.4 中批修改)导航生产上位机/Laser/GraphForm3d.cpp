// GraphForm3d.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "MainFrm.h"
#include "GraphForm3d.h"
#include "GraphForm2d.h"
#include "Scanf2dDlg.h"
#include "tchart1.h"
#include "CSeries.h"
#include "CAxis.h"
#include "CAxes.h"
#include "CIsoSurfaceSeries.h"
#include "math.h"
#include "windows.h"
#include "shlwapi.h"
#include "CSurfaceSeries.h"
#include <map>
#include <deque>
#include <vector>
using namespace std;
//using std::make_pair;
u32 g_u32TimerCnt = 0;
#define  qmaxsize 40 //最多40包
#define  pointnum 100  //图像上点数
unsigned long   packetlen = qmaxsize*361+1;
//int  recvdata[qmaxsize][361];
int  recvdatalen = 0;
extern int  createflag; //创建对话框标志
typedef multimap<int , double> MapSort;
typedef vector<int> Vectordata;
typedef deque< vector<int> > Dequedata;
Vectordata tempvector(361);
Dequedata updatedata;
int  tempArrary[361];
MapSort sorttwonum;
MapSort intertwonum;
MapSort::iterator pos;
// GraphForm3d
extern int g_nAngle;
int draw3dflag = 0;
IMPLEMENT_DYNCREATE(GraphForm3d, CFormView)

GraphForm3d::GraphForm3d()
	: CFormView(GraphForm3d::IDD)
{

}

GraphForm3d::~GraphForm3d()
{
}

void GraphForm3d::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_TAB_3DGRAPH, m_Tab3dGraphSet);
}

BEGIN_MESSAGE_MAP(GraphForm3d, CFormView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_NETCMD_DRAW3D,OnDraw3dChartSc)
END_MESSAGE_MAP()


// GraphForm3d 诊断

#ifdef _DEBUG
void GraphForm3d::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void GraphForm3d::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// GraphForm3d 消息处理程序

BEGIN_EASYSIZE_MAP(GraphForm3d)
    EASYSIZE(IDC_TAB_3DGRAPH,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	//EASYSIZE(IDD_SCANF2DPROPDLG,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)	
END_EASYSIZE_MAP

void GraphForm3d::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
	CRect rs,rs3d;

	//if (IsWindowVisible()&& GetWindow(GW_CHILD)!=NULL)
	// {
	//	 this->GetClientRect(&rs);
	//	 rs.top += 1;
	//	 rs.bottom -= 5;
	//	 rs.left += 1;
	//	 rs.right -= 2;
	//	 rs3d.top = rs.top;
	//	 rs3d.bottom = rs.bottom-5;
	//	 rs3d.left = rs.left;
	//	 rs3d.right = rs.right;
	//	 m_draw3d.MoveWindow(&rs);

	// }

	SetScrollSizes(MM_TEXT,CSize(0,0));
	//UPDATE_EASYSIZE;
  // UPDATE_EASYSIZE; 
      //if (m_bFisrtEasySize && IsWindowVisible()&& GetWindow(GW_CHILD)!=NULL)
      //{
      //      // INIT_EASYSIZE;
      //       m_bFisrtEasySize = FALSE;
      //       SetScrollSizes(MM_TEXT,CSize(0,0));
      //       UPDATE_EASYSIZE;
      //}else if (!m_bFisrtEasySize){
	 //IsWindowVisible()&& 
	 if (IsWindowVisible()&&GetWindow(GW_CHILD)!=NULL)    //报错，暂时注释掉
	 {       
		     UPDATE_EASYSIZE;
             CRect rs;
			 m_Tab3dGraphSet.GetClientRect(&rs);
			 rs.top += 25;
			 rs.bottom -= 5;
			 rs.left += 1;
			 rs.right -= 2;
			 //
			 	int CurSel = m_Tab3dGraphSet.GetCurSel();
				switch(CurSel)
				{
					case 0:
						m_scanf3ddlg.MoveWindow(&rs);
						m_scanf3ddlg.ShowWindow(true);
						break;
				}
      }
	// TODO: 在此处添加消息处理程序代码
}

typedef struct TagStructCOleSafeArrayS
{
	COleSafeArray Adx;
 	COleSafeArray Ady;
 	COleSafeArray Adz;
}StructCOleSafeArrayS;
//StructCOleSafeArrayS g_stCOleSafeArrayS[100];
u32 g_u32tCOleSafeArraySIn = 0;
u32 g_u32tCOleSafeArraySOut = 0;

 	COleSafeArray Adx;
 	COleSafeArray Ady;
 	COleSafeArray Adz;

void GraphForm3d::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	m_Tab3dGraphSet.InsertItem(0, _T("3d立体图"));
	//SendMessage(WM_SIZE);
	 	Adx.Create(VT_R8,1,&packetlen);
 	Ady.Create(VT_R8,1,&packetlen);
    Adz.Create(VT_I4,1,&packetlen);
	CRect rs;
	m_Tab3dGraphSet.GetClientRect(&rs);
	m_scanf3ddlg.Create(IDD_SCANF3DDLG,GetDlgItem(IDC_TAB_3DGRAPH));
	rs.top += 25;
    rs.bottom -= 4;
    rs.left += 1;
    rs.right -= 2;
	//
   // m_scanf2ddlg.ShowWindow(true);	
	m_scanf3ddlg.MoveWindow(&rs);
	m_scanf3ddlg.ShowWindow(true);	
	SendMessage(WM_SIZE);

	//((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Left()).put_Maximum(10000);
	//((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Left()).put_Minimum(0);
	
	//((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Left()).put_Maximum(m_scanf3ddlg.m_sScan3dCfgDlg.m_high);
	//((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Left()).put_Minimum(0);
	//SetTimer(0,100,0);
	//for(int i = 0;i<100;i++)
	//{
	//g_stCOleSafeArrayS[i].Adx.Create(VT_R8,1,&packetlen);
 //	g_stCOleSafeArrayS[i].Ady.Create(VT_R8,1,&packetlen);
 //   g_stCOleSafeArrayS[i].Adz.Create(VT_I4,1,&packetlen);
	//}
	//CRect rs;
	//m_TabGraphSet.GetClientRect(&rs);

	//rs.top += 25;
 //   rs.bottom -= 4;
 //   rs.left += 1;
 //   rs.right -= 2;
	////
 //  // m_scanf2ddlg.ShowWindow(true);	
	//m_scanf2ddlg.MoveWindow(&rs);
	//m_scanf2ddlg.ShowWindow(true);	
	//SendMessage(WM_SIZE);
	// TODO: 在此添加专用代码和/或调用基类
}

LRESULT GraphForm3d::OnDraw3dChartSc(WPARAM wParam,LPARAM lParam)
{
	int i,j,k,h;
 	double q;
 	double dx,dy/*,dz*/;
 	double /*bx,by,*/;
 	int nAngle = g_nAngle;    //直角坐标系零点横坐标
 	long ti =0;
	//nAngle = m_scanf2ddlg.m_sScan2dCfgDlg.m_nZeroX;
    NetDataStruct *l_psNetData;
	l_psNetData =  (NetDataStruct *)wParam;
	CSeries l_Chart3d =  ((CSeries)m_scanf3ddlg.m_draw3d.Series(0));


	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	GraphForm2d *pView = (GraphForm2d*)pMainFrame->m_wndLeftSplitter.GetPane(0,0);

     int /*inter,temp,*/sizea;
 	double /*k1,*/tempfirst,tempsecond,d1,d2;
 	i = 83;
 	int DataLen = ((l_psNetData->m_pcData[i]&0xFF)<<8) + (BYTE)l_psNetData->m_pcData[i+1];
     CString cs;
 	int intvaldata,startnum,df;
 	i += 2;
	int t = m_scanf3ddlg.m_sScan3dCfgDlg.m_high;
	((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Left()).put_Maximum(m_scanf3ddlg.m_sScan3dCfgDlg.m_high);	
	((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Left()).put_Minimum(0);
	((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Left()).put_Automatic(false);

	//((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Bottom()).put_Maximum(1000);	
	//((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Bottom()).put_Minimum(0);
	//((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Bottom()).put_Automatic(true);
 	long tlen;
	static int s_PreData;
 	if ( recvdatalen< qmaxsize)  //收数
 	{
 		//for(k = m_scanf3ddlg.m_sScan3dCfgDlg.m_n32jiaoduSta; k < m_scanf3ddlg.m_sScan3dCfgDlg.m_n32jiaoduEnd; k ++)
		for(k = 0; k < 361; k ++)
 		{

 			tempvector[k] = (((l_psNetData->m_pcData[i]&0xFF)<<8) + (BYTE)l_psNetData->m_pcData[i+1]);

				//if (pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_dealnegative) //修正
				//{
				//	if(tempvector[k]>=0 && tempvector[k]<=10)
				//	{
				//		tempvector[k] = s_PreData;
				//	}

				//}
				if(tempvector[k] > pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nMaxDistance)
				{
					tempvector[k] = pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_nMaxDistance;
				}
				
 			i += 2;
 		}
 		updatedata.push_back(tempvector);
 		recvdatalen++;
 	}
 	else                 //绘图
 	{   
 		
 		if(createflag)
 		{			
 			//l_Chart3d.Clear();			
 			for (h=0;h<qmaxsize;h++)  //z
 			{
 				tempvector = updatedata[h];
				for(j = m_scanf3ddlg.m_sScan3dCfgDlg.m_n32jiaoduSta; j < m_scanf3ddlg.m_sScan3dCfgDlg.m_n32jiaoduEnd; j ++)
 			   // for (j=0;j<361;j++)   //x
 				{				   
 					
 					//dy =abs((tempvector[j]*cos((j-nAngle)*3.1416/360)));
					if (pView->m_scanf2ddlg.m_sScan2dCfgDlg.m_dealnegative)
					{
						dy =abs((tempvector[j]*cos((j-nAngle)*3.1416/360)));
						if(tempvector[j]>=0 && tempvector[j]<=10)
						{
							if(j != 360)
							{
							dy =s_PreData;
							dx = s_PreData*tan((j-nAngle)*3.1416/360);
							tempvector[j] = dy/cos((j-nAngle)*3.1416/360);
							}
							else
							{
								dy =s_PreData;
								dx =0;
								tempvector[j] = 0;
							}
						}
						s_PreData = dy;
						
					}
					else
					{
						dy =abs((tempvector[j]*cos((j-nAngle)*3.1416/360)));
					}

 					if (dy > m_scanf3ddlg.m_sScan3dCfgDlg.m_ymax)
 					{

 						dx = (tempvector[j]*sin((j-nAngle)*3.1416/360));
						//if(dy < 1000 && dy > 400 && dx < -5000 && dx > -10000)
						//{
						//	dx = dx+0;
						//}
 						if (dx > m_scanf3ddlg.m_sScan3dCfgDlg.m_xmin && dx < m_scanf3ddlg.m_sScan3dCfgDlg.m_xmax )
 					    {
 							if(dy<m_scanf3ddlg.m_sScan3dCfgDlg.m_high - m_scanf3ddlg.m_sScan3dCfgDlg.m_ymin)
 							{
 									sorttwonum.insert(make_pair(dx,m_scanf3ddlg.m_sScan3dCfgDlg.m_high-dy));
 							}
 							else
 							{
 							       	sorttwonum.insert(make_pair(dx,0));
 							}
 
 						
 					    }
 					/*	else
 						{
 							sorttwonum.insert(make_pair(dx,0));	
 						}*/
 										
 					}				
 				}
 				
                //////////////////////////////////////////////////////////////////////
 				d2 = sorttwonum.rbegin()->first;
 				
 				startnum = m_scanf3ddlg.m_sScan3dCfgDlg.m_xmin;///*sorttwonum.begin()->first; //*/-7500;
 				intvaldata = m_scanf3ddlg.m_sScan3dCfgDlg.m_xmax - m_scanf3ddlg.m_sScan3dCfgDlg.m_xmin;///*d2 - startnum;  //*/15000;

 				pos = sorttwonum.begin();
                 for (q=0;q<pointnum;q++)
                 {
 					sizea = sorttwonum.size();
 					if ( pos != sorttwonum.end())
 					{					
 						tempsecond = 0;
 
 						while(1)
 						{					
 							d1 = pos->first;	
 							df = (d1 - startnum)/intvaldata *pointnum;
 							if(df < q)
 							{
 								tempsecond = pos->second;	
 								tempfirst = df;
 								pos++;
 								if(pos == sorttwonum.end())
 								    break;
 							}
 							else if(df > q)
 							{
 								tempsecond = 0;//(tempsecond + pos->second)/2;
 								break;
 							}
 							else
 							{
 								tempsecond  = pos->second;
 								pos++;
 								break;
 							}
 						}
 						Adx.PutElement(&ti,&q);
 						Ady.PutElement(&ti,&tempsecond);
 				        Adz.PutElement(&ti,&h);

 					}
 					else
 					{
 						tempsecond = 0;
 						Adx.PutElement(&ti,&q);
 						Ady.PutElement(&ti,&tempsecond);
 				        Adz.PutElement(&ti,&h);
 
 					}
 				  ti++;
                 }
 				sorttwonum.clear();	
 				
 			}
			//if(draw3dflag == 1)
			{
 				((CSurfaceSeries)l_Chart3d.get_asSurface()).AddArrayXYZ(Adx,Ady,Adz);	
				draw3dflag = 0;
			}
 	
 		}
 		
        updatedata.pop_front();
 		recvdatalen--;
 		updatedata.pop_front();
 		recvdatalen--;
 		updatedata.pop_front();
 		recvdatalen--;
 		updatedata.pop_front();
 		recvdatalen--;
 		updatedata.pop_front();
 		recvdatalen--;
		UpdateData(false);
 		
 	}
 
	return 1;
}

//CString GetCurTick(CString text)
//{
//	static long pret1;
//	CString str1;
//	long t1=GetTickCount();//程序段开始前取得系统运行时间(ms)
//	
//	CString text1 = text;
//	CString	str;
//	str.Format("%d\n",t1);//前后之差即 程序运行时间
//	text1 += str;
//    str1.Format("%d\n",t1-pret1);
//	FILE *fp;
//	if((fp=fopen("d:\\tick.txt","at+"))!=NULL)
//	{
//			fwrite(text1,strlen(text1),1,fp);
//			text1 = "耗时：";
//			text1 +=str1;
//			fwrite(text1,strlen(text1),1,fp);
//			if(text == "结束绘图tick：")
//			{
//				text1 = "----------------------\n";
//				fwrite(text1,strlen(text1),1,fp);
//			}
//			fclose(fp);
//	}
//	pret1 = t1;
//	return str;
//}

void GraphForm3d::OnTimer(UINT nIDEvent) 
{
	int i,j,k,h;
 	double q;
 	double dx,dy/*,dz*/;
 	double /*bx,by,*/;
 	int nAngle = g_nAngle;    //直角坐标系零点横坐标
 	long ti =0;
	u8 l_u8flag;
	
	//printf("开始tick：%s\n",GetCurTick("开始tick:"));

 	COleSafeArray Adx;
 	COleSafeArray Ady;
 	COleSafeArray Adz;
 	Adx.Create(VT_R8,1,&packetlen);
 	Ady.Create(VT_R8,1,&packetlen);
    Adz.Create(VT_I4,1,&packetlen);

    NetDataStruct *l_psNetData;
	CSeries l_Chart3d =  ((CSeries)m_scanf3ddlg.m_draw3d.Series(0));
     int /*inter,temp,*/sizea;
 	double /*k1,*/tempfirst,tempsecond,d1,d2;
 	i = 83;
 	//int DataLen = //((l_psNetData->m_pcData[i]&0xFF)<<8) + (BYTE)l_psNetData->m_pcData[i+1];
     CString cs;
 	int intvaldata,startnum,df;
 	i += 2;
	int t = m_scanf3ddlg.m_sScan3dCfgDlg.m_high;
	((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Left()).put_Maximum(m_scanf3ddlg.m_sScan3dCfgDlg.m_high);	
	((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Left()).put_Minimum(0);
	((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Left()).put_Automatic(false);

	//((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Bottom()).put_Maximum(1000);	
	//((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Bottom()).put_Minimum(0);
	//((CAxis)((CAxes)m_scanf3ddlg.m_draw3d.get_Axis()).get_Bottom()).put_Automatic(true);
	//printf("开始计算tick：%s\n",GetCurTick("开始计算tick:"));
 	long tlen;
 	if ( recvdatalen< qmaxsize)  //收数
 	{
		
 		for(k = 0; k < 361; k ++)
 		{

 			tempvector[k] = ((((rand()%0xff)&0xFF)<<8) + (BYTE)(rand()%0xff));
 			i += 2;
 		}
 		updatedata.push_back(tempvector);
 		recvdatalen++;
 	}
 	else                 //绘图
 	{   
 		
 		if(createflag)
 		{			
 			//l_Chart3d.Clear();	
			//for (h=0;h<1;h++)
 			for (h=0;h<qmaxsize;h++)  //z
 			{
 				tempvector = updatedata[h];
 			    for (j=0;j<361;j++)   //x
 				{				   
 					
 					  dy =abs((tempvector[j]*cos((j-nAngle)*3.1416/360)));
 					if (dy > m_scanf3ddlg.m_sScan3dCfgDlg.m_ymax)
 					{
 						dx = (tempvector[j]*sin((j-nAngle)*3.1416/360));
 						if (dx > m_scanf3ddlg.m_sScan3dCfgDlg.m_xmin && dx < m_scanf3ddlg.m_sScan3dCfgDlg.m_xmax )
 					    {
 							if(dy<m_scanf3ddlg.m_sScan3dCfgDlg.m_high - m_scanf3ddlg.m_sScan3dCfgDlg.m_ymin)
 							{
 									sorttwonum.insert(make_pair(dx,m_scanf3ddlg.m_sScan3dCfgDlg.m_high-dy));
 							}
 							else
 							{
 							       	sorttwonum.insert(make_pair(dx,0));
 							}
 
 						
 					    }
 					/*	else
 						{
 							sorttwonum.insert(make_pair(dx,0));	
 						}*/
 										
 					}				
 				}
 				
                //////////////////////////////////////////////////////////////////////
 				d2 = sorttwonum.rbegin()->first;
 				
 				startnum = m_scanf3ddlg.m_sScan3dCfgDlg.m_xmin;///*sorttwonum.begin()->first; //*/-7500;
 				intvaldata = m_scanf3ddlg.m_sScan3dCfgDlg.m_xmax - m_scanf3ddlg.m_sScan3dCfgDlg.m_xmin;///*d2 - startnum;  //*/15000;

 				pos = sorttwonum.begin();
                 for (q=0;q<pointnum;q++)
                 {
 					sizea = sorttwonum.size();
 					if ( pos != sorttwonum.end())
 					{					
 						tempsecond = 0;
 
 						while(1)
 						{					
 							d1 = pos->first;	
 							df = (d1 - startnum)/intvaldata *pointnum;
 							if(df < q)
 							{
 								tempsecond = pos->second;	
 								tempfirst = df;
 								pos++;
 								if(pos == sorttwonum.end())
 								    break;
 							}
 							else if(df > q)
 							{
 								tempsecond = 0;//(tempsecond + pos->second)/2;
 								break;
 							}
 							else
 							{
 								tempsecond  = pos->second;
 								pos++;
 								break;
 							}
 						}
 						Adx.PutElement(&ti,&q);
 						Ady.PutElement(&ti,&tempsecond);
 				        Adz.PutElement(&ti,&h);

 					}
 					else
 					{
 						tempsecond = 0;
 						Adx.PutElement(&ti,&q);
 						Ady.PutElement(&ti,&tempsecond);
 				        Adz.PutElement(&ti,&h);
 
 					}
 				  ti++;
                 }

 				sorttwonum.clear();	
 			}
			
			//if(draw3dflag == 1)
			{
				//printf("开始绘图tick：%s\n",GetCurTick("开始绘图tick："));
 				((CSurfaceSeries)l_Chart3d.get_asSurface()).AddArrayXYZ(Adx,Ady,Adz);	
				//printf("结束绘图tick：%s\n",GetCurTick("结束绘图tick："));
				draw3dflag = 0;
			}
 	
 		}
 		
        updatedata.pop_front();
 		recvdatalen--;
 		updatedata.pop_front();
 		recvdatalen--;
 		updatedata.pop_front();
 		recvdatalen--;
 		updatedata.pop_front();
 		recvdatalen--;
 		updatedata.pop_front();
 		recvdatalen--;
		//UpdateData(false);
 		
 	}
}