// SIMUAGVTEST.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "SIMUAGVTEST.h"
#include "afxdialogex.h"

#include "Uart/SerialUtils.h"
#include "NetParam.h"
#include "MainFrm.h"
#include "FileUtils.h"
#include "CSeries.h"
#include <cmath> 
#include "File/CDMLogInfo.h"
#define PI  3.14159265358979323846

bool MNLaserGetPostRuning = false;
bool MNLaserSetSpeedRuning = false;
bool MNAGVControlRunning = false;
bool f_SimuAGV = false;//MainFrm中是否向SimuAgvDlg发消息标志
CMainFrame* pMNMainFrameThis;
int MNSpeedInterval;
int MNPoseInterval;
HANDLE g_MNPXMap = CreateEvent(NULL, TRUE, FALSE, NULL);//靶标获取响应事件
//串口
BOOL m_MNUartOpenFlag;//串口打开关闭切换标志
BOOL m_MNbSendData;	// 是否正在发送数据
int g_MNbAloneFrameSize;
int nMNCase;
BYTE g_MNbAloneFrame[10240];
unsigned char g_MNbSendParameData[200];
int g_MNbSendParameDataSize;

//AGV全模拟自动运行
int LaserTimestap;//激光器时间戳
int *MNlmInfoBuf = NULL;//靶标获取信息
int n_MNGetLM = 0;//靶标获取到的靶标数量
bool MNSimulationRun = false;
int g_MNGoalX = 0;
int g_MNGoalY = 0;
CEvent Event_MNPOSE;//获取到可用位置
CEvent Event_MNNOPOSE;//获取到不可用位置
CEvent Event_MNSTOP; //停止
HANDLE Event_MNList[3] = { Event_MNPOSE, Event_MNNOPOSE, Event_MNSTOP };

HANDLE g_MNEventPose = CreateEvent(NULL, FALSE, FALSE, NULL);  //获取位置事件

//写日志
CString m_MNSavePath = "";
CCDMLogInfo* m_MNpMyLog = NULL;

int MNStopState = 0;   //结束状态，1:成功结束； 2:超时结束；3:任务停止

//多目标位置移动
int g_MNAllofGoalX[20];  //目标点的X坐标
int g_MNAllofGoalY[20];  //目标点的Y坐标
int g_MNGoalsTotal;   //目标点个数
bool g_MNbSingleorMulti = false;   //false为读取配置文件的多目标，true为手动设置的一个目标
// SIMUAGVTEST 对话框

IMPLEMENT_DYNAMIC(SIMUAGVTEST, CDialogEx)

SIMUAGVTEST::SIMUAGVTEST(CWnd* pParent /*=NULL*/)
: CDialogEx(SIMUAGVTEST::IDD, pParent)
, m_MNUartPort(0)
, m_MNLineSpeed(_T(""))
, m_MNAngleSpeed(_T(""))
, MNSpeedY(0)
, MNSpeedAngle(0)
, MNSpeedSetInterval(40)
, MNPoseGetInterval(150)
, MNPoseX(0)
, MNPoseY(0)
, MNPoseAngle(0)
, MNPoseGetCount(0)
, MNPoseTimestamp(_T(""))
, MNInvalidCount(0)
, MNSpeedX(0)
, sMNGoalX(_T(""))
, sMNGoalY(_T(""))
, m_VelCheck(FALSE)
{

}

SIMUAGVTEST::~SIMUAGVTEST()
{
}

void SIMUAGVTEST::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_COMBOX_PORTNO, m_MNUartPort);
	DDX_Text(pDX, IDC_EDIT_MNLINESPEED, m_MNLineSpeed);
	DDX_Text(pDX, IDC_EDIT_MNANGLESPEED, m_MNAngleSpeed);
	DDX_Text(pDX, IDC_EDIT_MNSPEEDY, MNSpeedY);
	DDX_Text(pDX, IDC_EDIT_MNSPEEDANGLE, MNSpeedAngle);
	DDX_Text(pDX, IDC_EDIT_MNSPEEDSETINTERVAL, MNSpeedSetInterval);
	DDX_Text(pDX, IDC_EDIT_MNPOSEGETINTERVAL, MNPoseGetInterval);
	DDX_Text(pDX, IDC_EDIT_MNPOSEX, MNPoseX);
	DDX_Text(pDX, IDC_EDIT_MNPOSEY, MNPoseY);
	DDX_Text(pDX, IDC_EDIT_MNPOSEANGLE, MNPoseAngle);
	DDX_Text(pDX, IDC_EDIT_MNGETCOUNT, MNPoseGetCount);
	DDX_Text(pDX, IDC_EDIT_MNPOSETIMESTAMP, MNPoseTimestamp);
	DDX_Text(pDX, IDC_EDIT_MNINVALIDCOUNT, MNInvalidCount);
	DDX_Text(pDX, IDC_EDIT_MNSPEEDX, MNSpeedX);
	DDX_Text(pDX, IDC_EDIT_MNGOALLOCATIONX, sMNGoalX);
	DDX_Text(pDX, IDC_EDIT_MNGOALLOCATIONY, sMNGoalY);
	DDX_Control(pDX, IDC_COMBOX_GETGOALSINFOMODE, m_ComxbGetGoalsInfoMode);
	DDX_Control(pDX, IDC_RICHEDIT21, m_MNrichlog);
	DDX_Control(pDX, IDC_LIST2, m_MNlistctrl);
	DDX_Check(pDX, IDC_CHECK_Vel, m_VelCheck);
}


BEGIN_MESSAGE_MAP(SIMUAGVTEST, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_OPENUART, &SIMUAGVTEST::OnBnClickedBtnOpenuart)
	ON_BN_CLICKED(IDC_BTN_CLOSEUART, &SIMUAGVTEST::OnBnClickedBtnCloseuart)
	ON_BN_CLICKED(IDC_BTN_GOFORWARD, &SIMUAGVTEST::OnBnClickedBtnGoforward)
	ON_BN_CLICKED(IDC_BTN_STOPAGV, &SIMUAGVTEST::OnBnClickedBtnStopagv)
	ON_BN_CLICKED(ID_BTN_OK, &SIMUAGVTEST::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_BTN_LOADDATA, &SIMUAGVTEST::OnBnClickedBtnLoaddata)
	ON_BN_CLICKED(IDC_BUTTON_AUTOTEST, &SIMUAGVTEST::OnBnClickedButtonAutotest)
	ON_BN_CLICKED(IDC_BTN_STOPAUTOTEST, &SIMUAGVTEST::OnBnClickedBtnStopautotest)
	ON_MESSAGE(WM_MNWRITELOG, &SIMUAGVTEST::OnMnwritelog)
	ON_MESSAGE(WM_MNLMInfoView, &SIMUAGVTEST::OnMnlminfoview)
	ON_BN_CLICKED(IDC_BTN_STOPTEST, &SIMUAGVTEST::OnBnClickedBtnStoptest)
	ON_EN_CHANGE(IDC_EDIT_MNLINESPEED, &SIMUAGVTEST::OnEnChangeEditMnlinespeed)
	ON_EN_CHANGE(IDC_EDIT_MNANGLESPEED, &SIMUAGVTEST::OnEnChangeEditMnanglespeed)
	ON_MESSAGE(WM_MNLMSettingInfo, &SIMUAGVTEST::OnMnlmsettinginfo)
	ON_CBN_SELCHANGE(IDC_COMBOX_GETGOALSINFOMODE, &SIMUAGVTEST::OnCbnSelchangeComboxGetgoalsinfomode)
	ON_BN_CLICKED(IDC_BTN_CreateMap, &SIMUAGVTEST::OnBnClickedBtnCreatemap)
