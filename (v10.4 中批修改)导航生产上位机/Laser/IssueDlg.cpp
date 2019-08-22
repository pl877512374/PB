// IssueDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "IssueDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "WriteLog.h"
#include "NetPublish.h"
#include "InsideApI.h"
#include "Dll_Func.h"
#include "Mwic_32.h"
#include "NetWork\NetworkUtils.h"
IssueLogInfo g_sIssueInfo;
#define SECRET_TEST

int g_n32IssueCnt;
extern char g_recvother[2000];
// 串口
BYTE ComRecvBuf[1024];
int ComRecvLen = 0;
int	g_serial_fd = 0;			// 串口文件描述符
BOOL g_bOpenCom = FALSE;

unsigned char g_szKeyB[16];
unsigned char szDivideRootResults[256];
unsigned char szDecResults[256];	//根密钥解密后的明文
unsigned char szChipID[256];	//芯片ID
unsigned char szProductCode[256];	//芯片ID
unsigned char szRootKeyDec[256];	//根密钥解密后的明文
unsigned char szDividedByProductResults[256];
unsigned char szTransferKeyInfo[256];
BOOL g_com_recv;
static unsigned char s_nNum;//记录表格中索引
CString strSavePath;
int g_7TimeCount = 0;//记录发送广播次数，用来清空界面上的ip控件信息
CString DelTime,DelID,DelProductCode;//删除数据时记录时间、id、产品代码
int DelRow;//删除数据所选行
// CIssueDlg 对话框

IMPLEMENT_DYNAMIC(CIssueDlg, CDialogEx)

	CIssueDlg::CIssueDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CIssueDlg::IDD, pParent)
{

}

CIssueDlg::~CIssueDlg()
{
}

void CIssueDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RICHEDIT_SHOWINFO, m_ShowInfo);
	DDX_Control(pDX, IDC_BUTTON_ISSUE, m_btnIssue);
	DDX_Control(pDX, IDC_BUTTON_OPENCT, m_OpenCom);
	DDX_Control(pDX, IDC_COMBO_COM, m_ComNum);
	DDX_Control(pDX, IDC_COMBO_BAUD, m_CombBaud);
	DDX_Control(pDX, IDC_COMBO_EQUPSEL, m_CombEquipSel);
	DDX_Control(pDX, IDC_BUTTON_CHECKSTATE, m_btnCheckState);
}


BEGIN_MESSAGE_MAP(CIssueDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ISSUE, &CIssueDlg::OnBnClickedButtonIssue)
	ON_BN_CLICKED(IDC_BUTTON_CLEARLIST, &CIssueDlg::OnBnClickedButtonClearlist)
	ON_BN_CLICKED(IDC_BUTTON_OPENCT, &CIssueDlg::OnBnClickedButtonOpenct)
	ON_BN_CLICKED(IDC_BUTTON_CHECKSTATE, &CIssueDlg::OnBnClickedButtonCheckstate)
	ON_BN_CLICKED(IDC_BUTTON_ISSUE2, &CIssueDlg::OnBnClickedButtonIssue2)
	ON_WM_TIMER()
	ON_MESSAGE(WM_ISSUELOG,OnIssueLog)
END_MESSAGE_MAP()

LONG CIssueDlg::OnIssueLog(WPARAM wParam, LPARAM lParam)
{
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32Out]);
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	g_sIssueInfo.m_n32Out = (g_sIssueInfo.m_n32Out + 1)%ISSUELOGNUM;
	return 1;
}
// CIssueDlg 消息处理程序
UINT ThreadWriteIssueLog(LPVOID pParam)
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame *pMainDlg;
	CIssueDlg *pDlg = (CIssueDlg *)pParam;
	pMainDlg = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	while(1)
	{
		Sleep(10);
		if(g_sIssueInfo.m_n32In != g_sIssueInfo.m_n32Out)
		{
			PostMessage(pDlg->GetSafeHwnd(),WM_ISSUELOG,NULL,NULL);					
		}

	}
}
UINT ThreadNetKeyState(LPVOID pParam)
{
	// TODO: 在此添加控件通知处理程序代码
	CMainFrame *pMainDlg;
	CIssueDlg *pDlg = (CIssueDlg *)pParam;
	pMainDlg = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	int l_n32FailedFlag = 0;//标识密钥已下载，但发行失败的情况
	memset(ComRecvBuf,0,sizeof(ComRecvBuf));
	g_frame_flag = 0; 

	CString strGetComState;

	if(!pMainDlg->m_NetConnection.bConnected)
	{
		pDlg->ShowInfomation(CString("请先打开网络！\n"));
		pDlg->m_btnCheckState.EnableWindow(true);
		pDlg->m_btnIssue.EnableWindow(true);

		return 1;
	}
	REP_DATA sdata;
	unsigned char chipID[8] = "";
	unsigned char subkey[8] = "";
	unsigned char calcrand[8] = "";
	unsigned char hsmac[20] = "";
	unsigned char page_data[32]= "";
	unsigned char code[8] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88}; 

	int page = 0 ;
	long ret = 0;
	pDlg->GetFactoryID_Net_rq();//获取秘钥ID
	ret = pDlg->GetFactoryID_Net_rs((char *)chipID);//拷贝秘钥ID
	memcpy(sdata.st_chipID,chipID,8);
	memcpy(szChipID,chipID,8);

	if (ret != 0)
	{
		pDlg->ShowInfomation(CString("获取芯片ID返回失败\n"));
		pDlg->m_btnCheckState.EnableWindow(true);
		pDlg->m_btnIssue.EnableWindow(true);
		//pDlg->m_btnIssue.EnableWindow(TRUE);
		//PostMessage(GetSafeHwnd(),WM_BTNEN,NULL,NULL);
		return 1;
	}
	CString strChipID;
	strChipID = "";
	for(int i = 0;i < 8; i++)
	{
		CString strtmp;
		strtmp.Format("%x",chipID[i]);
		strChipID += strtmp;
	}
	pDlg->ShowInfomation(CString("芯片ID号为："));
	pDlg->ShowInfomation(strChipID);
	pDlg->ShowInfomation(CString("\n"));

	int ret1 = ReadFailedIdLog(strChipID);

	if(ret1 == 1)//若不存在才执行密钥版本检测
	{
		pDlg->ShowInfomation(CString("该密钥发行异常，处于未发行或未入库状态，请重新下载!\n"));
		pDlg->m_btnCheckState.EnableWindow(true);
		pDlg->m_btnIssue.EnableWindow(true);
		return 1;
	}
	pDlg->m_btnCheckState.EnableWindow(true);
	pDlg->m_btnIssue.EnableWindow(true);
	pDlg->GetKeyState_Net_rq();//发送查询秘钥状态
	pDlg->GetKeyState_Net_rs();
}

