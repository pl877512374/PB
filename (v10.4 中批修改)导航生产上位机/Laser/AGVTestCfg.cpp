// AGVTestCfg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "AGVTestCfg.h"
#include "afxdialogex.h"
#include "Interp.h"
#include "CAxis.h"
#include "CAxes.h"
#include "Uart/SerialUtils.h"
#include "NetParam.h"
#include "MainFrm.h"
#include "FileUtils.h"
#include "CSeries.h"
#include "io.h"
#include <fstream>
#include <cmath> 
#include "File/CDMLogInfo.h"
#include "InputDlg.h"
#define  arrayXYsize  100
#define TargetAngCount 5
#define PI  3.14159265358979323846
using namespace std;
typedef struct tagSysDevParam
{
	int nPort;
	int nBuad;
	BOOL ComConnected;
}SysDevParam;
SysDevParam g_SysDevParam;
bool LaserGetPostRuning = false;
bool AGVControlRunning = false;
typedef struct TagStructExcel
{
	CString m_strName;
	u32 m_u32Row;
	u32 m_u32Col;
}StructExcel;
unsigned int g_u32MeasurePXInfoIndex;
//ƫ�ı�����
#define readPXbuflen  50000   //��д����������
int g_nReadPXbuf[readPXbuflen];  //��д����
int g_nReadPXbuflen = 0; //ʵ�ʳ���
short bccW_PXpacks = 0;
int g_PXnNum = 0;
BOOL g_PXflag = FALSE;
BOOL g_NAVflag = FALSE;//�Ƿ���get pose��־λ
HANDLE g_PXevnet = CreateEvent(NULL, TRUE, FALSE, NULL);
HANDLE g_PXDownevnet = CreateEvent(NULL, TRUE, FALSE, NULL);//ƫ��(�������ر�)��Ӧ�¼�
HANDLE g_PXAngevent = CreateEvent(NULL, TRUE, FALSE, NULL);//�ǶȻ�ȡ��Ӧ�¼�
HANDLE g_PXMap = CreateEvent(NULL, TRUE, FALSE, NULL);//�б��ȡ��Ӧ�¼�
//����
bool f_HundredNav = false;//�����Ƿ�ﵽ100�α�־λ
int NavPoseX[100] = { 0 };//�洢100�ε����Ķ�λ����X
int NavPoseY[100] = { 0 };//�洢100�ε����Ķ�λ����Y
int NavHundredCount = 0;//�ﵽ100�ζ�λ�Ĵ���
int AnalyNavXPoseCenter[4] = { 0 };//�������ݷ���X����
int AnalyNavYPoseCenter[4] = { 0 };//�������ݷ���Y����
int *lmInfoBuf = NULL;//�б��ȡ��Ϣ
int n_GetLM = 0;//�б��ȡ���İб�����
bool f_QualiStandard = false;//�Ƿ�ȷ�Ϻϸ��׼��־λ
int QualiStandard = 5;//�ϸ��׼��Ĭ��5mm
StrRecordInfo g_sRecordPXInfo;
int ScanLmNum = 0;//05 1aָ��õ��İб����
bool f_SuccessZero = false;//�����Ƿ�ɹ���־λ
bool f_Realgap = true;//���������б����ʵ�ǶȲ��־λ
int pointRatio = 800;//��������
int everyGap = 0;//ƫ���������ݲɼ���ʱ��ÿһ�ε������б��ֵ
bool f_AGVTest = false;//MainFrm�Ƿ���AGVDlg����Ϣ��־
///////���������ļ�
CString FilePathNav = "";//���������ļ���ַ
CFile FileNav;//���������ļ�������
bool f_NavFileOpen = false;//���������ļ��Ƿ�򿪱�־λ
bool f_StopTest = false;//ֹͣ���Ա�־λ
bool f_OpenOrClosePX;//��ʼ���߹ر�ƫ������ true ����  
typedef struct TagStructMeasureInfo
{
	int m_OriginalOne;//ԭʼ1
	int m_OriginalTwo;//ԭʼ2
	int m_CompensationOne;//����1
	int m_CompensationTwo;//����2
	int m_AngularDifference;//�ǶȲ�
	float m_ActualValue;//��ʵֵ
	float m_Correction;//������
}StructMeasurePXInfo;
StructMeasurePXInfo g_sMeasurePXInfo[1000];
StructExcel g_sExcelPXInfo;
CMainFrame* pMainFrameThis;
CSetFormView *pSetView1;
int SpeedInterval;
int PoseInterval;
//����
BOOL m_UartOpenFlag;//���ڴ򿪹ر��л���־
BOOL m_bSendData;	// �Ƿ����ڷ�������
int g_bAloneFrameSize;
int nCase;
BYTE g_bAloneFrame[10240];
unsigned char g_bSendParameData[200];
int g_bSendParameDataSize;
//�˶�����
unsigned char a_ucTemp9[9] = { 0x01, 0x27, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }; //΢������
unsigned char a_ucTemp10[6] = { 0x01, 0x28, 0x01, 0x10, 0x00, 0x3A };   //�����ſ��˶�
unsigned char a_ucTemp11[6] = { 0x01, 0x28, 0x01, 0x20, 0x00, 0x4A };   //�����պ��˶�
unsigned char a_ucTemp12[6] = { 0x01, 0x29, 0x01, 0x20, 0x00, 0x4B };   //��ֹͣ�˶�
unsigned char a_ucTemp13[9] = { 0x01, 0x23, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };   //���ó��ٶ�
unsigned char a_ucTemp14[9] = { 0x01, 0x24, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };   //���������ٶ�
unsigned char a_ucTemp15[5] = { 0x01, 0x30, 0x00, 0x00, 0x31 };   //��ѯ����״̬
int t_LMAng = 0;//��һ��Ҫ�˶����ĽǶ�
int temp_ang1 = 0, targetAng2 = 0;//�����ж��Ƿ��ȶ�;��һ��Ҫ�˶��ĽǶ�ֵֵ
bool f_CircleDetOK = true, f_DetFalseLM = true;//һȦ�б�̽���Ƿ�ɹ���ɣ��Ƿ����һ�������µļٰ�
bool f_MoveZeroCompensation = false, f_SlopCompensation = false, f_AngleCompensation = true;//�����־λ������΢��������־λ���Ƿ����AngleCompensation�̱߳�־λ
bool f_booby = false, f_lastslop = false;//ƫ�����������ڶ����Ƿ���ɣ��Լ����һ���Ƿ���ɱ�־λ
bool f_HandOrAutoMoveZero = false;//�ֶ������Զ����㣬�ֶ�ֻ���㣬������������ false�����Զ� true�����ֶ�
bool f_HandOrAutoNavVeri = false; //�ֶ������Զ�������֤ false�����Զ� true�����ֶ�
int AfterNAVCount = 0;//�������ݲ��ϸ��Ƿ���Ҫ�����ж������������ڵ��������ļ��Ƿ���Ҫ����ϴ����ݵ�
int FuncEncoderCorrectionCount = 0, temp_ang2 = 0;//�����ж���һ��ƫ���������ݺ���һ���Ƿ�ƥ��,����1��a1 b1  ��2:b1 c1  �ж�c1-b1�ķ�Χ�Ƿ����
bool f_FirDataLMSet = false;////�б����õĵ�һ��������û�з��ͳɹ���־λ
bool f_SlopMoveNetOn = true;//��������û�ر�
bool f_MoveZeroNetOn = true;
bool f_CorrectionNetState = true;
int CalGap = 451, Realgap = 451;
//AGVȫģ���Զ�����
bool SimulationRun = false;
bool LaserSetSpeedRuning = false;
int g_GoalX = 0;
int g_GoalY = 0;
CEvent Event_POSE;//��ȡ������λ��
CEvent Event_NOPOSE;//��ȡ��������λ��
CEvent Event_STOP; //ֹͣ
HANDLE Event_List[3] = { Event_POSE, Event_NOPOSE, Event_STOP };
HANDLE g_EventPose = CreateEvent(NULL, FALSE, FALSE, NULL);  //��ȡλ���¼�

//д��־
CString m_SaveFolderPath = "";
CString FilePath = "";
CCDMLogInfo* m_pMyLog = NULL;
CCDMLogInfo* m_pMyOperLog = NULL;

int StopState = 0;   //����״̬��1:�ɹ������� 2:��ʱ������3:����ֹͣ

//��Ŀ��λ���ƶ�
int g_AllofGoalX[20];  //Ŀ����X����
int g_AllofGoalY[20];  //Ŀ����Y����
int g_GoalsTotal;   //Ŀ������
bool g_bSingleorMulti = false;   //falseΪ��ȡ�����ļ��Ķ�Ŀ�꣬trueΪ�ֶ����õ�һ��Ŀ��

// AGVTestCfg �Ի���

IMPLEMENT_DYNAMIC(AGVTestCfg, CDialogEx)

AGVTestCfg::AGVTestCfg(CWnd* pParent /*=NULL*/)
: CDialogEx(AGVTestCfg::IDD, pParent)
, SpeedX(0)
, SpeedY(0)
, SpeedAngle(0)
, SpeedSetInterval(40)
, PoseGetInterval(150)
, PoseX(0)
, PoseY(0)
, PoseAngle(0)
, PoseTimestamp(_T(""))
, PoseGetCount(0)
, InvalidCount(0)
, m_UartPort(0)
, m_LineSpeed(_T(""))
, m_AngleSpeed(_T(""))
, sGoalX(_T(""))
, sGoalY(_T(""))
, m_slopSteps(0)
, m_ang1(0)
, m_ang2(0)
, m_InitialSpeed(0)
, m_WorkingSpeed(0)
, m_LastAng2(0)
, m_FineAdjustCount(0)
, m_QualiStandard(0)
, m_CurrentState(_T(""))
{

}

AGVTestCfg::~AGVTestCfg()
{
}

void AGVTestCfg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SPEEDX, SpeedX);
	DDX_Text(pDX, IDC_EDIT_SPEEDY, SpeedY);
	DDX_Text(pDX, IDC_EDIT_SPEEDANGLE, SpeedAngle);
	DDX_Text(pDX, IDC_EDIT_SPEEDSETINTERVAL, SpeedSetInterval);
	DDX_Text(pDX, IDC_EDIT_POSEGETINTERVAL, PoseGetInterval);
	DDX_Text(pDX, IDC_EDIT_POSEX, PoseX);
	DDX_Text(pDX, IDC_EDIT_POSEY, PoseY);
	DDX_Text(pDX, IDC_EDIT_POSEANGLE, PoseAngle);
	DDX_Text(pDX, IDC_EDIT_POSETIMESTAMP, PoseTimestamp);
	DDX_Text(pDX, IDC_EDIT_GETCOUNT, PoseGetCount);
	DDX_Text(pDX, IDC_EDIT_INVALIDCOUNT, InvalidCount);
	DDX_CBIndex(pDX, IDC_COMBO_PORTNO, m_UartPort);
	DDX_Text(pDX, IDC_EDIT_LINESPEED, m_LineSpeed);
	DDX_Text(pDX, IDC_EDIT_ANGLESPEED, m_AngleSpeed);
	DDX_Text(pDX, IDC_EDIT_GOALLOCATIONX, sGoalX);
	DDX_Text(pDX, IDC_EDIT_GOALLOCATIONY, sGoalY);
	DDX_Control(pDX, IDC_COMBO_GETGOALSINFOMODE, m_CombGetGoalsInfoMode);
	DDX_Control(pDX, IDC_LIST1, m_listctrl);
	DDX_Control(pDX, IDC_log, m_richlog);
	DDX_Control(pDX, IDC_COMBO_SlopDir, m_slopDir);
	DDX_Text(pDX, IDC_EDIT_SlopSteps, m_slopSteps);
	DDX_Text(pDX, IDC_EDIT_Ang1, m_ang1);
	DDX_Text(pDX, IDC_EDIT_Ang2, m_ang2);
	DDX_Text(pDX, IDC_EDIT_InitialSpeed, m_InitialSpeed);
	DDX_Text(pDX, IDC_EDIT_WorkingSpeed, m_WorkingSpeed);
	//  DDX_Text(pDX, IDC_EDIT_Ang3, m_lastang2);
	DDX_Text(pDX, IDC_EDIT_LastAng2, m_LastAng2);
	DDX_Text(pDX, IDC_EDIT_FineAdjustCount, m_FineAdjustCount);
	DDX_Control(pDX, IDC_BTN_HandNavVeri, m_HandNavTest);
	DDX_Control(pDX, IDOK, m_AutoTest);
	DDX_Control(pDX, IDC_TCHART1, m_tchartinterp_px);
	DDX_Text(pDX, IDC_EDIT_Qualified, m_QualiStandard);
	DDX_Control(pDX, IDC_StepImg, m_PicControl);
	DDX_Control(pDX, IDC_BUTTON_STOPTEST, m_StopTest);
	DDX_Text(pDX, IDC_EDIT_CurrentState, m_CurrentState);
	DDX_Control(pDX, IDC_BTN_HandDownPXTable, m_HanfDownPX);
	DDX_Control(pDX, IDC_BTN_GetLMAng, m_GetLMAng);
	DDX_Control(pDX, IDC_BTN_SlopMoveZero, m_SlopMove);
	DDX_Control(pDX, IDC_BUTTON_LOADDATA, m_ReadNAVData);
	DDX_Control(pDX, IDC_BTN_OkQuliStandard, m_SureSQ);
}


BEGIN_MESSAGE_MAP(AGVTestCfg, CDialogEx)
	ON_BN_CLICKED(IDOK, &AGVTestCfg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &AGVTestCfg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_BUTTON_STOPTEST, &AGVTestCfg::OnBnClickedButtonStoptest)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_LOADDATA, &AGVTestCfg::OnBnClickedButtonLoaddata)
	ON_BN_CLICKED(IDC_BUTTON_OPENUART, &AGVTestCfg::OnBnClickedButtonOpenuart)
	ON_BN_CLICKED(IDC_BUTTON_CLOSEUART, &AGVTestCfg::OnBnClickedButtonCloseuart)
	ON_BN_CLICKED(IDC_BUTTON_GOFORWARD, &AGVTestCfg::OnBnClickedButtonGoforward)
	ON_BN_CLICKED(IDC_BUTTON_STOPAGV, &AGVTestCfg::OnBnClickedButtonStopagv)
	ON_EN_CHANGE(IDC_EDIT_LINESPEED, &AGVTestCfg::OnChangeEditLinespeed)
	ON_EN_CHANGE(IDC_EDIT_ANGLESPEED, &AGVTestCfg::OnChangeEditAnglespeed)
	ON_BN_CLICKED(IDC_BUTTON_AUTOTEST, &AGVTestCfg::OnBnClickedButtonAutotest)
	ON_BN_CLICKED(IDC_BUTTON_STOPAUTOTEST, &AGVTestCfg::OnBnClickedButtonStopAutoTest)
	ON_MESSAGE(WM_WRITELOG, &AGVTestCfg::OnWriteLog)
	ON_CBN_SELCHANGE(IDC_COMBO_GETGOALSINFOMODE, &AGVTestCfg::OnSelchangeComboGetgoalsinfomode)
	ON_MESSAGE(WM_LMInfoView, &AGVTestCfg::OnLminfoview)
	ON_MESSAGE(WM_LMSettingInfo, &AGVTestCfg::OnLmsettinginfo)
	ON_BN_CLICKED(IDC_BTN_ClosewiseMove, &AGVTestCfg::OnBnClickedBtnClosewisemove)
	ON_BN_CLICKED(IDC_BTN_AntiClockwiseMove, &AGVTestCfg::OnBnClickedBtnAnticlockwisemove)
	ON_BN_CLICKED(IDC_BTN_StopMove, &AGVTestCfg::OnBnClickedBtnStopmove)
	ON_BN_CLICKED(IDC_BTN_SetIniVel, &AGVTestCfg::OnBnClickedBtnSetinivel)
	ON_BN_CLICKED(IDC_BTN_SetMoveVel, &AGVTestCfg::OnBnClickedBtnSetmovevel)
	ON_BN_CLICKED(IDC_BTN_QueryCurrState, &AGVTestCfg::OnBnClickedBtnQuerycurrstate)
	ON_MESSAGE(WM_LMAngleDeal, &AGVTestCfg::OnLmangledeal)
	ON_BN_CLICKED(IDC_BTN_SlopMoveZero, &AGVTestCfg::OnBnClickedBtnEncoderCorrect)
	ON_BN_CLICKED(IDC_BTN_Slop, &AGVTestCfg::OnBnClickedBtnSlop)
	ON_BN_CLICKED(BTN_Interpola, &AGVTestCfg::OnBnClickedInterpola)
	ON_BN_CLICKED(IDC_BTN_IniSlop, &AGVTestCfg::OnBnClickedBtnInislop)
	ON_BN_CLICKED(IDC_BTN_HandDownPXTable, &AGVTestCfg::OnBnClickedBtnHanddownpxtable)
	ON_BN_CLICKED(IDC_BTN_HandNavVeri, &AGVTestCfg::OnBnClickedBtnHandnavveri)
	ON_BN_CLICKED(IDC_BTN_OkQuliStandard, &AGVTestCfg::OnBnClickedBtnOkqulistandard)
	ON_MESSAGE(WM_COM_PARSE, &AGVTestCfg::OnComParse)
	ON_MESSAGE(WM_MOTOSTATE, &AGVTestCfg::OnMotostate)
	ON_BN_CLICKED(IDC_BTN_GetLMAng, &AGVTestCfg::OnBnClickedBtnGetlmang)
	ON_MESSAGE(WM_GetLMAngle, &AGVTestCfg::OnGetlmangle)
	ON_BN_CLICKED(IDC_BTN_CircleLMDet, &AGVTestCfg::OnBnClickedBtnCirclelmdet)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &AGVTestCfg::OnLvnItemchangedList1)
END_MESSAGE_MAP()

BOOL AGVTestCfg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	DispTips(IDB_PNG0, "����״̬");
	m_slopDir.SetCurSel(1);
	m_slopSteps = 2;
	m_InitialSpeed = 10;
	m_WorkingSpeed = 500;
	UpdateData(FALSE);
	memset(&g_SysDevParam, 0, sizeof(SysDevParam));
	m_listctrl.InsertColumn(0, _T("ID"), LVCFMT_CENTER, 100, -1);
	m_listctrl.InsertColumn(1, _T("X����(mm)"), LVCFMT_CENTER, 100, -1);
	m_listctrl.InsertColumn(2, _T("Y����(mm)"), LVCFMT_CENTER, 100, -1);
	m_listctrl.InsertColumn(3, _T("�ߴ�"), LVCFMT_CENTER, 100, -1);
	m_listctrl.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_SHOWSELALWAYS | LVS_EDITLABELS);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	pMainFrameThis = (CMainFrame*)AfxGetMainWnd();
	pSetView1 = (CSetFormView*)pMainFrameThis->m_wndSplitter.GetPane(1, 0);
	GetDlgItem(IDC_BUTTON_OPENUART)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CLOSEUART)->EnableWindow(FALSE);

	m_CombGetGoalsInfoMode.SetCurSel(0);
	GetDlgItem(IDC_EDIT_GOALLOCATIONX)->EnableWindow(FALSE);
	GetDlgItem(IDC_EDIT_GOALLOCATIONY)->EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}

// AGVTestCfg ��Ϣ�������

