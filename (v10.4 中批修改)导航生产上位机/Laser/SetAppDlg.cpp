// SetAppDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "SetAppDlg.h"
#include "afxdialogex.h"
#include "NetParam.h"
#include "UserMsg.h"
// CSetAppDlg 对话框
#include "MainFrm.h"
#include "GraphForm2d.h"
#include "Scanf2dDlg.h"
#include "CAxes.h"
#include "CAxis.h"
#include "Scanf2dDlg.h"
#include "SetProdDlg.h"

IMPLEMENT_DYNAMIC(CSetAppDlg, CDialog)

CSetAppDlg::CSetAppDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetAppDlg::IDD, pParent)
{
	 m_fLLevel1=0;
	 m_u8LWaveSet=0;
	 m_fZeroDisc2=0.0f;
	 m_fLWholeDisc=0.0f;
	 m_ScanRangeNum=0;
	 m_MaPan=0;
	 m_Version="";
	 m_WirePowerFrequency=0.0f;
	 m_LMThreshold=0;
	 m_Test1=0;
	 m_Test2=0;
	 m_fOffCenterAngle=0;
}

CSetAppDlg::~CSetAppDlg()
{
}

void CSetAppDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_L_LEVEL1, m_fLLevel1);
	DDX_Text(pDX, IDC_EDIT_WAVENUM2_L, m_u8LWaveSet);
	DDX_Text(pDX, IDC_EDIT_PROD_ZERODISC2, m_fZeroDisc2);
	DDX_Text(pDX, IDC_EDIT_PROD_LWHOLEDISC, m_fLWholeDisc);
	DDX_Control(pDX, IDC_COMBO_PROD_SI2SCSET, m_CombSi2ScSet);
	DDX_Control(pDX, IDC_COMBO_ERRMODIFY3, m_CombErrModify1);
	DDX_Control(pDX, IDC_COMBO_MODIFY, m_CombModify);
	DDX_Control(pDX, IDC_COMBO_WDGSET, m_CombWdgSet);
	DDX_Control(pDX, IDC_COMBO_CLKMODE, m_ClkMode);
	DDX_Control(pDX, IDC_COMBO_BOTELV, m_BoTeLv);
	DDX_Text(pDX, IDC_EDIT_MaPan, m_MaPan);
	DDX_Text(pDX, IDC_EDIT_PROD_VERSION, m_Version);
	DDX_Text(pDX, IDC_EDIT_Scan_RangeNum, m_ScanRangeNum);
	DDX_Control(pDX, IDC_COMBO_HEART, m_Heart);
	DDX_Text(pDX, IDC_EDIT_FREQUENCYPOWER, m_WirePowerFrequency);
	DDX_Control(pDX, IDC_COMBO_FPGAWORKMODE, m_CombFPGAMode);
	DDX_Text(pDX, IDC_EDIT_LMTHRESHOLD, m_LMThreshold);
	DDX_Text(pDX, IDC_EDIT_TEST1, m_Test1);
	DDX_Text(pDX, IDC_EDIT_TEST2, m_Test2);
	DDX_Text(pDX, IDC_EDIT_OFFCENTERANGLE, m_fOffCenterAngle);
	DDX_Control(pDX, IDC_BTN_BASICSTATE_WRITE, m_DownApplicPara);
	DDX_Control(pDX, IDC_BTN_BASICSTATE_READ, m_ReadApplicPara);
}


