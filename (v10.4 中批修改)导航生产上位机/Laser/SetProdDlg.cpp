// SetProdDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "SetProdDlg.h"
#include "afxdialogex.h"
#include "UserMsg.h"
#include "NetParam.h"
#include "WriteLog.h"
#include <ctime> 
#include <afx.h> 
#include "AGVTestCfg.h"
// CSetProdDlg 对话框
const float c_fAPDTestStep = 10;
const int c_n32APDTestMax = 280*c_fAPDTestStep;
const int c_n32APDTestMin = 0*c_fAPDTestStep;

const float c_fLaserTestStep = 10;
const int c_n32LaserTestMax = 120*c_fLaserTestStep;
const int c_n32LaserTestMin = 0*c_fLaserTestStep;
extern StrRecordInfo g_sRecordInfo;
bool f_SetProDialog = false;//是否弹出电机参数下载成功对话框
IMPLEMENT_DYNAMIC(CSetProdDlg, CDialog)

CSetProdDlg::CSetProdDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSetProdDlg::IDD, pParent)
{


	m_fAPDHvValue = 0.0f;
	m_fAPDTemperValue = 0.0f;
	m_fAPDHV_OP_Ratio = 0.0f;
	m_fAPDHVTestWr = 0.0f;
	m_fLaserPower1 = 0.0f;
	m_strEquipMac = _T("");
	m_fPID_p1 = 0.0f;
	m_fPID_i1 = 0.0f;
	m_fPID_d1 = 0.0f;
	m_fLaserPower1 = 0.0f;
	m_u32PortSer = 0;
	m_MotoSpeed = 0;
}

CSetProdDlg::~CSetProdDlg()
{
}

void CSetProdDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Text(pDX, IDC_EDIT_APDHVVALUE, m_fAPDHvValue);
	DDX_Text(pDX, IDC_EDIT_APDTEMPERVALUE, m_fAPDTemperValue);
	DDX_Text(pDX, IDC_EDIT_APDHV_OP_RATIO, m_fAPDHV_OP_Ratio);
	DDX_Text(pDX, IDC_EDIT_APDTEST_APDHVTEST, m_fAPDHVTestWr);
	DDX_Control(pDX, IDC_IPADDRESS_EQUPIMASK, m_CIPAddSubMaskSer);
	DDX_Control(pDX, IDC_IPADDRESS_GETWAY, m_CIPAddGetWaySer);
	DDX_Control(pDX, IDC_IPADDRESS_IP_SER, m_CIPAddIPSer);
	DDX_Text(pDX, IDC_EDIT_EQUIPMAC, m_strEquipMac);
	DDX_Text(pDX, IDC_EDIT_PID_P1, m_fPID_p1);
	DDX_Text(pDX, IDC_EDIT_PID_I1, m_fPID_i1);
	DDX_Text(pDX, IDC_EDIT_PID_D1, m_fPID_d1);
	DDX_Control(pDX, IDC_COMBO_MOTOLOOP, m_CombMotoLoop);
	DDX_Control(pDX, IDC_COMBO_MOTODIR, m_CombMotoDir);
	DDX_Control(pDX, IDC_SLIDER_APDTEST, m_SliCtrl_APDTest);
	DDX_Text(pDX, IDC_EDIT_PORT_SER, m_u32PortSer);
	DDX_Control(pDX, IDC_BTN_PROD_APDTEST, m_btnAPDTest);
	DDX_Control(pDX, IDC_BTN_APDPIB_WR, m_btnAPDPibWr);
	DDX_Control(pDX, IDC_BTN_APDTEST_D1, m_btnAPDTestD1);
	DDX_Control(pDX, IDC_BTN_PIDPARAM_D1, m_btnPIDDn1);
	DDX_Text(pDX, IDC_EDIT_MotoSpeed, m_MotoSpeed);
	DDX_Control(pDX, IDC_COMBO_LOCATEMODE, m_CombLocateMode);
	DDX_Control(pDX, IDC_COMBO_DATAMODE, m_CombDataMode);
	DDX_Control(pDX, IDC_COMBO_POINTSRATIO, m_CombPointsRatio);
	DDX_Control(pDX, IDC_BTN_PIDPARAM_UP1, m_btnPIDRead);
}