DWORD WINAPI SetSpeedThreadProc(LPVOID lpParam)
{
	Sleep(100);
	while (LaserSetSpeedRuning == true)
	{
		if (pMainFrameThis->m_NetConnection.bConnected)
		{
			pMainFrameThis->PostMessage(WM_SENDCMD, SpeedSet_zhilingSize, (LPARAM)SpeedSet_zhiling);
		}
		else
		{
			LaserSetSpeedRuning = false;
		}
		Sleep(180);//(SpeedInterval);
	}
	return 0;
}
void getMaxAndMin(int *arr, int count, int *val)
{
	val[0] = arr[0];
	val[1] = arr[0];
	for (int i = 1; i < count; i++)
	{
		if (val[0]<arr[i])
		{
			val[0] = arr[i];
		}
		if (val[1]>arr[i])
		{
			val[1] = arr[i];
		}
	}
}
void getdouMaxAndMin(double *arr, int count, double *val)
{
	val[0] = arr[0];
	val[1] = arr[0];
	for (int i = 1; i < count; i++)
	{
		if (val[0]<arr[i])
		{
			val[0] = arr[i];
		}
		if (val[1]>arr[i])
		{
			val[1] = arr[i];
		}
	}
}
void CommSendStop(LPVOID lpParam, int nStopType)
{
	CString strTemp = "";
	strTemp.Format("Debug:Send-%d", nStopType);
	//CLaseAutoCorrectDlg *pMainDlg=(CLaseAutoCorrectDlg *)(AfxGetApp()->GetMainWnd());
	////pDlg->WriteLog(&(pDlg->m_RichEdit),pDlg->m_sLog);
	//::SendMessageA(pMainDlg->GetSafeHwnd(),WM_RECVDATA,0,(LPARAM)(LPCTSTR)strTemp);
}
BOOL Com_Send(BYTE *out_buff, int len)
{
	BOOL nRes = FALSE;
	SerPortPar serPortPar;
	AGVTestCfg *pMainDlg = (AGVTestCfg *)(AfxGetApp()->GetMainWnd());
	serPortPar.nPort = g_SysDevParam.nPort;
	serPortPar.nBaud = g_SysDevParam.nBuad;
	serPortPar.pDataBuf = out_buff;
	serPortPar.nDataBufSize = len;
	serPortPar.lpSendFun = (LPVOID)CommSendStop;
	serPortPar.lpParam = (LPVOID)pMainDlg;
	nRes = SendComm(&serPortPar);
	return nRes;
}

UINT FuncGetLMang(LPVOID lpParam)
{
	AGVTestCfg *pAgv = (AGVTestCfg *)lpParam;
	pMainFrameThis->PostMessage(WM_SENDCMD, LandmarkModeSet_zhilingSize, (LPARAM)LandmarkModeSet_zhiling);//�л����б�̽��
	Sleep(1000);//�ȴ�ʱ�����Ҫ��ô��
	pMainFrameThis->PostMessage(WM_SENDCMD, PXGetLmIndo_zhilingSize, (LPARAM)PXGetLmIndo_zhiling);//ƫ������
	Sleep(1000);//�ȴ�ʱ�����Ҫ��ô��
	DWORD dw = WaitForSingleObject(g_PXAngevent, 5000);
	ResetEvent(g_PXAngevent);//�����٣��������set�˵Ļ��ͻ�һֱ���ָ�״̬
	switch (dw)
	{
	case WAIT_OBJECT_0:
		//�ȵ��¼�	
		break;
	case WAIT_TIMEOUT:		//��ʱδ�յ��ظ�
		if (AfterNAVCount == 0)//�ϸ��׼ȷ�ϡ�ƫ��������ʱ���ȡ�б�ǶȲ��ɹ�
		{
			pAgv->StopSlop();
			pAgv->WriteToRichEdit("��ȡ�б�Ƕ�ʧ�ܣ������µ����ϸ��׼ȷ�ϰ�ť��");
			AfxMessageBox("��ȡ�б�Ƕ�ʧ�ܣ������µ����ϸ��׼ȷ�ϰ�ť��");
		}
		else if (AfterNAVCount % 2 != 0)//�������ݲ��ϸ�Ҫ�������ݲɼ���ʱ�򣬻�ȡ�ǶȲ��ɹ�������ʾ����ƫ��������ť
		{
			pAgv->StopSlop();
			pAgv->WriteToRichEdit("��ȡ�б�Ƕ�ʧ�ܣ��뵥��ƫ��������ť��");
			AfxMessageBox("��ȡ�б�Ƕ�ʧ�ܣ��뵥��ƫ��������ť��");
		}
		break;
	case WAIT_FAILED:
		if (AfterNAVCount == 0)//�ϸ��׼ȷ�ϡ�ƫ��������ʱ���ȡ�б�ǶȲ��ɹ�
		{
			pAgv->StopSlop();
			pAgv->WriteToRichEdit("��ȡ�б�Ƕ�ʧ�ܣ������µ����ϸ��׼ȷ�ϰ�ť");
			AfxMessageBox("��ȡ�б�Ƕ�ʧ�ܣ������µ����ϸ��׼ȷ�ϰ�ť");
		}
		else if (AfterNAVCount % 2 != 0)//�������ݲ��ϸ�Ҫ�������ݲɼ���ʱ�򣬻�ȡ�ǶȲ��ɹ�������ʾ����ƫ��������ť
		{
			pAgv->StopSlop();
			pAgv->WriteToRichEdit("��ȡ�б�Ƕ�ʧ�ܣ��뵥��ƫ��������ť��");
			AfxMessageBox("��ȡ�б�Ƕ�ʧ�ܣ��뵥��ƫ��������ť��");
		}
		break;
	}
	return 0;
}

UINT SetPidparamD1(LPVOID lparam)//DataContent:��Ϊ0  pxState��0�ر� 1����  poitPercet:��Ϊ6
{
	AGVTestCfg *pAgv = (AGVTestCfg *)lparam;
	CTime time = CTime::GetCurrentTime();
	char Hour = time.GetHour();
	char Miu = time.GetMinute();
	char Sec = time.GetSecond();
	unsigned char pn[14] = { 0x00, 0x24, 0x00, 0x00, 0x00, Hour, Miu, Sec, 0x01, 0x01, 0x00, 0x07, 0x05, 0x07 };
	ZhiLing(pid_zhiling, pn, 0);
	int tmp;
	tmp = 0;
	pid_zhiling[26] = (tmp >> 8);
	pid_zhiling[27] = (tmp & 0xff);
	tmp = 0;
	pid_zhiling[28] = (tmp >> 8);
	pid_zhiling[29] = (tmp & 0xff);
	if (!f_OpenOrClosePX)   //�ر�
	{
		tmp = 0;
	}
	else  //����
	{
		tmp = 100;
	}
	pid_zhiling[30] = (tmp >> 8);
	pid_zhiling[31] = (tmp & 0xff);
	pid_zhiling[32] = (pointRatio >> 8);
	pid_zhiling[33] = (pointRatio & 0xff);
	pid_zhiling[34] = ((-1) & 0xff);
	pid_zhiling[35] = ((-1) & 0xff);
	pMainFrameThis->PostMessage(WM_SENDCMD, pid_zhilingSize, (LPARAM)pid_zhiling);
	DWORD dw_PXDown = WaitForSingleObject(g_PXDownevnet, 4000); //���¼�  ������ INFINITE 
	ResetEvent(g_PXDownevnet);//�����٣��������set�˵Ļ��ͻ�һֱ���ָ�״̬
	switch (dw_PXDown)
	{
	case WAIT_OBJECT_0:		//�ȵ��¼�			
		break;
	case WAIT_TIMEOUT:		//��ʱδ�յ��ظ�
		if (!f_OpenOrClosePX)
		{
			pAgv->WriteToRichEdit("�ر�ƫ������ʧ��,�����½���ƫ������!");
			pAgv->StopSlop();
			AfxMessageBox("�ر�ƫ������ʧ��,�����½���ƫ��������");
		}
		else
		{
			if (AfterNAVCount % 2 != 0)//���ε�����λ������֤֮ǰ����ƫ������ʧ��
			{
				pAgv->WriteToRichEdit("����ƫ������ʧ��,�����½���һ��������");
				pAgv->StopSlop();
				AfxMessageBox("����ƫ������ʧ��,�����½���һ��������");
			}
			else//ƫ���������ݲɼ����֮����ƫ������ʧ��
			{
				pAgv->WriteToRichEdit("����ƫ������ʧ��,�뵥����λ������֤��ť��");
				pAgv->StopSlop();
				AfxMessageBox("����ƫ������ʧ��,�뵥����λ������֤��ť��");
			}
		}
		break;
	case WAIT_FAILED:
		if (!f_OpenOrClosePX)
		{
			pAgv->WriteToRichEdit("�ر�ƫ������ʧ��,�����½���ƫ������!");
			pAgv->StopSlop();
			AfxMessageBox("�ر�ƫ������ʧ��,�����½���ƫ������!");
		}
		else
		{
			if (AfterNAVCount % 2 != 0)
			{
				pAgv->WriteToRichEdit("����ƫ������ʧ��,�����½���һ��������");
				pAgv->StopSlop();
				AfxMessageBox("����ƫ������ʧ��,�����½���һ��������");
			}
			else
			{
				pAgv->WriteToRichEdit("����ƫ������ʧ��,�뵥����λ������֤��ť��");
				pAgv->StopSlop();
				AfxMessageBox("����ƫ����������ʧ��,�뵥����λ������֤��ť��");
			}
		}
		break;
	}
	return 0;
}

UINT GetPoseThreadProc(LPVOID lpParam)
{
	AGVTestCfg *pAgv = (AGVTestCfg *)lpParam;
	while (LaserGetPostRuning == true)
	{
		if (f_CorrectionNetState)
		{
			if (!f_HundredNav)//����û�е�100��
			{
				if (g_NAVflag)
					pMainFrameThis->PostMessage(WM_SENDCMD, GetPose_zhilingSize, (LPARAM)GetPose_zhiling);
			}
			else//������100��
			{
				Sleep(1000);
				CString str_val = "";
				int NavXVal[2] = { 0 }, NavYVal[2] = { 0 };
				getMaxAndMin(NavPoseX, 100, NavXVal);
				int XCenterVal = (int)((NavXVal[0] + NavXVal[1]) / 2);
				AnalyNavXPoseCenter[NavHundredCount] = XCenterVal;
				getMaxAndMin(NavPoseY, 100, NavYVal);
				int YCenterVal = (int)((NavYVal[0] + NavYVal[1]) / 2);
				str_val.Format("%d,%d", XCenterVal, YCenterVal);
				AnalyNavYPoseCenter[NavHundredCount] = YCenterVal;
				NavHundredCount++;
				CString str_icount;
				str_icount.Format("%d", NavHundredCount);
				CString strTemp = "��" + str_icount + "�ε������ݲɼ����,ͳ�Ƶõ���X,Y�ֱ�Ϊ" + str_val;
				pAgv->WriteToRichEdit(strTemp);
				memset(NavPoseX, 0, 100);
				memset(NavPoseY, 0, 100);
				if (NavHundredCount < 4)
				{
					a_ucTemp9[3] = ((-3500 >> 24) & 0xFF);
					a_ucTemp9[4] = ((-3500 >> 16) & 0xFF);
					a_ucTemp9[5] = ((-3500 >> 8) & 0xFF);
					a_ucTemp9[6] = (-3500 & 0xFF);
					UINT16 CaclCRC = CRC712_Add(a_ucTemp9, 7);
					a_ucTemp9[7] = (CaclCRC >> 8) & 0xFF;
					a_ucTemp9[8] = (CaclCRC)& 0xFF;
					if (Com_Send(a_ucTemp9, 9))
					{
						Sleep(9000);//ͣһ��ʱ���ת��90��
						Com_Send(a_ucTemp12, 6);//9s�����������ֹͣ���
						f_HundredNav = false;
					}
				}
				else
				{
					LaserGetPostRuning = false;
					int AnalyX[2] = { 0 }, AnalyY[2] = { 0 };
					getMaxAndMin(AnalyNavXPoseCenter, 4, AnalyX);
					getMaxAndMin(AnalyNavYPoseCenter, 4, AnalyY);
					CString strGap;
					int GapAnalyX = AnalyX[0] - AnalyX[1];
					int GapAnalyY = AnalyY[0] - AnalyY[1];
					if (GapAnalyX <= QualiStandard && GapAnalyY <= QualiStandard)
					{
						strGap.Format("%d,%d,%d", QualiStandard, GapAnalyX, GapAnalyY);
						pAgv->WriteToRichEdit("�ϸ�ı�׼���õ���X,Y���ݷֱ�Ϊ��" + strGap);
						pAgv->WriteToRichEdit("�������ݺϸ�");
						pAgv->DispTips(IDB_PNG_OK, "�������ݺϸ�");
						pMainFrameThis->SetTimer(0, 500, NULL);       //ѭ����״̬��ѯָ��
						AfterNAVCount = 0;
						g_NAVflag = FALSE;
					}
					else
					{
						strGap.Format("%d,%d,%d", QualiStandard, GapAnalyX, GapAnalyY);
						pAgv->WriteToRichEdit("�ϸ�ı�׼���õ���X,Y���ݷֱ�Ϊ��" + strGap);
						pAgv->WriteToRichEdit("�������ݲ��ϸ񣡣�����");
						pAgv->DispTips(IDB_PNG_Error, "�������ݲ��ϸ�");
						Sleep(4000);
						if (AfterNAVCount % 2 != 0 && f_CircleDetOK)
						{
							pAgv->WriteToRichEdit("���������......");
							pAgv->DispTips(IDB_PNG2, "�������");
							f_OpenOrClosePX = false;
							AfxBeginThread(SetPidparamD1, pAgv);//���ݲɼ���ʱ��ر�ƫ������
							Sleep(1000);
							AfxBeginThread(FuncGetLMang, pAgv);//����Ҫ��1s �ر�ƫ������֮���ȡ�Ƕ�
							Sleep(1000);
							pAgv->FuncMovezero();//�������ݲ��ϸ�Ϳ�ʼ����
						}
						else if (AfterNAVCount % 2 != 0 && !f_CircleDetOK)
						{
							AfterNAVCount = 0;
							if (!f_CircleDetOK)
							{
								f_CircleDetOK = true;
								pAgv->WriteToRichEdit("�б�̽�ⲻͨ����");
							}
						}
						else if (AfterNAVCount % 2 == 0)//�Զ���������£�ƫ�ı�����֮���⻹��ͨ���Ͳ��ټ�����
						{
							f_CircleDetOK = true;
							AfterNAVCount = 0;
						}
						g_NAVflag = FALSE;
					}
					Sleep(200);
					memset(AnalyNavXPoseCenter, 0, 100);
					memset(AnalyNavYPoseCenter, 0, 100);
					NavHundredCount = 0;
				}
			}
		}
		else
		{
			LaserGetPostRuning = false;
			if (AfterNAVCount == 1)
			{
				AfxMessageBox("�����ѶϿ����������������粢����һ������������");
				pAgv->WriteToRichEdit("�����ѶϿ����������������粢����һ������!!!");
			}
			else
			{
				AfxMessageBox("�����ѶϿ����������������粢���ж�λ������֤������");
				pAgv->WriteToRichEdit("�����ѶϿ����������������粢���ж�λ������֤������");
			}
			pAgv->StopSlop();//ֹͣ���в���
		}
		Sleep(150);//(PoseInterval);
	}
	return 0;
}

void AGVTestCfg::FuncSetIniVel()//���õ���ĳ�ʼ�����ٶ�500
{
	a_ucTemp14[3] = ((500 >> 24) & 0xFF);
	a_ucTemp14[4] = ((500 >> 16) & 0xFF);
	a_ucTemp14[5] = ((500 >> 8) & 0xFF);
	a_ucTemp14[6] = (500 & 0xFF);
	UINT CaclCRC = CRC712_Add(a_ucTemp14, 7);
	a_ucTemp14[7] = (CaclCRC >> 8) & 0xFF;
	a_ucTemp14[8] = (CaclCRC)& 0xFF;
	Com_Send(a_ucTemp14, 9);
}

UINT SetLaserMap(LPVOID lparam)
{
	AGVTestCfg *pAgv = (AGVTestCfg *)lparam;
	pMainFrameThis->PostMessage(WM_SENDCMD, MappingModeSet_zhilingSize, (LPARAM)MappingModeSet_zhiling);//�б��ȡģʽ����
	DWORD dw_PXMap = WaitForSingleObject(g_PXMap, 5000); //���¼�  ������ INFINITE 
	ResetEvent(g_PXMap);//�����٣��������set�˵Ļ��ͻ�һֱ���ָ�״̬
	switch (dw_PXMap)
	{
	case WAIT_OBJECT_0:		//�ȵ��¼�			
		break;
	case WAIT_TIMEOUT:		//��ʱδ�յ��ظ�
		if (AfterNAVCount % 2 == 1)
		{
			pAgv->WriteToRichEdit("�б��ȡʧ�ܣ�������һ������!");
			pAgv->StopSlop();
			AfxMessageBox("�б��ȡʧ�ܣ�������һ������!");
		}
		else
		{
			pAgv->WriteToRichEdit("�б��ȡʧ�ܣ��뵥����λ������֤��ť!");
			pAgv->StopSlop();
			AfxMessageBox("�б��ȡʧ�ܣ��뵥����λ������֤��ť!");
		}
		break;
	case WAIT_FAILED:
		if (AfterNAVCount % 2 == 1)
		{
			pAgv->WriteToRichEdit("�б��ȡʧ�ܣ�������һ������!");
			pAgv->StopSlop();
			AfxMessageBox("�б��ȡʧ�ܣ�������һ������!");
		}
		else
		{
			pAgv->WriteToRichEdit("�б��ȡʧ�ܣ��뵥����λ������֤��ť!");
			pAgv->StopSlop();
			AfxMessageBox("�б��ȡʧ�ܣ��뵥����λ������֤��ť!");
		}
		break;
	}
	return 0;
}
//1.�Զ�������ʱ��ƫ�ı��������֮����ƫ���������ֶ���λ��֤��ʱ���ƫ�����������κδ���  2.�ֶ�ƫ������ƫ�ı����سɹ�����֤ҲҪ����ƫ������
void AGVTestCfg::SendOpenPX()//��ƫ������ 
{
	f_OpenOrClosePX = true;
	AfxBeginThread(SetPidparamD1, this);//����ƫ������
	Sleep(1000);
}

