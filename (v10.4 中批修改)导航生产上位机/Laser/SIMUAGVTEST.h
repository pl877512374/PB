#pragma once
#include "afxwin.h"
#include "afxcmn.h"
extern bool MNSimulationRun;
extern HANDLE g_MNEventPose;  //��ȡλ���¼�
extern CEvent Event_MNPOSE;//��ȡ������λ��
extern CEvent Event_MNNOPOSE;//��ȡ��������λ��
// SIMUAGVTEST �Ի���

class SIMUAGVTEST : public CDialogEx
{
	DECLARE_DYNAMIC(SIMUAGVTEST)

public:
	SIMUAGVTEST(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SIMUAGVTEST();

// �Ի�������
	enum { IDD = IDD_SIMUAGVTEST };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

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
	void OnMNCreateFile(CString fileName);  //�����ļ�
	void OnMNPrepareStartTest(CString TestName);  //��ʼ����
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
