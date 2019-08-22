#pragma once
#include "afxwin.h"
#include "afxcmn.h"
extern bool MNSimulationRun;
extern HANDLE g_MNEventPose;  //获取位置事件
extern CEvent Event_MNPOSE;//获取到可用位置
extern CEvent Event_MNNOPOSE;//获取到不可用位置
// SIMUAGVTEST 对话框

class SIMUAGVTEST : public CDialogEx
{
	DECLARE_DYNAMIC(SIMUAGVTEST)

public:
	SIMUAGVTEST(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~SIMUAGVTEST();

// 对话框数据
	enum { IDD = IDD_SIMUAGVTEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CString LoadFilePath;
	afx_msg void OnBnClickedBtnOpenuart();
	int m_MNUartPort;
	afx_msg void OnBnClickedBtnPidparamD1();
	afx_msg void OnBnClickedBtnBtnopenuart();
	afx_msg void OnBnClickedBtnCloseuart();
	afx_msg void OnBnClickedBtnGoforward();
	CString m_MNLineSpeed;
	CString m_MNAngleSpeed;
	afx_msg void OnBnClickedBtnStopagv();
	afx_msg void OnBnClickedBtnOk();
	short MNSpeedY;
	int MNSpeedAngle;
	int MNSpeedSetInterval;
	int MNPoseGetInterval;
	int MNPoseX;
	int MNPoseY;
	int MNPoseAngle;
	int MNPoseGetCount;
	CString MNPoseTimestamp;
	int MNInvalidCount;
	short MNSpeedX;
	void OnMNCreateFile(CString fileName);  //创建文件
	void OnMNPrepareStartTest(CString TestName);  //开始测试
	void MNWriteToRichEdit(CString content);
	afx_msg void OnBnClickedBtnLoaddata();
	afx_msg void OnBnClickedButtonAutotest();
	CString sMNGoalX;
	CString sMNGoalY;
	afx_msg void OnBnClickedBtnStopautotest();
public:
	virtual BOOL OnInitDialog();
	CComboBox m_ComxbGetGoalsInfoMode;
protected:
//	afx_msg LRESULT OnSickMnposeread(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnMnwritelog(WPARAM wParam, LPARAM lParam);
public:
	CRichEditCtrl m_MNrichlog;
protected:
	afx_msg LRESULT OnMnlminfoview(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnBnClickedBtnStoptest();
	CListCtrl m_MNlistctrl;
	afx_msg void OnEnChangeEditMnlinespeed();
	afx_msg void OnEnChangeEditMnanglespeed();
protected:
	afx_msg LRESULT OnMnlmsettinginfo(WPARAM wParam, LPARAM lParam);
public:
	BOOL m_VelCheck;
	afx_msg void OnCbnSelchangeComboxGetgoalsinfomode();
	afx_msg void OnBnClickedBtnCreatemap();
};
