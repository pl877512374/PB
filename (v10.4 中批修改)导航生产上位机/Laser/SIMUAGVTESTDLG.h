#pragma once
#include "SIMUAGVTEST.h"
#include "EasySize.h"
#include "tchart1.h"
extern bool f_SimuAGV;
// CSIMUAGVTESTDLG �Ի���

class CSIMUAGVTESTDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CSIMUAGVTESTDLG)
	DECLARE_EASYSIZE
public:
	CSIMUAGVTESTDLG(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSIMUAGVTESTDLG();

// �Ի�������
	enum { IDD = IDD_SIMUAGVDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CTchart1 m_MNChartrsTrack;
	SIMUAGVTEST SimuAGV;
	DECLARE_EVENTSINK_MAP()
	void OnDblClickMntchartTrack();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	afx_msg LRESULT OnSickMnposeread(WPARAM wParam, LPARAM lParam);
};