extern CRITICAL_SECTION g_netcs;
extern u8 g_u8LoadParamDoneFlag;
UINT ThreadNetDownLoad(LPVOID pParam)
{

	CMainFrame *pMainDlg;
	CIssueDlg *pDlg = (CIssueDlg *)pParam;
	pMainDlg = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	//pMainDlg->KillTimer(0);
	//while(g_u8LoadParamDoneFlag == 0)
	//{
	//	Sleep(100);
	//}
	//int ret2 =pDlg->CheckIssueMaxCnt(pDlg->m_iDevId, CARD_TYPE_UP, szDivideRootResults);

	//int ret3 = pDlg->UpdateIssueMaxCnt(pDlg->m_iDevId, CARD_TYPE_UP,szTransferKeyInfo);
	///*if(ret2 < 0 || ret3 < 0)
	//{
	//	return 1;
	//}*/
	//return 1;
	CGraphForm2dState *pView1;
	//pView1 = (CGraphForm2dState*) pMainDlg->m_wndSplitter.GetPane(0,1);
	//pView1->KillTimer(0);
	//pView1->KillTimer(1);
	//pView1->KillTimer(2);
	//pView1->KillTimer(3);
	int l_n32FailedFlag = 0;//标识密钥已下载，但发行失败的情况
	memset(ComRecvBuf,0,sizeof(ComRecvBuf));
	g_frame_flag = 0; 

	CString strGetComState;

	if(!pMainDlg->m_NetConnection.bConnected)
	{
		pDlg->ShowInfomation(CString("请先打开网络！\n"));
		pDlg->m_btnIssue.EnableWindow(true);
		pDlg->m_btnCheckState.EnableWindow(true);
		return -1;
	}
	REP_DATA sdata;
	unsigned char chipID[8] = "";
	unsigned char subkey[8] = "";
	unsigned char calcrand[8] = "";
	unsigned char hsmac[20] = "";
	unsigned char page_data[32]= "";
	unsigned char code[8] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88}; 

	int page = 0 ;
	long ret = 0;

	ret = pDlg->CheckIssueMaxCnt(pDlg->m_iDevId, CARD_TYPE_UP, szDivideRootResults);
	if(ret == USER_MAXISSUEERR)
	{
		//MessageBox(CString("发行失败\r\n达到最大发行次数！"), _T("提示"), MB_ICONINFORMATION);
		pDlg->ShowInfomation(CString("发行失败\r\n达到最大发行次数！\r\n"));
		//PostMessage(pDlg->GetSafeHwnd(),WM_BTNEN,NULL,NULL);
		pDlg->m_btnIssue.EnableWindow(true);
		pDlg->m_btnCheckState.EnableWindow(true);
		return -1;
	}
	if((ret == USER_FAILED) || (ret == USER_READBINARYERR))
	{
		//MessageBox(CString("发行失败\r\n请检查根密钥卡！"), _T("提示"), MB_ICONINFORMATION);
		pDlg->ShowInfomation(CString("发行失败\r\n请检查根密钥卡！\n"));
		//PostMessage(pDlg->GetSafeHwnd(),WM_BTNEN,NULL,NULL);
		pDlg->m_btnIssue.EnableWindow(true);
		pDlg->m_btnCheckState.EnableWindow(true);
		return -1;
	}



	pDlg->GetFactoryID_Net_rq();
	ret = pDlg->GetFactoryID_Net_rs((char *)chipID);//获取芯片ID
	memcpy(sdata.st_chipID,chipID,8);
	memcpy(szChipID,chipID,8);

	if (ret != 0)
	{
		pDlg->ShowInfomation(CString("获取芯片ID返回失败\n"));
		//pDlg->m_btnIssue.EnableWindow(TRUE);
		//PostMessage(pDlg->GetSafeHwnd(),WM_BTNEN,NULL,NULL);
		pDlg->m_btnIssue.EnableWindow(true);
		pDlg->m_btnCheckState.EnableWindow(true);
		return -1;
	}

	//查询当前Id是否存在密钥已下载，但发行失败的情况20140901
	CString strChipID;
	strChipID = "";
	for (int i = 0;i<8;i++)
	{
		//strChipID+=chipID[i];
		CString strtmp;
		strtmp.Format("%x",chipID[i]);
		strChipID += strtmp;
	}

	//#ifndef SECRET_TEST
	int ret1 = ReadFailedIdLog(strChipID);

	if(ret1 != 1)//若不存在才执行密钥版本检测
	{
		ret = pDlg->CheckLatestKeyVersion(sdata,2);
		if(ret == 0)
		{
			pDlg->ShowInfomation(CString("密钥已是最新版本，无需更新\n"));
			//PostMessage(pDlg->GetSafeHwnd(),WM_BTNEN,NULL,NULL);
			pDlg->m_btnIssue.EnableWindow(true);
			pDlg->m_btnCheckState.EnableWindow(true);
			return -1;
		}
		else if(ret == -1)
		{
			//PostMessage(pDlg->GetSafeHwnd(),WM_BTNEN,NULL,NULL);
			pDlg->m_btnIssue.EnableWindow(true);
			pDlg->m_btnCheckState.EnableWindow(true);
			return -1;
		}
		WriteFailedIdLog(strChipID);
	}
	//#endif
	pDlg->ReplacesKey_Net_rq(sdata.st_chipID,szTransferKeyInfo,subkey);
	memcpy(sdata.st_subkey,subkey,8);
	ret = pDlg->ReplacesKey_Net_rs();

	if (ret != 0)
	{
		pDlg->ShowInfomation(CString("替换密钥返回失败\n"));
		pDlg->m_btnIssue.EnableWindow(true);
		pDlg->m_btnCheckState.EnableWindow(true);
		//PostMessage(pDlg->GetSafeHwnd(),WM_BTNEN,NULL,NULL);
		//pDlg->m_btnIssue.EnableWindow(TRUE);
		return -1;
	}

	sdata.st_page_rand = page ;
	ret = pDlg->CalcSendRands_Net_rq(sdata.st_page_rand,calcrand);	//随机数计算
	ret = pDlg->CalcSendRands_Net_rs(hsmac);
	memcpy(sdata.st_rand,calcrand,8);
	memset(sdata.st_hard_mac,0x00,21);
	memcpy(sdata.st_hard_mac,hsmac,20);
	if ( ret != 0)
	{
		//MessageBox("随机数返回失败", _T("提示"), MB_ICONINFORMATION);
		pDlg->ShowInfomation(CString("随机数返回失败\n"));
		pDlg->ShowInfomation(CString("密钥可能已经下载，但未做记录，请重新发行\n"));
		pDlg->m_btnIssue.EnableWindow(true);
		pDlg->m_btnCheckState.EnableWindow(true);
		//PostMessage(pDlg->GetSafeHwnd(),WM_BTNEN,NULL,NULL);
		//pDlg->m_btnIssue.EnableWindow(TRUE);
		return -1;
	}

	sdata.st_page_read = page;
	ret =pDlg->ReadPage_Net_rq(sdata.st_page_read);
	ret = pDlg->ReadPage_Net_rs(page_data);
	memcpy(sdata.st_page_data,page_data,32);
	if (ret != 0)
	{
		//MessageBox("存储器返回数据失败", _T("提示"), MB_ICONINFORMATION);
		pDlg->ShowInfomation(CString("存储器返回数据失败\n"));
		pDlg->ShowInfomation(CString("密钥可能已经下载，但未做记录，请重新发行\n"));
		pDlg->m_btnIssue.EnableWindow(true);
		pDlg->m_btnCheckState.EnableWindow(true);
		//PostMessage(pDlg->GetSafeHwnd(),WM_BTNEN,NULL,NULL);
		//pDlg->m_btnIssue.EnableWindow(TRUE);

		return -1;
	}

	memcpy(SHAVM_Message, sdata.st_subkey, 4);			/*	Copy the devicesecret to input buffer.	*/
	memcpy(&SHAVM_Message[4], sdata.st_page_data, 32);	/*	Fill 32 bytes page data to input buffer.	*/
	memcpy(&SHAVM_Message[36], &sdata.st_rand[2] , 2);	/*	Set 2 bytes of challenge to input buffer.	*/
	SHAVM_Message[38] = 0xFF;
	SHAVM_Message[39] = 0xFF;
	SHAVM_Message[40] = 0x40 | sdata.st_page_read >> 5;			/*	Set MPX.	*/ 
	memcpy(&SHAVM_Message[41], sdata.st_chipID, 7);			/*	Input ROMID	*/
	memcpy(&SHAVM_Message[48], &sdata.st_subkey[4], 4);/*	Input another 4 bytes of basic secret to input buffer.	*/
	memcpy(&SHAVM_Message[52], &sdata.st_rand[4], 3);	/*	Set the input buffer as defined by datasheet.	*/
	SHAVM_Message[55] = 0x80;						/*	Set the input buffer as defined by datasheet.	*/					
	memset(&SHAVM_Message[56], 0x00, 6);			/*	Set the input buffer as defined by datasheet.	*/		
	SHAVM_Message[62] = 0x01;						/*	Set the input buffer as defined by datasheet.	*/						
	SHAVM_Message[63] = 0xB8;
	pDlg->SHAVM_Compute();


	memset(sdata.st_soft_mac,0x00,21);
	memcpy(sdata.st_soft_mac,SHAVM_MAC, 20);
	ret = strcmp((const char*)sdata.st_hard_mac,(const char*)sdata.st_soft_mac);
	if (ret != 0)
	{
		//MessageBox("MAC校验出错", _T("提示"), MB_ICONINFORMATION);
		pDlg->ShowInfomation(CString("MAC校验出错\n"));
		pDlg->ShowInfomation(CString("密钥可能已经下载，但未做记录，请重新发行\n"));
		//PostMessage(pDlg->GetSafeHwnd(),WM_BTNEN,NULL,NULL);
		pDlg->m_btnIssue.EnableWindow(true);
		pDlg->m_btnCheckState.EnableWindow(true);
		//pDlg->m_btnIssue.EnableWindow(TRUE);
		//pDlg->m_btnIssue.EnableWindow(TRUE);
		return -1;
	}
	sdata.st_page_write = page;
	sdata.st_page_sub_write = page;
	pDlg->WriteMemo_Net_rq(sdata.st_page_write,sdata.st_page_sub_write);	
	ret = pDlg->WriteMemo_Net_rs();
	if (ret != 0)
	{
		//MessageBox("写数据出错", _T("提示"), MB_ICONINFORMATION);
		pDlg->ShowInfomation(CString("写数据出错\n"));
		pDlg->ShowInfomation(CString("密钥可能已经下载，但未做记录，请重新发行\n"));
		//PostMessage(pDlg->GetSafeHwnd(),WM_BTNEN,NULL,NULL);
		pDlg->m_btnIssue.EnableWindow(true);
		pDlg->m_btnCheckState.EnableWindow(true);
		//pDlg->m_btnIssue.EnableWindow(TRUE);
		return -1;
	}
	else
	{
		memcpy(szDividedByProductResults,subkey,sizeof(unsigned char)*8);
		//#ifndef SECRET_TEST
		g_n32IssueCnt--;

		long ret = pDlg->UpdateIssueMaxCnt(pDlg->m_iDevId, CARD_TYPE_UP,szTransferKeyInfo);
		if(ret != USER_SUCCESS)
		{
			//MessageBox("发行失败，请检查根密钥卡！", _T("提示"), MB_ICONINFORMATION);
			pDlg->ShowInfomation(CString("发行失败，请检查根密钥卡！\n"));
			pDlg->ShowInfomation(CString("密钥可能已经下载，但未做记录，请检测密钥卡后重新发行\n"));
			//PostMessage(pDlg->GetSafeHwnd(),WM_BTNEN,NULL,NULL);
			pDlg->m_btnIssue.EnableWindow(true);
			pDlg->m_btnCheckState.EnableWindow(true);
			//pDlg->m_btnIssue.EnableWindow(TRUE);

			return -1;
		}

		pDlg->WriteToBLog(strSavePath);
		//#endif
		//MessageBox("发行成功", _T("提示"), MB_ICONINFORMATION);
		pDlg->ShowInfomation(CString("发行成功！\n"));
		//ShowInfomation(CString("剩余发行次数为！"));
		CString str;
		str.Format("剩余发行次数为%d次！\r\n",g_n32IssueCnt);
		pDlg->ShowInfomation(str);
		//#ifndef SECRET_TEST
		//发行成功后，再将失败列表中的数据删除
		DeletFailedIdLog(strChipID);
		//#endif
		//pDlg->m_btnIssue.EnableWindow(TRUE);
		//PostMessage(pDlg->GetSafeHwnd(),WM_BTNEN,NULL,NULL);
		/*int nBufSize = 25;//重启设备
		char cBuf[26] = {(char)0x02, (char)0x02, (char)0x02, (char)0x02, (char)0x00, (char)0x00, (char)0x00, (char)0x11, (char)0x73,(char)0x47, (char)0x4E, 
		(char)0x20, (char)0x4C, (char)0x4D, (char)0x44, (char)0x73, (char)0x63, (char)0x61, (char)0x6E, (char)0x64, (char)0x61, (char)0x74, (char)0x61, (char)0x20, (char)0x00};*/
		CTime time=CTime::GetCurrentTime();
		char Hour= time.GetHour();
		char Miu= time.GetMinute();
		char Sec= time.GetSecond();
		unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x04,0x05};
		ZhiLing(Reset_zhiling,pn,0);
		CMainFrame *pMainDlg;	
		pMainDlg = (CMainFrame *)AfxGetApp()->m_pMainWnd;
		pMainDlg->SendMessage(WM_SENDCMD,Reset_zhilingSize, (LPARAM)Reset_zhiling);
		::AfxBeginThread(ConnectThreadProc,pMainDlg);
		//ShowInfomation(CString("正在断开网络...\n"));
		//OnBnClickedButtonOpennet();
		//::AfxBeginThread(ConnectThreadProc,this);
	}
	pDlg->m_btnIssue.EnableWindow(true);
	pDlg->m_btnCheckState.EnableWindow(true);
	return 1;
}
void CIssueDlg::OnBnClickedButtonIssue()
{
	// TODO: 在此添加控件通知处理程序代码

	CMainFrame *pMainDlg;
	pMainDlg = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMainDlg->KillTimer(0);

	CString strGetComState;

	if(!pMainDlg->m_NetConnection.bConnected)
	{
		ShowInfomation(CString("请先打开网络！\n"));
		m_btnIssue.EnableWindow(true);
		m_btnCheckState.EnableWindow(true);
		return ;
	}
	m_btnIssue.EnableWindow(false);
	m_btnCheckState.EnableWindow(false);
	ShowInfomation(CString("发行中，请勿进行其他操作！！！！！\n"));
	//OnBnClickedButtonIssue2();
	AfxBeginThread(ThreadNetDownLoad,this,THREAD_PRIORITY_NORMAL,0,0,NULL);
}


