#pragma once
#include "afxwin.h"
#include "afxcmn.h"
#include "CRC16.h"
#include "SetProdDlg.h"
#include "OperateExcelFile.h"
#include "tchart1.h"
#include "SetFormView.h"
#include "Utils\TeeChart\export.h"
void OnSendCommRecv(LPVOID lpParam, BYTE *pDataBuf, int nDataBufSize);
void OnSendCommSendStop(LPVOID lpParam, int nStopType);
extern int AfterNAVCount;
extern bool SimulationRun;
extern bool f_SlopMoveNetOn;
extern bool f_MoveZeroNetOn;
extern bool f_AGVTest;
extern HANDLE g_EventPose;  //获取位置事件
extern CEvent Event_POSE;//获取到可用位置
extern CEvent Event_NOPOSE;//获取到不可用位置
extern bool f_HundredNav;//完成100次导航标志
extern int NavHundredCount;//完成100次导航标志次数
extern int NavPoseX[100];
extern int NavPoseY[100];
extern bool LaserGetPostRuning;
extern bool f_FirDataLMSet;//靶标设置的第一包数据有没有发送成功标志位
extern int pointRatio;
// AGVTestCfg 对话框

class AGVTestCfg : public CDialogEx
{
	DECLARE_DYNAMIC(AGVTestCfg)

public:
	AGVTestCfg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~AGVTestCfg();

// 对话框数据
	enum { IDD = IDD_AGVTESTCFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	short SpeedX;
	short SpeedY;
	int SpeedAngle;
	int SpeedSetInterval;
	int PoseGetInterval;
	int PoseX;
	int PoseY;
	int PoseAngle;
	CString PoseTimestamp;
	CString LoadFilePath;
	afx_msg void OnBnClickedOk();
	int PoseGetCount;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedButtonStoptest();
	void OnTimer(UINT nIDEvent); 
	afx_msg void OnBnClickedButtonLoaddata();
	virtual BOOL OnInitDialog();
	int InvalidCount;
	int m_UartPort;
	CString PXTablePath;
	afx_msg void OnBnClickedButtonOpenuart();
	afx_msg void OnBnClickedButtonCloseuart();
	afx_msg void OnBnClickedButtonGoforward();
	CString m_LineSpeed;
	CString m_AngleSpeed;
	afx_msg void OnBnClickedButtonStopagv();
	afx_msg void OnChangeEditLinespeed();
	afx_msg void OnChangeEditAnglespeed();
	CString sGoalX;
	CString sGoalY;
	afx_msg void OnBnClickedButtonAutotest();
	afx_msg void OnBnClickedButtonStopAutoTest();
	void OnCreateFile(CString fileName,int FileKind);  //创建文件 FileKind为文件类型，0代表记录导航数据，1代表记录操作日志
	void OnPrepareStartTest(CString TestName);  //开始测试
	void OnPrepareStartRecordOperInfo(CString OperName);  //开始测试
	void WriteToRichEdit(CString content, bool rKind=true);
	IllusionExcelFile  m_OperExcel;
	bool CubicSplineInterpolation(int *g_datax,double* g_datay,int g_u32MeasurePXInfoIndex);//三次样条差值
	bool DownloadEccentricityTable(CString TablePath);//下载码盘偏心表
	int SetPXPacket(char index, char* sendpacket, int* datapacket, int len, int num);
	void OnCreateFolder(CString fileName);
	void DispTips(UINT nResID, CString Tips);
	bool LoadImageFromResource(IN CImage* pImage, IN UINT nResID, IN LPCSTR lpTyp);
	void SendOpenPX();//开启偏心修正
	//void SetPidparamD1(int DataContent,int pxState,int pointPercent);
protected:
	afx_msg LRESULT OnWriteLog(WPARAM wParam, LPARAM lParam);
public:
	CComboBox m_CombGetGoalsInfoMode;
	afx_msg void OnSelchangeComboGetgoalsinfomode();
protected:
	afx_msg LRESULT OnLminfoview(WPARAM wParam, LPARAM lParam);
public:
	CListCtrl m_listctrl;
protected:
	afx_msg LRESULT OnLmsettinginfo(WPARAM wParam, LPARAM lParam);
public:
	CRichEditCtrl m_richlog;
	CComboBox m_slopDir;
	void FuncEncoderCorrection();//修正运动
	void FuncMovezero();//回零
	void FuncSetIniVel();//设置电机的初始运行速度500
	void StopSlop();//停止修正
	UINT m_slopSteps;
	int m_ang1;
	int m_ang2;
	afx_msg void OnBnClickedBtnClosewisemove();
	afx_msg void OnBnClickedBtnAnticlockwisemove();
	afx_msg void OnBnClickedBtnStopmove();
	afx_msg void OnBnClickedBtnSetinivel();
	UINT m_InitialSpeed;
	afx_msg void OnBnClickedBtnSetmovevel();
	UINT m_WorkingSpeed;
	afx_msg void OnBnClickedBtnQuerycurrstate();
protected:
	afx_msg LRESULT OnLmangledeal(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedBtnEncoderCorrect();
	afx_msg void OnBnClickedBtnSlop();
//	CString m_lastang2;
	int m_LastAng2;
	afx_msg void OnBnClickedInterpola();
	afx_msg void OnBnClickedBtnInislop();
	int m_FineAdjustCount;
	afx_msg void OnBnClickedBtnHanddownpxtable();
	afx_msg void OnBnClickedBtnHandnavveri();
	CButton m_HandNavTest;
	CButton m_AutoTest;
	CTchart1 m_tchartinterp_px;
	int m_QualiStandard;
	afx_msg void OnBnClickedBtnOkqulistandard();
	CStatic m_PicControl;
	CButton m_StopTest;
protected:
	afx_msg LRESULT OnComParse(WPARAM wParam, LPARAM lParam);
public:
	CString m_CurrentState;
protected:
	afx_msg LRESULT OnMotostate(WPARAM wParam, LPARAM lParam);
public:
	CButton m_HanfDownPX;
	afx_msg void OnBnClickedBtnGetlmang();
	CButton m_GetLMAng;
	CButton m_SlopMove;
protected:
	afx_msg LRESULT OnGetlmangle(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedBtnCirclelmdet();
	CButton m_ReadNAVData;
	CButton m_SureSQ;
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
protected:
};