BEGIN_MESSAGE_MAP(CSetProdDlg, CDialog)
	ON_MESSAGE(WM_SETPROD_APDPIB_U,OnProdAPDPibR)
	ON_MESSAGE(WM_SETPROD_NetWork_Read,OnProdNetWork)
	ON_MESSAGE(WM_SETPROD_APDTEST_U1,OnProdAPDTest)
	ON_MESSAGE(WM_SETRD_MOTOPID_READ,OnProdMotoPid)
	ON_MESSAGE(WM_SETRD_MOTOPID_SET,OnProdMotoPidSet)
	ON_MESSAGE(WM_SETRD_APDTEST_BACK,OnAPDTestBack)
	ON_MESSAGE(WM_SETRD_LaserGL_BACK,OnLaserGLTestBack)
	
	ON_NOTIFY(NM_RELEASEDCAPTURE, IDC_SLIDER_APDTEST, &CSetProdDlg::OnNMReleasedcaptureSliderApdtest)
	ON_BN_CLICKED(IDC_BTN_APDPIB_WR, &CSetProdDlg::OnBnClickedBtnApdpibWr)
	ON_BN_CLICKED(IDC_BTN_APDPIB_READ, &CSetProdDlg::OnBnClickedBtnApdpibRead)
	ON_BN_CLICKED(IDC_BTN_PROD_APDTEST, &CSetProdDlg::OnBnClickedBtnProdApdtest)
	ON_BN_CLICKED(IDC_BTN_APDTEST_D1, &CSetProdDlg::OnBnClickedBtnApdtestD1)
	ON_BN_CLICKED(IDC_BTN_PIDPARAM_UP1, &CSetProdDlg::OnBnClickedBtnPidparamUp1)
	ON_BN_CLICKED(IDC_BTN_PIDPARAM_D1, &CSetProdDlg::OnBnClickedBtnPidparamD1)
	ON_BN_CLICKED(IDC_BTN_NetWork_READ, &CSetProdDlg::OnBnClickedBtnNetworkRead)
	ON_BN_CLICKED(IDC_BTN_NetWork_WR, &CSetProdDlg::OnBnClickedBtnNetworkWr)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER_APDTEST, &CSetProdDlg::OnNMCustomdrawSliderApdtest)
END_MESSAGE_MAP()


// CSetProdDlg 消息处理程序


BOOL CSetProdDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	m_SliCtrl_APDTest.SetRange(c_n32APDTestMin,c_n32APDTestMax);
	// TODO:  在此添加额外的初始化
	
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


u8 g_u8CurClkmode = 3;
char CSetProdDlg::ConvertHexChar(char ch)
{
	if((ch>='0')&&(ch<='9'))
		return ch-0x30;
	else if((ch>='A')&&(ch<='F'))
		return ch-'A'+10;
	else if((ch>='a')&&(ch<='f'))
		return ch-'a'+10;
	else return (-1);
}
//APD击穿参数配置获取
LONG CSetProdDlg::OnProdAPDPibR(WPARAM wParam, LPARAM lParam)
{
	char *pcBuf = (char *)wParam;
	int nBufSize = (int)lParam;
	u16 l_u16len;//数据部分长度，除去前2个字节,帧头
	l_u16len = nBufSize-2;
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
	//u16 *pu16Buf = (u16*)(&pcBuf[16]);
	u32 tmp = (pu8buf[24] << 8); 
	tmp=(pu8buf[25] |tmp);
	m_fAPDHvValue = tmp/100.0;
	 tmp = (pu8buf[26] << 8); 
	tmp=(pu8buf[27] |tmp);
	m_fAPDTemperValue= tmp/100.0;
	 tmp = (pu8buf[28] << 8); 
	 tmp= (pu8buf[29] |tmp);
	m_fAPDHV_OP_Ratio=tmp/100.0;
	GetDlgItem(IDC_BTN_APDPIB_WR)->EnableWindow(true);

	CString str;
	str.Format("%f",m_fAPDHvValue);
	g_sRecordInfo.m_strAPDHV = str;

	str.Format("%f",m_fAPDTemperValue);
	g_sRecordInfo.m_strAPDTemper = str;

	str.Format("%f",m_fAPDHV_OP_Ratio);
	g_sRecordInfo.m_strAPDRatio = str;
	UpdateData(false);
	GetDlgItem(IDC_BTN_APDPIB_WR)->EnableWindow(true);
	return 1;
	delete []pu8buf;
}