void AGVTestCfg::OnBnClickedOk()
{
	CWinThread *pThread;
	pMainFrameThis = (CMainFrame*)AfxGetMainWnd();
	if (!f_CorrectionNetState || !pMainFrameThis->m_NetConnection.bConnected || (!m_UartOpenFlag))
	{
		AfxMessageBox("����δ�򿪻�����û����!");
		return;
	}
	if (f_QualiStandard)
	{
		if (f_DetFalseLM || ScanLmNum == 5)
		{
			if (ScanLmNum >= 5)
			{
				f_CircleDetOK = true;//��ʱ���ã��ƻ������ж�̽���Ƿ�ͨ���������������ܷ���еı�־λ
				f_HandOrAutoMoveZero = false;//�Զ�
				f_HundredNav = false;//100�ε�����־λfalse
				NavHundredCount = 0;
				m_listctrl.DeleteAllItems();//�����һ�λ�ȡ�İб���Ϣ
				if (f_HandOrAutoNavVeri)
				{
					OnCreateFolder(LaserMAC);
				}
				else
				{
					AfterNAVCount++;
				}
				pMainFrameThis->KillTimer(0);
				UpdateData(true);
				m_LastAng2 = 0;
				PoseX = 0;
				PoseY = 0;
				PoseAngle = 0;
				PoseTimestamp = "";
				PoseGetCount = 0;
				InvalidCount = 0;
				m_LastAng2 = 0;
				UpdateData(false);
				m_SureSQ.EnableWindow(FALSE);
				GetDlgItem(IDOK)->EnableWindow(FALSE);
				GetDlgItem(IDC_BUTTON_LOADDATA)->EnableWindow(FALSE);
				m_HandNavTest.EnableWindow(FALSE);
				m_GetLMAng.EnableWindow(FALSE);
				m_HanfDownPX.EnableWindow(FALSE);
				m_SlopMove.EnableWindow(FALSE);
				GraphForm2d * pView;
				pView = (GraphForm2d*)pMainFrameThis->m_wndLeftSplitter.GetPane(0, 0);
				((CSeries)(pView->m_AGVTestDlg.m_ChartrsTrack.Series(0))).Clear();
				((CSeries)m_tchartinterp_px.Series(0)).Clear();
				((CSeries)m_tchartinterp_px.Series(1)).Clear();
				CSetFormView *pSetView = (CSetFormView*)pMainFrameThis->m_wndSplitter.GetPane(1, 0);
				pSetView->m_SetProdDlg.m_btnPIDDn1.EnableWindow(FALSE);//����ƫ���������ذ�ťʹ��
				pSetView->m_SetAppDlg.m_DownApplicPara.EnableWindow(FALSE);//����Ӧ�ò������ذ�ťʹ��
				if (AfterNAVCount % 2 == 1)//����������ٶ�����Ϊ500�������ļ���
				{
					FuncSetIniVel();//��������ٶ�����Ϊ500
					DispTips(IDB_PNG1, "���ζ�λ������֤");
					OnCreateFolder(LaserMAC);
				}
				else if (AfterNAVCount == 2)
				{
					if (!f_HandOrAutoNavVeri)
					{
						DispTips(IDB_PNG6, "ƫ��������λ������֤");
					}
					else
					{
						DispTips(IDB_PNG_HanDNAV, "��λ������֤");
					}
				}
				WriteToRichEdit("�б��ȡ��.......");
				AfxBeginThread(SetLaserMap, this);
				Sleep(500);
				LaserGetPostRuning = true;
				pThread = AfxBeginThread(GetPoseThreadProc, this);
				if (pThread == NULL)
				{
					StopSlop();
					return;
				}
			}
			else if (ScanLmNum < 5)
			{
				f_QualiStandard = false;
				CString str_ScanLmNum = "";
				str_ScanLmNum.Format("%d", ScanLmNum);
				CString str = "��������ǰʶ��" + str_ScanLmNum + "���б꣬ȷ�ϼ������ﵽ��������״̬���پ����Ƿ���Ҫ�޸İб�ʶ����ֵ��";
				WriteToRichEdit(str);
				AfxMessageBox(str);
			}
		}
		else
		{
			if (ScanLmNum > 5)
			{
				CString str_ScanLmNum = "";
				str_ScanLmNum.Format("%d", ScanLmNum - 5);
				AfxMessageBox("����" + str_ScanLmNum + "���ٰУ���ȷ�ϰб�����������޸İб�ʶ����ֵ");
			}
			else
			{
				CString str_ScanLmNum = "";
				str_ScanLmNum.Format("%d", ScanLmNum);
				AfxMessageBox("��������ǰʶ��" + str_ScanLmNum + "���б꣬��ȷ�ϼ������Ƿ�ﵽ��������״̬��");
			}
			f_QualiStandard = false;
			f_DetFalseLM = true;
		}
	}
	else
	{
		AfxMessageBox("��ȷ�Ϻϸ��׼");
	}
}

void AGVTestCfg::OnBnClickedCancel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnCancel();
}

DWORD WINAPI StopThreadProc(LPVOID lpParam)
{
	LaserGetPostRuning = false;
	Sleep(500);
	if (m_pMyLog)
		m_pMyLog->Release();
	if (m_pMyOperLog)
		m_pMyOperLog->Release();
	pMainFrameThis->SetTimer(0, 500, NULL);       //ѭ����״̬��ѯָ��
	return 0;
}

void AGVTestCfg::OnBnClickedButtonStoptest()
{
	m_richlog.SetSel(0, -1);
	m_richlog.ReplaceSel("");
	f_StopTest = true;
	if (f_NavFileOpen)
	{
		FileNav.Close();
		f_NavFileOpen = false;
	}
	DispTips(IDB_PNG0, "����״̬");
	OnBnClickedBtnStopmove();//ֹͣ����˶�
	AfterNAVCount = 0;
	LaserGetPostRuning = false;
	Sleep(500);
	pMainFrameThis->SetTimer(0, 500, NULL);
	m_AutoTest.EnableWindow(TRUE);
	m_HandNavTest.EnableWindow(TRUE);
	m_GetLMAng.EnableWindow(TRUE);
	m_HanfDownPX.EnableWindow(TRUE);
	m_SlopMove.EnableWindow(TRUE);
	m_SureSQ.EnableWindow(TRUE);
	f_HandOrAutoNavVeri = false;//��Ϊ�Զ�����
	f_QualiStandard = false;
	GetDlgItem(IDC_BUTTON_LOADDATA)->EnableWindow(TRUE);
	f_Realgap = true;
	f_AGVTest = false;
	Realgap = 450;
	m_SaveFolderPath = "";
	CSetFormView *pSetView;
	pSetView = (CSetFormView*)pMainFrameThis->m_wndSplitter.GetPane(1, 0);
	pSetView->m_SetProdDlg.m_btnPIDDn1.EnableWindow(TRUE);
	pSetView->m_SetAppDlg.m_DownApplicPara.EnableWindow(TRUE);
	pSetView->m_SetProdDlg.m_btnPIDRead.EnableWindow(TRUE);// ����ƫ��������ȡ��ťʹ��
	pSetView->m_SetAppDlg.m_ReadApplicPara.EnableWindow(TRUE);//����Ӧ�ò�����ȡ��ťʹ��
	f_FirCount = true;
	GraphForm2d * pView;
	pView = (GraphForm2d*)pMainFrameThis->m_wndLeftSplitter.GetPane(0, 0);
	((CSeries)(pView->m_AGVTestDlg.m_ChartrsTrack.Series(0))).Clear();
	((CSeries)m_tchartinterp_px.Series(0)).Clear();
	((CSeries)m_tchartinterp_px.Series(1)).Clear();
	m_listctrl.DeleteAllItems();//�����һ�λ�ȡ�İб���Ϣ
}

void AGVTestCfg::OnTimer(UINT nIDEvent)
{
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	switch (nIDEvent)
	{
	case 0:
		if (pMainFrame->m_NetConnection.bConnected)
		{
			pMainFrame->PostMessage(WM_SENDCMD, SpeedSet_zhilingSize, (LPARAM)SpeedSet_zhiling);
		}
		else
		{
			KillTimer(0);
		}
		break;
	case 1:
		if (pMainFrame->m_NetConnection.bConnected)
		{
			pMainFrame->PostMessage(WM_SENDCMD, GetPose_zhilingSize, (LPARAM)GetPose_zhiling);
		}
		else
		{
			KillTimer(1);
		}
		break;
	}
}

void AGVTestCfg::OnBnClickedButtonLoaddata()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
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
	int DataLocation = 0; //ȡ��λ��

	pView = (GraphForm2d*)pMainFrameThis->m_wndLeftSplitter.GetPane(0, 0);
	LoadFilePath = CFileUtils::OpenFileDlg(TRUE, _T("H Files (*.txt_NAV)|*.txt_NAV||"), NULL, NULL, NULL);
	CString l_sNameTmp;
	l_sNameTmp = LoadFilePath.Right(LoadFilePath.GetLength() - LoadFilePath.ReverseFind('.') - 1);
	if (l_sNameTmp != "txt_NAV")
	{
		MessageBox("��ѡ���׺Ϊtxt_NAV���ļ���");
		return;
	}

	PoseGetCount = 0;
	InvalidCount = 0;
	((CSeries)(pView->m_AGVTestDlg.m_ChartrsTrack.Series(0))).Clear();
	if (hufile.Open(LoadFilePath, CFile::modeRead))
	{
		while (hufile.ReadString(str))
		{
			IndexStart = str.Find("������Ϊ��");
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
						PoseGetCount++;
						((CSeries)(pView->m_AGVTestDlg.m_ChartrsTrack.Series(0))).AddXY(DataX, DataY, strTip, 0);
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
				IndexStart = str.Find("��Ч����");
				if (IndexStart != -1)
				{
					InvalidCount++;
				}
			}
		}
		hufile.Close();
		PoseX = 0;
		PoseY = 0;
		PoseAngle = 0;
		PoseTimestamp = "";
		UpdateData(false);
		MessageBox("��ȡ������ɣ���Ч����Ч��������ʾ��");
	}
}

#pragma region ���ڹ���
void AGVTestCfg::OnBnClickedButtonOpenuart()
{
	SerPortPar serPortPar;
	if (!m_UartOpenFlag)
	{
		UpdateData(TRUE);
		int port = m_UartPort + 1;
		g_SysDevParam.nPort = port;
		g_SysDevParam.nBuad = 9600;
		serPortPar.nPort = port;
		serPortPar.nBaud = 9600;
		serPortPar.lpRecvFun = (LPVOID)OnSendCommRecv;
		serPortPar.lpParam = (LPVOID)this;
		if (OpenComm(&serPortPar))
		{
			m_UartOpenFlag = TRUE;
			g_SysDevParam.ComConnected = TRUE;
			GetDlgItem(IDC_COMBO_PORTNO)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_OPENUART)->EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_CLOSEUART)->EnableWindow(TRUE);
		}
		else
		{
			MessageBox("û�з��ִ˴��ڻ�ռ�ã�");
			m_UartOpenFlag = FALSE;
		}
	}
	else
	{

	}
}


void AGVTestCfg::OnBnClickedButtonCloseuart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������	
	if (m_UartOpenFlag)
	{
		if (CloseComm(m_UartPort + 1))
		{
			m_UartOpenFlag = FALSE;
			m_bSendData = FALSE;
			g_SysDevParam.ComConnected = FALSE;
			GetDlgItem(IDC_COMBO_PORTNO)->EnableWindow(TRUE);//��ֹ���Ĵ��ڶ˿ڿؼ�
			GetDlgItem(IDC_BUTTON_OPENUART)->EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_CLOSEUART)->EnableWindow(FALSE);

		}
		else
		{
			MessageBox("���ڹر�ʧ�ܣ�");
			m_UartOpenFlag = TRUE;
		}
	}
	else
	{
		m_UartOpenFlag = FALSE;
	}
}

BYTE g_nComParseBuff[200] = { 0 };
void OnSendCommRecv(LPVOID lpParam, BYTE *pDataBuf, int nDataBufSize)
{
	//���ڽ���
	int i = 0;
	CString strTemp = "";
	AGVTestCfg *pDlg = (AGVTestCfg *)lpParam;
	if (nDataBufSize == 2)
	{
		memcpy(g_nComParseBuff, pDataBuf, 2);
		PostMessage(pDlg->GetSafeHwnd(), WM_COM_PARSE, g_nComParseBuff[1], 0);
	}
}

void OnSendCommSendStop(LPVOID lpParam, int nStopType)
{

}

DWORD WINAPI AGVControlProc(LPVOID lpParam)
{
	AGVTestCfg *pMainDLg = (AGVTestCfg *)lpParam;
	SerPortPar serPortPar;

	serPortPar.nPort = pMainDLg->m_UartPort + 1;
	serPortPar.pDataBuf = g_bSendParameData;
	serPortPar.nDataBufSize = g_bSendParameDataSize;

	while (AGVControlRunning == true)
	{
		if (m_UartOpenFlag)
		{
			serPortPar.lpSendFun = (LPVOID)OnSendCommSendStop;
			serPortPar.lpParam = (LPVOID)pMainDLg;
			if (!SendComm(&serPortPar))
			{
				pMainDLg->MessageBox("���ݷ��ͣ����ݷ���ʧ��!");
				AGVControlRunning = false;
			}
			else
			{
				m_bSendData = TRUE;
			}
		}
		else
		{
			//pMainDLg->MessageBox("����δ��!");
			AGVControlRunning = false;
		}
		Sleep(150);
	}
	pMainDLg->GetDlgItem(IDC_BUTTON_GOFORWARD)->EnableWindow(TRUE);
	pMainDLg->GetDlgItem(IDC_BUTTON_STOPAGV)->EnableWindow(FALSE);

	return 0;
}

float CalculateTicksFromLineSpeed(float LineSpeed)
{
	float ticksLineSpeed = LineSpeed * 1200 / (0.12*PI) / 30.00;  //������
	return ticksLineSpeed;
}

float CalculateTicksFromAngleSpeed(float AngleSpeed)
{
	float AngleSpeed2LineSpeed = AngleSpeed*0.35 / 2.00;
	float ticksAngleSpeed = AngleSpeed2LineSpeed * 1200 / (0.12*PI) / 30.00;
	return ticksAngleSpeed;
}

void AGVTestCfg::OnBnClickedButtonGoforward()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	float fLineSpeed = atof(m_LineSpeed) / 1000;
	float fAngleSpeed = atof(m_AngleSpeed) / 1000 * (PI / 180);
	float ticksLineSpeed = CalculateTicksFromLineSpeed(fLineSpeed);
	float ticksAngleSpeed = CalculateTicksFromAngleSpeed(fAngleSpeed);
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
		RightTicks = (int)(fRightTicks * 10 + 5) / 10;  //��������  
	}
	else
	{
		RightTicks = (int)(fRightTicks * 10 - 5) / 10;  //��������
	}

	CString temp = "";
	CString DataTemp = "z ";
	temp.Format("%d", LeftTicks);
	DataTemp += temp + " ";
	temp.Format("%d", RightTicks);
	DataTemp += temp + "\r";

	g_bSendParameDataSize = DataTemp.GetLength();
	memcpy(g_bSendParameData, DataTemp, g_bSendParameDataSize);
	DataTemp.ReleaseBuffer();

	DWORD ThreadID;
	HANDLE hThreadAGVControl;
	AGVControlRunning = true;
	hThreadAGVControl = CreateThread(NULL, 0, AGVControlProc, this, NULL, &ThreadID);

	GetDlgItem(IDC_BUTTON_GOFORWARD)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOPAGV)->EnableWindow(TRUE);
}

void AGVTestCfg::OnBnClickedButtonStopagv()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	AGVControlRunning = false;
	Sleep(150);
	CString DataTemp = "z 0 0\r";
	g_bSendParameDataSize = DataTemp.GetLength();
	memcpy(g_bSendParameData, DataTemp, g_bSendParameDataSize);
	DataTemp.ReleaseBuffer();

	SerPortPar serPortPar;
	if (m_UartOpenFlag)
	{
		serPortPar.nPort = m_UartPort + 1;

		serPortPar.pDataBuf = g_bSendParameData;
		serPortPar.nDataBufSize = g_bSendParameDataSize;
		serPortPar.lpSendFun = (LPVOID)OnSendCommSendStop;
		serPortPar.lpParam = (LPVOID)this;
		if (!SendComm(&serPortPar))
		{
			MessageBox("���ݷ��ͣ����ݷ���ʧ��!");
		}
		else
		{
			m_bSendData = TRUE;
		}
	}
	else
	{
		//MessageBox("����δ��!");			
	}
}


#pragma endregion

void AGVTestCfg::OnChangeEditLinespeed()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (SimulationRun == false)
	{
		UpdateData(TRUE);
		int LineSpeedTemp = atoi(m_LineSpeed);
		if (LineSpeedTemp<-500)
		{
			m_LineSpeed = "-500";
		}
		else if (LineSpeedTemp >500)
		{
			m_LineSpeed = "500";
		}
		SpeedX = LineSpeedTemp;
		UpdateData(FALSE);
	}
}

void AGVTestCfg::OnChangeEditAnglespeed()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (SimulationRun == false)
	{
		UpdateData(TRUE);
		int AngleSpeedTemp = atoi(m_AngleSpeed);
		if (AngleSpeedTemp<-10000)
		{
			m_AngleSpeed = "-10000";
		}
		else if (AngleSpeedTemp >10000)
		{
			m_AngleSpeed = "10000";
		}
		SpeedAngle = -AngleSpeedTemp;
		UpdateData(FALSE);
	}

}

DWORD WINAPI AutoSimulation(LPVOID lpParam);
void AGVTestCfg::OnBnClickedButtonAutotest()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (g_bSingleorMulti == false)
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
		g_GoalsTotal = atoi(strGoaltotal);
		if (g_GoalsTotal == 0 || (g_GoalsTotal > 20))
		{
			MessageBox("Ŀ��λ�ø���Ӧ��Ϊ 0-20 ����");
			return;
		}
		for (int i = 0; i < g_GoalsTotal; i++)
		{
			itoa(i + 1, strtmp.GetBuffer(10), 10);
			fileUtils.GetConfigfileString("TargetXY", strtmp, "0", strGoalInfo.GetBuffer(200), 200, szFileDir);
			strGoalInfo.ReleaseBuffer();
			index = strGoalInfo.Find("/");
			g_AllofGoalX[i] = atoi(strGoalInfo.Left(index));
			strGoalInfo.Delete(0, index + 1);

			index = strGoalInfo.Find('/');
			g_AllofGoalY[i] = atoi(strGoalInfo.Left(index));
			strGoalInfo.Delete(0, index + 1);
		}
	}
	else
	{
		UpdateData(TRUE);
		g_GoalX = atoi(sGoalX);
		g_GoalY = atoi(sGoalY);
	}
	CString temp = "";
	StopState = 0;
	if (pMainFrameThis->m_NetConnection.bConnected && m_UartOpenFlag)
	{
		DWORD ThreadID;
		HANDLE hThreadAutoSimulation;

		if (g_bSingleorMulti == false)
		{
			temp.Format("Ŀ���ȫ�Զ�����,��%d��Ŀ��", g_GoalsTotal);
		}
		else
		{
			temp.Format("Ŀ���ȫ�Զ�����,��Ŀ�������ΪX:%d, Y:%d ", g_GoalX, g_GoalY);
		}
		OnPrepareStartTest(temp);
		pMainFrameThis->KillTimer(0);
		SimulationRun = true;
		hThreadAutoSimulation = CreateThread(NULL, 0, AutoSimulation, this, NULL, &ThreadID);
		GetDlgItem(IDC_COMBO_GETGOALSINFOMODE)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_AUTOTEST)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_STOPAUTOTEST)->EnableWindow(TRUE);
	}
	else
	{
		MessageBox("�����������Ӻʹ������ӣ�");
	}
}