END_MESSAGE_MAP()


// SIMUAGVTEST 消息处理程序


void SIMUAGVTEST::OnBnClickedBtnOpenuart()
{
	// TODO:  在此添加控件通知处理程序代码
	// TODO: 在此添加控件通知处理程序代码
	SerPortPar serPortPar;
	if (!m_MNUartOpenFlag)
	{
		UpdateData(TRUE);
		serPortPar.nPort = m_MNUartPort + 1;
		serPortPar.nBaud = 115200;
		serPortPar.lpRecvFun = (LPVOID)OnSendCommRecv;
		serPortPar.lpParam = (LPVOID)this;

		if (OpenComm(&serPortPar))
		{
			m_MNUartOpenFlag = TRUE;
			GetDlgItem(IDC_COMBOX_PORTNO)->EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_OPENUART)->EnableWindow(FALSE);
			GetDlgItem(IDC_BTN_CLOSEUART)->EnableWindow(TRUE);
		}
		else
		{
			MessageBox("没有发现此串口或被占用！");
			m_MNUartOpenFlag = FALSE;
		}
	}
	else
	{

	}
}


void SIMUAGVTEST::OnBnClickedBtnCloseuart()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_MNUartOpenFlag)
	{
		if (CloseComm(m_MNUartPort + 1))
		{
			m_MNUartOpenFlag = FALSE;
			m_MNbSendData = FALSE;

			GetDlgItem(IDC_COMBOX_PORTNO)->EnableWindow(TRUE);//禁止更改串口端口控件
			GetDlgItem(IDC_BTN_OPENUART)->EnableWindow(TRUE);
			GetDlgItem(IDC_BTN_STOPAGV)->EnableWindow(FALSE);

		}
		else
		{
			MessageBox("串口关闭失败！");
			m_MNUartOpenFlag = TRUE;
		}
	}
	else
	{
		m_MNUartOpenFlag = FALSE;
	}
}

float MNCalculateTicksFromLineSpeed(float LineSpeed)
{
	float ticksLineSpeed = LineSpeed * 1200 / (0.12*PI) / 30.00;  //脉冲数
	return ticksLineSpeed;
}

float MNCalculateTicksFromAngleSpeed(float AngleSpeed)
{
	float AngleSpeed2LineSpeed = AngleSpeed*0.35 / 2.00;
	float ticksAngleSpeed = AngleSpeed2LineSpeed * 1200 / (0.12*PI) / 30.00;
	return ticksAngleSpeed;
}

DWORD WINAPI MNAGVControlProc(LPVOID lpParam)
{
	SIMUAGVTEST *pMainDLg = (SIMUAGVTEST *)lpParam;
	SerPortPar serPortPar;

	serPortPar.nPort = pMainDLg->m_MNUartPort + 1;
	serPortPar.pDataBuf = g_MNbSendParameData;
	serPortPar.nDataBufSize = g_MNbSendParameDataSize;

	while (MNAGVControlRunning == true)
	{
		if (m_MNUartOpenFlag)
		{
			serPortPar.lpSendFun = (LPVOID)OnSendCommSendStop;
			serPortPar.lpParam = (LPVOID)pMainDLg;
			if (!SendComm(&serPortPar))
			{
				pMainDLg->MessageBox("数据发送：数据发送失败!");
				MNAGVControlRunning = false;
			}
			else
			{
				m_MNbSendData = TRUE;
			}
		}
		else
		{
			//pMainDLg->MessageBox("串口未打开!");
			MNAGVControlRunning = false;
		}
		Sleep(150);
	}
	pMainDLg->GetDlgItem(IDC_BTN_GOFORWARD)->EnableWindow(TRUE);
	pMainDLg->GetDlgItem(IDC_BTN_STOPAGV)->EnableWindow(FALSE);

	return 0;
}

void SIMUAGVTEST::OnBnClickedBtnGoforward()
{
	// TODO:  在此添加控件通知处理程序代码
	UpdateData(TRUE);
	float fLineSpeed = atof(m_MNLineSpeed) / 1000;
	float fAngleSpeed = atof(m_MNAngleSpeed) / 1000 * (PI / 180);
	float ticksLineSpeed = MNCalculateTicksFromLineSpeed(fLineSpeed);
	float ticksAngleSpeed = MNCalculateTicksFromAngleSpeed(fAngleSpeed);
	float fLeftTicks = ticksLineSpeed + ticksAngleSpeed;
	float fRightTicks = ticksLineSpeed - ticksAngleSpeed;
	int LeftTicks = 0;
	int RightTicks = 0;
	if (fLeftTicks >= 0)
	{
		LeftTicks = (int)(fLeftTicks * 10 + 5) / 10;
	}
	else
	{
		LeftTicks = (int)(fLeftTicks * 10 - 5) / 10;
	}
	if (fRightTicks >= 0)
	{
		RightTicks = (int)(fRightTicks * 10 + 5) / 10;  //四舍五入  
	}
	else
	{
		RightTicks = (int)(fRightTicks * 10 - 5) / 10;  //四舍五入
	}

	CString temp = "";
	CString DataTemp = "z ";
	temp.Format("%d", LeftTicks);
	DataTemp += temp + " ";
	temp.Format("%d", RightTicks);
	DataTemp += temp + "\r";

	g_MNbSendParameDataSize = DataTemp.GetLength();
	memcpy(g_MNbSendParameData, DataTemp, g_MNbSendParameDataSize);
	DataTemp.ReleaseBuffer();

	DWORD ThreadID;
	HANDLE hThreadAGVControl;
	MNAGVControlRunning = true;
	hThreadAGVControl = CreateThread(NULL, 0, MNAGVControlProc, this, NULL, &ThreadID);

	GetDlgItem(IDC_BTN_GOFORWARD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_STOPAGV)->EnableWindow(TRUE);
}