//APD击穿测试
LONG CSetProdDlg::OnProdAPDTest(WPARAM wParam, LPARAM lParam)
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
		  case 1: checkres = CHECK_BCC(&pu8buf[2], l_u16len); break;
			//case 2: checkres = CHECK_CRC(&buf[2], l_u16len); break;
			default: checkres = FALSE; break;
			}
	if (!checkres)
	{
		return 0;
	}
	u16 *pu16Buf = (u16*)(&pu8buf[22]);
	m_SliCtrl_APDTest.SetPos(pu16Buf[0]);
	UpdateData(false);
	return 1;
	delete []pu8buf;
	delete pu16Buf;
}

//网络参数读取
LONG CSetProdDlg::OnProdNetWork(WPARAM wParam, LPARAM lParam)
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
	m_CIPAddIPSer.SetAddress(pu8buf[24],pu8buf[25],pu8buf[26],pu8buf[27]);//设备Ip
	m_CIPAddSubMaskSer.SetAddress(pu8buf[28],pu8buf[29],pu8buf[30],pu8buf[31]);//子网掩码
	m_CIPAddGetWaySer.SetAddress(pu8buf[32],pu8buf[33],pu8buf[34],pu8buf[35]);//默认网关
	m_u32PortSer =  (pu8buf[36]<<8)+pu8buf[37];//端口号
	m_strEquipMac.Format("%2.2X-%2.2X-%2.2X-%2.2X-%2.2X-%2.2X",(pu8buf[38]),(pu8buf[39]),(pu8buf[40]),(pu8buf[41]),(pu8buf[42]),(pu8buf[43]));//MAC
	LaserMAC = m_strEquipMac;
	if (LaserMAC.GetLength() > 12)
	{
		fileLaserMAC = LaserMAC;// LaserMAC.Mid(12);
	}
	UpdateData(false);
	GetDlgItem(IDC_BTN_NetWork_WR)->EnableWindow(true);
	return 1;
	delete []pu8buf;
}

//电机参数
LONG CSetProdDlg::OnProdMotoPid(WPARAM wParam, LPARAM lParam)
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
	//u16 *pu16Buf = (u16*)(&pu8buf[24]);
	m_MotoSpeed=((pu8buf[24]<<8)+(pu8buf[25]));
	m_fPID_p1 = ((pu8buf[26]<<8)+(pu8buf[27]))/100.0;
	if ((int(m_fPID_p1) >= 0) && (int(m_fPID_p1) <=2))
	{
		m_CombDataMode.SetCurSel(int(m_fPID_p1));
	}
    m_fPID_i1 = ((pu8buf[28]<<8)+(pu8buf[29]))/100.0;
	if ((int(m_fPID_i1) >= 0) && (int(m_fPID_i1) <=1))
	{
		m_CombLocateMode.SetCurSel(int(m_fPID_i1));
	}
	pointRatio = (pu8buf[30] << 8) + (pu8buf[31]);
    m_fPID_d1 = ((pu8buf[30]<<8)+(pu8buf[31]))/100.0;
	if (m_fPID_d1 != 0.00f)
	{
		m_fPID_d1=m_fPID_d1-4;
	}
	if ((int(m_fPID_d1) >= 0) && (int(m_fPID_d1) <=6))
	{
		m_CombPointsRatio.SetCurSel(int(m_fPID_d1));
	}

	m_CombMotoDir.SetCurSel(pu8buf[32]);
	m_CombMotoLoop.SetCurSel(pu8buf[33]);
	UpdateData(false);
	GetDlgItem(IDC_BTN_PIDPARAM_D1)->EnableWindow(true);
	return 1;
	delete []pu8buf;
	
}
//电机转速设置
LONG CSetProdDlg::OnProdMotoPidSet(WPARAM wParam, LPARAM lParam)
{
	char *pcBuf = (char *)wParam;
	int nBufSize = (int)lParam;
	UpdateData(false);
	return 1;
}