DWORD WINAPI AutoSimulation(LPVOID lpParam)
{
	AGVTestCfg *pMainDLg = (AGVTestCfg *)lpParam;
	float DistenceGP = 0;  //����
	float AngleGP = 0;  //�����
	float LaserRotateAngle = 0;  //����ת��
	float AGVRotateSpeed = 0; //С��ת��
	float AGVLineSpeed = 0;   //С�����ٶ�
	float k = 0.05;      //ϵ��
	float ticksLineSpeed = 0; //���ٶȵ�������
	float ticksAngleSpeed = 0; //���ٶȵ�������
	float fLeftTicks = 0;      //����
	float fRightTicks = 0;   //����
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
	CString sLog = "";
	int CurrentGoalNum = 0;   //��Ŀ��ʱ��ǰ��Ŀ�������
	int StatisticalPoseNum = 0;  //���ջ��Ƶļ���
	int ArriveJudgeCount = 0;
	bool OnlyGetPose = false;  //��ʼͳ������
	DWORD CaseState;

	if (g_bSingleorMulti == false)
	{
		CurrentGoalNum++;
		g_GoalX = g_AllofGoalX[0];
		g_GoalY = g_AllofGoalY[0];
		pMainDLg->sGoalX.Format("%d", g_GoalX);
		pMainDLg->sGoalY.Format("%d", g_GoalY);
		pMainDLg->GetDlgItem(IDC_EDIT_GOALLOCATIONX)->SetWindowText(pMainDLg->sGoalX);
		pMainDLg->GetDlgItem(IDC_EDIT_GOALLOCATIONY)->SetWindowText(pMainDLg->sGoalY);
		sLog.Format("��� %d ��Ŀ����˶�,����ΪX:%d, Y:%d ", CurrentGoalNum, g_GoalX, g_GoalY);
		pMainDLg->SendMessage(WM_WRITELOG, 0, (LPARAM)(LPCTSTR)sLog);
	}

	LaserGetPostRuning = true;
	//hThreadGetPose = CreateThread(NULL, 0, GetPoseThreadProc, pMainDLg, NULL, &ThreadID);
	while (SimulationRun)
	{
		CaseState = WaitForMultipleObjects(3, Event_List, false, 20000);	//�ȴ���ʱ
		if (CaseState == WAIT_TIMEOUT)
		{
			//��ʱδ�յ��ظ����˳�
			StopState = 2;
			SetEvent(Event_STOP);
		}
		else if (CaseState == WAIT_OBJECT_0) //Event_POSE
		{
			//����Ӧ�����ø�С�������ٶȺͽ��ٶ�
			DistenceGP = sqrt(pow(float(g_GoalX - pMainDLg->PoseX), 2) + pow(float(g_GoalY - pMainDLg->PoseY), 2));
			//�ж��Ƿ񵽴�
			if (OnlyGetPose == false)
			{
				if (DistenceGP<80)
				{
					DataTemp = "sMN mNPOSSetSpeed 0 0 0 0 1";
					SpeedSet_zhilingSize = DataTemp.GetLength();
					memcpy(SpeedSet_zhiling, DataTemp, SpeedSet_zhilingSize);
					DataTemp = "z 0 0\r";
					g_bSendParameDataSize = DataTemp.GetLength();
					memcpy(g_bSendParameData, DataTemp, g_bSendParameDataSize);
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
				//���ѵ����ֹͣ
				if (DistenceGP<100)
				{
					DataTemp = "sMN mNPOSSetSpeed 0 0 0 0 1";
					SpeedSet_zhilingSize = DataTemp.GetLength();
					memcpy(SpeedSet_zhiling, DataTemp, SpeedSet_zhilingSize);
					DataTemp = "z 0 0\r";
					g_bSendParameDataSize = DataTemp.GetLength();
					memcpy(g_bSendParameData, DataTemp, g_bSendParameDataSize);
					StatisticalPoseNum++;
					if (StatisticalPoseNum>50)
					{
						OnlyGetPose = false;
						StatisticalPoseNum = 0;
						if (g_bSingleorMulti == false && (CurrentGoalNum < g_GoalsTotal))
						{
							CurrentGoalNum++;
							g_GoalX = g_AllofGoalX[CurrentGoalNum - 1];
							g_GoalY = g_AllofGoalY[CurrentGoalNum - 1];
							pMainDLg->sGoalX.Format("%d", g_GoalX);
							pMainDLg->sGoalY.Format("%d", g_GoalY);
							pMainDLg->GetDlgItem(IDC_EDIT_GOALLOCATIONX)->SetWindowText(pMainDLg->sGoalX);
							pMainDLg->GetDlgItem(IDC_EDIT_GOALLOCATIONY)->SetWindowText(pMainDLg->sGoalY);
							sLog.Format("��� %d ��Ŀ����˶�,����ΪX:%d, Y:%d ", CurrentGoalNum, g_GoalX, g_GoalY);
							pMainDLg->SendMessage(WM_WRITELOG, 0, (LPARAM)(LPCTSTR)sLog);
						}
						else
						{
							StopState = 1;
							SetEvent(Event_STOP);
						}
					}
					continue;
				}
			}
			AngleGP = atan2(float(g_GoalY - pMainDLg->PoseY), float(g_GoalX - pMainDLg->PoseX)) * 180 / PI;
			if (AngleGP<0)
			{
				AngleGP = 360 + AngleGP;
			}
			LaserRotateAngle = AngleGP * 1000 - pMainDLg->PoseAngle;
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
			pMainDLg->m_LineSpeed.Format("%d", (int)AGVLineSpeed);
			pMainDLg->m_AngleSpeed.Format("%d", (int)AGVRotateSpeed);

			pMainDLg->SpeedX = (short)(AGVLineSpeed*float(g_GoalX - pMainDLg->PoseX) / DistenceGP);
			pMainDLg->SpeedY = (short)(AGVLineSpeed*float(g_GoalY - pMainDLg->PoseY) / DistenceGP);
			pMainDLg->SpeedAngle = -(int)AGVRotateSpeed;
			pMainDLg->GetDlgItem(IDC_EDIT_LINESPEED)->SetWindowText(pMainDLg->m_LineSpeed);
			pMainDLg->GetDlgItem(IDC_EDIT_ANGLESPEED)->SetWindowText(pMainDLg->m_AngleSpeed);
			pMainDLg->SetDlgItemInt(IDC_EDIT_SPEEDX, pMainDLg->SpeedX);
			pMainDLg->SetDlgItemInt(IDC_EDIT_SPEEDY, pMainDLg->SpeedY);
			pMainDLg->SetDlgItemInt(IDC_EDIT_SPEEDANGLE, pMainDLg->SpeedAngle);
			temp.Format("%d, %d, %d", pMainDLg->SpeedX, pMainDLg->SpeedY, pMainDLg->SpeedAngle);
			sLog = "���ߺͽ�: " + pMainDLg->m_LineSpeed + ", " + pMainDLg->m_AngleSpeed + "  ����XY��: " + temp + "\r\n";

			//����
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
			//��������
			ticksLineSpeed = CalculateTicksFromLineSpeed(AGVLineSpeed / 1000);
			ticksAngleSpeed = CalculateTicksFromAngleSpeed(AGVRotateSpeed / 1000 * (PI / 180));
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
			sLog = sLog + "         ����������: " + DataTemp.Mid(1, DataTemp.GetLength() - 2) + "\r\n";
			DataTemp = "z ";
			temp.Format("%d", LeftTicks);
			DataTemp += temp + " ";
			temp.Format("%d", RightTicks);
			DataTemp += temp + "\r";
			g_bSendParameDataSize = DataTemp.GetLength();
			memcpy(g_bSendParameData, DataTemp, g_bSendParameDataSize);
			sLog = sLog + "         ���͸�С��: " + DataTemp.Mid(0, DataTemp.GetLength() - 1) + "\r\n";

			if (LaserSetSpeedRuning == false)
			{
				LaserSetSpeedRuning = true;
				hThreadSetSpeed = CreateThread(NULL, 0, SetSpeedThreadProc, pMainDLg, NULL, &ThreadID);
			}

			if (AGVControlRunning == false)
			{
				AGVControlRunning = true;
				hThreadAGVControl = CreateThread(NULL, 0, AGVControlProc, pMainDLg, NULL, &ThreadID);
			}
			pMainDLg->SendMessage(WM_WRITELOG, 0, (LPARAM)(LPCTSTR)sLog);
		}
		else if (CaseState == (WAIT_OBJECT_0 + 1)) //Event_NOPOSE
		{
			DataTemp = "sMN mNPOSSetSpeed 0 0 0 0 1";
			SpeedSet_zhilingSize = DataTemp.GetLength();
			memcpy(SpeedSet_zhiling, DataTemp, SpeedSet_zhilingSize);
			DataTemp = "z 0 0\r";
			g_bSendParameDataSize = DataTemp.GetLength();
			memcpy(g_bSendParameData, DataTemp, g_bSendParameDataSize);
		}
		else if (CaseState == (WAIT_OBJECT_0 + 2)) //Event_STOP
		{
			//�����˳�		
			SimulationRun = false;
			AGVControlRunning = false;
			LaserGetPostRuning = false;
			LaserSetSpeedRuning = false;
			pMainDLg->OnBnClickedButtonStopagv();
			if (StopState == 1)
			{
				if (g_bSingleorMulti == false)
				{
					sLog = "������ɣ��ѵ�������Ŀ�ĵأ�\r\n";
					pMainDLg->SendMessage(WM_WRITELOG, 0, (LPARAM)(LPCTSTR)sLog);
					pMainDLg->MessageBox("������ɣ��ѵ�������Ŀ�ĵأ�", MB_OK);
				}
				else
				{
					sLog = "������ɣ��ѵ���Ŀ�ĵأ�\r\n";
					pMainDLg->SendMessage(WM_WRITELOG, 0, (LPARAM)(LPCTSTR)sLog);
					pMainDLg->MessageBox("������ɣ��ѵ���Ŀ�ĵأ�", MB_OK);
				}
			}
			else if (StopState == 2)
			{
				sLog = "���Խ�����δ��Ŀ�꣬��ʱ��\r\n";
				pMainDLg->SendMessage(WM_WRITELOG, 0, (LPARAM)(LPCTSTR)sLog);
				pMainDLg->MessageBox("���Խ�����δ��Ŀ�꣬��ʱ��");
			}
			else if (StopState == 3)
			{
				sLog = "�ɹ�������ֹ��\r\n";
				pMainDLg->SendMessage(WM_WRITELOG, 0, (LPARAM)(LPCTSTR)sLog);
				pMainDLg->MessageBox("�ɹ�������ֹ��");
			}
			pMainDLg->GetDlgItem(IDC_COMBO_GETGOALSINFOMODE)->EnableWindow(TRUE);
			pMainDLg->GetDlgItem(IDC_BUTTON_AUTOTEST)->EnableWindow(TRUE);

			m_pMyLog->Release();
			pMainFrameThis->SetTimer(0, 500, NULL);
		}

		Sleep(1);
	}
	return 0;
}

void AGVTestCfg::OnBnClickedButtonStopAutoTest()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������	
	StopState = 3;
	SetEvent(Event_STOP);
}

void AGVTestCfg::OnCreateFile(CString fileName, int FileKind)
{
	CFile file;
	if (FileKind == 0)
		OnCreateFolder(LaserMAC);
	FilePath = m_SaveFolderPath + "\\" + fileName;
	file.Open(FilePath, CFile::modeCreate | CFile::modeWrite);
	file.Close();
}

void AGVTestCfg::OnPrepareStartRecordOperInfo(CString OperName)
{
	CString FileName = "";
	FileName = LaserMAC + ".txt";
	OnCreateFile(FileName, 1);
	m_pMyOperLog = CCDMLogInfo::GetInstance(FilePath.GetBuffer(FilePath.GetLength()));
	//CString strTemp = "\r\n========" + CTime::GetCurrentTime().Format("%H:%M:%S ") + OperName + "========\r\n";
	//if (m_pMyOperLog)
	//	m_pMyOperLog->SetNotify(strTemp.GetBuffer(strTemp.GetLength()));
}

void AGVTestCfg::OnPrepareStartTest(CString TestName)
{
	//CString FileName = "";
	//FileName = LaserMAC + ".txt_NAV";
	//OnCreateFile(FileName, 0);
	OnCreateFolder(LaserMAC);
	/*m_pMyLog = CCDMLogInfo::GetInstance(FilePath.GetBuffer(FilePath.GetLength()));
	CString strTemp = "\r\n========" + CTime::GetCurrentTime().Format("%H:%M:%S ") + TestName + "========\r\n";
	if (m_pMyLog)
	m_pMyLog->SetNotify(strTemp.GetBuffer(strTemp.GetLength()));*/
}
afx_msg LRESULT AGVTestCfg::OnWriteLog(WPARAM wParam, LPARAM lParam)
{
	CString  str = (char*)lParam;
	CString getData = "";
	if (m_SaveFolderPath.GetLength() > 0 && str.GetLength() > 0)
	{
		CTime t = CTime::GetCurrentTime();
		getData = t.Format("%H:%M:%S ") + str;
		CString FileName = m_SaveFolderPath + "\\" + fileLaserMAC + ".txt_NAV";
		if (FilePathNav != FileName || AfterNAVCount == 2 || f_StopTest)
		{
			if (f_StopTest)
			{
				f_StopTest = false;
			}
			if (AfterNAVCount == 2)
			{
				AfterNAVCount = 0;
			}
			if (f_NavFileOpen)
			{
				FileNav.Close();
				f_NavFileOpen = false;
			}
			FilePathNav = FileName;
			f_NavFileOpen = FileNav.Open(FilePathNav, CFile::modeCreate | CFile::modeWrite);
		}
		FileNav.Write(getData.GetBuffer(), getData.GetLength() * sizeof(TCHAR));//strΪCString����
		FileNav.Flush();
	}
	//д����־
	return 0;
}


void AGVTestCfg::OnSelchangeComboGetgoalsinfomode()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (m_CombGetGoalsInfoMode.GetCurSel() == 0)
	{
		GetDlgItem(IDC_EDIT_GOALLOCATIONX)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_GOALLOCATIONY)->EnableWindow(FALSE);
		g_bSingleorMulti = false;
	}
	else if (m_CombGetGoalsInfoMode.GetCurSel() == 1)
	{
		GetDlgItem(IDC_EDIT_GOALLOCATIONX)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_GOALLOCATIONY)->EnableWindow(TRUE);
		g_bSingleorMulti = true;
	}
}
UINT SendLMSettingFirzhiling(LPVOID lpParam)
{
	AGVTestCfg *pAgv = (AGVTestCfg *)lpParam;
	if (n_GetLM < 5)
	{
		CString str_nlm;
		str_nlm.Format("%d", n_GetLM);
		CString strTemp = "��ȡ�İб�����Ϊ" + str_nlm+"������ȷ�ϰб����Ϊ5����������һ��������";
		AfxMessageBox(strTemp);
		pAgv->WriteToRichEdit(strTemp);
		pAgv->StopSlop();//ֹͣ����
	}
	else
	{
		////�б����õ�һ������
		CTime time = CTime::GetCurrentTime();
		char Hour = time.GetHour();
		char Miu = time.GetMinute();
		char Sec = time.GetSecond();
		unsigned char pn[14] = { 0x00, 0x20, 0x00, 0x00, 0x00, Hour, Miu, Sec, 0x01, 0x01, 0x00, 0x07, 0x06, 0x03 };
		ZhiLing(LMSettingFir_zhiling, pn, 0);
		LMSettingFir_zhiling[26] = 0;
		LMSettingFir_zhiling[27] = 0;
		LMSettingFir_zhiling[28] = 0;
		LMSettingFir_zhiling[29] = n_GetLM;
		LMSettingFir_zhiling[30] = 0;
		LMSettingFir_zhiling[31] = 2;
		AfxGetMainWnd()->PostMessage(WM_SENDCMD, LMSettingFir_zhilingSize, (LPARAM)LMSettingFir_zhiling);
		Sleep(2000);//�ȴ�2s
		///�б����÷ǵ�һ������
		if (f_FirDataLMSet)
		{
			f_FirDataLMSet = false;
			int L_Frame = n_GetLM * 14 + 28;
			int LMSettingSec_zhilingSize = L_Frame + 4;
			unsigned char LMSettingSec_zhiling[500] = { 0 };
			CTime time1 = CTime::GetCurrentTime();
			char Hour1 = time1.GetHour();
			char Miu1 = time1.GetMinute();
			char Sec1 = time1.GetSecond();
			unsigned char pn1[14] = { L_Frame >> 8, L_Frame & 0xff, 0x00, 0x00, 0x00, Hour1, Miu1, Sec1, 0x01, 0x01, 0x00, 0x07, 0x06, 0x03 };
			ZhiLing(LMSettingSec_zhiling, pn1, 0);
			LMSettingSec_zhiling[26] = 0;
			LMSettingSec_zhiling[27] = 1;
			for (int i = 0; i < n_GetLM; i++)
			{
				LMSettingSec_zhiling[28 + i * 14] = lmInfoBuf[i * 5] >> 8;
				LMSettingSec_zhiling[29 + i * 14] = lmInfoBuf[i * 5] & 0xff;
				LMSettingSec_zhiling[30 + i * 14] = lmInfoBuf[1 + i * 5] >> 24;
				LMSettingSec_zhiling[31 + i * 14] = lmInfoBuf[1 + i * 5] >> 16;
				LMSettingSec_zhiling[32 + i * 14] = lmInfoBuf[1 + i * 5] >> 8;
				LMSettingSec_zhiling[33 + i * 14] = lmInfoBuf[1 + i * 5] & 0xFF;
				LMSettingSec_zhiling[34 + i * 14] = lmInfoBuf[2 + i * 5] >> 24;
				LMSettingSec_zhiling[35 + i * 14] = lmInfoBuf[2 + i * 5] >> 16;
				LMSettingSec_zhiling[36 + i * 14] = lmInfoBuf[2 + i * 5] >> 8;
				LMSettingSec_zhiling[37 + i * 14] = lmInfoBuf[2 + i * 5] & 0xFF;
				LMSettingSec_zhiling[38 + i * 14] = 0;
				LMSettingSec_zhiling[39 + i * 14] = 0;
				LMSettingSec_zhiling[40 + i * 14] = lmInfoBuf[3 + i * 5];//��״
				LMSettingSec_zhiling[41 + i * 14] = lmInfoBuf[4 + i * 5];//�ߴ�
				if (i == n_GetLM - 1)
				{
					AfxGetMainWnd()->PostMessage(WM_SENDCMD, LMSettingSec_zhilingSize, (LPARAM)LMSettingSec_zhiling);
					Sleep(300);
				}
			}
		}
		else
		{
			pAgv->WriteToRichEdit("�б�����ʧ�ܣ������½���һ��������");
			AfxMessageBox("�б�����ʧ�ܣ������½���һ��������");
		}
	}
	return 0;
}
afx_msg LRESULT AGVTestCfg::OnLminfoview(WPARAM wParam, LPARAM lParam)
{
	lmInfoBuf = (int *)lParam;
	n_GetLM = (int)wParam;//�б����
	CString str;
	for (int i = 0; i < n_GetLM; i++)
	{
		str.Format("%d", lmInfoBuf[i * 5]);
		m_listctrl.InsertItem(i, str);
		str.Format("%d", lmInfoBuf[1 + i * 5]);
		m_listctrl.SetItemText(i, 1, str);
		str.Format("%d", lmInfoBuf[2 + i * 5]);
		m_listctrl.SetItemText(i, 2, str);
		str.Format("%d", lmInfoBuf[4 + i * 5]);
		m_listctrl.SetItemText(i, 3, str);
		Sleep(10);
	}
	CString str_nlm = "";
	str_nlm.Format("%d", n_GetLM);
	WriteToRichEdit("����ɰб��ȡ����ȡ�İб����Ϊ��" + str_nlm);
	///�б�������ʾ��listctrl
	AfxBeginThread(SendLMSettingFirzhiling, this);//�б�����
	return 0;

}
afx_msg LRESULT AGVTestCfg::OnLmsettinginfo(WPARAM wParam, LPARAM lParam)
{
	int LMSetInfo = (int)lParam;
	if (LMSetInfo == 1)
	{
		CString strTemp = "�б���Ϣ���óɹ���";
		WriteToRichEdit(strTemp);
		///�л�������ģʽ
		f_nav = true;
		AfxGetMainWnd()->PostMessage(WM_SENDCMD, NaviModeSet_zhilingSize, (LPARAM)NaviModeSet_zhiling);
		g_NAVflag = TRUE;
		WriteToRichEdit("����ģʽ���óɹ���");
	}
	else
	{
		CString strTemp = "�б���Ϣ����ʧ�ܣ������½���һ��������";
		WriteToRichEdit(strTemp);
		AfxMessageBox(strTemp);
		g_NAVflag = FALSE;
	}
	return 0;
}

