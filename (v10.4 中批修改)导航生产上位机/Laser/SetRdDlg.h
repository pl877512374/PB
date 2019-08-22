#pragma once
#include "tchart1.h"
#include "EasySize.h"

// CSetRdDlg �Ի���

class CSetRdDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetRdDlg)
	DECLARE_EASYSIZE
public:
	CSetRdDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSetRdDlg();

// �Ի�������
	enum { IDD = IDD_SET_RDDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	
	virtual BOOL OnInitDialog();
	CTchart1 m_ChartMotorSpeed;
	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnBnClickedBtnPidstartCheck();
	afx_msg LONG OnMotoSpeedTest(WPARAM wParam, LPARAM lParam);
	afx_msg LONG OnMotoSpeedTestTeeChart(WPARAM wParam, LPARAM lParam);
	CButton m_MotoSpeedTest;
};