//设置APD参数
void CSetProdDlg::OnBnClickedBtnApdpibWr()
{
	UpdateData(TRUE);
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x20,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x05};
	ZhiLing(apdjc_zhiling,pn,0);
	UpdateData(TRUE);
	CString strtmp;
	float tmp1 = (float)(m_fAPDHvValue*100.0);
	strtmp.Format("%.0f",tmp1);
	u32 tmp = atoi(strtmp);
	apdjc_zhiling[26]=((u32)(tmp) >> 8);
	apdjc_zhiling[27]=((u32)(tmp) & 0xff);
    tmp1 = (float)(m_fAPDTemperValue*100.0);
	strtmp.Format("%.0f",tmp1);
    tmp = atoi(strtmp);
	apdjc_zhiling[28]=((u32)(tmp) >> 8);
	apdjc_zhiling[29]=((u32)(tmp) & 0xff);
	 tmp1 = (float)(m_fAPDHV_OP_Ratio*100.0);
	strtmp.Format("%.0f",tmp1);
	tmp = atoi(strtmp);
	apdjc_zhiling[30]=((u32)(tmp) >> 8);
	apdjc_zhiling[31]=((u32)(tmp) & 0xff);
	AfxGetMainWnd()->SendMessage(WM_SENDCMD,apdjc_zhilingSize, (LPARAM)apdjc_zhiling);	
	
}

//发送获取APD参数
void CSetProdDlg::OnBnClickedBtnApdpibRead()
{
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x04};
	ZhiLing(apdjc_zhiling_S,pn,0);
	AfxGetMainWnd()->PostMessage(WM_SENDCMD,apdjc_zhilingSize_S, (LPARAM)apdjc_zhiling_S);
	 
}

//电机gonglv测试返回
LONG CSetProdDlg::OnLaserGLTestBack(WPARAM wParam, LPARAM lParam)
{

	CString l_strbtnName;
	m_btnAPDTest.GetWindowText(l_strbtnName);
	if ("开启测试" == l_strbtnName)
	{
		
	}
	else if ("关闭测试" == l_strbtnName)
	{
			
	}
	return 1;
}

//APD开启测试
void CSetProdDlg::OnBnClickedBtnProdApdtest()
{
	CString l_strbtnName;
	m_btnAPDTest.GetWindowText(l_strbtnName);
	if ("开启测试" == l_strbtnName)
	{


		CTime time=CTime::GetCurrentTime();
		char Hour= time.GetHour();
		char Miu= time.GetMinute();
		char Sec= time.GetSecond();
		unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x0A};
		ZhiLing(APDTest_zhiling,pn,0);
		APDTest_zhiling[26]=0xAA;
		AfxGetMainWnd()->PostMessage(WM_SENDCMD,APDTest_zhilingSize, (LPARAM)APDTest_zhiling);


	}
	else if ("关闭测试" == l_strbtnName)
	{

		CTime time=CTime::GetCurrentTime();
		char Hour= time.GetHour();
		char Miu= time.GetMinute();
		char Sec= time.GetSecond();
		unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x0A};
		ZhiLing(APDTest_zhiling,pn,0);
		APDTest_zhiling[26]=0xBB;
		AfxGetMainWnd()->PostMessage(WM_SENDCMD,APDTest_zhilingSize, (LPARAM)APDTest_zhiling);

	}
}

//APD测试返回
LONG CSetProdDlg::OnAPDTestBack(WPARAM wParam, LPARAM lParam)
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
	CString l_strbtnName;
	m_btnAPDTest.GetWindowText(l_strbtnName);
	if ("开启测试" == l_strbtnName)
	{
		m_btnAPDTest.SetWindowText("关闭测试");
		m_SliCtrl_APDTest.SetPos(80*c_fAPDTestStep);
		(GetDlgItem(IDC_SLIDER_APDTEST))->EnableWindow(TRUE);
		(GetDlgItem(IDC_EDIT_APDTEST_APDHVTEST))->EnableWindow(TRUE);
		(GetDlgItem(IDC_BTN_APDTEST_D1))->EnableWindow(TRUE);
	}
	else if ("关闭测试" == l_strbtnName)
	{
			m_btnAPDTest.SetWindowText("开启测试");
			(GetDlgItem(IDC_SLIDER_APDTEST))->EnableWindow(FALSE);
		(GetDlgItem(IDC_EDIT_APDTEST_APDHVTEST))->EnableWindow(FALSE);
		(GetDlgItem(IDC_BTN_APDTEST_D1))->EnableWindow(FALSE);
	}
	delete []pu8buf;
	return 1;
}
void CSetProdDlg::OnNMReleasedcaptureSliderApdtest(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	UpdateData(true);
	int l_n32CurPos = m_SliCtrl_APDTest.GetPos();
	m_fAPDHVTestWr = (float)l_n32CurPos/c_fAPDTestStep;
	//OnBnClickedBtnApdtestD1();
	UpdateData(false);
}