BEGIN_MESSAGE_MAP(CSetAppDlg, CDialog)
	ON_WM_SIZE()
	ON_MESSAGE(WM_SETAPP_BASICPIB_U,OnProdGetParm)
	ON_MESSAGE(WM_HEATBACK_U,OnHeatBack)
	//ON_MESSAGE(WM_RESETLASER_U,OnRestLaser)
	
	ON_BN_CLICKED(IDC_BTN_BASICSTATE_READ, &CSetAppDlg::OnBnClickedBtnBasicstateRead)
	ON_BN_CLICKED(IDC_BTN_BASICSTATE_WRITE, &CSetAppDlg::OnBnClickedBtnBasicstateWrite)
	
	ON_BN_CLICKED(IDC_BTN_RESET, &CSetAppDlg::OnBnClickedBtnReset)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CSetAppDlg)
   // EASYSIZE(IDC_TAB_DATASET,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)
	//EASYSIZE(IDD_SCANF2DPROPDLG,ES_BORDER,ES_BORDER,ES_BORDER,ES_BORDER,0)	
END_EASYSIZE_MAP

// CSetAppDlg 消息处理程序


void CSetAppDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);
	if (IsWindowVisible()&& GetWindow(GW_CHILD)!=NULL)
	 {
		// m_ChartJi.MoveWindow(&rsJi);
		 UPDATE_EASYSIZE; 
	 }
	// TODO: 在此处添加消息处理程序代码
}


BOOL CSetAppDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
		INIT_EASYSIZE;
	// TODO:  在此添加额外的初始化
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

LONG CSetAppDlg::OnProdGetParm(WPARAM wParam, LPARAM lParam)
{
	char *pcBuf = (char *)wParam;
	int nBufSize = (int)lParam;
	u16 l_u16len;//数据部分长度，除去前2个字节,帧头
	l_u16len = nBufSize - 2;
	u16  checktype = 1;  // 校验类型:0 不校验 1 异或校验 2 CRC校验
	u16  checkres=0;
	u8 *pu8buf = new u8[nBufSize];
	memcpy(pu8buf,&pcBuf[2],l_u16len);
	checktype = pu8buf[8];
	switch(checktype) { // 校验过程
			case 0: checkres = TRUE; break;
		  case 1: checkres = CHECK_BCC(&pu8buf[0], l_u16len); break;
			//case 2: checkres = CHECK_CRC(&buf[2], l_u16len); break;
			default: checkres = FALSE; break;
			}
	if (!checkres)
	{
		return 0;
	}
	//硬件版本号24
	CString strtmp;
	m_Version="";
	for (u8 l_u8i = 32;l_u8i < 52; l_u8i++)
	{
		strtmp.Format("%c",pcBuf[l_u8i]);
		m_Version += strtmp;
	}
	m_CombWdgSet.SetCurSel(pu8buf[52]);
	m_BoTeLv.SetCurSel(pu8buf[53]-1);
	m_CombSi2ScSet.SetCurSel(pu8buf[54]);
	//模式47
	m_ClkMode.SetCurSel(pu8buf[55]);
	if (pu8buf[55] == 0x00)
	{
		m_ScanRangeNum= 3600;
	} 
	else if (pu8buf[55] == 0x01)
	{
		m_ScanRangeNum= 7200;
	}
	
	//if(pu8buf[47]-1==1&&pu8buf[46]==1)//720k扫描
	//{
	//	m_scanf2ddlg.m_sScan2dCfgDlg;
	//}
	m_MaPan=(pu8buf[56]<<8)+(pu8buf[57]);
	m_fLLevel1= ((pu8buf[58]<<8)+(pu8buf[59]))/1000.0;
	m_u8LWaveSet=(pu8buf[60]<<8)+(pu8buf[61]);
	m_fLWholeDisc=(pu8buf[62]<<8)+(pu8buf[63]);

	m_CombErrModify1.SetCurSel(pu8buf[64]);
	m_CombModify.SetCurSel(pu8buf[65]);
	m_Heart.SetCurSel(pu8buf[66]);
	m_WirePowerFrequency=(pu8buf[67]<<8)+(pu8buf[68]);
	m_CombFPGAMode.SetCurSel(pu8buf[69]-1);
	m_LMThreshold=(pu8buf[70]<<8)+(pu8buf[71]);
	m_Test1=(pu8buf[72]<<8)+(pu8buf[73]);
	//m_Test2=(pu8buf[74]<<8)+(pu8buf[75]);
	m_Test2 = (pcBuf[76] << 8) + (unsigned char)(pcBuf[77]);
	//m_fOffCenterAngle=((pu8buf[76]<<24)+(pu8buf[77]<<16)+(pu8buf[78]<<8)+(pu8buf[79]))/1000.000;
	m_fOffCenterAngle=(pu8buf[76]<<8)+(pu8buf[77]);

	UpdateData(false);
	(GetDlgItem(IDC_BTN_BASICSTATE_WRITE))->EnableWindow(TRUE);
	return 1;
}

