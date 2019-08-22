// SIMUAGVTESTDLG.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "SIMUAGVTESTDLG.h"
#include "afxdialogex.h"
#include "UserMsg.h"

#include "CAxis.h"
#include "CAxes.h"
#include "CLegend.h"
#include "CSeries.h"

CString MNChartTip = "";
int MNLastTimestamp = 0;
// CSIMUAGVTESTDLG 对话框

IMPLEMENT_DYNAMIC(CSIMUAGVTESTDLG, CDialogEx)

CSIMUAGVTESTDLG::CSIMUAGVTESTDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CSIMUAGVTESTDLG::IDD, pParent)
{

	

}

CSIMUAGVTESTDLG::~CSIMUAGVTESTDLG()
{
}

void CSIMUAGVTESTDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MNTCHART_TRACK, m_MNChartrsTrack);
}


BEGIN_MESSAGE_MAP(CSIMUAGVTESTDLG, CDialogEx)
	ON_WM_SIZE()
	ON_MESSAGE(WM_SICK_MNPOSEREAD, &CSIMUAGVTESTDLG::OnSickMnposeread)
END_MESSAGE_MAP()


// CSIMUAGVTESTDLG 消息处理程序
BEGIN_EVENTSINK_MAP(CSIMUAGVTESTDLG, CDialogEx)
	ON_EVENT(CSIMUAGVTESTDLG, IDC_MNTCHART_TRACK, 8, CSIMUAGVTESTDLG::OnDblClickMntchartTrack, VTS_NONE)
END_EVENTSINK_MAP()
BEGIN_EASYSIZE_MAP(CSIMUAGVTESTDLG)
	EASYSIZE(IDC_MNTCHART_TRACK, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP

void CSIMUAGVTESTDLG::OnDblClickMntchartTrack()
{
	// TODO:  在此处添加消息处理程序代码
	SimuAGV.ShowWindow(true);
}


BOOL CSIMUAGVTESTDLG::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	INIT_EASYSIZE;
	((CAxis)((CAxes)m_MNChartrsTrack.get_Axis()).get_Bottom()).put_Automatic(true);
	((CAxis)((CAxes)m_MNChartrsTrack.get_Axis()).get_Left()).put_Automatic(true);
	((CSeries)m_MNChartrsTrack.Series(0)).Clear();
	SimuAGV.Create(IDD_SIMUAGVTEST);
	// TODO:  在此添加额外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CSIMUAGVTESTDLG::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	UPDATE_EASYSIZE;
	// TODO:  在此处添加消息处理程序代码
}


afx_msg LRESULT CSIMUAGVTESTDLG::OnSickMnposeread(WPARAM wParam, LPARAM lParam)
{
	char *pcBuf = (char *)wParam;
	int nBufSize = (int)lParam;
	CString SICKTempData = "";
	CString g_SICKData[50];
	int pos = 0;
	int pre_pos = 0;
	int index = 0;
	CString sLog = "";
	int CurrentTimestamp = 0;

	SICKTempData = pcBuf;
	SICKTempData = SICKTempData.Left(nBufSize);
	while (-1 != pos)
	{
		pre_pos = pos;
		pos = SICKTempData.Find(" ", (pos + 1));
		if (-1 == pos)
		{
			g_SICKData[index] = (SICKTempData.Mid(pre_pos + 1, nBufSize - pre_pos - 2));
		}
		else
		{
			g_SICKData[index++] = (SICKTempData.Mid(pre_pos + 1, (pos - pre_pos) - 1));
		}
	}

	if (g_SICKData[0] == "sAN" && g_SICKData[1] == "mNPOSGetPose")
	{
		if (g_SICKData[5] == "1")
		{
			SimuAGV.MNPoseGetCount++;
			SimuAGV.MNPoseX = strtoul(g_SICKData[6], NULL, 16);
			SimuAGV.MNPoseY = strtoul(g_SICKData[7], NULL, 16);
			SimuAGV.MNPoseAngle = strtol(g_SICKData[8], NULL, 16);
			SimuAGV.MNPoseTimestamp = g_SICKData[11];
			CurrentTimestamp = strtol(g_SICKData[11], NULL, 16);
			SimuAGV.UpdateData(FALSE);
			MNChartTip.Format("%d, %d, %d, %d, ", SimuAGV.MNPoseGetCount, SimuAGV.MNPoseX, SimuAGV.MNPoseY, (CurrentTimestamp - MNLastTimestamp));
			MNLastTimestamp = CurrentTimestamp;
			MNChartTip += SimuAGV.MNPoseTimestamp;
			((CSeries)m_MNChartrsTrack.Series(0)).AddXY(SimuAGV.MNPoseX, SimuAGV.MNPoseY, MNChartTip, 0);
			sLog = SICKTempData + "\r\n";
			sLog = sLog + "解析后为：" + MNChartTip;
			if (MNSimulationRun == true)
			{
				SetEvent(Event_MNPOSE);
			}
		}
		else
		{
			SimuAGV.MNInvalidCount++;
			SimuAGV.UpdateData(FALSE);
			sLog = SICKTempData + "\r\n";
			sLog = sLog + "       解析为无效数据" + "\r\n";
			if (MNSimulationRun == true)
			{
				SetEvent(Event_MNNOPOSE);
			}
		}
	}
	SimuAGV.SendMessage(WM_MNWRITELOG, 0, (LPARAM)(LPCTSTR)sLog);     //写入日志
	return 0;
}