void SIMUAGVTEST::OnBnClickedBtnStopagv()
{
	// TODO:  在此添加控件通知处理程序代码
	MNAGVControlRunning = false;
	Sleep(150);
	CString DataTemp = "z 0 0\r";
	g_MNbSendParameDataSize = DataTemp.GetLength();
	memcpy(g_MNbSendParameData, DataTemp, g_MNbSendParameDataSize);
	DataTemp.ReleaseBuffer();

	SerPortPar serPortPar;
	if (m_MNUartOpenFlag)
	{
		serPortPar.nPort = m_MNUartPort + 1;

		serPortPar.pDataBuf = g_MNbSendParameData;
		serPortPar.nDataBufSize = g_MNbSendParameDataSize;
		serPortPar.lpSendFun = (LPVOID)OnSendCommSendStop;
		serPortPar.lpParam = (LPVOID)this;
		if (!SendComm(&serPortPar))
		{
			MessageBox("数据发送：数据发送失败!");
		}
		else
		{
			m_MNbSendData = TRUE;
		}
	}
	else
	{
		//MessageBox("串口未打开!");
	}
}

void SIMUAGVTEST::OnMNCreateFile(CString fileName)
{
	CFile file;
	CString path;
	GetModuleFileName(NULL, path.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	path.ReleaseBuffer();
	int pos = path.ReverseFind('\\');
	path = path.Left(pos);
	m_MNSavePath = path + _T("\\") + "AGV测试结果";
	if (!PathIsDirectory(m_MNSavePath))
	{
		BOOL bRet = CreateDirectory(m_MNSavePath, NULL);
	}

	m_MNSavePath = m_MNSavePath + "\\" + fileName;

	file.Open(m_MNSavePath, CFile::modeCreate | CFile::modeWrite);
	file.Close();
}

void SIMUAGVTEST::OnMNPrepareStartTest(CString TestName)
{
	GraphForm2d * pView;
	pView = (GraphForm2d*)pMNMainFrameThis->m_wndLeftSplitter.GetPane(0, 0);
	((CSeries)(pView->m_SIMUAGVTESTDLG.m_MNChartrsTrack.Series(0))).Clear();
	CString FileName = "";
	CString strTemp = "";
	FileName = CTime::GetCurrentTime().Format("测试日志_%Y-%m-%d-%H-%M-%S.txt_AGV");
	OnMNCreateFile(FileName);
	m_MNpMyLog = CCDMLogInfo::GetInstance(m_MNSavePath.GetBuffer(m_MNSavePath.GetLength()));
	strTemp = "\r\n========" + CTime::GetCurrentTime().Format("%H:%M:%S ") + TestName + "========\r\n";
	if (m_MNpMyLog)
		m_MNpMyLog->SetNotify(strTemp.GetBuffer(strTemp.GetLength()));
}

DWORD WINAPI GetMNPoseThreadProc(LPVOID lpParam)
{
	SIMUAGVTEST *pMainDLg = (SIMUAGVTEST *)lpParam;
	Sleep(100);
	while (MNLaserGetPostRuning == true)
	{
		if (pMNMainFrameThis->m_NetConnection.bConnected)
		{
			pMNMainFrameThis->PostMessage(WM_SENDCMD, GetPose_zhilingSize, (LPARAM)GetPose_zhiling);
		}
		else
		{
			MNLaserGetPostRuning = false;
		}
		if (MNPoseInterval > 0)
		{
			Sleep(MNPoseInterval);
		}
		else
		{
			Sleep(150);
		}
	//(PoseInterval);
	}
	pMainDLg->OnBnClickedBtnStopagv();
	pMainDLg->GetDlgItem(ID_BTN_OK)->EnableWindow(TRUE);
	pMainDLg->GetDlgItem(IDC_BTN_LOADDATA)->EnableWindow(TRUE);
	return 0;
}
DWORD WINAPI MNSetSpeedThreadProc(LPVOID lpParam)
{
	Sleep(100);
	SIMUAGVTEST *pSim = (SIMUAGVTEST *)lpParam;
	int numSendSpeed=0;
	while (MNLaserSetSpeedRuning == true)
	{
		if (pMNMainFrameThis->m_NetConnection.bConnected)
		{
			if (LaserTimestap > 0)
			{
				CString temp;
				CString DataTemp = "sMN mNPOSSetSpeed ";  //包括开头的开始符
				temp.Format("%X", pSim->MNSpeedX);
				DataTemp += temp.Right(4) + " ";
				temp.Format("%X", pSim->MNSpeedY);
				DataTemp += temp.Right(4) + " ";
				temp.Format("%X", pSim->MNSpeedAngle);
				DataTemp += temp + " ";
				temp.Format("%X", 40 * numSendSpeed + LaserTimestap + 100);
				DataTemp += temp;   //时间戳
				DataTemp += " 1";   //CoordBase+结束符
				SpeedSet_zhilingSize = DataTemp.GetLength();
				memcpy(SpeedSet_zhiling, DataTemp, SpeedSet_zhilingSize);
				DataTemp.ReleaseBuffer();
				pMNMainFrameThis->PostMessage(WM_SENDCMD, SpeedSet_zhilingSize, (LPARAM)SpeedSet_zhiling);
			}
			numSendSpeed++;
		}
		else
		{
			MNLaserSetSpeedRuning = false;
		}
		if (MNSpeedInterval > 0)
		{
			Sleep(MNSpeedInterval);//(SpeedInterval);
		}
		else
		{
			Sleep(40);
		}
	}
	return 0;
}

UINT MNSetLaserMap(LPVOID lparam)
{
	SIMUAGVTEST *pAgv = (SIMUAGVTEST *)lparam;
	pMNMainFrameThis->PostMessage(WM_SENDCMD, MappingModeSet_zhilingSize, (LPARAM)MappingModeSet_zhiling);//靶标获取模式设置
	DWORD dw_PXMap = WaitForSingleObject(g_MNPXMap, 5000); //等事件  调试用 INFINITE 
	ResetEvent(g_MNPXMap);//不能少，少了如果set了的话就会一直保持该状态
	switch (dw_PXMap)
	{
	case WAIT_OBJECT_0:		//等到事件			
		break;
	case WAIT_TIMEOUT:		//超时未收到回复
			pAgv->MNWriteToRichEdit("靶标获取失败，请重新开始测试!");
			pAgv->OnBnClickedBtnStoptest();
			AfxMessageBox("靶标获取失败，请重新开始测试!");
		break;
	case WAIT_FAILED:
		pAgv->MNWriteToRichEdit("靶标获取失败，请重新开始测试!");
		pAgv->OnBnClickedBtnStoptest();
		AfxMessageBox("靶标获取失败，请重新开始测试!");
		break;
	}
	return 0;
}

UINT FuncGetLaserTimeStap(LPVOID lparam)
{
	CString GetTimeStampDataTemp = "sMN mNAVGetTimestamp";  //包括开头的开始符
	GetTimeStamp_zhilingSize = GetTimeStampDataTemp.GetLength();
	memcpy(GetTimeStamp_zhiling, GetTimeStampDataTemp, GetTimeStamp_zhilingSize);
	GetTimeStamp_zhiling[GetTimeStamp_zhilingSize] = 3;
	GetTimeStampDataTemp.ReleaseBuffer();
	pMNMainFrameThis->PostMessage(WM_SENDCMD, GetTimeStamp_zhilingSize+1, (LPARAM)GetTimeStamp_zhiling);
	Sleep(2000);
	return 0;
}

void SIMUAGVTEST::OnBnClickedBtnOk()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!pMNMainFrameThis->m_NetConnection.bConnected)//|| (!m_MNUartOpenFlag)
	{
		MessageBox("网口没连接!");
		return;
	}
	f_SimuAGV = true;//MainFrm中向SimuAgvDlg发消息
	f_AGVTest = false;//MainFrm中不向AgvDlg发消息
	m_MNrichlog.SetSel(0, -1);
	m_MNrichlog.ReplaceSel("");
	m_MNlistctrl.DeleteAllItems();//清空上一次获取的靶标信息
	//小车控制
	OnBnClickedBtnGoforward();
	DWORD ThreadID;
	HANDLE hThreadSetSpeed;
	HANDLE hThreadGetPose;
	UpdateData(true);
	CString temp = "";
	MNPoseInterval = MNPoseGetInterval;
	MNPoseX = 0;
	MNPoseY = 0;
	MNPoseAngle = 0;
	MNPoseTimestamp = "";
	MNPoseGetCount = 0;
	MNInvalidCount = 0;
	bool VelCheck = m_VelCheck;
	UpdateData(false);
	GetDlgItem(ID_BTN_OK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BTN_LOADDATA)->EnableWindow(FALSE);
	pMNMainFrameThis->KillTimer(0);
	temp = "匀速测试";
	OnMNPrepareStartTest(temp);
	//pMNMainFrameThis->PostMessage(WM_SENDCMD, MappingModeSet_zhilingSize, (LPARAM)MappingModeSet_zhiling);//靶标获取模式设置
	Sleep(1000);
	if (VelCheck)
	{
		//////////////////////////同步时间戳
		AfxBeginThread(FuncGetLaserTimeStap, this);
		Sleep(500);
		///////////////////////////////
		MNSpeedInterval = MNSpeedSetInterval;
		MNLaserSetSpeedRuning = true;
		hThreadSetSpeed = CreateThread(NULL, 0, MNSetSpeedThreadProc, this, NULL, &ThreadID);
	}
	MNLaserGetPostRuning = true;
	hThreadGetPose = CreateThread(NULL, 0, GetMNPoseThreadProc, this, NULL, &ThreadID);
}