void CIssueDlg::ShowInfomation(CString& info)
{
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =info;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(info);
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

long CIssueDlg::CheckIssueMaxCnt(HANDLE iDevID, unsigned char CardType, unsigned char *Key)
{

	//// TODO: 在此添加控件通知处理程序代码
	//// TODO: 在此添加控件通知处理程序代码

	long ret = 0;
	int sLen = 0;
	int rLen = 0;
	unsigned char szRecvBuf[256];
	CString strTemp;
	//认证？


	//复位
	ret = sam_slt_reset(iDevID, CardType, (short*)&rLen, szRecvBuf);
	if (ret != 0)
	{
		strTemp.Format(_T("\n复位失败: 状态码 = 0x%.4x:\n"), ret);

		/*g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] = strTemp;
		g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;*/
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return -1;//USER_RESETERR;
	}
	Sleep(1000);
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] = _T("\n复位成功\n");
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("\n复位成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);



	//选择主目录 00A4000C023F00
	unsigned char szChooseRootDir[ICC_COMMAN_BUF_LEN] = {0x00, 0xA4, 0x00, 0x0C, 0x02, 0xDF, 0x01};
	sLen = 7;
	memset(szRecvBuf,0,sizeof(szRecvBuf));
	ret = IC_Command(iDevID, CardType, sLen, szChooseRootDir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("选择主目录失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}
	strTemp = _T("选择主目录成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("选择主目录成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	unsigned char szRead0017File[ICC_COMMAN_BUF_LEN] = {0x00, 0xB0, 0x97, 0x00, 0x1E};
	sLen = 5;
	ret = IC_Command(iDevID, CardType, sLen, szRead0017File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("选择DF01目录失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp = _T("选择DF01目录成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("选择DF01目录成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	unsigned char sztmp[256];

	memcpy(szTransferKeyInfo,szRecvBuf,sizeof(unsigned char)*16);
	g_n32IssueCnt = ((szRecvBuf[16]<<8)|szRecvBuf[17]);
	if(g_n32IssueCnt <= 0)
		return USER_MAXISSUEERR;
	return USER_SUCCESS;
}

int CIssueDlg::GetFactoryID_Net_rq()
{
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	static char zhiling[34]={0xFF,0xAA,0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00};//获取秘钥ID

	int len = 34;
	ResetEvent(GETFACTORYID_EVENT_NET_rs);

	CMainFrame *pMainDlg;
	//pMainDlg = (CMainFrame *)AfxGetMainWnd();
	pMainDlg = (CMainFrame *)AfxGetApp()->m_pMainWnd;

	pMainDlg->m_NetConnection.pcSendDataBuf = zhiling;
	pMainDlg->m_NetConnection.nSendDataBufSize = len;

	SendData(&pMainDlg->m_NetConnection);
	return 0;
}




int  CIssueDlg::CheckLatestKeyVersion(REP_DATA sdata,int mode)
{

	unsigned char chipID[8] = "";
	unsigned char subkey[8] = "";
	unsigned char calcrand[8] = "";
	unsigned char hsmac[20] = "";
	unsigned char page_data[32]= "";
	int ret = SendKey_rq(sdata.st_chipID,szTransferKeyInfo,subkey); //动态库计算,subkey改为8字节
	memcpy(sdata.st_subkey,subkey,8);
	sdata.st_page_rand = 0 ;
	if(mode == 1)
	{
		GetKeyState_rq();
		ret = GetKeyState_rs();
		if (ret != 0)
		{
			return 1;
		}
		CalcSendRands(sdata.st_page_rand,calcrand);	
		memcpy(sdata.st_rand,calcrand,8);
		ret = CalcSendRands_rs(hsmac);
		memset(sdata.st_hard_mac,0x00,21);
		memcpy(sdata.st_hard_mac,hsmac,20);
		if ( ret != 0)
		{
			//MessageBox("随机数返回失败", _T("提示"), MB_ICONINFORMATION);
			ShowInfomation(CString("随机数返回失败\n"));

			m_btnIssue.EnableWindow(TRUE);
			return -1;
		}
		sdata.st_page_read = 0;
		ReadPage(sdata.st_page_read);
		ret = ReadPage_rs(page_data);
		memcpy(sdata.st_page_data,page_data,32);
		if (ret != 0)
		{
			//MessageBox("存储器返回数据失败", _T("提示"), MB_ICONINFORMATION);
			ShowInfomation(CString("存储器返回数据失败\n"));
			m_btnIssue.EnableWindow(TRUE);

			return -1;
		}
	}
	else if(mode == 2)
	{
		GetKeyState_Net_rq();
		ret = GetKeyState_Net_rs();
		if (ret != 0)
		{
			return 1;
		}
		CalcSendRands_Net_rq(sdata.st_page_rand,calcrand);	
		memcpy(sdata.st_rand,calcrand,8);
		ret = CalcSendRands_Net_rs(hsmac);
		memset(sdata.st_hard_mac,0x00,21);
		memcpy(sdata.st_hard_mac,hsmac,20);
		if ( ret != 0)
		{
			//MessageBox("随机数返回失败", _T("提示"), MB_ICONINFORMATION);
			ShowInfomation(CString("随机数返回失败\n"));
			m_btnIssue.EnableWindow(TRUE);
			return -1;
		}
		sdata.st_page_read = 0;
		ReadPage_Net_rq(sdata.st_page_read);
		ret = ReadPage_Net_rs(page_data);
		memcpy(sdata.st_page_data,page_data,32);
		if (ret != 0)
		{
			//MessageBox("存储器返回数据失败", _T("提示"), MB_ICONINFORMATION);
			ShowInfomation(CString("存储器返回数据失败\n"));
			m_btnIssue.EnableWindow(TRUE);

			return -1;
		}
	}
	//调用动态库中的hash算法计算出hash值，然后比较
	//hashCalMac(sdata.st_subkey,sdata.st_page_data,sdata.st_rand,sdata.st_page_read,sdata.st_chipID,sdata.st_soft_mac);

	memcpy(SHAVM_Message, sdata.st_subkey, 4);			/*	Copy the devicesecret to input buffer.	*/
	memcpy(&SHAVM_Message[4], sdata.st_page_data, 32);	/*	Fill 32 bytes page data to input buffer.	*/
	memcpy(&SHAVM_Message[36], &sdata.st_rand[2] , 2);	/*	Set 2 bytes of challenge to input buffer.	*/
	SHAVM_Message[38] = 0xFF;
	SHAVM_Message[39] = 0xFF;
	SHAVM_Message[40] = 0x40 | sdata.st_page_read >> 5;			/*	Set MPX.	*/ 
	memcpy(&SHAVM_Message[41], sdata.st_chipID, 7);			/*	Input ROMID	*/
	memcpy(&SHAVM_Message[48], &sdata.st_subkey[4], 4);/*	Input another 4 bytes of basic secret to input buffer.	*/
	memcpy(&SHAVM_Message[52], &sdata.st_rand[4], 3);	/*	Set the input buffer as defined by datasheet.	*/
	SHAVM_Message[55] = 0x80;						/*	Set the input buffer as defined by datasheet.	*/					
	memset(&SHAVM_Message[56], 0x00, 6);			/*	Set the input buffer as defined by datasheet.	*/		
	SHAVM_Message[62] = 0x01;						/*	Set the input buffer as defined by datasheet.	*/						
	SHAVM_Message[63] = 0xB8;
	SHAVM_Compute();


	memset(sdata.st_soft_mac,0x00,21);
	memcpy(sdata.st_soft_mac,SHAVM_MAC, 20);
	/*ret = strcmp((const char*)sdata.st_hard_mac,(const char*)sdata.st_soft_mac);
	if (ret != 0)
	{
	return 1;
	}*/

	return 0;
}

void CIssueDlg::SHAVM_Compute(void)
{
	SHAVM_KTN[0]=(signed long)0x5a827999;        //Kt序列
	SHAVM_KTN[1]=(signed long)0x6ed9eba1;
	SHAVM_KTN[2]=(signed long)0x8f1bbcdc;
	SHAVM_KTN[3]=(signed long)0xca62c1d6;   
	for(SHAVM_cnt=0; SHAVM_cnt<16; SHAVM_cnt++)   //Wt序列  0<=t<=15
	{
		SHAVM_MTword[SHAVM_cnt]
		= (((signed long)SHAVM_Message[SHAVM_cnt*4]&0x00FF) << 24)
			| (((signed long)SHAVM_Message[SHAVM_cnt*4+1]&0x00FF) << 16)
			| (((signed long)SHAVM_Message[SHAVM_cnt*4+2]&0x00FF) << 8)
			|  ((signed long)SHAVM_Message[SHAVM_cnt*4+3]&0x00FF);
	}

	for(; SHAVM_cnt<80; SHAVM_cnt++)      //Wt序列  16<=t<=79
	{
		SHAVM_Temp
			= SHAVM_MTword[SHAVM_cnt-3]  ^ SHAVM_MTword[SHAVM_cnt-8]
		^ SHAVM_MTword[SHAVM_cnt-14] ^ SHAVM_MTword[SHAVM_cnt-16];
		SHAVM_MTword[SHAVM_cnt]           
		= ((SHAVM_Temp << 1) & 0xFFFFFFFE)     //循环移位
			| ((SHAVM_Temp >> 31) & 0x00000001);
	}

	SHAVM_Hash[0] = 0x67452301;          //ABCDE初始值
	SHAVM_Hash[1] = 0xEFCDAB89;
	SHAVM_Hash[2] = 0x98BADCFE;
	SHAVM_Hash[3] = 0x10325476;
	SHAVM_Hash[4] = 0xC3D2E1F0;

	for(SHAVM_cnt=0; SHAVM_cnt<80; SHAVM_cnt++)
	{
		SHAVM_Temp
			= ((SHAVM_Hash[0] << 5) & 0xFFFFFFE0)
			| ((SHAVM_Hash[0] >> 27) & 0x0000001F);
		if(SHAVM_cnt<20)
			SHAVM_Temp += ((SHAVM_Hash[1]&SHAVM_Hash[2])|((~SHAVM_Hash[1])&SHAVM_Hash[3]));
		else if(SHAVM_cnt<40)
			SHAVM_Temp += (SHAVM_Hash[1]^SHAVM_Hash[2]^SHAVM_Hash[3]);
		else if(SHAVM_cnt<60)
			SHAVM_Temp += ((SHAVM_Hash[1]&SHAVM_Hash[2])
			|(SHAVM_Hash[1]&SHAVM_Hash[3])
			|(SHAVM_Hash[2]&SHAVM_Hash[3]));
		else
			SHAVM_Temp += (SHAVM_Hash[1]^SHAVM_Hash[2]^SHAVM_Hash[3]);
		SHAVM_Temp += SHAVM_Hash[4] + SHAVM_KTN[SHAVM_cnt/20]
		+ SHAVM_MTword[SHAVM_cnt];
		SHAVM_Hash[4] = SHAVM_Hash[3];
		SHAVM_Hash[3] = SHAVM_Hash[2];
		SHAVM_Hash[2]
		= ((SHAVM_Hash[1] << 30) & 0xC0000000)
			| ((SHAVM_Hash[1] >> 2) & 0x3FFFFFFF);
		SHAVM_Hash[1] = SHAVM_Hash[0];
		SHAVM_Hash[0] = SHAVM_Temp;
	}

	//iButtons use LSB first, so we have to turn
	//the result around a little bit.  Instead of
	//result A-B-C-D-E, our result is E-D-C-B-A,
	//where each letter represents four bytes of
	//the result.
	for (SHAVM_cnt=0; SHAVM_cnt<5; SHAVM_cnt++) {	
		SHAVM_Temp = SHAVM_Hash[4-SHAVM_cnt];
		SHAVM_MAC[((SHAVM_cnt)*4)+0] = (unsigned char)SHAVM_Temp;
		SHAVM_Temp >>= 8;
		SHAVM_MAC[((SHAVM_cnt)*4)+1] = (unsigned char)SHAVM_Temp;
		SHAVM_Temp >>= 8;
		SHAVM_MAC[((SHAVM_cnt)*4)+2] = (unsigned char)SHAVM_Temp;
		SHAVM_Temp >>= 8;
		SHAVM_MAC[((SHAVM_cnt)*4)+3] = (unsigned char)SHAVM_Temp;
	}
}

long CIssueDlg::UpdateIssueMaxCnt(HANDLE iDevId, unsigned char CarType, unsigned char *Key)
{
	long ret = 0;
	int sLen = 0;//发送指令的长度
	int rLen = 0;//返回指令的长度
	CString strTemp;
	unsigned char szRecvBuf[256];//指令返回缓冲区
	//PostMessage(WM_ISSUELOG, strTemp, (LPARAM)g_cPauseBuf);
	//复位

	ret = sam_slt_reset(iDevId, CarType, (short*)&rLen, szRecvBuf);
	if (ret != 0)
	{
		strTemp.Format(_T("\n复位失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}	
	Sleep(1000);
	strTemp = _T("\n复位成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("\n复位成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//选择主目录 00A4000C023F00
	unsigned char szChooseRootDir[ICC_COMMAN_BUF_LEN] = {0x00, 0xA4, 0x00, 0x0C, 0x02, 0x3F, 0x00};
	sLen = 7;
	memset(szRecvBuf,0,sizeof(szRecvBuf));
	ret = IC_Command(iDevId, CarType, sLen, szChooseRootDir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("选择主目录失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp = _T("选择主目录成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("选择主目录成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	memset(szRecvBuf,0,sizeof(szRecvBuf));
	//取随机数 0084000008
	unsigned char szGetRandom[ICC_COMMAN_BUF_LEN] = {0x00, 0x84, 0x00, 0x00, 0x08};
	sLen = 5;
	ret = IC_Command(iDevId, CarType, sLen, szGetRandom, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("取随机数失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}
	strTemp.Format(_T("取随机数成功\n"));
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(strTemp);
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0); 

	//将8字节随机数作为报文，使用16字节0x00作为密钥，进行3DES运算
	byte szZero[16];
	memset(szZero, 0x00, 16);
	byte szTriDesResult[16];
	TripleDES(1, szZero, 8, szRecvBuf, szTriDesResult);

	//外部认证 0082000008+上面3DES运算产生的8字节密文
	unsigned char szExternalAuthentication[ICC_COMMAN_BUF_LEN] = {0x00, 0x82, 0x00, 0x00, 0x08};
	sLen = 5;
	memcpy(&szExternalAuthentication[sLen], szTriDesResult, 8);
	sLen += 8;
	ret = IC_Command(iDevId, CarType, sLen, szExternalAuthentication, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("外部认证失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}
	strTemp.Format(_T("外部认证成功\n"));

	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(strTemp);
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0); 

	//清卡 80E80000023F00
	unsigned char szClearCard[ICC_COMMAN_BUF_LEN] = {0x80, 0xE8, 0x00, 0x00, 0x02, 0x3F, 0x00};
	sLen = 7;
	ret = IC_Command(iDevId, CarType, sLen, szClearCard, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("清卡失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("清卡成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("清卡成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//建立主目录 80E00000130102030405060708020000080A5F0A0A000003
	unsigned char szCreateRootDir[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x00, 0x00, 0x13, 0x01, 0x02, 0x03, 0x04, 0x05,
		0x06, 0x07, 0x08, 0x02, 0x00, 0x00, 0x08, 0x0A, 0x5F, 0x0A, 
		0x0A, 0x00, 0x00, 0x03};
	sLen = 24;
	ret = IC_Command(iDevId, CarType, sLen, szCreateRootDir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建立主目录失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("建立主目录成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建立主目录成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//建立EF00文件 80E0030010EF005F5F5F5F5F5F5F5F5F000091001A
	unsigned char szCreateEF00File[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x03, 0x00, 0x10, 0xEF, 0x00, 0x5F, 0x5F, 0x5F,
		0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x00, 0x00, 0x91, 0x00, 
		0x1A};
	sLen = 21;
	ret = IC_Command(iDevId, CarType, sLen, szCreateEF00File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		//strTemp.Format(_T("建立EF00文件失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}
	strTemp = _T("建立EF00文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建立EF00文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//写EF00文件 00D600001A00176F15840E315041592E5359532E4444463031A503880101FF
	unsigned char szWriteEF00File[ICC_COMMAN_BUF_LEN] = {0x00, 0xD6, 0x00, 0x00, 0x1A, 0x00, 0x17, 0x6F, 0x15, 0x84,
		0x0E, 0x31, 0x50, 0x41, 0x59, 0x2E, 0x53, 0x59, 0x53, 0x2E,
		0x44, 0x44, 0x46, 0x30, 0x31, 0xA5, 0x03, 0x88, 0x01, 0x01,
		0xFF};
	sLen = 31;
	ret = IC_Command(iDevId, CarType, sLen, szWriteEF00File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写EF00文件失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp = _T("写EF00文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写EF00文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//建立0014文件 80E003001000145F5F5F5F5F5F5F5F5F0014810006
	unsigned char szCreate0014File[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x03, 0x00, 0x10, 0x00, 0x14, 0x8F, 0x8F, 0xFF,
		0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x00, 0x14, 0x81, 0x00, 
		0x06};
	sLen = 21;
	ret = IC_Command(iDevId, CarType, sLen, szCreate0014File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建立0014文件失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}
	strTemp = _T("建0014文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建0014文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//写0014文件 00D6000006EE333333338A
	unsigned char szWrite0014File[ICC_COMMAN_BUF_LEN] = {0x00, 0xD6, 0x00, 0x00, 0x06, 0xEE, 0x33, 0x33, 0x33, 0x33, 0x8A};
	sLen = 11;
	//unsigned char szIssueCnt[6] = {0x00,0x00,0x00,0x00,0x00,0x05};
	//memcpy(&szWrite0014File[5],szIssueCnt,sizeof(szIssueCnt));
	ret = IC_Command(iDevId, CarType, sLen, szWrite0014File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写0014文件失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp = _T("写0014文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写0014文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//建立0019文件 80E003001000195FBF5F5F5F5F5F5F5F0319820317
	unsigned char szCreate0019File[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x03, 0x00, 0x10, 0x00, 0x19, 0x8F, 0xBF, 0xFF, 
		0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x03, 0x19, 0x82, 0x03,
		0x17};
	sLen = 21;
	ret = IC_Command(iDevId, CarType, sLen, szCreate0019File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建立0019文件失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp = _T("建立0019文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建立0019文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);


	//选择主目录 00A4000C023F00
	sLen = 7;
	ret = IC_Command(iDevId, CarType, sLen, szChooseRootDir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("选择主目录失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp = _T("选择主目录成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("选择主目录成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);


	//写主控密钥 80D400001300000000000000000000000000000000000000
	unsigned char szWriteMainCtrlKey[ICC_COMMAN_BUF_LEN] = {0x80, 0xD4, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00};
	sLen = 24;
	ret = IC_Command(iDevId, CarType, sLen, szWriteMainCtrlKey, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写主控密钥失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("写主控密钥成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写主控密钥成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//DF01 8052025808DF0105A000000003 
	unsigned char szDF01Dir[ICC_COMMAN_BUF_LEN] = {0x80, 0x52, 0x02, 0x58, 0x08, 0xDF, 0x01, 0x05, 0xA0, 0x00,
		0x00, 0x00, 0x03};
	sLen = 13;
	ret = IC_Command(iDevId, CarType, sLen, szDF01Dir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("DF01失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("DF01成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("DF01成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//建DF01文件 80E0010019DF0105A000000003000000000000000000000000081A5F8003 
	unsigned char szCreateDF01Dir[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x01, 0x00, 0x19, 0xDF, 0x01, 0x05, 0xA0, 0x00,
		0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1A, 0x5F, 0x80, 0x03};
	sLen = 30;
	ret = IC_Command(iDevId, CarType, sLen, szCreateDF01Dir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建DF01文件失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("建DF01文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建DF01文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//建EF00文件 80E0030010EF005F5F5F5F5F5F5F5F5F000091000C 
	unsigned char szCreateEF00File_2[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x03, 0x00, 0x10, 0xEF, 0x00, 0x5F, 0x5F, 0x5F,
		0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x00, 0x00, 0x91, 0x00, 
		0x1C};
	sLen = 21;
	ret = IC_Command(iDevId, CarType, sLen, szCreateEF00File_2, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建EF00文件失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("建EF00文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建EF00文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//写EF00文件 00D600000C000A6F088400A5049F080102 
	unsigned char szWriteEF00File_2[ICC_COMMAN_BUF_LEN] = {0x00, 0xD6, 0x00, 0x00, 0x0C, 0x00, 0x0A, 0x6F, 0x08, 0x84,
		0x00, 0xA5, 0x04, 0x9F, 0x08, 0x01, 0x02};
	sLen = 17;
	ret = IC_Command(iDevId, CarType, sLen, szWriteEF00File_2, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写EF00文件失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("写EF00文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写EF00文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//建0014文件 80E003001000145F5F5F5F5F5F5F5F5F0014810006 
	sLen = 21;
	ret = IC_Command(iDevId, CarType, sLen, szCreate0014File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建0014文件失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}
	strTemp =_T("建0014文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建0014文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//写0014文件 00D600000633333333330A 
	unsigned char szWrite0014File_2[ICC_COMMAN_BUF_LEN] = {0x00, 0xD6, 0x00, 0x00, 0x06, 0xEE, 0x33, 0x33, 0x33, 0x33, 0x0A};
	sLen = 11;
	ret = IC_Command(iDevId, CarType, sLen, szWrite0014File_2, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写0014文件失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("写0014文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写0014文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);


	//80 E0 03 00 10 00 17 9F BF 5F 5F 5F 5F 5F 5F 5F 01 17 81 00 19
	unsigned char szCreate0017File[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x03, 0x00, 0x10, 0x00, 0x17, 0x9F, 0xBF, 0xFF,
		0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x01, 0x17, 0x81, 0x00, 
		0x19};
	sLen = 21;
	ret = IC_Command(iDevId, CarType, sLen, szCreate0017File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建立0017文件失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("建0017文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建0017文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//00 D6 00 00 19 01 B1 B1 BE A9 B1 B1 BE A9 00 00 00 00 00 00 00 01 20 10 03 01 20 20 03 01
	//写0017文件 00D6000006EE333333338A
	unsigned char szWrite0017File[ICC_COMMAN_BUF_LEN] = {0x00, 0xD6, 0x00, 0x00, 0x19, 0xEE, 0x33, 0x33, 0x33, 0x33, 0x8A};
	sLen = 30;
	memcpy(&szWrite0017File[5],Key,sizeof(unsigned char)*16);
	unsigned char szIssueCnt[2];// ={0x00,0x05};
	szIssueCnt[0] = (g_n32IssueCnt >> 8);
	szIssueCnt[1] = (g_n32IssueCnt & 0xff);
	memcpy(&szWrite0017File[21],szIssueCnt,sizeof(szIssueCnt));
	ret = IC_Command(iDevId, CarType, sLen, szWrite0017File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写0014文件失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("写0014文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写0014文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);


	//建0019文件 80E003001000195FBF5F5F5F5F5F5F5F0319820F17 
	unsigned char szCreate0019File_2[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x03, 0x00, 0x10, 0x00, 0x19, 0x5F, 0xBF, 0x5F, 
		0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x03, 0x19, 0x82, 0x0F,
		0x17};
	sLen = 21;
	ret = IC_Command(iDevId, CarType, sLen, szCreate0019File_2, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建0019文件失败: 状态码 = 0x%.4x:\n"), ret);;
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("建0019文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建0019文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//选择DF01目录 00A4000C02DF01
	unsigned char szChooseDF01Dir[ICC_COMMAN_BUF_LEN] = {0x00, 0xA4, 0x00, 0x0C, 0x02, 0xDF, 0x01};
	sLen = 7;
	ret = IC_Command(iDevId, CarType, sLen, szChooseDF01Dir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("选择DF01目录失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("选择DF01目录成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("选择DF01目录成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//写应用主控密钥 80D4000013000000E481FC5658391418CE71A4ADE46F414F 
	unsigned char szWriteAppMainCtrlKey[ICC_COMMAN_BUF_LEN] = {0x80, 0xD4, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0xE4, 0x81,
		0xFC, 0x56, 0x58, 0x39, 0x14, 0x18, 0xCE, 0x71, 0xA4, 0xAD,
		0xE4, 0x6F, 0x41, 0x4F};
	sLen = 24;
	ret = IC_Command(iDevId, CarType, sLen, szWriteAppMainCtrlKey, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写应用主控密钥失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("写应用主控密钥成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写应用主控密钥成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//写应用维护密钥 80D40000130100005FD2B70A10036AA22B1A5FCADFD8CD38
	unsigned char szWriteAppMaintainKey[ICC_COMMAN_BUF_LEN] = {0x80, 0xD4, 0x00, 0x00, 0x13, 0x01, 0x00, 0x00, 0x5F, 0xD2,
		0xB7, 0x0A, 0x10, 0x03, 0x6A, 0xA2, 0x2B, 0x1A, 0x5F, 0xCA,
		0xDF, 0xD8, 0xCD, 0x38};
	sLen = 24;
	ret = IC_Command(iDevId, CarType, sLen, szWriteAppMaintainKey, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写应用维护密钥失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("写应用维护密钥成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写应用维护密钥成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	unsigned char szDesResult[32];
	unsigned char szDividFactor[8] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};
	//unsigned char szDividFactor[8] = {0x12,0x34,0x56,0x78,0x12,0x34,0x56,0x78};
	//	ic_decrypt((char*)szRecvBuf, (char*)szTransCardKey, 16, (char*)szDesResult);
	//将A按照1122334455667788
	//SingleDES(1, szKey, 16, m_LeaderInfoOrignBufA, szDesResult);
	//	DividedFun(m_LeaderInfoOrignBufA,szDividFactor,szDesResult);
	//
	//#ifdef USER_DEBUG
	//
	//	printf("传输卡加密解密密钥为：\n");
	//	for(int i =0;i<16;i++)
	//	{		
	//		printf("%x,",szDesResult[i]);
	//	}
	//	printf("\n");
	//
	//#endif
	//	//写DES加密密钥 80D4000013070100 
	//	unsigned char szWriteEncryptKey[ICC_COMMAN_BUF_LEN] = {0x80, 0xD4, 0x00, 0x00, 0x13, 0x07, 0x01, 0x00};
	//	sLen = 8;
	//	memcpy(&szWriteEncryptKey[sLen], szDesResult, 16);
	//	sLen += 16;
	//
	//	ret = IC_Command(iDevId, CarType, sLen, szWriteEncryptKey, &rLen, szRecvBuf);
	//	if (ret != ICC_COMMAND_SUCCESS)
	//	{
	//		strTemp.Format(_T("写DES加密密钥: 状态码 = 0x%.4x:\n"), ret);
	//		m_ShowInfo.SetSel(-1, -1);
	//		m_ShowInfo.ReplaceSel(strTemp);
	//		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	//		return USER_FAILED;
	//	}
	//	m_ShowInfo.SetSel(-1, -1);
	//	m_ShowInfo.ReplaceSel(_T("写DES加密密钥成功\n"));
	//	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	//
	//	//写DES解密密钥 80D4000013190100
	//	unsigned char szWriteDecryptKey[ICC_COMMAN_BUF_LEN] = {0x80, 0xD4, 0x00, 0x00, 0x13, 0x19, 0x01, 0x00};
	//	sLen = 8;
	//	memcpy(&szWriteDecryptKey[sLen], szDesResult, 16);
	//	sLen += 16;
	//
	//	ret = IC_Command(iDevId, CarType, sLen, szWriteDecryptKey, &rLen, szRecvBuf);
	//	if (ret != ICC_COMMAND_SUCCESS)
	//	{
	//		strTemp.Format(_T("写DES解密密钥: 状态码 = 0x%.4x:\n"), ret);
	//		m_ShowInfo.SetSel(-1, -1);
	//		m_ShowInfo.ReplaceSel(strTemp);
	//		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	//		return USER_FAILED;
	//	}
	//	m_ShowInfo.SetSel(-1, -1);
	//	m_ShowInfo.ReplaceSel(_T("写DES解密密钥成功\n"));
	//	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//选择DF01 00A4000C02DF01 
	sLen = 7;
	ret = IC_Command(iDevId, CarType, sLen, szChooseDF01Dir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("选择DF01失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("选择DF01成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;im

	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("选择DF01成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//退出DF01 80E0020002DF01
	unsigned char szExitDF01Dir[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x02, 0x00, 0x02, 0xDF, 0x01};
	sLen = 7;
	ret = IC_Command(iDevId, CarType, sLen, szExitDF01Dir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("退出DF01失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}
	strTemp =_T("退出DF01成功\n");
	/*g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;*/
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("退出DF01成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//退出主目录 80E00200023F00 
	unsigned char szExitRootDir[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x02, 0x00, 0x02, 0x3F, 0x00};
	sLen = 7;
	ret = IC_Command(m_iDevId, CarType, sLen, szExitRootDir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("退出主目录失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		return USER_FAILED;
	}

	strTemp =_T("退出主目录成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("退出主目录成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);


	strTemp =_T("根密钥传输卡创建完成\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("根密钥传输卡创建完成\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);


	return USER_SUCCESS;
}
int CIssueDlg::WriteToBLog(CString strPath)
{
	//7100000020140000
	u8 u32ProductCode[256];
	if(m_CombEquipSel.GetCurSel() == 0)
	{
		u32ProductCode[0] = 0x71;
		u32ProductCode[1] = 0x00;
		u32ProductCode[2] = 0x00;
		u32ProductCode[3] = 0x00;
		u32ProductCode[4] = 0x20;
		u32ProductCode[5] = 0x14;
		u32ProductCode[6] = 0x00;
		u32ProductCode[7] = 0x00;
	}
	else if(m_CombEquipSel.GetCurSel() == 1)
	{
		u32ProductCode[0] = 0x71;
		u32ProductCode[1] = 0x10;
		u32ProductCode[2] = 0x00;
		u32ProductCode[3] = 0x00;
		u32ProductCode[4] = 0x20;
		u32ProductCode[5] = 0x15;
		u32ProductCode[6] = 0x00;
		u32ProductCode[7] = 0x00;
	}

	char *pPath = (LPSTR)(LPCTSTR)strPath;
	FILE *fp;
	fp = fopen(pPath,"ab+");

	SYSTEMTIME l_stime = {0};
	CString l_StrFilename,l_StrTmp;
	GetLocalTime(&l_stime);//获得当前本地时间

	unsigned char Head[2];
	Head[0] = 0xaa;
	Head[1] = 0x55;

	for(int i=0;i<2;i++)
	{
		fprintf(fp,"%c",Head[i]);
	}

	unsigned char time[7];
	time[0] = (unsigned char)(l_stime.wYear >> 8);
	time[1] = (unsigned char)(l_stime.wYear &0xff);
	time[2] = (unsigned char)l_stime.wMonth;
	time[3] = (unsigned char)l_stime.wDay;
	time[4] = (unsigned char)l_stime.wHour;
	time[5] = (unsigned char)l_stime.wMinute;
	time[6] = (unsigned char)l_stime.wSecond;
	//写日期
	for(int i =0;i<7;i++)
	{
		fprintf(fp,"%c",time[i]);
	}
	//写产品代码分散后的发行密钥
	for(int i =0;i<16;i++)
	{
		//fprintf(fp,"%c",szTransferKeyInfo[i]);
		fprintf(fp,"%c",0);
	}
	//写芯片ID
	for(int i=0;i<8;i++)
	{
		fprintf(fp,"%c",szChipID[i]);
	}
	//写被芯片分散后的代码
	for(int i=0;i<8;i++)
	{
		//szDividedByProductResults[i] = 0x12;
		fprintf(fp,"%c",szDividedByProductResults[i]);
	}

	for(int i=0;i<8;i++)
	{
		//szDividedByProductResults[i] = 0x12;
		fprintf(fp,"%c",u32ProductCode[i]);
	}


	//写剩余发行次数
	unsigned char buf[2];
	buf[0] = (g_n32IssueCnt >> 8);
	buf[1] = (g_n32IssueCnt & 0xff);

	for(int i=0;i<2;i++)
	{
		fprintf(fp,"%c",buf[i]);
	}

	unsigned char Tail[2];
	Tail[0] = 0x55;
	Tail[1] = 0xaa;

	for(int i=0;i<2;i++)
	{
		fprintf(fp,"%c",Tail[i]);
	}

	fclose(fp);

	//char buf1[50];
	//fp = fopen(strPath,"rb+");
	//while(fgets(buf1,50,fp))
	//{
	//	unsigned char buf2[50];
	//	memcpy(buf2,buf1,sizeof(buf1));
	//}
	//fclose(fp);	

	return 1;
}
int test[10];
long CIssueDlg::IC_Command(HANDLE icdev, unsigned char CardType, int slen, unsigned char *send_data, int *rlen, unsigned char *receive_data)
{
	unsigned char icSend[256];
	unsigned char icRecv[256];
	memset(icSend, 0, 256);
	memset(icRecv, 0, 256);
	memset(test,0,sizeof(test));
	test[0] = CardType;
	test[1] = slen;
	Sleep(20);
	int pos = 0;
	int i = 0;


	int ret = 0;
	if(CardType == CARD_TYPE_WATCH)
	{
		memcpy(&icSend[pos], send_data, slen);
		pos += slen;
		ret = ICC_tsi_apiVB((int)icdev, pos, icSend, rlen, icRecv);
		return ret;
	}
	else
	{
		icSend[pos++] = 0x00; //nad
		icSend[pos++] = 0x40; //pcb
		icSend[pos++] = slen;
		memcpy(&icSend[pos], send_data, slen);
		pos += slen;
		icSend[pos++] = 0x00; //init bcc
		//计算校验值
		for(i = 0; i < pos - 1; i++)
		{
			icSend[pos-1] ^= icSend[i]; 
		}
		if(pos >= 256 || pos <1)
		{
			icSend[0] = 0x00; 
		}
		ret = sam_slt_protocol(icdev, CardType, pos, icSend, (short*)rlen, icRecv);
		if (ret == 0)
		{
			*rlen -= 2;//除去2字节指令执行状态码
			memcpy(receive_data, &icRecv[3], *rlen);
			return ((icRecv[*rlen + 3] << 8 ) | icRecv[*rlen + 4]);
		}
		else
		{
			return USER_FAILED;
		}
	}
}

int CIssueDlg::GetKeyState_rq()
{
	int ret;
	unsigned char *temp = new unsigned char[10];


	temp[0] = 0x00;
	temp[1] = 0xaa;
	ResetEvent(CHECKKEYSTATE_EVENT_rs);
	ret = Serial_TransChannelSend(temp,3);

	return 1;
}

int CIssueDlg::GetKeyState_rs()
{
	int ret = 0;
	int TimeOut = 1000;
	//	page_data = new unsigned char [32];
	DWORD dRet = WaitForSingleObject(CHECKKEYSTATE_EVENT_rs,TimeOut);
	if (dRet == WAIT_OBJECT_0)
	{

		ret = zIn_pkt_decode(ComRecvBuf, ComRecvLen); //解码,校验等
		if ((ret == 0) && (ComRecvBuf[2] == 0x00))
		{

			//memcpy(page_data,&ComRecvBuf[3],32);
			if(ComRecvBuf[3] == 0x00)
			{
				//((CEdit *)GetDlgItem(IDC_EDIT_KEYREPEAT))->SetWindowText("已下载");
				ShowInfomation(CString("已下载\n"));
				return 0;
			}
			else if(ComRecvBuf[3] == 0x01)
			{
				//((CEdit *)GetDlgItem(IDC_EDIT_KEYREPEAT))->SetWindowText("未下载");
				ShowInfomation(CString("未下载\n"));
			}
			else
				//((CEdit *)GetDlgItem(IDC_EDIT_KEYREPEAT))->SetWindowText("下位机异常");
				ShowInfomation(CString("下位机异常\n"));
			return 1 ;
		} 
		else
		{
			ShowInfomation(CString("下位机异常\n"));
			return 2; //解码失败
		}
	} 
	else
	{	
		//((CEdit *)GetDlgItem(IDC_EDIT_KEYREPEAT))->SetWindowText("查询超时");
		ShowInfomation(CString("查询超时\n"));
		return 3; //超时
	}
}

int CIssueDlg::CalcSendRands(int page,unsigned char * rands)  //3
{
	SendRand_rq(rands); //32字节的随机数 取前8位d
	//Save *rands[temp[2]] = new unsigned  char[8], memcpy(*rands[temp[2]],rands,8)
	unsigned char *temp = new unsigned char[10];
	temp[0] = 0x00;
	temp[1] = 0x02;	
	temp[2] = page & 0xf;
#if 0
	//memset(rands,2,8);  //调试 固定随机数
	rands[0] = 0x9c;
	rands[1] = 0x79;
	rands[2] = 0x55;
	rands[3] = 0x32;
	rands[4] = 0x0f;
	rands[5] = 0xeb;
	rands[6] = 0xc8;
	rands[7] = 0xac;
#endif

	memcpy(&temp[3],rands,8);
	int ret = 0;
	ResetEvent(CALCSENDRANDS_EVENT_rs);
	ret = Serial_TransChannelSend(temp,11);
	return ret;
}



int CIssueDlg::CalcSendRands_rs(unsigned char *hsmac) //3-3
{
	int ret = 0;
	int TimeOut = 2000;
	//unsigned char ucMac[20];
	//	hsmac = new unsigned char [20];
	DWORD dRet = WaitForSingleObject(CALCSENDRANDS_EVENT_rs,TimeOut);
	if (dRet == WAIT_OBJECT_0)
	{

		ret = zIn_pkt_decode(ComRecvBuf, ComRecvLen); //解码,校验等
		if ((ret == 0) && (ComRecvBuf[2] == 0x00))
		{

			memcpy(hsmac,&ComRecvBuf[3],20);
			//memcpy(ucMac,&ComRecvBuf[3],20);			
			//ret = SendRand_rs(in_rand, in_subkey, ucMac);  //8,8,20
			return ret ;
		} 
		else
		{
			return 2; //解码失败
		}
	} 
	else
	{		
		return 3; //超时
	}	

}

int CIssueDlg::ReadPage(int page)  //4
{	
	int ret = 0;
	unsigned char *temp = new unsigned char[10];
	temp[0] = 0x00;
	temp[1] = 0x03;	
	temp[2] = page & 0xf;	
	ResetEvent(READPAGE_EVENT_rs);
	ret = Serial_TransChannelSend(temp,3);
	//ResetEvent(SENDRAND_EVENT_rs);  //简化后，该函数不调用

	return ret;
}

int CIssueDlg::ReadPage_rs(unsigned char *page_data)  //4-4
{
	int ret = 0;
	int TimeOut = 2000;
	//	page_data = new unsigned char [32];
	DWORD dRet = WaitForSingleObject(READPAGE_EVENT_rs,TimeOut);
	if (dRet == WAIT_OBJECT_0)
	{

		ret = zIn_pkt_decode(ComRecvBuf, ComRecvLen); //解码,校验等
		if ((ret == 0) && (ComRecvBuf[2] == 0x00))
		{

			memcpy(page_data,&ComRecvBuf[3],32);			
			return ret ;
		} 
		else
		{
			return 2; //解码失败
		}
	} 
	else
	{		
		return 3; //超时
	}	

}

int CIssueDlg::GetKeyState_Net_rq()
{
	int ret = 0;
	ResetEvent(CHECKKEYSTATE_EVENT_NET_rs);

	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	static char zhiling[34]={0xFF,0xAA,0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x06,0x00,0x00,0x00,0x00,0x00,0x00};//获取秘钥状态
	int len = 34;

	CMainFrame *pMainDlg;
	//pMainDlg = (CMainFrame *)AfxGetMainWnd();
	pMainDlg = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pMainDlg->m_NetConnection.pcSendDataBuf = zhiling;
	pMainDlg->m_NetConnection.nSendDataBufSize = len;

	SendData(&pMainDlg->m_NetConnection);
	return 0;


	return 1;
}

int CIssueDlg::GetKeyState_Net_rs()
{
	int ret = 0;
	int TimeOut = 1000;
	//	page_data = new unsigned char [32];
	DWORD dRet = WaitForSingleObject(CHECKKEYSTATE_EVENT_NET_rs,TimeOut);
	if (dRet == WAIT_OBJECT_0)
	{

		/*if ((ret == 0) && (g_recvother[24]== 0x00))*/
		if ((ret == 0) )
		{

			//memcpy(page_data,&ComRecvBuf[3],32);
			if(g_recvother[29] == 0x01)
			{
				//((CEdit *)GetDlgItem(IDC_EDIT_KEYREPEAT))->SetWindowText("已下载");
				ShowInfomation(CString("已下载\n"));
				return 0;
			}
			else if(g_recvother[29] == 0x00)
			{
				//((CEdit *)GetDlgItem(IDC_EDIT_KEYREPEAT))->SetWindowText("未下载");
				ShowInfomation(CString("未下载,请更新密钥\n"));
			}
			else
				//((CEdit *)GetDlgItem(IDC_EDIT_KEYREPEAT))->SetWindowText("下位机异常");
				ShowInfomation(CString("下位机异常\n"));
			return 1 ;
		} 
		else
		{
			ShowInfomation(CString("下位机异常\n"));
			return 2; //解码失败
		}
	} 

	else
	{	
		//((CEdit *)GetDlgItem(IDC_EDIT_KEYREPEAT))->SetWindowText("查询超时");
		ShowInfomation(CString("查询超时\n"));
		return 3; //超时
	}
}

int CIssueDlg::GetFactoryID_Net_rs(char* ID)
{
	int ret = 0;
	int TimeOut = 4000;
	DWORD dRet = WaitForSingleObject(GETFACTORYID_EVENT_NET_rs,TimeOut);
	if (dRet == WAIT_OBJECT_0)
	{
		memcpy(ID,&g_recvother[26],8);
		return 0;
	} 
	else
	{		
		return 3; //超时
	}
}

int CIssueDlg::ReplacesKey_Net_rq(unsigned char *chipID,unsigned char *srckey,unsigned char *subkey) //2
{
	int ret = 0;	
	CMainFrame *pMainDlg;
	//pMainDlg = (CMainFrame *)AfxGetMainWnd();
	pMainDlg = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	ret = SendKey_rq(chipID,srckey,subkey); //动态库计算,subkey改为8字节

	ResetEvent(REPLACESKEY_EVENT_rs);
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	static char zhiling[38]={0xFF,0xAA,0x00,0x22,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//替换秘钥指令
	int len = 38;//34+4
	memcpy(&zhiling[26],subkey,sizeof(char)*8);
	ResetEvent(REPLACESKEY_EVENT_NET_rs);

	pMainDlg->m_NetConnection.pcSendDataBuf = zhiling;
	pMainDlg->m_NetConnection.nSendDataBufSize = len;

	SendData(&pMainDlg->m_NetConnection);
	return 0;
}

int CIssueDlg::ReplacesKey_Net_rs(void)
{
	int ret = 0;
	int TimeOut = 4000;
	DWORD dRet = WaitForSingleObject(REPLACESKEY_EVENT_NET_rs,TimeOut);
	if (dRet == WAIT_OBJECT_0)
	{
		if(g_recvother[29]== 0x01)
			return 0;
		else
			return 1;
	} 
	else
	{		
		return 3; //超时
	}
}

int CIssueDlg::CalcSendRands_Net_rq(int page,unsigned char * rands)  //3
{
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	static char Calczhiling[38]={0xFF,0xAA,0x00,0x22,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//随机数计算指令

	int ret = 0;
	CMainFrame *pMainDlg;
	//pMainDlg = (CMainFrame *)AfxGetMainWnd();
	pMainDlg = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	SendRand_rq(rands); //32字节的随机数 取前8位

#if 1
	memset(rands,0x1c,8);  //调试 固定随机数

	/*rands[0] = 0x9c;
	rands[1] = 0x79;
	rands[2] = 0x55;
	rands[3] = 0x32;
	rands[4] = 0x0f;
	rands[5] = 0xeb;
	rands[6] = 0xc8;
	rands[7] = 0xac;*/
#endif	
	int len = 38;
	//buf[24] = page;
	memcpy(&Calczhiling[26],rands,8);
	ResetEvent(CALCSENDRANDS_EVENT_NET_rs);

	pMainDlg->m_NetConnection.pcSendDataBuf = Calczhiling;
	pMainDlg->m_NetConnection.nSendDataBufSize = len;


	SendData(&pMainDlg->m_NetConnection);
	return 0;
}

int CIssueDlg::CalcSendRands_Net_rs(unsigned char *hsmac)  //3
{
	int ret = 0;
	int TimeOut = 2000;

	//unsigned char ucMac[20];
	//	hsmac = new unsigned char [20];
	DWORD dRet = WaitForSingleObject(CALCSENDRANDS_EVENT_NET_rs,TimeOut);
	if (dRet == WAIT_OBJECT_0)
	{
		memcpy(hsmac,&g_recvother[27],20);
		return 0;
	} 
	else
	{		
		return 3; //超时
	}	
}

int  CIssueDlg::ReadPage_Net_rq(int page)
{
	int ret = 0;
	CMainFrame *pMainDlg;
	//pMainDlg = (CMainFrame *)AfxGetMainWnd();
	pMainDlg = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	unsigned char *temp = new unsigned char[10];
	temp[0] = 0x00;
	temp[1] = 0x03;	
	temp[2] = page & 0xf;

	ret = Serial_TransChannelSend(temp,3);
	//ResetEvent(SENDRAND_EVENT_rs);  //简化后，该函数不调用


	ResetEvent(READPAGE_EVENT_NET_rs);

	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	static char zhiling[34]={0xFF,0xAA,0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x04,0x00,0x00,0x00,0x00,0x00,0x00};//随机数计算指令
	int len = 34;
	//buf[24] = 0x03;
	zhiling[26] = page & 0xf;

	pMainDlg->m_NetConnection.pcSendDataBuf = zhiling;
	pMainDlg->m_NetConnection.nSendDataBufSize = len;

	SendData(&pMainDlg->m_NetConnection);
	return 0;

}

int  CIssueDlg::ReadPage_Net_rs(unsigned char *page_data)
{
	int ret = 0;
	int TimeOut = 2000;

	//	page_data = new unsigned char [32];
	DWORD dRet = WaitForSingleObject(READPAGE_EVENT_NET_rs,TimeOut);
	if (dRet == WAIT_OBJECT_0)
	{
		if ((ret == 0) && (g_recvother[26] == 0x01))
		{
			memcpy(page_data,&g_recvother[27],32);
			return ret ;
		} 
		else
		{
			return 2; //解码失败
		}
	} 
	else
	{		
		return 3; //超时
	}	
}

int  CIssueDlg::WriteMemo_Net_rq(int page, int subpage)
{	

	int ret = 0;
	unsigned char *dataCont[8];
	CMainFrame *pMainDlg;
	//pMainDlg = (CMainFrame *)AfxGetMainWnd();
	pMainDlg = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	memset(dataCont,0x01,8);   //存储的内容
	ResetEvent(WRITEMEMO_EVENT_NET_rs);
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	static char zhiling[40]={0xFF,0xAA,0x00,0x24,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};//随机数计算指令

	int len = 40;
	//buf[24] = 0x03;
	zhiling[26] = page & 0xff;
	zhiling[27] = subpage & 0xff;
	memcpy(&zhiling[28],dataCont,8);
	pMainDlg->m_NetConnection.pcSendDataBuf = zhiling;
	pMainDlg->m_NetConnection.nSendDataBufSize = len;

	SendData(&pMainDlg->m_NetConnection);

	return 0;
}

int  CIssueDlg::WriteMemo_Net_rs()
{
	int ret = 0;
	int TimeOut = 2000;	
	DWORD dRet = WaitForSingleObject(WRITEMEMO_EVENT_NET_rs,TimeOut);
	if (dRet == WAIT_OBJECT_0)
	{
		if ((ret == 0) && (g_recvother[26] == 0x00))
		{				
			return ret ;
		} 
		else
		{
			return 2; //解码失败
		}
	} 
	else
	{		
		return 3; //超时
	}
}

void CIssueDlg::OnBnClickedButtonClearlist()
{
	// TODO: 在此添加控件通知处理程序代码
	m_ShowInfo.SetWindowText(_T(" "));
}


void CIssueDlg::OnBnClickedButtonOpenct()
{
	// TODO: 在此添加控件通知处理程序代码
	__int16 port = 0;
	unsigned long baud = 0;
	port = m_ComNum.GetCurSel();
	CString strBaud;
	//((CEdit*)GetDlgItem(IDC_EDIT_BAUD))->GetWindowText(strBaud);
	m_CombBaud.GetWindowText(strBaud);
	if (strBaud.GetLength() == 0)
	{
		MessageBox(_T("请输入串口波特率"), _T("提示"), MB_ICONINFORMATION);
		return;
	}
	CString strTemp;
	UpdateData(TRUE);
	baud = atoi(strBaud);

	if (!g_bOpenCom)//串口未打开，才能执行连接操作
	{
		m_iDevId = ic_init(port, baud);

		int ret=GetLastError();	//获得最后一次错误信息，判断串口是否已经打开
		if (ret != 0)
		{

			strTemp =_T("连接明华卡机失败\n");
			//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
			//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
			m_ShowInfo.SetSel(-1, -1);
			m_ShowInfo.ReplaceSel(_T("连接明华卡机失败\n"));
			m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
			return;
		}
		else 
		{
			//打开通信设备成功
			g_bOpenCom = TRUE;
			m_OpenCom.SetWindowText(_T("断开卡机1"));

			strTemp =_T("连接明华卡机成功\n");
			//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
			//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
			m_ShowInfo.SetSel(-1, -1);
			m_ShowInfo.ReplaceSel(_T("连接明华卡机成功\n"));
			m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

			//卡机连接期间不允许设置串口
			m_CombBaud.EnableWindow(FALSE);
			m_ComNum.EnableWindow(FALSE);

			WriteALog(CString("连接卡机"));
		}
	}
	else
	{
		if(ic_exit(m_iDevId) == 0)
		{
			//成功关闭通讯口
			g_bOpenCom = FALSE;
			m_OpenCom.SetWindowText(_T("连接卡机"));


			//卡机断开，可以重新设置串口
			m_CombBaud.EnableWindow(TRUE);
			m_ComNum.EnableWindow(TRUE);

			WriteALog(CString("断开卡机"));
		}
		else
		{
			strTemp =_T("关闭明华卡机连接失败\n");
			//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
			//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
			m_ShowInfo.SetSel(-1, -1);
			m_ShowInfo.ReplaceSel(_T("关闭明华卡机连接失败\n"));
			m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
			return;
		}
	}
}


BOOL CIssueDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_CombBaud.SetCurSel(0);
	m_ComNum.SetCurSel(0);
	// TODO:  在此添加额外的初始化
	hDLL_watch = LoadLibrary(_T("wdcrwv.dll"));//加载动态库
	TripleDES = (pTripleDES)::GetProcAddress(hDLL_watch, "TripleDES");
	SingleDES = (pSingleDES)::GetProcAddress(hDLL_watch, "SingleDES");
	CT_open = (pCT_open)::GetProcAddress(hDLL_watch, "CT_open");
	CT_close = (pCT_close)::GetProcAddress(hDLL_watch, "CT_close");
	ICC_present = (pICC_present)::GetProcAddress(hDLL_watch, "ICC_present");
	ICC_set_NAD = (pICC_set_NAD)::GetProcAddress(hDLL_watch, "ICC_set_NAD");
	ICC_resetVB = (pICC_resetVB)::GetProcAddress(hDLL_watch, "ICC_resetVB");
	ICC_tsi_apiVB = (pICC_tsi_apiVB)::GetProcAddress(hDLL_watch, "ICC_tsi_apiVB");

	GETFACTORYID_EVENT_NET_rs = CreateEvent(NULL,true,true,"GETFACTORYID_EVENT_NET_rs");
	REPLACESKEY_EVENT_NET_rs = CreateEvent(NULL,true,true,"REPLACESKEY_EVENT_NET_rs");
	CALCSENDRANDS_EVENT_NET_rs = CreateEvent(NULL,true,true,"CALCSENDRANDS_EVENT_NET_rs");
	READPAGE_EVENT_NET_rs = CreateEvent(NULL,true,true,"READPAGE_EVENT_NET_rs");
	WRITEMEMO_EVENT_NET_rs = CreateEvent(NULL,true,true,"WRITEMEMO_EVENT_NET_rs");
	CHECKKEYSTATE_EVENT_NET_rs = CreateEvent(NULL,true,true,"CHECKKEYSTATE_EVENT_NET_rs");
	//RichEditInit();
	m_ShowInfo.SetWindowText(_T(""));

	TCHAR szPath[MAX_PATH];   
	GetModuleFileName(NULL, szPath, MAX_PATH);
	CString PathName(szPath);
	strSavePath = PathName.Left(PathName.ReverseFind(_T('\\')) + 1);
	//strSavePath += "log\\";
	SYSTEMTIME l_stime = {0};
	CString l_StrFilename,l_StrTmp;
	GetLocalTime(&l_stime);//获得当前本地时间

	l_StrTmp.Format("%02d",l_stime.wYear);//月
	strSavePath += l_StrTmp;

	l_StrTmp.Format("%02d",l_stime.wMonth);//月
	strSavePath += l_StrTmp;

	l_StrTmp.Format("%02d",l_stime.wDay);//日
	strSavePath += l_StrTmp;
	strSavePath += ".wjbin";
	//SetTimer(0,10,NULL);
	//AfxBeginThread(ThreadWriteIssueLog,this,THREAD_PRIORITY_NORMAL,0,0,NULL);

	//网络
#ifdef USER_DEBUG
	OpenConsole();
#endif
	//m_editBaud.SetWindowText("9600");

	g_strDbOldPassword = "05e9dcd752678426";
	int ret = OpenDb(CString("日志.mdb"),g_strDbOldPassword);
	if(ret != 0)
	{
		MessageBox(_T("打开数据库失败！"),_T("提示"),MB_ICONINFORMATION);
		return FALSE;
	}

	//	m_btnIssue.EnableWindow(FALSE);

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

//获取秘钥状态，是否发行
void CIssueDlg::OnBnClickedButtonCheckstate()
{
	m_btnCheckState.EnableWindow(false);
	m_btnIssue.EnableWindow(false);
	AfxBeginThread(ThreadNetKeyState,this,THREAD_PRIORITY_NORMAL,0,0,NULL);
}


void CIssueDlg::OnBnClickedButtonIssue2()
{
	// TODO: 在此添加控件通知处理程序代码
	HANDLE iDevId;
	unsigned char CarType;
	unsigned char *Key;

	long ret = 0;
	int sLen = 0;//发送指令的长度
	int rLen = 0;//返回指令的长度
	CString strTemp;
	unsigned char szRecvBuf[256];//指令返回缓冲区
	//PostMessage(WM_ISSUELOG, strTemp, (LPARAM)g_cPauseBuf);
	//复位

	iDevId = m_iDevId;
	CarType = CARD_TYPE_UP;
	Key = szTransferKeyInfo;
	ret = sam_slt_reset(m_iDevId, CarType, (short*)&rLen, szRecvBuf);
	if (ret != 0)
	{
		strTemp.Format(_T("\n复位失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}	
	Sleep(1000);
	strTemp = _T("\n复位成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("\n复位成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//选择主目录 00A4000C023F00
	unsigned char szChooseRootDir[ICC_COMMAN_BUF_LEN] = {0x00, 0xA4, 0x00, 0x0C, 0x02, 0x3F, 0x00};
	sLen = 7;
	memset(szRecvBuf,0,sizeof(szRecvBuf));
	ret = IC_Command(iDevId, CarType, sLen, szChooseRootDir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("选择主目录失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp = _T("选择主目录成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("选择主目录成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	memset(szRecvBuf,0,sizeof(szRecvBuf));
	//取随机数 0084000008
	unsigned char szGetRandom[ICC_COMMAN_BUF_LEN] = {0x00, 0x84, 0x00, 0x00, 0x08};
	sLen = 5;
	ret = IC_Command(iDevId, CarType, sLen, szGetRandom, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("取随机数失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}
	strTemp.Format(_T("取随机数成功\n"));
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(strTemp);
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0); 

	//将8字节随机数作为报文，使用16字节0x00作为密钥，进行3DES运算
	byte szZero[16];
	memset(szZero, 0x00, 16);
	byte szTriDesResult[16];
	TripleDES(1, szZero, 8, szRecvBuf, szTriDesResult);

	//外部认证 0082000008+上面3DES运算产生的8字节密文
	unsigned char szExternalAuthentication[ICC_COMMAN_BUF_LEN] = {0x00, 0x82, 0x00, 0x00, 0x08};
	sLen = 5;
	memcpy(&szExternalAuthentication[sLen], szTriDesResult, 8);
	sLen += 8;
	ret = IC_Command(iDevId, CarType, sLen, szExternalAuthentication, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("外部认证失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}
	strTemp.Format(_T("外部认证成功\n"));

	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(strTemp);
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0); 

	//清卡 80E80000023F00
	unsigned char szClearCard[ICC_COMMAN_BUF_LEN] = {0x80, 0xE8, 0x00, 0x00, 0x02, 0x3F, 0x00};
	sLen = 7;
	ret = IC_Command(iDevId, CarType, sLen, szClearCard, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("清卡失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("清卡成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("清卡成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//建立主目录 80E00000130102030405060708020000080A5F0A0A000003
	unsigned char szCreateRootDir[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x00, 0x00, 0x13, 0x01, 0x02, 0x03, 0x04, 0x05,
		0x06, 0x07, 0x08, 0x02, 0x00, 0x00, 0x08, 0x0A, 0x5F, 0x0A, 
		0x0A, 0x00, 0x00, 0x03};
	sLen = 24;
	ret = IC_Command(iDevId, CarType, sLen, szCreateRootDir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建立主目录失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("建立主目录成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建立主目录成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//建立EF00文件 80E0030010EF005F5F5F5F5F5F5F5F5F000091001A
	unsigned char szCreateEF00File[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x03, 0x00, 0x10, 0xEF, 0x00, 0x5F, 0x5F, 0x5F,
		0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x00, 0x00, 0x91, 0x00, 
		0x1A};
	sLen = 21;
	ret = IC_Command(iDevId, CarType, sLen, szCreateEF00File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		//strTemp.Format(_T("建立EF00文件失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}
	strTemp = _T("建立EF00文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建立EF00文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//写EF00文件 00D600001A00176F15840E315041592E5359532E4444463031A503880101FF
	unsigned char szWriteEF00File[ICC_COMMAN_BUF_LEN] = {0x00, 0xD6, 0x00, 0x00, 0x1A, 0x00, 0x17, 0x6F, 0x15, 0x84,
		0x0E, 0x31, 0x50, 0x41, 0x59, 0x2E, 0x53, 0x59, 0x53, 0x2E,
		0x44, 0x44, 0x46, 0x30, 0x31, 0xA5, 0x03, 0x88, 0x01, 0x01,
		0xFF};
	sLen = 31;
	ret = IC_Command(iDevId, CarType, sLen, szWriteEF00File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写EF00文件失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp = _T("写EF00文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写EF00文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//建立0014文件 80E003001000145F5F5F5F5F5F5F5F5F0014810006
	unsigned char szCreate0014File[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x03, 0x00, 0x10, 0x00, 0x14, 0x8F, 0x8F, 0xFF,
		0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x00, 0x14, 0x81, 0x00, 
		0x06};
	sLen = 21;
	ret = IC_Command(iDevId, CarType, sLen, szCreate0014File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建立0014文件失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}
	strTemp = _T("建0014文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建0014文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//写0014文件 00D6000006EE333333338A
	unsigned char szWrite0014File[ICC_COMMAN_BUF_LEN] = {0x00, 0xD6, 0x00, 0x00, 0x06, 0xEE, 0x33, 0x33, 0x33, 0x33, 0x8A};
	sLen = 11;
	//unsigned char szIssueCnt[6] = {0x00,0x00,0x00,0x00,0x00,0x05};
	//memcpy(&szWrite0014File[5],szIssueCnt,sizeof(szIssueCnt));
	ret = IC_Command(iDevId, CarType, sLen, szWrite0014File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写0014文件失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp = _T("写0014文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写0014文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//建立0019文件 80E003001000195FBF5F5F5F5F5F5F5F0319820317
	unsigned char szCreate0019File[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x03, 0x00, 0x10, 0x00, 0x19, 0x8F, 0xBF, 0xFF, 
		0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x03, 0x19, 0x82, 0x03,
		0x17};
	sLen = 21;
	ret = IC_Command(iDevId, CarType, sLen, szCreate0019File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建立0019文件失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp = _T("建立0019文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建立0019文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);


	//选择主目录 00A4000C023F00
	sLen = 7;
	ret = IC_Command(iDevId, CarType, sLen, szChooseRootDir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("选择主目录失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp = _T("选择主目录成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("选择主目录成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);


	//写主控密钥 80D400001300000000000000000000000000000000000000
	unsigned char szWriteMainCtrlKey[ICC_COMMAN_BUF_LEN] = {0x80, 0xD4, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00};
	sLen = 24;
	ret = IC_Command(iDevId, CarType, sLen, szWriteMainCtrlKey, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写主控密钥失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("写主控密钥成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写主控密钥成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//DF01 8052025808DF0105A000000003 
	unsigned char szDF01Dir[ICC_COMMAN_BUF_LEN] = {0x80, 0x52, 0x02, 0x58, 0x08, 0xDF, 0x01, 0x05, 0xA0, 0x00,
		0x00, 0x00, 0x03};
	sLen = 13;
	ret = IC_Command(iDevId, CarType, sLen, szDF01Dir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("DF01失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("DF01成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("DF01成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//建DF01文件 80E0010019DF0105A000000003000000000000000000000000081A5F8003 
	unsigned char szCreateDF01Dir[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x01, 0x00, 0x19, 0xDF, 0x01, 0x05, 0xA0, 0x00,
		0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x1A, 0x5F, 0x80, 0x03};
	sLen = 30;
	ret = IC_Command(iDevId, CarType, sLen, szCreateDF01Dir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建DF01文件失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("建DF01文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建DF01文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//建EF00文件 80E0030010EF005F5F5F5F5F5F5F5F5F000091000C 
	unsigned char szCreateEF00File_2[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x03, 0x00, 0x10, 0xEF, 0x00, 0x5F, 0x5F, 0x5F,
		0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x00, 0x00, 0x91, 0x00, 
		0x1C};
	sLen = 21;
	ret = IC_Command(iDevId, CarType, sLen, szCreateEF00File_2, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建EF00文件失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("建EF00文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建EF00文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//写EF00文件 00D600000C000A6F088400A5049F080102 
	unsigned char szWriteEF00File_2[ICC_COMMAN_BUF_LEN] = {0x00, 0xD6, 0x00, 0x00, 0x0C, 0x00, 0x0A, 0x6F, 0x08, 0x84,
		0x00, 0xA5, 0x04, 0x9F, 0x08, 0x01, 0x02};
	sLen = 17;
	ret = IC_Command(iDevId, CarType, sLen, szWriteEF00File_2, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写EF00文件失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("写EF00文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写EF00文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//建0014文件 80E003001000145F5F5F5F5F5F5F5F5F0014810006 
	sLen = 21;
	ret = IC_Command(iDevId, CarType, sLen, szCreate0014File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建0014文件失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}
	strTemp =_T("建0014文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建0014文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//写0014文件 00D600000633333333330A 
	unsigned char szWrite0014File_2[ICC_COMMAN_BUF_LEN] = {0x00, 0xD6, 0x00, 0x00, 0x06, 0xEE, 0x33, 0x33, 0x33, 0x33, 0x0A};
	sLen = 11;
	ret = IC_Command(iDevId, CarType, sLen, szWrite0014File_2, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写0014文件失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("写0014文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写0014文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);


	//80 E0 03 00 10 00 17 9F BF 5F 5F 5F 5F 5F 5F 5F 01 17 81 00 19
	unsigned char szCreate0017File[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x03, 0x00, 0x10, 0x00, 0x17, 0x9F, 0xBF, 0xFF,
		0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x01, 0x17, 0x81, 0x00, 
		0x19};
	sLen = 21;
	ret = IC_Command(iDevId, CarType, sLen, szCreate0017File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建立0017文件失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("建0017文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建0017文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//00 D6 00 00 19 01 B1 B1 BE A9 B1 B1 BE A9 00 00 00 00 00 00 00 01 20 10 03 01 20 20 03 01
	//写0017文件 00D6000006EE333333338A
	unsigned char szWrite0017File[ICC_COMMAN_BUF_LEN] = {0x00, 0xD6, 0x00, 0x00, 0x19, 0xEE, 0x33, 0x33, 0x33, 0x33, 0x8A};
	sLen = 30;
	memcpy(&szWrite0017File[5],Key,sizeof(unsigned char)*16);
	unsigned char szIssueCnt[2];// ={0x00,0x05};
	szIssueCnt[0] = (g_n32IssueCnt >> 8);
	szIssueCnt[1] = (g_n32IssueCnt & 0xff);
	memcpy(&szWrite0017File[21],szIssueCnt,sizeof(szIssueCnt));
	ret = IC_Command(iDevId, CarType, sLen, szWrite0017File, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写0014文件失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("写0014文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写0014文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);


	//建0019文件 80E003001000195FBF5F5F5F5F5F5F5F0319820F17 
	unsigned char szCreate0019File_2[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x03, 0x00, 0x10, 0x00, 0x19, 0x5F, 0xBF, 0x5F, 
		0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x5F, 0x03, 0x19, 0x82, 0x0F,
		0x17};
	sLen = 21;
	ret = IC_Command(iDevId, CarType, sLen, szCreate0019File_2, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("建0019文件失败: 状态码 = 0x%.4x:\n"), ret);;
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("建0019文件成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("建0019文件成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//选择DF01目录 00A4000C02DF01
	unsigned char szChooseDF01Dir[ICC_COMMAN_BUF_LEN] = {0x00, 0xA4, 0x00, 0x0C, 0x02, 0xDF, 0x01};
	sLen = 7;
	ret = IC_Command(iDevId, CarType, sLen, szChooseDF01Dir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("选择DF01目录失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("选择DF01目录成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("选择DF01目录成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//写应用主控密钥 80D4000013000000E481FC5658391418CE71A4ADE46F414F 
	unsigned char szWriteAppMainCtrlKey[ICC_COMMAN_BUF_LEN] = {0x80, 0xD4, 0x00, 0x00, 0x13, 0x00, 0x00, 0x00, 0xE4, 0x81,
		0xFC, 0x56, 0x58, 0x39, 0x14, 0x18, 0xCE, 0x71, 0xA4, 0xAD,
		0xE4, 0x6F, 0x41, 0x4F};
	sLen = 24;
	ret = IC_Command(iDevId, CarType, sLen, szWriteAppMainCtrlKey, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写应用主控密钥失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("写应用主控密钥成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写应用主控密钥成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//写应用维护密钥 80D40000130100005FD2B70A10036AA22B1A5FCADFD8CD38
	unsigned char szWriteAppMaintainKey[ICC_COMMAN_BUF_LEN] = {0x80, 0xD4, 0x00, 0x00, 0x13, 0x01, 0x00, 0x00, 0x5F, 0xD2,
		0xB7, 0x0A, 0x10, 0x03, 0x6A, 0xA2, 0x2B, 0x1A, 0x5F, 0xCA,
		0xDF, 0xD8, 0xCD, 0x38};
	sLen = 24;
	ret = IC_Command(iDevId, CarType, sLen, szWriteAppMaintainKey, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("写应用维护密钥失败: 状态码 = 0x%.4x:\n"), ret);

		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("写应用维护密钥成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("写应用维护密钥成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	unsigned char szDesResult[32];
	unsigned char szDividFactor[8] = {0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88};

	//选择DF01 00A4000C02DF01 
	sLen = 7;
	ret = IC_Command(iDevId, CarType, sLen, szChooseDF01Dir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("选择DF01失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("选择DF01成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;im

	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("选择DF01成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//退出DF01 80E0020002DF01
	unsigned char szExitDF01Dir[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x02, 0x00, 0x02, 0xDF, 0x01};
	sLen = 7;
	ret = IC_Command(iDevId, CarType, sLen, szExitDF01Dir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("退出DF01失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}
	strTemp =_T("退出DF01成功\n");
	/*g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;*/
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("退出DF01成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	//退出主目录 80E00200023F00 
	unsigned char szExitRootDir[ICC_COMMAN_BUF_LEN] = {0x80, 0xE0, 0x02, 0x00, 0x02, 0x3F, 0x00};
	sLen = 7;
	ret = IC_Command(m_iDevId, CarType, sLen, szExitRootDir, &rLen, szRecvBuf);
	if (ret != ICC_COMMAND_SUCCESS)
	{
		strTemp.Format(_T("退出主目录失败: 状态码 = 0x%.4x:\n"), ret);
		//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
		//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
		m_ShowInfo.SetSel(-1, -1);
		m_ShowInfo.ReplaceSel(strTemp);
		m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}

	strTemp =_T("退出主目录成功\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("退出主目录成功\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);


	strTemp =_T("根密钥传输卡创建完成\n");
	//g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32In] =strTemp;
	//g_sIssueInfo.m_n32In = (g_sIssueInfo.m_n32In + 1)%ISSUELOGNUM;
	m_ShowInfo.SetSel(-1, -1);
	m_ShowInfo.ReplaceSel(_T("根密钥传输卡创建完成\n"));
	m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}
void CIssueDlg::OnTimer(UINT nIDEvent) 
{
	CString str;
	switch(nIDEvent)
	{
	case 0:
		if(g_sIssueInfo.m_n32In != g_sIssueInfo.m_n32Out)
		{
			if(m_ShowInfo.GetLineCount()>2000)
			{
				m_ShowInfo.SetSel(0, -1);		
				m_ShowInfo.ReplaceSel("");
			}
			str = g_sIssueInfo.m_strbuf[g_sIssueInfo.m_n32Out];
			m_ShowInfo.ReplaceSel(str);
			m_ShowInfo.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
			g_sIssueInfo.m_n32Out = (g_sIssueInfo.m_n32Out + 1)%ISSUELOGNUM;
		}
	}
}