UINT ThreadSavePXDataExcel(LPVOID pParam)//ƫ�����ݱ��浽EXCEL������ƫ�ı�����ƫ�ı�
{
	AGVTestCfg *pAgv = (AGVTestCfg *)pParam;
	CString szGetName = "", str = "";
	szGetName = m_SaveFolderPath + _T('\\') + fileLaserMAC + ".xlsx";//��ǰ��������Ӧ��ƫ������excel��ַ
	CString path = "";
	GetModuleFileName(NULL, path.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	path.ReleaseBuffer();
	int pos = path.ReverseFind('\\');
	path = path.Left(pos);
	CString strExcelDemoPath = path + _T("\\") + "����ƫ����������ģ��.xlsx";////ģ��excel��ַ
	CString strDecName = "����";
	g_sExcelPXInfo.m_u32Col = 1;
	g_sExcelPXInfo.m_u32Row = 10;
	g_sExcelPXInfo.m_strName = szGetName;
	if (g_u32MeasurePXInfoIndex >= 1000)
	{
		pAgv->WriteToRichEdit("�Ѵﵽ����¼���ݣ����½���ƫ��������");
		AfxMessageBox("�Ѵﵽ����¼���ݣ����½���ƫ��������");
		return 1;
	}
	bool bInit = pAgv->m_OperExcel.InitExcel();
	if (!bInit)
	{
		pAgv->WriteToRichEdit("��ʼ��excelʧ��,��ȷ�ϱ����Ƿ�װExcel�����½���ƫ��������");
		AfxMessageBox("��ʼ��excelʧ��,��ȷ�ϱ����Ƿ�װExcel�����½���ƫ��������");
		return 1;
	}
	else
	{
		pAgv->WriteToRichEdit("��ʼ��excel�ɹ���");
	}
	bool bRet = pAgv->m_OperExcel.OpenExcelFile(strExcelDemoPath);
	CString strSheetName = pAgv->m_OperExcel.GetSheetName(1);
	bool bLoad = pAgv->m_OperExcel.LoadSheet(strSheetName);
	g_sRecordPXInfo.m_strName = strDecName;
	pAgv->m_OperExcel.SetCellString_Cells("A3", 0, 2, CTime::GetCurrentTime().Format("%Y-%m-%d-%H-%M-%S"));
	pAgv->m_OperExcel.SetCellString_Cells("A3", 0, 4, g_sRecordPXInfo.m_strAPDBV);
	pAgv->m_OperExcel.SetCellString_Cells("A3", 1, 2, g_sRecordPXInfo.m_strName);
	pAgv->m_OperExcel.SetCellString_Cells("A3", 1, 4, g_sRecordPXInfo.m_strAPDHV);
	pAgv->m_OperExcel.SetCellString_Cells("A3", 2, 2, g_sRecordPXInfo.m_strAPDTemper);
	pAgv->m_OperExcel.SetCellString_Cells("A3", 2, 4, g_sRecordPXInfo.m_strAPDRatio);
	g_sExcelPXInfo.m_u32Row = 2;
	for (int i = 0; i < g_u32MeasurePXInfoIndex; i++)
	{
		if (i == 0)
		{
			//����ԭʼֵ1
			g_sMeasurePXInfo[i].m_OriginalOne = 0;
		}
		else
		{
			//����ԭʼֵ1
			g_sMeasurePXInfo[i].m_OriginalOne = g_sMeasurePXInfo[i - 1].m_OriginalTwo;
		}
		//���㲹��ֵ1
		if (i != g_u32MeasurePXInfoIndex - 1)
			g_sMeasurePXInfo[i].m_CompensationOne = g_sMeasurePXInfo[i].m_OriginalOne;
		else
			g_sMeasurePXInfo[i].m_CompensationOne = g_sMeasurePXInfo[i].m_OriginalOne + 7200;
		str.Format("%d", g_sMeasurePXInfo[i].m_OriginalTwo);
		pAgv->m_OperExcel.SetCellString_Cells("A6", g_sExcelPXInfo.m_u32Row, 2, str);
		g_sExcelPXInfo.m_u32Row++;
	}
	g_sExcelPXInfo.m_u32Row = 2;
	pAgv->WriteToRichEdit("ԭʼֵ�Ͳ���ֵ����ɹ���");
	pAgv->m_OperExcel.SaveasXSLFile(g_sExcelPXInfo.m_strName);
	pAgv->m_OperExcel.CloseExcelFile(0);
	pAgv->m_OperExcel.ReleaseExcel();
	pAgv->WriteToRichEdit("Excel���ݱ���ɹ���");
	pAgv->WriteToRichEdit("���ڽ�������ƫ�ı�.......");
	int g_PXdatax[arrayXYsize] = { 0 };
	double g_PXdatay[arrayXYsize] = { 0 };
	double OneRealVal = _ttof(pAgv->m_OperExcel.GetCellString(g_sExcelPXInfo.m_strName, 8, 6));//1*��ʵֵ
	for (int k = 0; k < g_u32MeasurePXInfoIndex; k++)
	{

		g_PXdatax[k] = g_sMeasurePXInfo[k].m_CompensationOne;
		//������ʵֵ
		g_sMeasurePXInfo[k].m_ActualValue = OneRealVal*k;
		//������=��ʵֵ-����1
		g_sMeasurePXInfo[k].m_Correction = g_sMeasurePXInfo[k].m_ActualValue - g_sMeasurePXInfo[k].m_CompensationOne;
		g_PXdatay[k] = g_sMeasurePXInfo[k].m_Correction;
	}
	if (pAgv->CubicSplineInterpolation(g_PXdatax, g_PXdatay, g_u32MeasurePXInfoIndex))//����������ֵ����ƫ�ı�
	{
		pAgv->WriteToRichEdit("ƫ�ı����ɳɹ���");
		pAgv->DispTips(IDB_PNG5, "����ƫ�ı�");
		pAgv->WriteToRichEdit("����ƫ�ı�");
		if (f_CorrectionNetState)
		{
			pAgv->DownloadEccentricityTable(pAgv->PXTablePath);//����ƫ�ı�
		}
		else
		{
			pAgv->StopSlop();
			pAgv->WriteToRichEdit("�����ѶϿ����޷���������ƫ�ı�����,���ֶ�����ƫ�ı����ж�λ������֤��");
			AfxMessageBox("�����ѶϿ����޷���������ƫ�ı�����,���ֶ�����ƫ�ı����ж�λ������֤��");
		}
	}
	else
	{
		pAgv->WriteToRichEdit("ƫ�ı�����ʧ�ܣ����½���ƫ����������");
		AfxMessageBox("ƫ�ı�����ʧ�ܣ����½���ƫ����������");
	}
	memset(g_PXdatax, 0, arrayXYsize);
	memset(g_PXdatay, 0, arrayXYsize);
	//memset(&g_sMeasurePXInfo, 0, sizeof(g_sMeasurePXInfo));
	//memset(&g_sExcelPXInfo, 0, sizeof(g_sExcelPXInfo));
	return 1;
}

bool AGVTestCfg::CubicSplineInterpolation(int *g_pxdatax, double* g_pxdatay, int g_u32MeasurePXInfoIndex)
{
	CFile sfile;
	BOOL pfile;
	int  i;
	static int j = 0; //���int���� yjun
	double dx, dy = 0;
	CString inttostr = " ";
	CString temp;
	double pdx[arrayXYsize] = { 0 };
	double pdy[arrayXYsize] = { 0 };
	((CAxis)((CAxes)m_tchartinterp_px.get_Axis()).get_Bottom()).put_Maximum(7200);
	for (i = 0; i < g_u32MeasurePXInfoIndex; i++)
	{
		pdx[i] = (double)g_pxdatax[i];
		pdy[i] = g_pxdatay[i];
		((CSeries)m_tchartinterp_px.Series(1)).AddXY(pdx[i], pdy[i], NULL, RGB(255, 0, 0));
		((CSeries)m_tchartinterp_px.Series(0)).AddXY(pdx[i], pdy[i], NULL, RGB(255, 255, 0));
	}
	Interp hmh(pdx, pdy, g_u32MeasurePXInfoIndex);
	double Arraydx[7200] = { 0 }, Arraydy[7200] = { 0 };
	for (i = 0; i <= 7199; i += 1)
	{
		dx = (double)i;
		Arraydx[i] = dx;
		if (hmh.GetYByX(dx, dy))
		{
			Arraydy[i] = dy;
			inttostr.Format("%d\t\t", (INT32)(5 * (INT32)dy));
			temp += inttostr;
			j++;
			if (j > 4)
			{
				temp += "\r\n";
				j = 0;
			}
		}
		else
		{
			WriteToRichEdit("����Ĳ�ֵ����Դ����ȷ�����ʧ�ܣ�������ѡ������!");
			MessageBox("����Ĳ�ֵ����Դ����ȷ�����ʧ�ܣ�������ѡ������!");
			PostNcDestroy();
			return false;
		}

	}
	COleSafeArray XValues;
	COleSafeArray YValues;
	long k(0);
	DWORD wLength = 7200;
	XValues.Create(VT_R8, 1, &wLength);
	YValues.Create(VT_R8, 1, &wLength);
	for (k = 0; k < 7200; k++)
	{
		XValues.PutElement(&k, Arraydx + k);
		YValues.PutElement(&k, Arraydy + k);
	}
	((CSeries)m_tchartinterp_px.Series(0)).AddArray(7200, YValues, XValues);
	//�����ļ�
	CString savepath = "";
	savepath = m_SaveFolderPath + "\\" + fileLaserMAC + ".txt_PX";
	PXTablePath = savepath;
	pfile = sfile.Open(savepath, CFile::modeReadWrite | CFile::modeCreate);
	if (pfile == NULL)
	{
		WriteToRichEdit("��ƫ��������ʧ�ܣ�");
		MessageBox("��ƫ��������ʧ�ܣ�");
		return false;
	}
	sfile.SeekToEnd();
	sfile.Write(temp, temp.GetLength());
	sfile.Close();
	Sleep(200);
	//��ͼ
	CExport res = (CExport)m_tchartinterp_px.get_Export();
	CString PicPath = m_SaveFolderPath + "\\" + fileLaserMAC + "���ͼ.jpg";
	res.SaveToBitmapFile(PicPath);
	Sleep(1500);
	return true;
}
#define FramePX_02020202 1
int AGVTestCfg::SetPXPacket(char index, char* sendpacket, int* datapacket, int len, int num)  //Դ���ݵĳ��� ����
{
#if FramePX_02020202
	bool calcuBCC = TRUE;
	static int currentNum = -1;
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
	int i = 0;
	int j = 512 * (num - 1);//���Ŵ�0��ʼ
	int dlen;
	int plen = (len % 512 == 0) ? (len / 512) : (len / 512 + 1);
	CString tm;
	tm.Format("%d,%d", len, plen);
	TRACE(tm);
	if (j >= len)
	{
		return 0;
	}
	CTime time = CTime::GetCurrentTime();
	char Hour = time.GetHour();
	char Miu = time.GetMinute();
	char Sec = time.GetSecond();
	unsigned char zhiling[26] = { 0xFF, 0xAA, 0x00, 0x1E, 0x00, 0x00, 0x00, Hour, Miu, Sec, 0x01, 0x01, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x16, 0x00, 0x00 };//����ƫ�ı�
	memcpy(acTmp, zhiling, 26);
	acTmp[26] = ((char)plen >> 8);    //�ܰ���
	acTmp[27] = ((char)plen & 0xff);
	acTmp[28] = ((char)num >> 8);//����
	acTmp[29] = ((char)num & 0xff);
	int packlen_cur = 0;
	for (i = 0; i < 512 && (j + i) < len; i++)
	{
		acTmp[32 + i] = (char)(datapacket[j + i]);
		packlen_cur++;
	}
	j = packlen_cur;
	if (calcuBCC)
	{
		char bccTemp = Xor_download((char)(bccW_PXpacks & 0xff), &acTmp[32], packlen_cur);
		bccW_PXpacks = bccTemp & 0x00ff;
	}
	acTmp[30] = (char)(j >> 8);//�������ݳ���
	acTmp[31] = (char)(j & 0xff);
	int lenth = j + 32;
	acTmp[2] = (char)(lenth >> 8);
	acTmp[3] = (char)(lenth & 0xff);
	memcpy(sendpacket, acTmp, (j + 36));
	return (j + 36);

#else
	int i = 0;
	int j = 512 * (num - 1);//���Ŵ�0��ʼ
	int dlen;
	int plen = (len % 512 == 0) ? (len / 512) : (len / 512 + 1);
	CString tm;
	tm.Format("%d,%d", len, plen);
	TRACE(tm);
	if (j > len)
	{
		return 0;
	}
	sendpacket[0] = (char)0xFF;//֡ͷ
	sendpacket[1] = (char)0xFF;
	sendpacket[2] = (char)0x58;//����
	sendpacket[3] = (char)num;//����
	sendpacket[6] = (char)plen;    //�ܰ���
	sendpacket[7] = (char)plen >> 8;

	for (i = 0; i < 512 && (j + i) < len; i++)
	{
		sendpacket[12 + i * 2] = (char)(datapacket[j + i]);
		sendpacket[13 + i * 2] = (char)(datapacket[j + i] >> 8);
	}
	j = i * 2 + 14;
	dlen = i * 2;
	sendpacket[4] = (j & 0xff);//������
	sendpacket[5] = (j >> 8) & 0xFF;
	sendpacket[8] = (char)0x55;
	sendpacket[9] = (char)0xaa;
	sendpacket[10] = (j - 14) & 0xff;
	sendpacket[11] = ((j - 14) >> 8) & 0xff;
	sendpacket[j - 2] = (char)0xFF;
	sendpacket[j - 1] = (char)0xFF;
	return j;
#endif

}

UINT SendPXDataThreadProc(LPVOID lpParm)
{
	bccW_PXpacks = 0;
	AGVTestCfg* pAgv = (AGVTestCfg*)lpParm;
	CFileUtils fileutils;
	const int nFrameLen = 1036;//֡��
	const int nDataLen = 1024; //���ݳ���
	const int nMaxFileData = 512 * 100;//�ļ�����������
	int k = 0;
	int nIndex1 = 0;
	int nIndex2 = 0;
	DWORD dw;
	CString hustring;
	int packetlen;//udp���ĳ���
	int cfnum = 0;//�ط�����
	int g_nReadbuf[readPXbuflen] = { 0 };  //��д����
	int g_nReadbuflen = 0; //ʵ�ʳ���
	char psenddata[2000];
	int sendnum = 1;
	CString str;
	DWORD biao;
	CStdioFile hufile;
	if (hufile.Open(pAgv->PXTablePath, CFile::modeRead))
	{
		while (hufile.ReadString(hustring))
		{
			hustring.TrimLeft();
			hustring.TrimRight();
			if (hustring == "")
			{
				break;
			}
			biao = sscanf(hustring, "%d %d %d %d %d", &g_nReadbuf[k], &g_nReadbuf[k + 1], &g_nReadbuf[k + 2], &g_nReadbuf[k + 3], &g_nReadbuf[k + 4]);
			if (biao != 0)
			{
				k += biao;
				if (biao != 5)
				{
					break;
				}
			}
		}
		hufile.Close();
	}//�������ݳɹ�
	if (0 == k)
	{
		pAgv->MessageBox("�ļ��������ݣ�", MB_OK);
		pAgv->WriteToRichEdit("�ļ��������ݣ�");
		return 0;
	}
	while (1)
	{
		packetlen = pAgv->SetPXPacket(0, psenddata, g_nReadbuf, k, sendnum);//�ӵ�1������
		if (packetlen != 0)  //����
		{
			CMainFrame *pMainDlg;
			pMainDlg = (CMainFrame *)AfxGetApp()->m_pMainWnd;
			pMainDlg->m_NetConnection.pcSendDataBuf = psenddata;
			pMainDlg->m_NetConnection.nSendDataBufSize = packetlen;
			if (pMainDlg->m_NetConnection.bConnected)
				SendData(&pMainDlg->m_NetConnection);
		}
		else
		{
			break;  //������ ����
		}
		dw = WaitForSingleObject(g_PXevnet, 10000); //���¼�  ������ INFINITE 
		ResetEvent(g_PXevnet);
		switch (dw)
		{
		case WAIT_OBJECT_0:
			//�ȵ��¼�			
			if (g_PXflag == TRUE && g_PXnNum == sendnum)  //���ͳɹ�
			{
				sendnum++;
				g_PXflag = FALSE;
				cfnum = 0;
			}
			else                      //����ʧ��
			{
				//	sendnum = 1;
				str.Format("%d", g_PXnNum);
				if (cfnum > 2)
				{
					pAgv->StopSlop();
					pAgv->MessageBox("��" + str + "��������дʧ�ܣ��������ֶ�����ƫ�ı����ж�λ������֤��", MB_OK);
					pAgv->WriteToRichEdit("��" + str + "��������дʧ�ܣ��������ֶ�����ƫ�ı����ж�λ������֤��");
					return 0;
				}
				cfnum++;
			}
			break;
		case WAIT_TIMEOUT:
			//��ʱδ�յ��ظ�		
			if (cfnum > 2)
			{
				str.Format("%d", sendnum);
				pAgv->WriteToRichEdit("��ʱδ�յ���" + str + "���ظ����������ֶ�����ƫ�ı����ж�λ������֤��");
				pAgv->StopSlop();
				pAgv->MessageBox("��ʱδ�յ���" + str + "���ظ����������ֶ�����ƫ�ı����ж�λ������֤��", MB_OK);
				sendnum = 1;
				return 0;
			}
			else
			{
				str.Format("%d", sendnum);
				pAgv->WriteToRichEdit("��ʱδ�յ���" + str + "���ظ����ط�");
				pAgv->MessageBox("��ʱδ�յ���" + str + "���ظ����ط�", MB_OK);
				cfnum++;
			}
			break;
		case WAIT_FAILED:
			return 0;
			break;
		}
		Sleep(500);
	}
	//����������֮���飬ͨ��֮�����д
	CTime time = CTime::GetCurrentTime();
	char Hour = time.GetHour();
	char Miu = time.GetMinute();
	char Sec = time.GetSecond();
	unsigned char pn[14] = { 0x00, 0x1E, 0x00, 0x00, 0x00, Hour, Miu, Sec, 0x01, 0x01, 0x00, 0x07, 0x05, 0x17 };
	ZhiLing(ShaoXie_zhiling, pn, 0);
	ShaoXie_zhiling[26] = (bccW_PXpacks >> 8);
	ShaoXie_zhiling[27] = bccW_PXpacks;
	AfxGetMainWnd()->PostMessage(WM_SENDCMD, ShaoXie_zhilingSize, (LPARAM)ShaoXie_zhiling);
	return 0;
}

bool AGVTestCfg::DownloadEccentricityTable(CString TablePath)
{
	g_PXnNum = 0;
	g_PXflag = FALSE;
	ResetEvent(g_PXevnet);
	CString l_sNameTmp;//���ļ��ĺ�׺��
	l_sNameTmp = TablePath.Right(TablePath.GetLength() - TablePath.ReverseFind('.') - 1);
	if (l_sNameTmp != "txt_PX")
	{
		WriteToRichEdit("���ļ�ʧ��o(>�n<)o��\n���ѡ���Ӧ����ѡ������ƫ�ı��ļ���");
		return false;
	}
	memset(g_nReadPXbuf, 0, readPXbuflen);
	g_nReadPXbuflen = 0; //ʵ�ʳ���
	if (TablePath != "")
	{
		WriteToRichEdit("����ƫ�ı����ݷ�����......");
		CWinThread* pThread;
		pThread = AfxBeginThread(SendPXDataThreadProc, this);
	}
	else
	{
		MessageBox("���ȴ��ļ���", MB_OK);
		return false;
	}
	return true;
}

void AGVTestCfg::StopSlop()
{
	f_StopTest = true;
	if (f_NavFileOpen)
	{
		FileNav.Close();
		f_NavFileOpen = false;
	}
	f_MoveZeroCompensation = false;//ֹͣ���㲹���˶�
	f_SlopCompensation = false;//ֹͣƫ�����������˶�
	AfterNAVCount = 0;
	LaserGetPostRuning = false;
	f_HandOrAutoNavVeri = false;//��Ϊ�Զ�����
	Com_Send(a_ucTemp12, 6);//���ָֹͣ��
	m_AutoTest.EnableWindow(TRUE);
	m_HandNavTest.EnableWindow(TRUE);
	m_GetLMAng.EnableWindow(TRUE);
	m_HanfDownPX.EnableWindow(TRUE);
	m_SlopMove.EnableWindow(TRUE);
	m_SureSQ.EnableWindow(TRUE);
	m_richlog.SetSel(0, -1);
	m_richlog.ReplaceSel("");
	GetDlgItem(IDC_BUTTON_LOADDATA)->EnableWindow(TRUE);
	f_Realgap = true;
	f_AGVTest = false;
	Realgap = 450;
	m_SaveFolderPath = "";
	f_QualiStandard = false;
	pSetView1->m_SetProdDlg.m_btnPIDDn1.EnableWindow(TRUE);
	pSetView1->m_SetAppDlg.m_DownApplicPara.EnableWindow(TRUE);
	pSetView1->m_SetProdDlg.m_btnPIDRead.EnableWindow(TRUE);// ��ƫ��������ȡ��ťʹ��
	pSetView1->m_SetAppDlg.m_ReadApplicPara.EnableWindow(TRUE);//��Ӧ�ò�����ȡ��ťʹ��
	f_FirCount = true;
	DispTips(IDB_PNG0, "����״̬");
}
UINT AngleCompensation(LPVOID lpParam)
{
	int AnyPositionCount = 0, ToPositionCount = 0, SumAng2 = 0, SuccSlopCount = 0, TruePoseCount = 0, n_SerialData = 0, temp_Ang = 0;
	int TempCompenAng[TargetAngCount] = { 0 };
	int TempSumAng[15] = { 0 };
	f_SlopCompensation = true;
	AGVTestCfg *pAgv = (AGVTestCfg*)lpParam;
	CMainFrame *pMainFrameThis;
	pMainFrameThis = (CMainFrame *)AfxGetApp()->m_pMainWnd;//AfxGetApp( )����������Եõ���ǰӦ�ý��̵�ָ��
	while ((!f_booby || f_lastslop || pAgv->m_ang1 != t_LMAng || pAgv->m_ang2 - pAgv->m_ang1 > 450) && f_SlopCompensation)
	{
		CString strTemp = "";
		UINT16 CaclCRC = 0;
		int g_ang = 0;
		if (pAgv->m_ang2 >= 6700)
		{
			if (pAgv->m_ang2 - pAgv->m_ang1 < 1000)
			{
				g_ang = t_LMAng - pAgv->m_ang1;//Ŀǰ�в�ȷ���費��Ҫ 0301
			}
			else if (abs(t_LMAng - pAgv->m_ang2)<1000)//t_LMAng=6650 6160 6650Ҫ�˶��� 6650 7140�������6640 7130
			{
				g_ang = t_LMAng - pAgv->m_ang2;
			}
			else if (abs(t_LMAng - pAgv->m_ang2)>1000)//t_LMAng=100 100 6900Ҫ�˶��� 100 450�������399 7199
			{
				g_ang = t_LMAng + 7200 - pAgv->m_ang2;
			}
		}
		else if (pAgv->m_ang2 < 6700 && t_LMAng - pAgv->m_ang1 >= 450)//t_LMAng=7140 6650 7140Ҫ�˶���430 7140 �������70����10 450 
		{
			g_ang = -(pAgv->m_ang1 + 7200 - t_LMAng);
		}
		else if (pAgv->m_ang2 < 6700 && t_LMAng - pAgv->m_ang1 <= 450)//t_LMAng=30 30 6740�˶���30 520�������20 510�Լ�������ͨ���
		{
			g_ang = t_LMAng - pAgv->m_ang1;
		}
		else
		{
			/*AfxMessageBox("û���ǵ���");*/
		}
		if (f_CorrectionNetState)
		{
			int g_step = g_ang * 2;
			if (g_step <= 1000)//������g_stepӦ��С�ڵ���450*2����100������
			{
				a_ucTemp9[3] = ((g_step >> 24) & 0xFF);
				a_ucTemp9[4] = ((g_step >> 16) & 0xFF);
				a_ucTemp9[5] = ((g_step >> 8) & 0xFF);
				a_ucTemp9[6] = (g_step & 0xFF);
				CaclCRC = CRC712_Add(a_ucTemp9, 7);
				a_ucTemp9[7] = (CaclCRC >> 8) & 0xFF;
				a_ucTemp9[8] = (CaclCRC)& 0xFF;
				if (Com_Send(a_ucTemp9, 9))
				{
					Sleep(1000);
					AnyPositionCount++;
				}
			}
		}
		else
		{
			if (!f_SlopMoveNetOn)//���öԻ���һֱ������
			{
				f_SlopMoveNetOn = true;
				AfxMessageBox("�����ѶϿ�����������������֮�����ƫ��������");
				pAgv->WriteToRichEdit("�����ѶϿ�����������������֮�����ƫ��������");
				Sleep(1000);
				pAgv->StopSlop();
			}
		}
		if (t_LMAng - pAgv->m_ang1 == 0 && (abs(pAgv->m_ang1 + 7200 - pAgv->m_ang2 - everyGap) <= 1 || abs(pAgv->m_ang2 - pAgv->m_ang1 - everyGap) <= 1))
		{
			TempSumAng[ToPositionCount] = pAgv->m_ang2;
			ToPositionCount++;
		}
		else if (t_LMAng - pAgv->m_ang2 == 0 && (abs(pAgv->m_ang1 + 7200 - pAgv->m_ang2 - everyGap) <= 1 || abs(pAgv->m_ang2 - pAgv->m_ang1 - everyGap) <= 1))
		{
			TempSumAng[ToPositionCount] = pAgv->m_ang1;
			ToPositionCount++;
		}
		if (pAgv->m_ang1 == t_LMAng || pAgv->m_ang2 == t_LMAng && (abs(pAgv->m_ang1 + 7200 - pAgv->m_ang2 - everyGap) <= 1 || abs(pAgv->m_ang2 - pAgv->m_ang1 - everyGap) <= 1))
		{
			if (pAgv->m_ang1 == t_LMAng)
				TempCompenAng[TruePoseCount] = pAgv->m_ang2;
			else
				TempCompenAng[TruePoseCount] = pAgv->m_ang1;
			if (TruePoseCount < TargetAngCount - 1)
			{
				TruePoseCount++;
			}
			else
			{
				int ValAng[2] = { 0 };
				getMaxAndMin(TempCompenAng, TargetAngCount, ValAng);
				if (ValAng[0] == ValAng[1])
				{
					if (ValAng[0] != temp_Ang)
					{
						targetAng2 = ValAng[0];
						SuccSlopCount++;
						CString strTemp;
						strTemp.Format("%d", SuccSlopCount);
						strTemp = "��" + strTemp + "������΢���ɹ�,";
						CString str;
						int CurGap = targetAng2 - t_LMAng;
						if (CurGap < 450 && CurGap >0)
						{
							str.Format("%d", CurGap);
						}
						else
						{
							str.Format("%d", targetAng2 + 7200 - t_LMAng);
						}
						pAgv->WriteToRichEdit(strTemp + "�����б�ĽǶȲ�ֵΪ��" + str);
						AnyPositionCount = 0;
						ToPositionCount = 0;
						SumAng2 = 0;
						if (!f_booby)//ƫ������û���
						{
							g_sMeasurePXInfo[n_SerialData].m_OriginalTwo = t_LMAng;
							n_SerialData++;
							f_lastslop = true;
							pAgv->FuncEncoderCorrection();
						}
						else//�������һ�α�־λ��Ϊfalse;ƫ����������ɣ���������������ֵ
						{
							f_lastslop = false;
							g_sMeasurePXInfo[n_SerialData].m_OriginalTwo = t_LMAng;
							n_SerialData++;
							g_sMeasurePXInfo[n_SerialData].m_OriginalTwo = targetAng2;
							g_u32MeasurePXInfoIndex = n_SerialData + 1;
							strTemp = "����ƫ��������ɣ�";
							pAgv->WriteToRichEdit(strTemp);
							for (int i = 0; i < g_u32MeasurePXInfoIndex; i++)
							{
								CString str = "";
								str.Format("%d", g_sMeasurePXInfo[i].m_OriginalTwo);
								str = "ԭʼֵ2��" + str;
								pAgv->WriteToRichEdit(str);
							}
							pAgv->DispTips(IDB_PNG4, "����Ecxel����ƫ�ı�");
							pAgv->WriteToRichEdit("��������Excel���.......");//�������ξͱ����ϵ���ڴ˴� 0306
							AfxBeginThread(ThreadSavePXDataExcel, pAgv);
							return 0;
						}
						temp_Ang = ValAng[0];
					}
				}
				memset(TempCompenAng, 0, TargetAngCount);
				TruePoseCount = 0;
			}
		}
		if (ToPositionCount > 10)
		{
			if (f_CorrectionNetState)
			{
				CString strTemp = "����΢����������10��Ĭ������΢���ɹ���";
				pAgv->WriteToRichEdit(strTemp);
				SuccSlopCount++;
				strTemp.Format("%d", SuccSlopCount);
				strTemp = "��" + strTemp + "������΢���ɹ���";
				CString str;
				for (int i = 0; i < 10; i++)//��С����
				{
					for (int j = i + 1; j < 10; j++)
					{
						if (TempSumAng[i]>TempSumAng[j])
						{
							int temp;        //��ʱ����
							temp = TempSumAng[i];
							TempSumAng[i] = TempSumAng[j];
							TempSumAng[j] = temp;
						}
					}
				}
				SumAng2 = TempSumAng[3] + TempSumAng[4] + TempSumAng[5] + TempSumAng[6];
				targetAng2 = (int)(SumAng2 / 4);
				int CurGap = targetAng2 - t_LMAng;
				if (CurGap < 450 && CurGap>0)
				{
					str.Format("%d", CurGap);
				}
				else
				{
					str.Format("%d", targetAng2 + 7200 - t_LMAng);
				}
				pAgv->WriteToRichEdit(strTemp + "�����б�ĽǶȲ�ֵΪ��" + str);
				AnyPositionCount = 0;
				SumAng2 = 0;
				if (!f_booby)//ƫ������û���
				{
					g_sMeasurePXInfo[n_SerialData].m_OriginalTwo = t_LMAng;
					n_SerialData++;
					f_lastslop = true;
					pAgv->FuncEncoderCorrection();
				}
				else//�������һ�α�־λ��λfalse;ƫ����������ɣ���������������ֵ
				{
					f_lastslop = false;
					g_sMeasurePXInfo[n_SerialData].m_OriginalTwo = t_LMAng;
					n_SerialData++;
					g_sMeasurePXInfo[n_SerialData].m_OriginalTwo =targetAng2;
					g_u32MeasurePXInfoIndex = n_SerialData + 1;
					strTemp = "����ƫ��������ɣ�";
					pAgv->WriteToRichEdit(strTemp);
					for (int i = 0; i < g_u32MeasurePXInfoIndex; i++)
					{
						CString str = "";
						str.Format("%d", g_sMeasurePXInfo[i].m_OriginalTwo);
						str = "ԭʼֵ2��" + str;
						pAgv->WriteToRichEdit(str);
					}
					pAgv->DispTips(IDB_PNG4, "����Ecxel����ƫ�ı�");
					pAgv->WriteToRichEdit("��������Excel���.......");
					AfxBeginThread(ThreadSavePXDataExcel, pAgv);
					return 0;
				}
			}
			TruePoseCount = 0;
			ToPositionCount = 0;
		}
		if (AnyPositionCount >100)//����100�λ����ɹ���֤��������ʶ��б������⣬ֹͣ��������鼤���� 0412��Ϊ50
		{
			pAgv->StopSlop();
			pAgv->WriteToRichEdit("������ʶ��б��쳣��������������");
			AfxMessageBox("������ʶ��б��쳣��������������");
			AnyPositionCount = 0;
		}
	}
	return 0;
}
void AGVTestCfg::FuncEncoderCorrection()
{
	CString str_ang = "";
	if (abs(m_ang2 - m_ang1) <= 450 && abs(m_ang2 - m_ang1) >= 150)
	{
		if (targetAng2 == 0)
		{
			everyGap = m_ang2 - t_LMAng;
			t_LMAng = m_ang2;
		}
		else
		{
			everyGap = targetAng2 - t_LMAng;
			t_LMAng = targetAng2;
			targetAng2 = 0;
		}
	}
	else if (abs(m_ang1 + 7200 - m_ang2) <= 450 && abs(m_ang1 + 7200 - m_ang2) >= 150)
	{
		f_booby = true;//�Ѿ����˵����ڶ���
		if (targetAng2 == 0)
		{
			everyGap = m_ang1+7200 - t_LMAng;
			t_LMAng = m_ang1;
		}
		else
		{
			everyGap = targetAng2 + 7200 - t_LMAng;
			t_LMAng = targetAng2;
			targetAng2 = 0;
		}
	}
	else
	{
		CString str_CalGap;
		str_CalGap.Format("%d,%d", m_ang1, m_ang2);
		WriteToRichEdit("���������������б��ֵ����150-450��Χ��,����ֵΪ��" + str_CalGap);
		StopSlop();
		MessageBox("���������аб����ݻ�ȡ�쳣�������¿�ʼ������");
	}
	if (t_LMAng > 0 && t_LMAng <= 7200)
	{
		str_ang.Format("%d", t_LMAng);
		GetDlgItem(IDC_EDIT_LastAng2)->SetWindowText(str_ang);
	}
	if (FuncEncoderCorrectionCount == 0)
	{
		temp_ang2 = m_ang2;
	}
	else
	{
		if ((m_ang2 - temp_ang2 >= 150 && m_ang2 - temp_ang2 <= 450) || (m_ang2 + 7200 - temp_ang2 >= 150 && m_ang2 + 7200 - temp_ang2 <= 450) || abs(m_ang2 - temp_ang2) < 10)
		{
			temp_ang2 = m_ang2;
		}
		else
		{
			CString str_CalGap;
			str_CalGap.Format("%d,%d", temp_ang2, m_ang2);
			WriteToRichEdit("��������ͳ�������б�2�Ĳ�ֵ���ϸ�,����ֵΪ��" + str_CalGap);
			StopSlop();
			MessageBox("���������аб����ݻ�ȡ�쳣�������¿�ʼ������");
		}
	}
	FuncEncoderCorrectionCount++;
	if (f_AngleCompensation)
	{
		AfxBeginThread(AngleCompensation, this);
		f_AngleCompensation = false;
	}
}


void AGVTestCfg::OnBnClickedBtnClosewisemove()
{
	pMainFrameThis = (CMainFrame*)AfxGetMainWnd();
	if (!m_UartOpenFlag)
	{
		MessageBox("����δ��");
		return;
	}
	CString strTemp = "";
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (Com_Send(a_ucTemp11, 6))
	{
		strTemp = "��ʱ���˶�ָ��ͳɹ�";
	}
	else
	{
		strTemp = "��ʱ���˶�ָ���ʧ��";
	}
	WriteToRichEdit(strTemp, false);
}


void AGVTestCfg::OnBnClickedBtnAnticlockwisemove()
{
	pMainFrameThis = (CMainFrame*)AfxGetMainWnd();
	if (!m_UartOpenFlag)
	{
		MessageBox("����δ��");
		return;
	}
	CString strTemp = "";
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (Com_Send(a_ucTemp10, 6))
	{
		strTemp = "˳ʱ���˶�ָ��ͳɹ�";
	}
	else
	{
		strTemp = "˳ʱ���˶�ָ���ʧ��";
	}
	WriteToRichEdit(strTemp, false);
}
CString FilePathOper = "";
CFile FileOper;
void AGVTestCfg::WriteToRichEdit(CString content, bool r_Kind)//r_Kind 0ֻ����ʾ��richtextbox
{
	CString strLog_Time = CTime::GetCurrentTime().Format(_T("%H:%M:%S ")) + ":";
	m_richlog.SetSel(-1, -1);
	CString strLog = strLog_Time + content + "\r\n";
	m_richlog.ReplaceSel(strLog);
	m_richlog.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	if (r_Kind)
	{
			if (FileOper.m_hFile != CFile::hFileNull)//���FileOperû��close�Ļ����Ȱ���close
			{
				FileOper.Close();
			}
			if (m_SaveFolderPath.GetLength() > 0)
			{
				CString FileName = m_SaveFolderPath + "\\" + fileLaserMAC + ".txt";
				if (FilePathOper != FileName)
				{
					FilePathOper = FileName;
					FileOper.Open(FileName, CFile::modeCreate | CFile::modeWrite | CFile::typeBinary);
				}
				else
				{
					FileOper.Open(FileName, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeBinary);
				}
				FileOper.SeekToEnd();
				FileOper.Write(strLog.GetBuffer(), strLog.GetLength() * sizeof(TCHAR));//strΪCString����
				FileOper.Close();
			}
		//д����־
	}
}

void AGVTestCfg::OnBnClickedBtnStopmove()
{
	pMainFrameThis = (CMainFrame*)AfxGetMainWnd();//����ɾ
	if (!m_UartOpenFlag)
	{
		MessageBox("����δ��");
		return;
	}
	CString strTemp = "";
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (Com_Send(a_ucTemp12, 6))
	{
		strTemp = "����ָֹͣ��ɹ�";
	}
	else
	{
		strTemp = "����ָֹͣ��ʧ��";
	}
	f_MoveZeroCompensation = false;//ֹͣ���㲹���˶�
	f_SlopCompensation = false;//ֹͣƫ�����������˶�
	WriteToRichEdit(strTemp, false);
}



void AGVTestCfg::OnBnClickedBtnSetinivel()
{
	if (!m_UartOpenFlag)
	{
		MessageBox("����δ��");
		return;
	}
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString str = "";
	UINT16 CaclCRC = 0;
	CString strTemp = "";
	GetDlgItem(IDC_EDIT_InitialSpeed)->GetWindowTextA(str);
	m_InitialSpeed = _ttoi(str);
	int speed = m_InitialSpeed;

	a_ucTemp13[3] = ((speed >> 24) & 0xFF);
	a_ucTemp13[4] = ((speed >> 16) & 0xFF);
	a_ucTemp13[5] = ((speed >> 8) & 0xFF);
	a_ucTemp13[6] = (speed & 0xFF);

	CaclCRC = CRC712_Add(a_ucTemp13, 7);

	a_ucTemp13[7] = (CaclCRC >> 8) & 0xFF;
	a_ucTemp13[8] = (CaclCRC)& 0xFF;

	if (Com_Send(a_ucTemp13, 9))
	{
		strTemp = "���ó��ٶ�ָ��ɹ�";
	}
	else
	{
		strTemp = "���ó��ٶ�ָ��ʧ��";
	}
	WriteToRichEdit(strTemp, false);
}


void AGVTestCfg::OnBnClickedBtnSetmovevel()
{
	if (!m_UartOpenFlag)
	{
		MessageBox("����δ��");
		return;
	}
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString str = "";
	UINT16 CaclCRC = 0;
	CString strTemp = "";
	GetDlgItem(IDC_EDIT_WorkingSpeed)->GetWindowTextA(str);
	m_WorkingSpeed = _ttoi(str);
	int speed = m_WorkingSpeed;

	a_ucTemp14[3] = ((speed >> 24) & 0xFF);
	a_ucTemp14[4] = ((speed >> 16) & 0xFF);
	a_ucTemp14[5] = ((speed >> 8) & 0xFF);
	a_ucTemp14[6] = (speed & 0xFF);

	CaclCRC = CRC712_Add(a_ucTemp14, 7);

	a_ucTemp14[7] = (CaclCRC >> 8) & 0xFF;
	a_ucTemp14[8] = (CaclCRC)& 0xFF;

	if (Com_Send(a_ucTemp14, 9))
	{
		strTemp = "���������ٶ�ָ��ɹ�";
	}
	else
	{
		strTemp = "���������ٶ�ָ��ʧ��";
	}
	WriteToRichEdit(strTemp, false);
}


void AGVTestCfg::OnBnClickedBtnQuerycurrstate()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString strTemp = "";
	if (Com_Send(a_ucTemp15, 5))
	{
		strTemp = "���ͻ�ȡ״ָ̬��ɹ�";
		WriteToRichEdit(strTemp);
	}
	else
	{
		strTemp = "���ͻ�ȡ״ָ̬��ʧ��";
		WriteToRichEdit(strTemp);
	}
}
void GetNeedLM(int *val, int len, int *rval)
{
	int cha = 0, chamin = 0;
	cha = val[0] + 7200 - val[len - 1];
	if (cha <= 450 && cha >= 150)
	{
		chamin = cha;
		rval[0] = val[0];
		rval[1] = val[len - 1];
	}
	else//������еİб�֮��û��150-450�Ĳ�ֵ�ͻ��0,1�б굱��������ֵ
	{
		chamin = val[1] - val[0];
		rval[0] = val[0];
		rval[1] = val[1];
	}
	for (int i = 1; i < len - 1; i++)
	{
		cha = abs(val[i + 1] - val[i]);
		if (cha < chamin)
		{
			chamin = cha;
			//abs(cha - Realgap)<=40��ʾ����˼�����������б�ǶȲ�ֵ���ÿ�ζ�ȡ�����Ϊ40
			if ((cha <= 450 && cha >= 150 && abs(cha - Realgap) <= 40) || (cha <= 450 && cha >= 150 && f_Realgap))
			{
				chamin = cha;
				rval[0] = val[i];
				rval[1] = val[i + 1];
			}
		}
	}
	if (rval[1] - rval[0] > 450)
	{
		int a = rval[1] - rval[0];
	}
}
afx_msg LRESULT AGVTestCfg::OnLmangledeal(WPARAM wParam, LPARAM lParam)
{
	char *pcBuf = (char *)wParam;
	ScanLmNum = ((pcBuf[26] & 0xFF) << 8) + (unsigned char)(pcBuf[27]);
	int LmAng[arrayXYsize] = { 0 };
	for (int i = 0; i < ScanLmNum; i++)
	{
		LmAng[i] = ((pcBuf[30 + i * 10] & 0xFF) << 24) + ((pcBuf[31 + i * 10] & 0xFF) << 16) +
			((pcBuf[32 + i * 10] & 0xFF) << 8) + (unsigned char)(pcBuf[33 + i * 10]);
	}
	for (int i = 0; i < ScanLmNum; i++)//��С����
	{
		for (int j = i + 1; j < ScanLmNum; j++)
		{
			if (LmAng[i]>LmAng[j])
			{
				int temp;        //��ʱ����
				temp = LmAng[i];
				LmAng[i] = LmAng[j];
				LmAng[j] = temp;
			}
		}
	}
	int rVal[2] = { 0 };
	GetNeedLM(LmAng, ScanLmNum, rVal);
	m_ang1 = rVal[0];
	m_ang2 = rVal[1];
	if (m_ang2 - m_ang1 <= 450)
		CalGap = m_ang2 - m_ang1;
	else
		CalGap = m_ang1 + 7200 - m_ang2;
	if (ScanLmNum == 5)
	{
		if (CalGap <= 450 && CalGap >= 150)
		{
			if (f_Realgap)//Realgap�Ե�һ����Ϊ�ȶ�Ĭ��ֵ
			{
				Realgap = CalGap;
				f_Realgap = false;
			}
			CString str;
			str.Format("%d", m_ang1);
			GetDlgItem(IDC_EDIT_Ang1)->SetWindowText(str);
			str.Format("%d", m_ang2);
			GetDlgItem(IDC_EDIT_Ang2)->SetWindowText(str);
		}
		else
		{
			if (!f_OpenOrClosePX)//ƫ�������ر�
			{
				CString str_AllLM, str_CalGap;
				str_AllLM.Format("%d,%d,%d,%d,%d", LmAng[0], LmAng[1], LmAng[2], LmAng[3], LmAng[4]);
				WriteToRichEdit("��ǰ����ɨ�赽�İб�ֵΪ��" + str_AllLM);
				str_CalGap.Format("%d", CalGap);
				WriteToRichEdit("�б�Ƕ�ֵ��ȡ�쳣");
			}
			//f_CircleDetOK = false;//360�Ȱб�̽�ⲻͨ��
		}
	}
	else//�б������Ϊ5
	{
		if (ScanLmNum > 0)
		{
			if (!(CalGap <= 450 && CalGap >= 150))//����õ������������б��ֵ����150-450��
			{
				CString str_CalGap;
				str_CalGap.Format("%d", CalGap);
				if (ScanLmNum < 5)
				{
					CString str_ScanLmNum;
					str_ScanLmNum.Format("%d", ScanLmNum);
					CString strTemp = "�б����Ϊ��" + str_ScanLmNum + "���Ƕ�ֵ�ֱ�Ϊ:";
					CString strTemp1 = "";
					CString str_Rec = "";
					for (int i = 0; i < ScanLmNum; i++)
					{
						strTemp1.Format("%d", LmAng[i]);
						str_Rec += " " + strTemp1;
					}
					WriteToRichEdit(strTemp + str_Rec);
				}
				/*WriteToRichEdit("�����õ������б��ֵ����150-450��Χ��,��ֵΪ��" + str_CalGap);*/
			}
			else if (ScanLmNum == 3 || ScanLmNum == 4)
			{
				CString str;
				str.Format("%d", m_ang1);
				GetDlgItem(IDC_EDIT_Ang1)->SetWindowText(str);
				str.Format("%d", m_ang2);
				GetDlgItem(IDC_EDIT_Ang2)->SetWindowText(str);
			}
			else if (ScanLmNum > 5)
			{
				for (int i = 0; i < ScanLmNum; i++)
				{
					for (int j = i + 1; j < ScanLmNum; j++)//��С����
					{
						if (LmAng[i]>LmAng[j])
						{
							int temp;        //��ʱ����
							temp = LmAng[i];
							LmAng[i] = LmAng[j];
							LmAng[j] = temp;
						}
					}
				}
				int QualifiedCount = 0;
				int MaxMinGap = LmAng[0] + 7200 - LmAng[ScanLmNum - 1];
				if (MaxMinGap <= Realgap&&MaxMinGap >= 150)
				{
					QualifiedCount++;
				}
				for (int k = 0; k < ScanLmNum - 1; k++)
				{
					int gap = LmAng[k + 1] - LmAng[k];
					if (gap <= Realgap && gap >= 150)
					{
						QualifiedCount++;
					}
				}
				if (QualifiedCount>1) //|| QualifiedCount == 0
				{
					int FalLMCount = ScanLmNum - 5;
					CString str_FalLMCount;
					str_FalLMCount.Format("%d", FalLMCount);
					WriteToRichEdit("����" + str_FalLMCount + "���ٰУ�");
					f_DetFalseLM = false;
				}
				else
				{
					CString str;
					str.Format("%d", m_ang1);
					GetDlgItem(IDC_EDIT_Ang1)->SetWindowText(str);
					str.Format("%d", m_ang2);
					GetDlgItem(IDC_EDIT_Ang2)->SetWindowText(str);
				}
			}
			else if (ScanLmNum < 3)
			{
				CString str_ScanLmNum;
				str_ScanLmNum.Format("%d", ScanLmNum);
				CString strTemp = "��ǰʶ�𵽵İб����Ϊ��" + str_ScanLmNum;
				WriteToRichEdit(strTemp);
				//f_CircleDetOK = false;//360�Ȱб�̽�ⲻͨ��
			}
		}
	}
	memset(LmAng, 0, arrayXYsize);
	return 0;
}