void SIMUAGVTEST::MNWriteToRichEdit(CString content)
{
	CString strLog_Time = CTime::GetCurrentTime().Format(_T("%H:%M:%S ")) + ":";
	m_MNrichlog.SetSel(-1, -1);
	CString strLog = strLog_Time + content + "\n";
	m_MNrichlog.ReplaceSel(strLog);
	m_MNrichlog.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

void SIMUAGVTEST::OnBnClickedBtnLoaddata()
{
	// TODO:  在此添加控件通知处理程序代码
	CStdioFile hufile;
	GraphForm2d * pView;
	CString str = "";
	int IndexStart = 0;
	int len = 0;
	CString strTip = "";
	CString temp = "";
	int IndexDotStart = 0;
	int IndexDotEnd = 0;
	int DataX = 0;
	int DataY = 0;
	int DataLocation = 0; //取数位置

	pView = (GraphForm2d*)pMNMainFrameThis->m_wndLeftSplitter.GetPane(0, 0);
	LoadFilePath = CFileUtils::OpenFileDlg(TRUE, _T("H Files (*.txt_AGV)|*.txt_AGV||"), NULL, NULL, NULL);
	CString l_sNameTmp;
	l_sNameTmp = LoadFilePath.Right(LoadFilePath.GetLength() - LoadFilePath.ReverseFind('.') - 1);
	if (l_sNameTmp != "txt_AGV")
	{
		MessageBox("请选择后缀为txt_AGV的文件！");
		return;
	}

	MNPoseGetCount = 0;
	MNInvalidCount = 0;
	((CSeries)(pView->m_SIMUAGVTESTDLG.m_MNChartrsTrack.Series(0))).Clear();//m_AGVTestDlg 不对
	if (hufile.Open(LoadFilePath, CFile::modeRead))
	{
		while (hufile.ReadString(str))
		{
			IndexStart = str.Find("解析后为：");
			len = str.GetLength();
			if (IndexStart != -1)
			{
				strTip = str.Mid(IndexStart + 10, len - 5);
				while (IndexDotEnd != -1)
				{
					IndexDotStart = IndexDotEnd;
					IndexDotEnd = strTip.Find(", ", IndexDotStart + 1);
					DataLocation++;
					if (DataLocation == 2)
					{
						temp = strTip.Mid(IndexDotStart + 2, IndexDotEnd - IndexDotStart - 2);
						DataX = atoi(temp);
					}
					else if (DataLocation == 3)
					{
						temp = strTip.Mid(IndexDotStart + 2, IndexDotEnd - IndexDotStart - 2);
						DataY = atoi(temp);
						MNPoseGetCount++;
						((CSeries)(pView->m_SIMUAGVTESTDLG.m_MNChartrsTrack.Series(0))).AddXY(DataX, DataY, strTip, 0);
					}
					else if (DataLocation > 3)
					{
						DataLocation = 0;
						IndexDotStart = 0;
						IndexDotEnd = 0;
						break;
					}
				}
			}
			else
			{
				IndexStart = str.Find("无效数据");
				if (IndexStart != -1)
				{
					MNInvalidCount++;
				}
			}
		}
		hufile.Close();
		MNPoseX = 0;
		MNPoseY = 0;
		MNPoseAngle = 0;
		MNPoseTimestamp = "";
		UpdateData(false);
		MessageBox("读取绘制完成，有效和无效数据已显示！");
	}
}

DWORD WINAPI MNAutoSimulation(LPVOID lpParam)
{
	SIMUAGVTEST *pMainDLg = (SIMUAGVTEST *)lpParam;
	float DistenceGP = 0;  //距离
	float AngleGP = 0;  //方向角
	float LaserRotateAngle = 0;  //激光转角
	float AGVRotateSpeed = 0; //小车转角
	float AGVLineSpeed = 0;   //小车线速度
	float k = 0.05;      //系数
	float ticksLineSpeed = 0; //线速度的脉冲数
	float ticksAngleSpeed = 0; //角速度的脉冲数
	float fLeftTicks = 0;      //左轮
	float fRightTicks = 0;   //右轮
	int LeftTicks = 0;
	int RightTicks = 0;
	short RealSendSpeedX = 0;
	short RealSendSpeedY = 0;
	int RealSendSpeedAngle = 0;
	CString temp = "";
	CString DataTemp = "";
	DWORD ThreadID;
	HANDLE hThreadAGVControl;
	HANDLE hThreadSetSpeed;
	HANDLE hThreadGetPose;
	CString sLog = "";
	int CurrentGoalNum = 0;   //多目标时当前的目标序号数
	int StatisticalPoseNum = 0;  //接收绘制的计数
	int ArriveJudgeCount = 0;
	bool OnlyGetPose = false;  //开始统计数据
	DWORD CaseState;

	if (g_MNbSingleorMulti == false)
	{
		CurrentGoalNum++;
		g_MNGoalX = g_MNAllofGoalX[0];
		g_MNGoalY = g_MNAllofGoalY[0];
		pMainDLg->sMNGoalX.Format("%d", g_MNGoalX);
		pMainDLg->sMNGoalY.Format("%d", g_MNGoalY);
		pMainDLg->GetDlgItem(IDC_EDIT_MNGOALLOCATIONX)->SetWindowText(pMainDLg->sMNGoalX);
		pMainDLg->GetDlgItem(IDC_EDIT_MNGOALLOCATIONY)->SetWindowText(pMainDLg->sMNGoalY);
		sLog.Format("向第 %d 个目标点运动,坐标为X:%d, Y:%d ", CurrentGoalNum, g_MNGoalX, g_MNGoalY);
		pMainDLg->SendMessage(WM_MNWRITELOG, 0, (LPARAM)(LPCTSTR)sLog);
	}

	MNLaserGetPostRuning = true;
	hThreadGetPose = CreateThread(NULL, 0, GetMNPoseThreadProc, pMainDLg, NULL, &ThreadID);
	while (MNSimulationRun)
	{
		CaseState = WaitForMultipleObjects(3, Event_MNList, false, 20000);	//等待超时
		if (CaseState == WAIT_TIMEOUT)
		{
			//超时未收到回复，退出
			MNStopState = 2;
			SetEvent(Event_MNSTOP);
		}
		else if (CaseState == WAIT_OBJECT_0) //Event_POSE
		{
			//计算应该设置给小车的线速度和角速度
			DistenceGP = sqrt(pow(float(g_MNGoalX - pMainDLg->MNPoseX), 2) + pow(float(g_MNGoalY - pMainDLg->MNPoseY), 2));
			//判断是否到达
			if (OnlyGetPose == false)
			{
				if (DistenceGP<80)
				{
					DataTemp = "sMN mNPOSSetSpeed 0 0 0 0 1";
					SpeedSet_zhilingSize = DataTemp.GetLength();
					memcpy(SpeedSet_zhiling, DataTemp, SpeedSet_zhilingSize);
					DataTemp = "z 0 0\r";
					g_MNbSendParameDataSize = DataTemp.GetLength();
					memcpy(g_MNbSendParameData, DataTemp, g_MNbSendParameDataSize);
					ArriveJudgeCount++;
					if (ArriveJudgeCount > 10)
					{
						ArriveJudgeCount = 0;
						OnlyGetPose = true;
					}
					continue;
				}
			}
			else
			{
				//若已到达就停止
				if (DistenceGP<100)
				{
					DataTemp = "sMN mNPOSSetSpeed 0 0 0 0 1";
					SpeedSet_zhilingSize = DataTemp.GetLength();
					memcpy(SpeedSet_zhiling, DataTemp, SpeedSet_zhilingSize);
					DataTemp = "z 0 0\r";
					g_MNbSendParameDataSize = DataTemp.GetLength();
					memcpy(g_MNbSendParameData, DataTemp, g_MNbSendParameDataSize);
					StatisticalPoseNum++;
					if (StatisticalPoseNum>50)
					{
						OnlyGetPose = false;
						StatisticalPoseNum = 0;
						if (g_MNbSingleorMulti == false && (CurrentGoalNum < g_MNGoalsTotal))
						{
							CurrentGoalNum++;
							g_MNGoalX = g_MNAllofGoalX[CurrentGoalNum - 1];
							g_MNGoalY = g_MNAllofGoalY[CurrentGoalNum - 1];
							pMainDLg->sMNGoalX.Format("%d", g_MNGoalX);
							pMainDLg->sMNGoalY.Format("%d", g_MNGoalY);
							pMainDLg->GetDlgItem(IDC_EDIT_MNGOALLOCATIONX)->SetWindowText(pMainDLg->sMNGoalX);
							pMainDLg->GetDlgItem(IDC_EDIT_MNGOALLOCATIONY)->SetWindowText(pMainDLg->sMNGoalY);
							sLog.Format("向第 %d 个目标点运动,坐标为X:%d, Y:%d ", CurrentGoalNum, g_MNGoalX, g_MNGoalY);
							pMainDLg->SendMessage(WM_MNWRITELOG, 0, (LPARAM)(LPCTSTR)sLog);
						}
						else
						{
							MNStopState = 1;
							SetEvent(Event_MNSTOP);
						}
					}
					continue;
				}
			}
			AngleGP = atan2(float(g_MNGoalY - pMainDLg->MNPoseY), float(g_MNGoalX - pMainDLg->MNPoseX)) * 180 / PI;
			if (AngleGP<0)
			{
				AngleGP = 360 + AngleGP;
			}
			LaserRotateAngle = AngleGP * 1000 - pMainDLg->MNPoseAngle;
			if (LaserRotateAngle>180000)
			{
				LaserRotateAngle = LaserRotateAngle - 360000;
			}
			else if (LaserRotateAngle < -180000)
			{
				LaserRotateAngle = 360000 + LaserRotateAngle;
			}

			AGVRotateSpeed = -LaserRotateAngle / 5;
			AGVLineSpeed = k*DistenceGP;
			pMainDLg->m_MNLineSpeed.Format("%d", (int)AGVLineSpeed);
			pMainDLg->m_MNAngleSpeed.Format("%d", (int)AGVRotateSpeed);

			pMainDLg->MNSpeedX = (short)(AGVLineSpeed*float(g_MNGoalX - pMainDLg->MNPoseX) / DistenceGP);
			pMainDLg->MNSpeedY = (short)(AGVLineSpeed*float(g_MNGoalY - pMainDLg->MNPoseY) / DistenceGP);
			pMainDLg->MNSpeedAngle = -(int)AGVRotateSpeed;
			pMainDLg->GetDlgItem(IDC_EDIT_MNLINESPEED)->SetWindowText(pMainDLg->m_MNLineSpeed);
			pMainDLg->GetDlgItem(IDC_EDIT_MNANGLESPEED)->SetWindowText(pMainDLg->m_MNAngleSpeed);
			pMainDLg->SetDlgItemInt(IDC_EDIT_MNSPEEDX, pMainDLg->MNSpeedX);
			pMainDLg->SetDlgItemInt(IDC_EDIT_MNSPEEDY, pMainDLg->MNSpeedY);
			pMainDLg->SetDlgItemInt(IDC_EDIT_MNSPEEDANGLE, pMainDLg->MNSpeedAngle);
			temp.Format("%d, %d, %d", pMainDLg->MNSpeedX, pMainDLg->MNSpeedY, pMainDLg->MNSpeedAngle);
			sLog = "车线和角: " + pMainDLg->m_MNLineSpeed + ", " + pMainDLg->m_MNAngleSpeed + "  激光XY角: " + temp + "\r\n";

			//限制
			if (AGVLineSpeed<-200)
			{
				AGVLineSpeed = -200;
			}
			else if (AGVLineSpeed >200)
			{
				AGVLineSpeed = 200;
			}

			if (AGVRotateSpeed<-4000)
			{
				AGVRotateSpeed = -4000;
			}
			else if (AGVRotateSpeed >4000)
			{
				AGVRotateSpeed = 4000;
			}

			if (LaserRotateAngle > 20000)
			{
				AGVRotateSpeed = -2000;
				AGVLineSpeed = 0;
			}
			else if (LaserRotateAngle < -20000)
			{
				AGVRotateSpeed = 2000;
				AGVLineSpeed = 0;
			}

			RealSendSpeedAngle = AGVRotateSpeed;
			RealSendSpeedX = (short)(AGVLineSpeed*cos((double)RealSendSpeedAngle / 180 * PI));
			RealSendSpeedY = (short)(AGVLineSpeed*sin((double)RealSendSpeedAngle / 180 * PI));
			//计算脉冲
			ticksLineSpeed = MNCalculateTicksFromLineSpeed(AGVLineSpeed / 1000);
			ticksAngleSpeed = MNCalculateTicksFromAngleSpeed(AGVRotateSpeed / 1000 * (PI / 180));
			fLeftTicks = ticksLineSpeed + ticksAngleSpeed;
			fRightTicks = ticksLineSpeed - ticksAngleSpeed;
			if (fLeftTicks >= 0)
			{
				LeftTicks = (int)(fLeftTicks * 10 + 5) / 10;
			}
			else
			{
				LeftTicks = (int)(fLeftTicks * 10 - 5) / 10;
			}
			if (fRightTicks >= 0)
			{
				RightTicks = (int)(fRightTicks * 10 + 5) / 10;
			}
			else
			{
				RightTicks = (int)(fRightTicks * 10 - 5) / 10;
			}

			DataTemp = "sMN mNPOSSetSpeed ";
			temp.Format("%X", RealSendSpeedX);
			DataTemp += temp.Right(4) + " ";
			temp.Format("%X", RealSendSpeedY);
			DataTemp += temp.Right(4) + " ";
			temp.Format("%X", RealSendSpeedAngle);
			DataTemp += temp + " ";
			DataTemp += "0 ";
			DataTemp += "1";
			SpeedSet_zhilingSize = DataTemp.GetLength();
			memcpy(SpeedSet_zhiling, DataTemp, SpeedSet_zhilingSize);
			sLog = sLog + "         发给激光器: " + DataTemp.Mid(1, DataTemp.GetLength() - 2) + "\r\n";
			DataTemp = "z ";
			temp.Format("%d", LeftTicks);
			DataTemp += temp + " ";
			temp.Format("%d", RightTicks);
			DataTemp += temp + "\r";
			g_MNbSendParameDataSize = DataTemp.GetLength();
			memcpy(g_MNbSendParameData, DataTemp, g_MNbSendParameDataSize);
			sLog = sLog + "         发送给小车: " + DataTemp.Mid(0, DataTemp.GetLength() - 1) + "\r\n";

			if (MNLaserSetSpeedRuning == false)
			{
				MNLaserSetSpeedRuning = true;
				hThreadSetSpeed = CreateThread(NULL, 0, MNSetSpeedThreadProc, pMainDLg, NULL, &ThreadID);
			}

			if (MNAGVControlRunning == false)
			{
				MNAGVControlRunning = true;
				hThreadAGVControl = CreateThread(NULL, 0, MNAGVControlProc, pMainDLg, NULL, &ThreadID);
			}
			pMainDLg->SendMessage(WM_MNWRITELOG, 0, (LPARAM)(LPCTSTR)sLog);
		}
		else if (CaseState == (WAIT_OBJECT_0 + 1)) //Event_NOPOSE
		{
			DataTemp = "sMN mNPOSSetSpeed 0 0 0 0 1";
			SpeedSet_zhilingSize = DataTemp.GetLength();
			memcpy(SpeedSet_zhiling, DataTemp, SpeedSet_zhilingSize);
			DataTemp = "z 0 0\r";
			g_MNbSendParameDataSize = DataTemp.GetLength();
			memcpy(g_MNbSendParameData, DataTemp, g_MNbSendParameDataSize);
		}
		else if (CaseState == (WAIT_OBJECT_0 + 2)) //Event_STOP
		{
			//主动退出		
			MNSimulationRun = false;
			MNAGVControlRunning = false;
			MNLaserGetPostRuning = false;
			MNLaserSetSpeedRuning = false;
			pMainDLg->OnBnClickedBtnStopagv();
			if (MNStopState == 1)
			{
				if (g_MNbSingleorMulti == false)
				{
					sLog = "测试完成，已到达所有目的地！\r\n";
					pMainDLg->SendMessage(WM_MNWRITELOG, 0, (LPARAM)(LPCTSTR)sLog);
					pMainDLg->MessageBox("测试完成，已到达所有目的地！", MB_OK);
				}
				else
				{
					sLog = "测试完成，已到达目的地！\r\n";
					pMainDLg->SendMessage(WM_MNWRITELOG, 0, (LPARAM)(LPCTSTR)sLog);
					pMainDLg->MessageBox("测试完成，已到达目的地！", MB_OK);
				}
			}
			else if (MNStopState == 2)
			{
				sLog = "测试结束，未达目标，超时！\r\n";
				pMainDLg->SendMessage(WM_MNWRITELOG, 0, (LPARAM)(LPCTSTR)sLog);
				pMainDLg->MessageBox("测试结束，未达目标，超时！");
			}
			else if (MNStopState == 3)
			{
				sLog = "成功测试终止！\r\n";
				pMainDLg->SendMessage(WM_MNWRITELOG, 0, (LPARAM)(LPCTSTR)sLog);
				pMainDLg->MessageBox("成功测试终止！");
			}
			pMainDLg->GetDlgItem(IDC_COMBOX_GETGOALSINFOMODE)->EnableWindow(TRUE);
			pMainDLg->GetDlgItem(IDC_BUTTON_AUTOTEST)->EnableWindow(TRUE);

			m_MNpMyLog->Release();
			pMNMainFrameThis->SetTimer(0, 500, NULL);
		}

		Sleep(1);
	}
	return 0;
}

void SIMUAGVTEST::OnBnClickedButtonAutotest()
{
	f_SimuAGV = true;//MainFrm中向SimuAgvDlg发消息
	f_AGVTest = false;//MainFrm中不向AgvDlg发消息
	// TODO:  在此添加控件通知处理程序代码
	if (g_MNbSingleorMulti == false)
	{
		CString strGoaltotal;
		CString strtmp;
		CString strGoalInfo;
		CFileUtils fileUtils;
		int index = 0;
		char szFileDir[300];
		GetCurrentDirectory(300, szFileDir);
		strcat(szFileDir, "\\setup.ini");

		fileUtils.GetConfigfileString("TargetXY", "Num_Of_Target", "0", strGoaltotal.GetBuffer(40), 40, szFileDir);
		strGoaltotal.ReleaseBuffer();
		g_MNGoalsTotal = atoi(strGoaltotal);
		if (g_MNGoalsTotal == 0 || (g_MNGoalsTotal > 20))
		{
			MessageBox("目标位置个数应该为 0-20 个！");
			return;
		}
		for (int i = 0; i < g_MNGoalsTotal; i++)
		{
			itoa(i + 1, strtmp.GetBuffer(10), 10);
			fileUtils.GetConfigfileString("TargetXY", strtmp, "0", strGoalInfo.GetBuffer(200), 200, szFileDir);
			strGoalInfo.ReleaseBuffer();
			index = strGoalInfo.Find("/");
			g_MNAllofGoalX[i] = atoi(strGoalInfo.Left(index));
			strGoalInfo.Delete(0, index + 1);

			index = strGoalInfo.Find('/');
			g_MNAllofGoalY[i] = atoi(strGoalInfo.Left(index));
			strGoalInfo.Delete(0, index + 1);
		}
	}
	else
	{
		UpdateData(TRUE);
		g_MNGoalX = atoi(sMNGoalX);
		g_MNGoalY = atoi(sMNGoalY);
	}
	CString temp = "";
	MNStopState = 0;
	if (pMNMainFrameThis->m_NetConnection.bConnected && m_MNUartOpenFlag)
	{
		DWORD ThreadID;
		HANDLE hThreadAutoSimulation;

		if (g_MNbSingleorMulti == false)
		{
			temp.Format("目标点全自动运行,共%d个目标", g_MNGoalsTotal);
		}
		else
		{
			temp.Format("目标点全自动运行,单目标点坐标为X:%d, Y:%d ", g_MNGoalX, g_MNGoalY);
		}
		OnMNPrepareStartTest(temp);
		pMNMainFrameThis->KillTimer(0);
		MNSimulationRun = true;
		hThreadAutoSimulation = CreateThread(NULL, 0, MNAutoSimulation, this, NULL, &ThreadID);
		GetDlgItem(IDC_COMBOX_GETGOALSINFOMODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_AUTOTEST)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_STOPAUTOTEST)->EnableWindow(TRUE);
	}
	else
	{
		MessageBox("请检查网络连接和串口连接！");
	}
}


