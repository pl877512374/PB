// AGVTestDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "AGVTestDlg.h"
#include "afxdialogex.h"
#include "UserMsg.h"

#include "CAxis.h"
#include "CAxes.h"
#include "CLegend.h"
#include "CSeries.h"

CString ChartTip = "";
int LastTimestamp = 0;
// AGVTestDlg 对话框

IMPLEMENT_DYNAMIC(AGVTestDlg, CDialogEx)

AGVTestDlg::AGVTestDlg(CWnd* pParent /*=NULL*/)
: CDialogEx(AGVTestDlg::IDD, pParent)
//, m_ChartrsTrack(0)
{

}

AGVTestDlg::~AGVTestDlg()
{
}

void AGVTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TCHART_TRACK, m_ChartrsTrack);
}


BEGIN_MESSAGE_MAP(AGVTestDlg, CDialogEx)
	ON_WM_SIZE()
	ON_MESSAGE(WM_SICK_POSEREAD, &AGVTestDlg::OnSickPoseRead)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(AGVTestDlg)
	EASYSIZE(IDC_TCHART_TRACK, ES_BORDER, ES_BORDER, ES_BORDER, ES_BORDER, 0)
END_EASYSIZE_MAP

// AGVTestDlg 消息处理程序

BOOL AGVTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	INIT_EASYSIZE;

	((CAxis)((CAxes)m_ChartrsTrack.get_Axis()).get_Bottom()).put_Automatic(true);
	((CAxis)((CAxes)m_ChartrsTrack.get_Axis()).get_Left()).put_Automatic(true);
	((CSeries)m_ChartrsTrack.Series(0)).Clear();

	m_AGVTestCfg.Create(IDD_AGVTESTCFG);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void AGVTestDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	// TODO: 在此处添加消息处理程序代码
	UPDATE_EASYSIZE;
}

BEGIN_EVENTSINK_MAP(AGVTestDlg, CDialog)
	ON_EVENT(AGVTestDlg, IDC_TCHART_TRACK, 8, AGVTestDlg::OnDblClickTchartTrack, VTS_NONE)
END_EVENTSINK_MAP()

void AGVTestDlg::OnDblClickTchartTrack()
{
	// TODO: 在此处添加消息处理程序代码
	m_AGVTestCfg.ShowWindow(true);
}
int FirCount = 0;//第一次统计100次的时候要舍去前10次的定位数据，此变量用于计前10次的数量
bool f_FirCount = true;//第一次统计100次的时候要舍去前10次的定位数据，此变量用于标位
afx_msg LRESULT AGVTestDlg::OnSickPoseRead(WPARAM wParam, LPARAM lParam)
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
			if (NavHundredCount == 0 && f_FirCount)
			{
				FirCount++;
				if (FirCount > 10)
				{
					FirCount = 0;
					f_FirCount = false;
					if (m_AGVTestCfg.PoseGetCount < 100)
					{
						m_AGVTestCfg.PoseGetCount++;
					}
					else
					{
						m_AGVTestCfg.PoseGetCount = 1;
					}
					m_AGVTestCfg.PoseX = strtoul(g_SICKData[6], NULL, 16);
					NavPoseX[m_AGVTestCfg.PoseGetCount - 1] = m_AGVTestCfg.PoseX;
					m_AGVTestCfg.PoseY = strtoul(g_SICKData[7], NULL, 16);
					NavPoseY[m_AGVTestCfg.PoseGetCount - 1] = m_AGVTestCfg.PoseY;
					m_AGVTestCfg.PoseAngle = strtol(g_SICKData[8], NULL, 16);
					m_AGVTestCfg.PoseTimestamp = g_SICKData[11];
					CurrentTimestamp = strtol(g_SICKData[11], NULL, 16);
					m_AGVTestCfg.UpdateData(FALSE);
					ChartTip.Format("%d, %d, %d, %d, ", m_AGVTestCfg.PoseGetCount, m_AGVTestCfg.PoseX, m_AGVTestCfg.PoseY, (CurrentTimestamp - LastTimestamp));
					LastTimestamp = CurrentTimestamp;
					ChartTip += m_AGVTestCfg.PoseTimestamp;
					((CSeries)m_ChartrsTrack.Series(0)).AddXY(m_AGVTestCfg.PoseX, m_AGVTestCfg.PoseY, ChartTip, 0);
					sLog = SICKTempData + "\n";
					sLog = sLog + "解析后为：" + ChartTip + "\n";
					if (SimulationRun == true)
					{
						SetEvent(Event_POSE);
					}
					if (m_AGVTestCfg.PoseGetCount == 100)
					{
						f_HundredNav = true;
						m_AGVTestCfg.PoseGetCount = 0;
					}
				}
			}
			else
			{
				if (!f_HundredNav)
				{
					if (NavHundredCount != 0)
						f_FirCount = true;
					m_AGVTestCfg.PoseGetCount++;
					m_AGVTestCfg.PoseX = strtoul(g_SICKData[6], NULL, 16);
					NavPoseX[m_AGVTestCfg.PoseGetCount - 1] = m_AGVTestCfg.PoseX;
					m_AGVTestCfg.PoseY = strtoul(g_SICKData[7], NULL, 16);
					NavPoseY[m_AGVTestCfg.PoseGetCount - 1] = m_AGVTestCfg.PoseY;
					m_AGVTestCfg.PoseAngle = strtol(g_SICKData[8], NULL, 16);
					m_AGVTestCfg.PoseTimestamp = g_SICKData[11];
					CurrentTimestamp = strtol(g_SICKData[11], NULL, 16);
					m_AGVTestCfg.UpdateData(FALSE);
					ChartTip.Format("%d, %d, %d, %d, ", m_AGVTestCfg.PoseGetCount, m_AGVTestCfg.PoseX, m_AGVTestCfg.PoseY, (CurrentTimestamp - LastTimestamp));
					LastTimestamp = CurrentTimestamp;
					ChartTip += m_AGVTestCfg.PoseTimestamp;
					((CSeries)m_ChartrsTrack.Series(0)).AddXY(m_AGVTestCfg.PoseX, m_AGVTestCfg.PoseY, ChartTip, 0);
					sLog = SICKTempData + "\n";
					sLog = sLog + "解析后为：" + ChartTip + "\n";
					if (SimulationRun == true)
					{
						SetEvent(Event_POSE);
					}
					if (m_AGVTestCfg.PoseGetCount == 100)
					{
						f_HundredNav = true;
						m_AGVTestCfg.PoseGetCount = 0;
						Sleep(150);
					}
				}
			}
		}
		else
		{
			m_AGVTestCfg.InvalidCount++;
			m_AGVTestCfg.UpdateData(FALSE);
			sLog = SICKTempData + "\n";
			sLog = sLog + "       解析为无效数据" + "\n";
			if (SimulationRun == true)
			{
				SetEvent(Event_NOPOSE);
			}
		}
	}
	m_AGVTestCfg.SendMessage(WM_WRITELOG, 0, (LPARAM)(LPCTSTR)sLog);     //写入日志
	return 0;
}