UINT MoveZero(LPVOID lpParam)
{
	int AnyResultCount = 0, MoveZeroCount = 0, SumAng2 = 0, ZeroCount = 0;
	int TempAng[TargetAngCount] = { 0 };
	int tempSumAngMoveZero[15] = { 0 };
	AGVTestCfg *pAgv = (AGVTestCfg*)lpParam;
	UINT16 CaclCRC = 0;
	int RealMoveZeroSteps = 0;
	while (!f_SuccessZero&& f_MoveZeroCompensation)
	{
		if (pAgv->m_ang1 <= 3600 && pAgv->m_ang2 <= 3600)
		{
			RealMoveZeroSteps = -pAgv->m_ang1 * 2;
		}
		else if (pAgv->m_ang1 <= 450 && pAgv->m_ang2 >= 6700)
		{
			RealMoveZeroSteps = (7200 - pAgv->m_ang2) * 2;
		}
		else if (pAgv->m_ang1 >= 3600 && pAgv->m_ang2 >= 3600)
		{
			RealMoveZeroSteps = (7200 - pAgv->m_ang1) * 2;
		}
		else if (pAgv->m_ang1 <= 3600 && pAgv->m_ang2 >= 3600)
		{
			RealMoveZeroSteps = -pAgv->m_ang1 * 2;
		}
		if (f_CorrectionNetState)//���������������������
		{
			if (RealMoveZeroSteps <= 8000)//һ����಻���߳���200��
			{
				AnyResultCount++;
				a_ucTemp9[3] = ((RealMoveZeroSteps >> 24) & 0xFF);
				a_ucTemp9[4] = ((RealMoveZeroSteps >> 16) & 0xFF);
				a_ucTemp9[5] = ((RealMoveZeroSteps >> 8) & 0xFF);
				a_ucTemp9[6] = (RealMoveZeroSteps & 0xFF);
				CaclCRC = CRC712_Add(a_ucTemp9, 7);
				a_ucTemp9[7] = (CaclCRC >> 8) & 0xFF;
				a_ucTemp9[8] = (CaclCRC)& 0xFF;
				if (Com_Send(a_ucTemp9, 9))
				{
					Sleep(1000);

				}
			}
		}
		else//�������������Ͽ�
		{
			if (!f_MoveZeroNetOn)
			{
				f_MoveZeroNetOn = true;
				pAgv->StopSlop();
				AfxMessageBox("�����ѶϿ������������Ӻ����ƫ��������");
				pAgv->WriteToRichEdit("�����ѶϿ������������Ӻ����ƫ��������");
			}
		}
		if (pAgv->m_ang2 >= 150 && pAgv->m_ang2 <= 450 &&pAgv->m_ang1== 0)
		{
			tempSumAngMoveZero[MoveZeroCount] = pAgv->m_ang2;
			MoveZeroCount++;
		}
		if (MoveZeroCount > 10 && pAgv->m_ang2 <= 450 && pAgv->m_ang2 >= 150)
		{
			CString strTemp = "�����������10��Ĭ�ϻ���ɹ���";
			AnyResultCount = 0;
			for (int i = 0; i < 10; i++)//��С����
			{
				for (int j = i + 1; j < 10; j++)
				{
					if (tempSumAngMoveZero[i]>tempSumAngMoveZero[j])
					{
						int temp;        //��ʱ����
						temp = tempSumAngMoveZero[i];
						tempSumAngMoveZero[i] = tempSumAngMoveZero[j];
						tempSumAngMoveZero[j] = temp;
					}
				}
			}
			SumAng2 = tempSumAngMoveZero[3] + tempSumAngMoveZero[4] + tempSumAngMoveZero[5] + tempSumAngMoveZero[6];
			targetAng2 = (int)(SumAng2 / 4);
			CString str;
			str.Format("%d", targetAng2);
			pAgv->WriteToRichEdit(strTemp + "�����б�ĽǶȲ�ֵΪ��" + str);
			f_AngleCompensation = true;
			f_booby = false;
			if (!f_HandOrAutoMoveZero)
			{
				Sleep(500);
				pAgv->DispTips(IDB_PNG3, "ƫ������ԭʼ���ݲɼ�");
				pAgv->WriteToRichEdit("ƫ������ԭʼ���ݲɼ���......");
				FuncEncoderCorrectionCount = 0;
				if (targetAng2 <= 450 && targetAng2 >= 150)
				{
					pAgv->FuncEncoderCorrection();//��ʼƫ������
				}
				else
				{
					pAgv->StopSlop();
					pAgv->WriteToRichEdit("��㴦���ݲɼ���������������");
					AfxMessageBox("��㴦���ݲɼ���������������");
				}
			}
			ZeroCount = 0;
			MoveZeroCount = 0;
			break;
		}
		if (pAgv->m_ang1 == 0)
		{
			if (pAgv->m_ang2 >= 150 && pAgv->m_ang2 <= 450)
				TempAng[ZeroCount] = pAgv->m_ang2;
			if (ZeroCount < TargetAngCount - 1)
				ZeroCount++;
			else
			{
				int ValAng[2] = { 0 };
				getMaxAndMin(TempAng, TargetAngCount, ValAng);
				if (ValAng[0] == ValAng[1] && ValAng[0] >= 150 && ValAng[0] <= 450)
				{
					targetAng2 = ValAng[0];
					f_SuccessZero = true;
					AnyResultCount = 0;
					CString strTemp = "����ɹ���";
					CString str;
					str.Format("%d", pAgv->m_ang2);
					pAgv->WriteToRichEdit(strTemp + "�����б�ĽǶȲ�ֵΪ��" + str);
					MoveZeroCount = 0;
					f_AngleCompensation = true;
					f_booby = false;
					if (!f_HandOrAutoMoveZero)
					{
						Sleep(1000);
						pAgv->WriteToRichEdit("ƫ������ԭʼ���ݲɼ���......");
						FuncEncoderCorrectionCount = 0;
						pAgv->DispTips(IDB_PNG3, "ƫ������ԭʼ���ݲɼ�");
						if (targetAng2 <= 450 && targetAng2 >= 150)
						{
							pAgv->FuncEncoderCorrection();//��ʼƫ������
						}
						else
						{
							pAgv->StopSlop();
							pAgv->WriteToRichEdit("��㴦���ݲɼ���������������");
							AfxMessageBox("��㴦���ݲɼ���������������");
						}
					}
					memset(TempAng, 0, 5);
					ZeroCount = 0;
					return 0;
				}
				memset(TempAng, 0, 5);
				ZeroCount = 0;
			}
		}
		if (AnyResultCount >= 50)//0604 ���������20�����Ҿ�OK 100�λ����ɹ���˵��������ʶ��б��쳣��ֹͣ���� 0412��Ϊ50�� 
		{
			pAgv->StopSlop();
			pAgv->WriteToRichEdit("������ʶ��б��쳣��������������");
			AfxMessageBox("������ʶ��б��쳣��������������");
			AnyResultCount = 0;
		}
	}
	return 0;
}