void SIMUAGVTEST::OnBnClickedBtnStopautotest()
{
	// TODO:  在此添加控件通知处理程序代码
	MNStopState = 3;
	SetEvent(Event_MNSTOP);
}

BOOL SIMUAGVTEST::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	// TODO:  在此添加额外的初始化
	pMNMainFrameThis = (CMainFrame*)AfxGetMainWnd();
	m_MNlistctrl.InsertColumn(0, _T("ID"), LVCFMT_CENTER, 100, -1);
	m_MNlistctrl.InsertColumn(1, _T("X坐标(mm)"), LVCFMT_CENTER, 100, -1);
	m_MNlistctrl.InsertColumn(2, _T("Y坐标(mm)"), LVCFMT_CENTER, 100, -1);
	m_MNlistctrl.InsertColumn(3, _T("尺寸"), LVCFMT_CENTER, 100, -1);
	m_MNlistctrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_SHOWSELALWAYS | LVS_EDITLABELS);
	GetDlgItem(IDC_BTN_OPENUART)->EnableWindow(TRUE);
	GetDlgItem(IDC_BTN_CLOSEUART)->EnableWindow(FALSE);

	m_ComxbGetGoalsInfoMode.SetCurSel(0);
	GetDlgItem(IDC_EDIT_MNGOALLOCATIONX)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_MNGOALLOCATIONY)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