void CSetAppDlg::OnBnClickedBtnBasicstateRead()
{
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x01};
	ZhiLing(BasicState_zhiling,pn,0);
	BasicState_zhiling[25] = 0x01;   //代表生产版上位机发送
	AfxGetMainWnd()->PostMessage(WM_SENDCMD,BasicState_zhilingSize, (LPARAM)BasicState_zhiling);
}

char CSetAppDlg::ConvertHexChar(char ch)
{
	if((ch>='0')&&(ch<='9'))
		return ch-0x30;
	else if((ch>='A')&&(ch<='F'))
		return ch-'A'+10;
	else if((ch>='a')&&(ch<='f'))
		return ch-'a'+10;
	else return (0);
}

void CSetAppDlg::OnBnClickedBtnBasicstateWrite()
{
	UpdateData(true);
	CTime time=CTime::GetCurrentTime(); 
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x58,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x0F};
	ZhiLing(BasicStateWR_zhiling,pn,0);
	//硬件版本号26
	unsigned char tmp[2]={0};
	for(int i=0;i<20;i++)
	{
		//ss.Format(_T("%s"),);
		//BasicStateWR_zhiling[26+i]=((_ttoi(m_Version.Mid(i,1))) &0xff);
		memcpy(&tmp,m_Version.Mid(i),1);
		BasicStateWR_zhiling[34+i]=(tmp[0]);
	}
	BasicStateWR_zhiling[54]=((m_CombWdgSet.GetCurSel()) &0xff);
	//串口波特率47
	BasicStateWR_zhiling[55]=((m_BoTeLv.GetCurSel()+1) &0xff);
	//单点or扫描
	BasicStateWR_zhiling[56]= ((m_CombSi2ScSet.GetCurSel()) &0xff);
	//分辨率
	BasicStateWR_zhiling[57]=((m_ClkMode.GetCurSel()) &0xff);
	//码盘刻度调整3
	BasicStateWR_zhiling[58]=(m_MaPan >>8);
	BasicStateWR_zhiling[59]=(m_MaPan & 0xff);
	//低阈值电压
	m_fLLevel1=m_fLLevel1*1000;
	BasicStateWR_zhiling[60]=((int)m_fLLevel1>>8);
	BasicStateWR_zhiling[61]=((int)m_fLLevel1& 0xff);
	//低回波数
	BasicStateWR_zhiling[62]=((int)m_u8LWaveSet>>8);
	BasicStateWR_zhiling[63]=((int)m_u8LWaveSet& 0xff);
	//低整体偏移
	BasicStateWR_zhiling[64]=((int)m_fLWholeDisc>>8);
	BasicStateWR_zhiling[65]=((int)m_fLWholeDisc& 0xff);
	//错误修正
	BasicStateWR_zhiling[66]=((m_CombErrModify1.GetCurSel()) &0xff);
	//修正表状态
	BasicStateWR_zhiling[67]=((m_CombModify.GetCurSel()) &0xff);
	//心跳
	BasicStateWR_zhiling[68]=((m_Heart.GetCurSel()) &0xff);
	//无线供电频率
	BasicStateWR_zhiling[69]=((int)m_WirePowerFrequency>>8);
	BasicStateWR_zhiling[70]=((int)m_WirePowerFrequency& 0xff);
	//Fpga工作模式
	BasicStateWR_zhiling[71]=((m_CombFPGAMode.GetCurSel()+1) &0xff);
	//靶标识别阈值
	BasicStateWR_zhiling[72]=((int)m_LMThreshold>>8);
	BasicStateWR_zhiling[73]=((int)m_LMThreshold& 0xff);
	//测试1
	BasicStateWR_zhiling[74]=((int)m_Test1>>8);
	BasicStateWR_zhiling[75]=((int)m_Test1& 0xff);
	//测试2
	BasicStateWR_zhiling[76]=((int)m_Test2>>8);
	BasicStateWR_zhiling[77]=((int)m_Test2& 0xff);
	//偏心角度
	//int temp_OffCenterAngle=m_fOffCenterAngle*1000;
	//BasicStateWR_zhiling[78]=(temp_OffCenterAngle>>24);
	//BasicStateWR_zhiling[79]=(temp_OffCenterAngle>>16);
	//BasicStateWR_zhiling[80]=(temp_OffCenterAngle>>8);
	//BasicStateWR_zhiling[81]=(temp_OffCenterAngle& 0xff);
	int temp_OffCenterAngle=m_fOffCenterAngle;
	BasicStateWR_zhiling[78]=(temp_OffCenterAngle>>8);
	BasicStateWR_zhiling[79]=(temp_OffCenterAngle& 0xff);

	AfxGetMainWnd()->PostMessage(WM_SENDCMD,BasicStateWR_zhilingSize, (LPARAM)BasicStateWR_zhiling);
	Sleep(100);
	//切换到靶标探测获取靶标角度
	//CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	//GraphForm2d * pView;
	//pView = (GraphForm2d*)pMainFrame->m_wndLeftSplitter.GetPane(0, 0);
	//pView->m_AGVTestDlg.m_AGVTestCfg.SendMessage(WM_GetLMAngle,0,0);
	//AfxGetMainWnd()->PostMessage(WM_SENDCMD, LandmarkModeSet_zhilingSize, (LPARAM)LandmarkModeSet_zhiling);//切换到靶标探测
	//Sleep(100);
	//AfxGetMainWnd()->PostMessage(WM_SENDCMD, PXGetLmIndo_zhilingSize, (LPARAM)PXGetLmIndo_zhiling);//偏心修正
}