void AGVTestCfg::FuncMovezero()
{
	if (m_UartOpenFlag)
	{
		everyGap = 0;
		t_LMAng = 0;
		f_SuccessZero = false;
		f_MoveZeroCompensation = true;//���㲹����־λ��1
		AfxBeginThread(MoveZero, this);
	}
	else
	{
		CString strTemp = "δ�����ϴ��ڣ�";
		WriteToRichEdit(strTemp);
	}
}


void AGVTestCfg::OnBnClickedBtnEncoderCorrect()
{
	pMainFrameThis = (CMainFrame*)AfxGetMainWnd();
	if (!f_CorrectionNetState || !pMainFrameThis->m_NetConnection.bConnected || (!m_UartOpenFlag))
	{
		MessageBox("����δ�򿪻�����û����!");
		return;
	}
	FuncSetIniVel();
	if (f_QualiStandard)
	{
		if (f_DetFalseLM || ScanLmNum == 5)
		{
			if (ScanLmNum >= 5)
			{
				m_LastAng2 = 0;
				pMainFrameThis->KillTimer(0);
				UpdateData(FALSE);
				m_SureSQ.EnableWindow(FALSE);
				m_HanfDownPX.EnableWindow(FALSE);
				m_HandNavTest.EnableWindow(FALSE);
				m_AutoTest.EnableWindow(FALSE);
				m_SlopMove.EnableWindow(FALSE);
				m_GetLMAng.EnableWindow(FALSE);
				m_ReadNAVData.EnableWindow(FALSE);
				CSetFormView *pSetView = (CSetFormView*)pMainFrameThis->m_wndSplitter.GetPane(1, 0);
				f_OpenOrClosePX = false;
				AfxBeginThread(SetPidparamD1, this);//�ر�����ƫ������
				pSetView->m_SetProdDlg.m_btnPIDDn1.EnableWindow(FALSE);//����ƫ���������ذ�ťʹ��
				pSetView->m_SetAppDlg.m_DownApplicPara.EnableWindow(FALSE);//����Ӧ�ò������ذ�ťʹ��
				AfterNAVCount = 1;
				AfxBeginThread(FuncGetLMang, this);//��ȡ�Ƕ���Ϣ
				AfterNAVCount = 2;
				f_HandOrAutoNavVeri = true;//��ʾ�ֶ�������֤
				OnCreateFolder(LaserMAC);
				WriteToRichEdit("���������......");
				FuncMovezero();
			}
			else
			{
				f_QualiStandard = false;
				CString str_ScanLmNum = "";
				str_ScanLmNum.Format("%d", ScanLmNum);
				AfxMessageBox("��������ǰʶ��" + str_ScanLmNum + "���б꣬��ȷ�ϼ������Ƿ�ﵽ��������״̬��");
			}
		}
		else
		{
			if (ScanLmNum > 5)
			{
				CString str_ScanLmNum = "";
				str_ScanLmNum.Format("%d", ScanLmNum - 5);
				AfxMessageBox("����" + str_ScanLmNum + "���ٰУ���ȷ�ϰб�����������޸İб�ʶ����ֵ");
			}
			else
			{
				CString str_ScanLmNum = "";
				str_ScanLmNum.Format("%d", ScanLmNum);
				AfxMessageBox("��������ǰʶ��" + str_ScanLmNum + "���б꣬��ȷ�ϼ������Ƿ�ﵽ��������״̬��");
			}
			f_QualiStandard = false;
			f_DetFalseLM = true;
		}
	}
	else
	{
		AfxMessageBox("��ȷ�Ϻϸ��׼");
	}
}