//APD测试设置
void CSetProdDlg::OnBnClickedBtnApdtestD1()
{
	UpdateData(TRUE);
	if(m_fAPDHVTestWr < c_n32APDTestMin/c_fAPDTestStep || m_fAPDHVTestWr >c_n32APDTestMax/c_fAPDTestStep)
	{
		CString strMin,strMax,str;
		strMin.Format("%f",c_n32APDTestMin/c_fAPDTestStep);
		strMax.Format("%f",c_n32APDTestMax/c_fAPDTestStep);
		//str = _T("请输入")+strMin+"到"+strMax+"范围之内的数";
		MessageBox(str);
		return;
	}
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x0B};
	float tmp1= (float)(m_fAPDHVTestWr*100.0);
	CString strtmp;	
	strtmp.Format("%.0f",tmp1);
	u32 tmp = atoi(strtmp);
	ZhiLing(APDTest_zhiling,pn,0);
	APDTest_zhiling[26]=((u32)(tmp) >> 8);
	APDTest_zhiling[27]=((u32)(tmp) & 0xff);

	AfxGetMainWnd()->PostMessage(WM_SENDCMD,APDTest_zhilingSize, (LPARAM)APDTest_zhiling);

}

//获取点击转速及PID
void CSetProdDlg::OnBnClickedBtnPidparamUp1()
{
	UpdateData(TRUE);
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x06};
	ZhiLing(pid_zhiling_S,pn,0);
	AfxGetMainWnd()->PostMessage(WM_SENDCMD,pid_zhilingSize_S, (LPARAM)pid_zhiling_S);
}

//下载电机转速及PID
void CSetProdDlg::OnBnClickedBtnPidparamD1()
{
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x24,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x07};
	ZhiLing(pid_zhiling,pn,0);
	int tmp;
	tmp= (int)m_MotoSpeed;
	pid_zhiling[26]=(tmp>> 8);
    pid_zhiling[27]=(tmp & 0xff);
	//tmp= (int)(m_fPID_p1*100.0);
	if (m_CombDataMode.GetCurSel() == 0)   //距离
	{
		m_fPID_p1=0;
		tmp = 0;
	}
	else if (m_CombDataMode.GetCurSel() == 1)   //没切脉宽
	{
		m_fPID_p1=1;
		tmp = 100;
	}
	else if (m_CombDataMode.GetCurSel() == 2)   //切后脉宽
	{
		m_fPID_p1=2;
		tmp = 200;
	}
	pid_zhiling[28]=(tmp>> 8);
    pid_zhiling[29]=(tmp & 0xff);
	//tmp= (int)(m_fPID_i1*100.0);
	if (m_CombLocateMode.GetCurSel() == 0)   //关闭
	{
		m_fPID_i1=0;
		tmp = 0;
	}
	else if (m_CombLocateMode.GetCurSel() == 1)  //开启
	{
		m_fPID_i1=1;
		tmp = 100;
	}
    pid_zhiling[30]=(tmp>> 8);
    pid_zhiling[31]=(tmp & 0xff);
	//tmp= (int)(m_fPID_d1*100.0);
	if (m_CombPointsRatio.GetCurSel() == 0)   
	{
		m_fPID_d1=0;
		tmp = 0;
	}
	else if (m_CombPointsRatio.GetCurSel() == 1) 
	{
		m_fPID_d1=1;
		tmp = 500;
	}
	else if (m_CombPointsRatio.GetCurSel() == 2) 
	{
		m_fPID_d1=2;
		tmp = 600;
	}
	else if (m_CombPointsRatio.GetCurSel() == 3) 
	{
		m_fPID_d1=3;
		tmp = 700;
	}
	else if (m_CombPointsRatio.GetCurSel() == 4) 
	{
		m_fPID_d1=4;
		tmp = 800;
	}
	else if (m_CombPointsRatio.GetCurSel() == 5) 
	{
		m_fPID_d1=5;
		tmp = 900;
	}
	else if (m_CombPointsRatio.GetCurSel() == 6) 
	{
		m_fPID_d1=6;
		tmp = 1000;
	}
	pointRatio = tmp;
	pid_zhiling[32]=(tmp>> 8);
	pid_zhiling[33]=(tmp & 0xff);
	pid_zhiling[34]=((m_CombMotoDir.GetCurSel()) &0xff);
	pid_zhiling[35]=((m_CombMotoLoop.GetCurSel()) &0xff);
	f_SetProDialog = true;//允许弹出下载成功对话框
	AfxGetMainWnd()->PostMessage(WM_SENDCMD,pid_zhilingSize, (LPARAM)pid_zhiling);
}