afx_msg LRESULT SIMUAGVTEST::OnMnwritelog(WPARAM wParam, LPARAM lParam)
{
	CString   str = (char*)lParam;
	CString getData = "";
	CTime t = CTime::GetCurrentTime();
	getData = t.Format("%H:%M:%S ") + str;
	if (m_MNpMyLog)
		m_MNpMyLog->SetNotify(getData.GetBuffer(getData.GetLength()));
	getData.ReleaseBuffer();
	return 0;
}

UINT MNSendLMSettingFirzhiling(LPVOID lpParam)
{
	SIMUAGVTEST *pAgv = (SIMUAGVTEST *)lpParam;
	if (false)//n_MNGetLM < 3
	{
		CString strTemp = "获取的靶标数量小于3个，请确认靶标个数为3个，并重新获取！";
		pAgv->MNWriteToRichEdit(strTemp);
		pAgv->OnBnClickedBtnStoptest();//停止
	}
	else
	{
		////靶标设置第一包数据
		CTime time = CTime::GetCurrentTime();
		char Hour = time.GetHour();
		char Miu = time.GetMinute();
		char Sec = time.GetSecond();
		unsigned char pn[14] = { 0x00, 0x20, 0x00, 0x00, 0x00, Hour, Miu, Sec, 0x01, 0x01, 0x00, 0x07, 0x06, 0x03 };
		ZhiLing(LMSettingFir_zhiling, pn, 0);
		LMSettingFir_zhiling[26] = 0;
		LMSettingFir_zhiling[27] = 0;
		LMSettingFir_zhiling[28] = 0;
		LMSettingFir_zhiling[29] = n_MNGetLM;
		LMSettingFir_zhiling[30] = 0;
		LMSettingFir_zhiling[31] = 2;
		AfxGetMainWnd()->PostMessage(WM_SENDCMD, LMSettingFir_zhilingSize, (LPARAM)LMSettingFir_zhiling);
		Sleep(2000);//等待2s
		///靶标设置非第一包数据
		if (f_FirDataLMSet)
		{
			f_FirDataLMSet = false;
			int L_Frame = n_MNGetLM * 14 + 28;
			int LMSettingSec_zhilingSize = L_Frame + 4;
			unsigned char LMSettingSec_zhiling[500] = {0};
			//LMSettingSec_zhiling = new unsigned char[LMSettingSec_zhilingSize];
			CTime time1 = CTime::GetCurrentTime();
			char Hour1 = time1.GetHour();
			char Miu1 = time1.GetMinute();
			char Sec1 = time1.GetSecond();
			unsigned char pn1[14] = { L_Frame >> 8, L_Frame & 0xff, 0x00, 0x00, 0x00, Hour1, Miu1, Sec1, 0x01, 0x01, 0x00, 0x07, 0x06, 0x03 };
			ZhiLing(LMSettingSec_zhiling, pn1, 0);
			LMSettingSec_zhiling[26] = 0;
			LMSettingSec_zhiling[27] = 1;
			for (int i = 0; i < n_MNGetLM; i++)
			{
				LMSettingSec_zhiling[28 + i * 14] = MNlmInfoBuf[i * 5] >> 8;
				LMSettingSec_zhiling[29 + i * 14] = MNlmInfoBuf[i * 5] & 0xff;
				LMSettingSec_zhiling[30 + i * 14] = MNlmInfoBuf[1 + i * 5] >> 24;
				LMSettingSec_zhiling[31 + i * 14] = MNlmInfoBuf[1 + i * 5] >> 16;
				LMSettingSec_zhiling[32 + i * 14] = MNlmInfoBuf[1 + i * 5] >> 8;
				LMSettingSec_zhiling[33 + i * 14] = MNlmInfoBuf[1 + i * 5] & 0xFF;
				LMSettingSec_zhiling[34 + i * 14] = MNlmInfoBuf[2 + i * 5] >> 24;
				LMSettingSec_zhiling[35 + i * 14] = MNlmInfoBuf[2 + i * 5] >> 16;
				LMSettingSec_zhiling[36 + i * 14] = MNlmInfoBuf[2 + i * 5] >> 8;
				LMSettingSec_zhiling[37 + i * 14] = MNlmInfoBuf[2 + i * 5] & 0xFF;
				LMSettingSec_zhiling[38 + i * 14] = 0;
				LMSettingSec_zhiling[39 + i * 14] = 0;
				LMSettingSec_zhiling[40 + i * 14] = MNlmInfoBuf[3 + i * 5];//形状
				LMSettingSec_zhiling[41 + i * 14] = MNlmInfoBuf[4 + i * 5];//尺寸
				if (i == n_MNGetLM - 1)
				{
					AfxGetMainWnd()->PostMessage(WM_SENDCMD, LMSettingSec_zhilingSize, (LPARAM)LMSettingSec_zhiling);
					Sleep(300);
					//if (LMSettingSec_zhiling) delete[]LMSettingSec_zhiling;
				}
			}
		}
		else
		{
			pAgv->MNWriteToRichEdit("靶标设置失败！");
		}
	}
	return 0;
}