bool AGVTestCfg::LoadImageFromResource(IN CImage* pImage, IN UINT nResID, IN LPCSTR lpTyp)
{
	if (pImage == NULL)
		return false;

	pImage->Destroy();

	// ������Դ
	HRSRC hRsrc = ::FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(nResID), lpTyp);
	if (hRsrc == NULL) return false;

	// ������Դ
	HGLOBAL hImgData = ::LoadResource(AfxGetResourceHandle(), hRsrc);
	if (hImgData == NULL)
	{
		::FreeResource(hImgData);
		return false;
	}

	// �����ڴ��е�ָ����Դ
	LPVOID lpVoid = ::LockResource(hImgData);

	LPSTREAM pStream = NULL;
	DWORD dwSize = ::SizeofResource(AfxGetResourceHandle(), hRsrc);
	HGLOBAL hNew = ::GlobalAlloc(GHND, dwSize);
	LPBYTE lpByte = (LPBYTE)::GlobalLock(hNew);
	::memcpy(lpByte, lpVoid, dwSize);

	// ����ڴ��е�ָ����Դ
	::GlobalUnlock(hNew);

	// ��ָ���ڴ洴��������
	HRESULT ht = ::CreateStreamOnHGlobal(hNew, TRUE, &pStream);
	if (ht != S_OK)
	{
		GlobalFree(hNew);
	}
	else
	{
		// ����ͼƬ
		pImage->Load(pStream);

		GlobalFree(hNew);
	}

	// �ͷ���Դ
	::FreeResource(hImgData);

	return true;
}
void AGVTestCfg::DispTips(UINT nResID, CString Tips)
{
	CStatic* pWnd = (CStatic*)GetDlgItem(IDC_StepImg); // �õ� Picture Control ���
	CImage image;
	LoadImageFromResource(&image, nResID, _T("PNG"));
	//image.Load(_T("res\\i.png"));
	pWnd->ModifyStyle(0xF, SS_BITMAP | SS_CENTERIMAGE);
	HBITMAP hBmp = image.Detach();
	pWnd->SetBitmap(hBmp);
	GetDlgItem(IDC_STATIC_TIPS)->SetWindowTextA(Tips);
}
void AGVTestCfg::OnBnClickedBtnSlop()
{
	pMainFrameThis = (CMainFrame*)AfxGetMainWnd();
	if (!pMainFrameThis->m_NetConnection.bConnected || (!m_UartOpenFlag))
	{
		MessageBox("����δ�򿪻�����û����!");
		return;
	}
	pMainFrameThis->PostMessage(WM_SENDCMD, LandmarkModeSet_zhilingSize, (LPARAM)LandmarkModeSet_zhiling);//�л����б�̽��
	Sleep(100);
	pMainFrameThis->PostMessage(WM_SENDCMD, PXGetLmIndo_zhilingSize, (LPARAM)PXGetLmIndo_zhiling);//ƫ����������ȡ�б�
	while (true)
	{
		if (abs(t_LMAng - m_ang1) < 7200)
		{
			if (m_ang1 != t_LMAng&&m_ang1 == temp_ang1)
			{
				CString strTemp = "";
				UINT16 CaclCRC = 0;
				int g_ang = 0;
				if (m_ang2 - m_ang1 <= 450)
				{
					g_ang = t_LMAng - m_ang1;
				}
				else
				{
					g_ang = t_LMAng - m_ang2;
				}
				int g_step = g_ang * 2;
				a_ucTemp9[3] = ((g_step >> 24) & 0xFF);
				a_ucTemp9[4] = ((g_step >> 16) & 0xFF);
				a_ucTemp9[5] = ((g_step >> 8) & 0xFF);
				a_ucTemp9[6] = (g_step & 0xFF);
				CaclCRC = CRC712_Add(a_ucTemp9, 7);
				a_ucTemp9[7] = (CaclCRC >> 8) & 0xFF;
				a_ucTemp9[8] = (CaclCRC)& 0xFF;
				if (Com_Send(a_ucTemp9, 9))
				{
					strTemp = "����΢���ɹ�";
					WriteToRichEdit(strTemp);
				}
				else
				{
					strTemp = "����΢��ʧ��";
					WriteToRichEdit(strTemp);
				}
				Sleep(100);
				break;
			}
			else if (m_ang1 == t_LMAng)
			{
				break;
			}
			temp_ang1 = m_ang1;
			Sleep(500);
		}
		continue;
	}
}
void AGVTestCfg::OnBnClickedInterpola()
{
	int g_pxdatax[21] = { 0, 364, 728, 1092, 1457, 1822, 2187, 2552, 2917, 3282, 3647
		, 4012, 4376, 4740, 5104, 5467, 5830, 6193, 6557, 6920, 7283 };
	double g_pxdatay[21] = { 0
		, 0.142857143
		, 0.285714286
		, 0.428571429
		, -0.428571429
		, -1.285714286
		, -2.142857143
		, -3
		, -3.857142857
		, -4.714285714
		, -5.571428571
		, -6.428571429
		, -6.285714286
		, -6.142857143
		, -6
		, -4.857142857
		, -3.714285714
		, -2.571428571
		, -2.428571429
		, -1.285714286
		, -0.142857143
	};
	CFile sfile;
	BOOL pfile;
	int  i;
	static int j = 0; //���int���� yjun
	double dx, dy = 0;
	CString inttostr = " ";
	CString temp;
	double pdx[arrayXYsize] = { 0 };
	double pdy[arrayXYsize] = { 0 };
	((CAxis)((CAxes)m_tchartinterp_px.get_Axis()).get_Bottom()).put_Maximum(7200);
	for (i = 0; i < 21; i++)
	{
		pdx[i] = (double)g_pxdatax[i];
		pdy[i] = g_pxdatay[i];
		((CSeries)m_tchartinterp_px.Series(1)).AddXY(pdx[i], pdy[i], NULL, RGB(255, 0, 0));
		((CSeries)m_tchartinterp_px.Series(0)).AddXY(pdx[i], pdy[i], NULL, RGB(255, 255, 0));
	}

	Interp hmh(pdx, pdy, 21);
	double yval[7200] = { 0 };
	for (i = 0; i <7200; i += 1)
	{
		dx = (double)i;
		if (hmh.GetYByX(dx, dy))
		{
			yval[i] = dy;
			((CSeries)m_tchartinterp_px.Series(0)).AddXY(dx, dy, NULL, RGB(255, 255, 0));
			inttostr.Format("%d\t\t", (INT32)(5 * (INT32)dy));
			temp += inttostr;
			j++;
			if (j > 4)
			{
				temp += "\r\n";
				j = 0;
			}
		}
		else
		{
			MessageBox("����Ĳ�ֵ����Դ����ȷ�����ʧ�ܣ�������ѡ������!");
			PostNcDestroy();
			return;
		}

	}
	double val[2] = { 0 };
	getdouMaxAndMin(yval, 7200, val);
	//�����ļ�
	CString savepath = _T("");
	CFileDialog Open(TRUE, _T("*.bmp"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Files (*.txt_PX)|*.txt_PX|"), NULL); //��_T()�У��������޸ģ����Զ�����Ҫ�򿪵��ļ����͸�ʽ  
	if (Open.DoModal() == IDOK)
	{
		savepath = Open.GetPathName();//����ļ���ȫ·��  
	}

	pfile = sfile.Open(savepath, CFile::modeReadWrite | CFile::modeCreate);
	if (pfile == NULL)
	{
		MessageBox("open file failure");
		return;
	}
	sfile.SeekToEnd();
	sfile.Write(temp, temp.GetLength());
	sfile.Close();
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	//��ͼ
	CExport res = (CExport)m_tchartinterp_px.get_Export();
	res.SaveToBitmapFile("d:\\1.jpg");
}



void AGVTestCfg::OnBnClickedBtnInislop()
{
	pMainFrameThis = (CMainFrame*)AfxGetMainWnd();
	if (!m_UartOpenFlag)
	{
		MessageBox("����δ��");
		return;
	}
	UINT16 CaclCRC = 0;
	CString str = "";
	CString strTemp = "";
	GetDlgItem(IDC_EDIT_SlopSteps)->GetWindowTextA(str);
	m_slopSteps = _ttoi(str);
	int step = m_slopSteps;
	if (m_slopSteps == 0)
	{
		step = 1;
		//UpdateData(FALSE);
		str.Format("%d", step);
		GetDlgItem(IDC_EDIT_SlopSteps)->SetWindowTextA((LPCTSTR)str);
	}
	if (m_slopDir.GetCurSel() == 0)//��ʱ��
	{
		step = -step;
		strTemp = "��ʱ��" + str;
	}
	else if (m_slopDir.GetCurSel() == 1)//˳ʱ��
	{
		strTemp = "˳ʱ��" + str;
	}
	else
	{
		strTemp = "˳ʱ��" + str;
	}
	a_ucTemp9[3] = ((step >> 24) & 0xFF);
	a_ucTemp9[4] = ((step >> 16) & 0xFF);
	a_ucTemp9[5] = ((step >> 8) & 0xFF);
	a_ucTemp9[6] = (step & 0xFF);

	CaclCRC = CRC712_Add(a_ucTemp9, 7);

	a_ucTemp9[7] = (CaclCRC >> 8) & 0xFF;
	a_ucTemp9[8] = (CaclCRC)& 0xFF;
	if (Com_Send(a_ucTemp9, 9))
	{
		strTemp = strTemp + ",����΢��ָ��ɹ�";
		WriteToRichEdit(strTemp, false);
	}
	else
	{
		strTemp = strTemp + ",����΢��ָ��ʧ��";
		WriteToRichEdit(strTemp, false);
	}
}

void AGVTestCfg::OnBnClickedBtnHanddownpxtable()
{
	pMainFrameThis = (CMainFrame*)AfxGetMainWnd();
	if (!f_CorrectionNetState || !pMainFrameThis->m_NetConnection.bConnected)
	{
		MessageBox("����û����!");
		return;
	}
	f_PXAtSi2dCfg = false;//ƫ�ı��������ڸô��������ڱ����ش�
	AfterNAVCount = 0;//MainFrm�в�����ȡִ��OnBnClickedOk�еĴ���
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CFileDialog Open(TRUE, _T("*.txt_PX"), NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Files (*.txt_PX)|*.txt_PX|"), NULL); //��_T()�У��������޸ģ����Զ�����Ҫ�򿪵��ļ����͸�ʽ  
	if (Open.DoModal() == IDOK)
	{
		PXTablePath = Open.GetPathName();//����ļ���ȫ·�� 
		DownloadEccentricityTable(PXTablePath);
	}
}


void AGVTestCfg::OnBnClickedBtnHandnavveri()
{
	pMainFrameThis = (CMainFrame*)AfxGetMainWnd();
	if (!f_CorrectionNetState || !pMainFrameThis->m_NetConnection.bConnected || (!m_UartOpenFlag))
	{
		MessageBox("���ڻ�����δ��");
		return;
	}
	FuncSetIniVel();
	AfterNAVCount = 2;
	f_HandOrAutoNavVeri = true;//��ʾ�ֶ�������֤
	OnBnClickedOk();
}

void AGVTestCfg::OnCreateFolder(CString fileName)
{
	CString path = "";
	GetModuleFileName(NULL, path.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	path.ReleaseBuffer();
	int pos = path.ReverseFind('\\');
	path = path.Left(pos);
	m_SaveFolderPath = path + _T("\\") + "����ƫ���������";
	m_SaveFolderPath = m_SaveFolderPath + _T("\\") + fileName;
	if (PathIsDirectory(m_SaveFolderPath))//����ļ��д��ھ�ɾ����
	{
		RemoveDirectory(m_SaveFolderPath);
	}
	CreateDirectory(m_SaveFolderPath, NULL);//�½�����Ϊ������MAC��ַ���ļ���
}

void AGVTestCfg::OnBnClickedBtnOkqulistandard()
{
	f_PXAtSi2dCfg = false;//ƫ�ı��������ڸô��������ڱ����ش�
	f_AGVTest = true;//MainFrm����AgvDlg����Ϣ
	f_SimuAGV = false;//MainFrm�в���SimuAgvDlg����Ϣ
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	pMainFrameThis = (CMainFrame*)AfxGetMainWnd();
	if (!f_CorrectionNetState || !pMainFrameThis->m_NetConnection.bConnected || (!m_UartOpenFlag))
	{
		MessageBox("����δ�򿪻�����û����!");
		return;
	}
	UpdateData(TRUE);
	///��ȡ�ϸ��׼�����ļ�
	CString lpPath = "";
	CString path = "";
	GetModuleFileName(NULL, path.GetBufferSetLength(MAX_PATH + 1), MAX_PATH);
	path.ReleaseBuffer();
	int pos = path.ReverseFind('\\');
	path = path.Left(pos);
	lpPath = path + _T("\\") + "setup.ini";
	fstream _file;
	_file.open(lpPath, ios::in);
	LPTSTR l_getPositionAccuracyErr = new char[50];
	CString PositionAccuracyErr;
	if (!_file)
	{
		::AfxMessageBox(_T("�����ļ���ʧ�ܡ�"));
	}
	else
	{
		memset(l_getPositionAccuracyErr, 0, sizeof(l_getPositionAccuracyErr));
		GetPrivateProfileString("CONFIG", "PositionAccuracyErr", "", l_getPositionAccuracyErr, 50, lpPath);
		PositionAccuracyErr.Format(_T("%s"), l_getPositionAccuracyErr);
		m_QualiStandard = _ttoi(PositionAccuracyErr);
	}
	QualiStandard = m_QualiStandard;
	UpdateData(FALSE);
	AfterNAVCount = 0;
	f_QualiStandard = true;
	m_richlog.SetSel(0, -1);
	m_richlog.ReplaceSel("");
	GraphForm2d * pView;
	pView = (GraphForm2d*)pMainFrameThis->m_wndLeftSplitter.GetPane(0, 0);
	((CSeries)(pView->m_AGVTestDlg.m_ChartrsTrack.Series(0))).Clear();
	((CSeries)m_tchartinterp_px.Series(0)).Clear();
	((CSeries)m_tchartinterp_px.Series(1)).Clear();
	m_listctrl.DeleteAllItems();//�����һ�λ�ȡ�İб���Ϣ
	CWinThread *pGetLMangThread;
	pGetLMangThread = AfxBeginThread(FuncGetLMang, 0);//�����˰�ťҪȷ�����������Ͱб�Ƕ���Ϣ���Ա���аб���Ϣ�ж�
	WriteToRichEdit("�ϸ��׼�ѵõ�ȷ�ϣ�", false);
}


afx_msg LRESULT AGVTestCfg::OnComParse(WPARAM wParam, LPARAM lParam)
{
	CString strTempState = "";
	int nCmd = (int)wParam;
	switch (nCmd)
	{
	case 0x01:
		strTempState = "�˶���";
		::SendMessageA(this->GetSafeHwnd(), WM_MOTOSTATE, 0, (LPARAM)(LPCTSTR)strTempState);
		break;
	case 0x02:
		strTempState = "ֹͣ";
		::SendMessageA(this->GetSafeHwnd(), WM_MOTOSTATE, 0, (LPARAM)(LPCTSTR)strTempState);
		break;
	}
	return 0;
}


afx_msg LRESULT AGVTestCfg::OnMotostate(WPARAM wParam, LPARAM lParam)
{
	m_CurrentState = (char*)lParam;
	GetDlgItem(IDC_EDIT_CurrentState)->SetWindowTextA(m_CurrentState);
	return 0;
}


void AGVTestCfg::OnBnClickedBtnGetlmang()
{
	CMainFrame *pMainFrameThis;
	pMainFrameThis = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (!f_CorrectionNetState || !pMainFrameThis->m_NetConnection.bConnected || !m_UartOpenFlag)
	{
		MessageBox("���ڻ�����δ��");
		return;
	}
	pMainFrameThis->PostMessage(WM_SENDCMD, LandmarkModeSet_zhilingSize, (LPARAM)LandmarkModeSet_zhiling);//�л����б�̽��
	Sleep(100);
	pMainFrameThis->PostMessage(WM_SENDCMD, PXGetLmIndo_zhilingSize, (LPARAM)PXGetLmIndo_zhiling);//ƫ������
	Sleep(100);
}


afx_msg LRESULT AGVTestCfg::OnGetlmangle(WPARAM wParam, LPARAM lParam)
{
	Sleep(500);
	if (!pMainFrameThis->m_NetConnection.bConnected || (!m_UartOpenFlag))
	{
		MessageBox("���ڻ�����δ��");
		return 0;
	}
	pMainFrameThis->PostMessage(WM_SENDCMD, LandmarkModeSet_zhilingSize, (LPARAM)LandmarkModeSet_zhiling);//�л����б�̽��
	Sleep(1000);
	pMainFrameThis->PostMessage(WM_SENDCMD, PXGetLmIndo_zhilingSize, (LPARAM)PXGetLmIndo_zhiling);//ƫ������
	Sleep(1000);
	WriteToRichEdit("��ȡ�������Ƕ�ָ��ͳɹ���", false);
	return 0;
}


void AGVTestCfg::OnBnClickedBtnCirclelmdet()
{
	//CFile FILETEST;
	//CString strLog_Time = CTime::GetCurrentTime().Format(_T("%H:%M:%S ")) + ":";
	//m_richlog.SetSel(-1, -1);
	//CString strLog = strLog_Time + "123" + "\n";
	//m_richlog.ReplaceSel(strLog);
	//m_richlog.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	//CString FileName = m_SaveFolderPath + "\\" + LaserMAC + ".txt_OPER";
	//FilePathOper = FileName;
	///*b_FileOperOpen = FILETEST.Open(FilePathOper, CFile::modeCreate | CFile::modeWrite);*/
	//FILETEST.Open(FilePathOper, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeBinary);
	//FILETEST.SeekToEnd();
	//FILETEST.Write(strLog.GetBuffer(), strLog.GetLength() * sizeof(TCHAR));//strΪCString����
	//FILETEST.Close();
	////д����־
	//CString strPath = "E:\\100w.txt_OPER";
	//CString strLog_Time = CTime::GetCurrentTime().Format(_T("%H:%M:%S ")) + ":";
	//CString strLog = strLog_Time + "123" + "\n";
	//m_richlog.SetSel(-1, -1);
	//m_richlog.ReplaceSel(strLog);
	//m_richlog.PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	//CFile file;
	//try
	//{
	//	//CFile::modeCreate  ������ʽ���ļ������ļ��Ѵ������䳤������Ϊ0
	//	//CFile::modeNoTruncate �����ļ�ʱ���ļ��Ѵ��ڲ�������нض�
	//	//CFile::modeRead ֻ����ʽ���ļ�
	//	//CFile::modeReadWrite ��д��ʽ���ļ�
	//	//CFile::modeWrite д�뷽ʽ���ļ�
	//	//CFile::typeBinary �����ļ�Ϊ������ģʽ
	//	//CFile::typeText �����ļ�Ϊ�ı�ģʽ
	//	file.Open(strPath, CFile::modeCreate | CFile::modeNoTruncate | CFile::modeWrite | CFile::typeBinary);
	//	//CArchive ar(&file, CArchive::store);//���ݴ򿪵��ļ��������ļ����л�����
	//	//Serialize(ar); //д�ļ�����
	//	//ar.Write(buf, NUM);
	//	////������رն���
	//	//ar.Close();
	//	file.SeekToEnd();
	//	file.Write(strLog.GetBuffer(), strLog.GetLength() * sizeof(TCHAR));//CString m_data
	//	//file.Flush();
	//	file.Close();
	//}
	//catch (CFileException *e)
	//{
	//	CString str;
	//	str.Format("д��ʧ�ܵ�ԭ����:%d", e->m_cause);
	//	//MessageBox("str");
	//	file.Abort();
	//	e->Delete();
	//}
}


void AGVTestCfg::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;
}