//Ip和端口
void CSetProdDlg::OnBnClickedBtnNetworkRead()
{
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x02};
	ZhiLing(IpPort_zhiling_S,pn,0);
	AfxGetMainWnd()->PostMessage(WM_SENDCMD,IpPort_zhilingSize_S, (LPARAM)IpPort_zhiling_S);

}

//IP和网络端口下载
void CSetProdDlg::OnBnClickedBtnNetworkWr()
{
	UpdateData(TRUE);
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x2F,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x03};
	ZhiLing(IpPort_zhiling,pn,0);
	unsigned char ch1,ch2,ch3,ch4;
	m_CIPAddIPSer.GetAddress(ch1,ch2,ch3,ch4);
	IpPort_zhiling[26]=(u16)ch1;
	IpPort_zhiling[27]=(u16)ch2;
	IpPort_zhiling[28]=(u16)ch3;
	IpPort_zhiling[29]=(u16)ch4;
	m_CIPAddSubMaskSer.GetAddress(ch1,ch2,ch3,ch4);
	IpPort_zhiling[30]=(u16)ch1;
	IpPort_zhiling[31]=(u16)ch2;
	IpPort_zhiling[32]=(u16)ch3;
	IpPort_zhiling[33]=(u16)ch4;
	m_CIPAddGetWaySer.GetAddress(ch1,ch2,ch3,ch4);
	IpPort_zhiling[34]=(u16)ch1;
	IpPort_zhiling[35]=(u16)ch2;
	IpPort_zhiling[36]=(u16)ch3;
	IpPort_zhiling[37]=(u16)ch4;

	IpPort_zhiling[38]=(m_u32PortSer>>8);
	IpPort_zhiling[39]=(m_u32PortSer&0xff);

	unsigned char tmp[2]={0};
	memcpy(&tmp,m_strEquipMac.Left(2),2);
	IpPort_zhiling[40]=(ConvertHexChar(tmp[0])<<4)+ConvertHexChar(tmp[1]);
	memcpy(&tmp,m_strEquipMac.Mid(3,2),2);
	IpPort_zhiling[41]=(ConvertHexChar(tmp[0])<<4)+ConvertHexChar(tmp[1]);//MAC
	memcpy(&tmp,m_strEquipMac.Mid(6,2),2);
	IpPort_zhiling[42]=(ConvertHexChar(tmp[0])<<4)+ConvertHexChar(tmp[1]);
	memcpy(&tmp,m_strEquipMac.Mid(9,2),2);
	IpPort_zhiling[43]=(ConvertHexChar(tmp[0])<<4)+ConvertHexChar(tmp[1]);
	memcpy(&tmp,m_strEquipMac.Mid(12,2),2);
	IpPort_zhiling[44]=(ConvertHexChar(tmp[0])<<4)+ConvertHexChar(tmp[1]);
	memcpy(&tmp,m_strEquipMac.Mid(15,2),2);
	IpPort_zhiling[45]=(ConvertHexChar(tmp[0])<<4)+ConvertHexChar(tmp[1]);

	AfxGetMainWnd()->PostMessage(WM_SENDCMD,IpPort_zhilingSize, (LPARAM)IpPort_zhiling);
}


void CSetProdDlg::OnNMCustomdrawSliderApdtest(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	*pResult = 0;
	UpdateData(true);
	int l_n32CurPos = m_SliCtrl_APDTest.GetPos();
	m_fAPDHVTestWr = (float)l_n32CurPos/c_fAPDTestStep;
	UpdateData(false);
}