afx_msg LRESULT SIMUAGVTEST::OnMnlminfoview(WPARAM wParam, LPARAM lParam)
{
	MNlmInfoBuf = (int *)lParam;
	n_MNGetLM = (int)wParam;//靶标个数
	CString str;
	for (int i = 0; i < n_MNGetLM; i++)
	{
		str.Format("%d", MNlmInfoBuf[i * 5]);
		m_MNlistctrl.InsertItem(i, str);
		str.Format("%d", MNlmInfoBuf[1 + i * 5]);
		m_MNlistctrl.SetItemText(i, 1, str);
		str.Format("%d", MNlmInfoBuf[2 + i * 5]);
		m_MNlistctrl.SetItemText(i, 2, str);
		str.Format("%d", MNlmInfoBuf[4 + i * 5]);
		m_MNlistctrl.SetItemText(i, 3, str);
		Sleep(10);
	}
	CString str_nlm = "";
	str_nlm.Format("%d", n_MNGetLM);
	MNWriteToRichEdit("已完成靶标获取，获取的靶标个数为：" + str_nlm);
	///靶标数据显示到listctrl
	AfxBeginThread(MNSendLMSettingFirzhiling, this);//靶标设置
	return 0;
}

DWORD WINAPI MNStopThreadProc(LPVOID lpParam)
{
	MNLaserGetPostRuning = false;
	MNLaserSetSpeedRuning = false;
	Sleep(500);
	if (m_MNpMyLog)
		m_MNpMyLog->Release();
	pMNMainFrameThis->SetTimer(0, 500, NULL);       //循环发状态查询指令

	return 0;
}

