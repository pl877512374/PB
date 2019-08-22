#pragma once
#include "EasySize.h"
#include "tchart1.h"
#include "Scan2dCfg.h"

// CScanf2dDlg �Ի���

class CScanf2dDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScanf2dDlg)
	DECLARE_EASYSIZE
public:
	CScanf2dDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CScanf2dDlg();

// �Ի�������
	enum { IDD = IDD_SCANF2DDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnInitDialog();

	CTchart1 m_ChartZhi;
	CTchart1 m_ChartJi;

	CScan2dCfg m_sScan2dCfgDlg;
	//BOOL Xianshi;

	DECLARE_EVENTSINK_MAP()
	void OnDblClickTchartZhi();
	afx_msg LONG OnShow(WPARAM wParam, LPARAM lParam);
	void OnDblClickTchartJi();
	afx_msg void OnDropFiles(HDROP hDropInfo);
};
