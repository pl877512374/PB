#pragma once
#include "tchart1.h"
#include "EasySize.h"
#include "AGVTestCfg.h"

// AGVTestDlg �Ի���
extern bool f_FirCount;
class AGVTestDlg : public CDialogEx
{
	DECLARE_DYNAMIC(AGVTestDlg)
	DECLARE_EASYSIZE

public:
	AGVTestDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~AGVTestDlg();
	AGVTestCfg m_AGVTestCfg;

// �Ի�������
	enum { IDD = IDD_AGVTESTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CTchart1 m_ChartrsTrack;
	DECLARE_EVENTSINK_MAP()
	virtual BOOL OnInitDialog();
	void OnDblClickTchartTrack();
protected:
	afx_msg LRESULT OnSickPoseRead(WPARAM wParam, LPARAM lParam);
};