void SIMUAGVTEST::OnBnClickedBtnStoptest()
{
	// TODO:  在此添加控件通知处理程序代码
	DWORD ThreadID;
	HANDLE hThreadStop;
	if (!pMNMainFrameThis->m_NetConnection.bConnected)//|| (!m_MNUartOpenFlag)
	{
		MessageBox("网口没连接!");
		return;
	}
	GetDlgItem(ID_BTN_OK)->EnableWindow(TRUE);
	hThreadStop = CreateThread(NULL, 0, MNStopThreadProc, this, NULL, &ThreadID);
}


void SIMUAGVTEST::OnEnChangeEditMnlinespeed()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if (MNSimulationRun == false)
	{
		UpdateData(TRUE);
		int LineSpeedTemp = atoi(m_MNLineSpeed);
		if (LineSpeedTemp<-500)
		{
			m_MNLineSpeed = "-500";
		}
		else if (LineSpeedTemp >500)
		{
			m_MNLineSpeed = "500";
		}
		MNSpeedX = LineSpeedTemp;
		UpdateData(FALSE);
	}
}


void SIMUAGVTEST::OnEnChangeEditMnanglespeed()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	if (MNSimulationRun == false)
	{
		UpdateData(TRUE);
		int AngleSpeedTemp = atoi(m_MNAngleSpeed);
		if (AngleSpeedTemp<-10000)
		{
			m_MNAngleSpeed = "-10000";
		}
		else if (AngleSpeedTemp >10000)
		{
			m_MNAngleSpeed = "10000";
		}
		MNSpeedAngle = -AngleSpeedTemp;
		UpdateData(FALSE);
	}
}


afx_msg LRESULT SIMUAGVTEST::OnMnlmsettinginfo(WPARAM wParam, LPARAM lParam)
{
	int LMSetInfo = (int)lParam;
	if (LMSetInfo == 1)
	{
		CString strTemp = "靶标信息设置成功！";
		MNWriteToRichEdit(strTemp);
		///切换到导航模式
		AfxGetMainWnd()->PostMessage(WM_SENDCMD, NaviModeSet_zhilingSize, (LPARAM)NaviModeSet_zhiling);
		MNWriteToRichEdit("导航模式设置成功！");
	}
	else
	{
		CString strTemp = "靶标信息设置失败！";
		MNWriteToRichEdit(strTemp);
	}
	return 0;
}


void SIMUAGVTEST::OnCbnSelchangeComboxGetgoalsinfomode()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_ComxbGetGoalsInfoMode.GetCurSel() == 0)
	{
		GetDlgItem(IDC_EDIT_MNGOALLOCATIONX)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_MNGOALLOCATIONY)->EnableWindow(FALSE);
		g_MNbSingleorMulti = false;
	}
	else if (m_ComxbGetGoalsInfoMode.GetCurSel() == 1)
	{
		GetDlgItem(IDC_EDIT_MNGOALLOCATIONX)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_MNGOALLOCATIONY)->EnableWindow(TRUE);
		g_MNbSingleorMulti = true;
	}
}


void SIMUAGVTEST::OnBnClickedBtnCreatemap()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!pMNMainFrameThis->m_NetConnection.bConnected)//|| (!m_MNUartOpenFlag)
	{
		MessageBox("网口没连接!");
		return;
	}
	f_SimuAGV = true;//MainFrm中向SimuAgvDlg发消息
	f_AGVTest = false;//MainFrm中不向AgvDlg发消息
	m_MNlistctrl.DeleteAllItems();//清空上一次获取的靶标信息
	AfxBeginThread(MNSetLaserMap, this);
}