//开启加热返回
LONG CSetAppDlg::OnHeatBack(WPARAM wParam, LPARAM lParam)
{
	char *pcBuf = (char *)wParam;
	int nBufSize = (int)lParam;
	u16 l_u16len;//数据部分长度，除去前2个字节,帧头
	l_u16len = nBufSize - 2;
	u16  checktype = 1;  // 校验类型:0 不校验 1 异或校验 2 CRC校验
	u16  checkres=0;
	u8 *pu8buf = new u8[nBufSize];
	memcpy(pu8buf,&pcBuf[2],l_u16len);
	checktype = pu8buf[8];
	switch(checktype) { // 校验过程
			case 0: checkres = TRUE; break;
		  case 1: checkres = CHECK_BCC(&pu8buf[0], l_u16len); break;
			//case 2: checkres = CHECK_CRC(&buf[2], l_u16len); break;
			default: checkres = FALSE; break;
			}
	if (!checkres)
	{
		return 0;
	}

	if(pu8buf[27]==0x01)
	{
	 AfxMessageBox(_T("设置成功！"));
	}else{
	 AfxMessageBox(_T("设置失败！"));
	}
}
void CSetAppDlg::OnBnClickedBtnReset()
{
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x04,0x05};
	ZhiLing(Reset_zhiling,pn,0);
	AfxGetMainWnd()->PostMessage(WM_SENDCMD,Reset_zhilingSize, (LPARAM)Reset_zhiling);
}
//LONG CSetAppDlg::OnRestLaser(WPARAM wParam, LPARAM lParam)
//{
//
//}
