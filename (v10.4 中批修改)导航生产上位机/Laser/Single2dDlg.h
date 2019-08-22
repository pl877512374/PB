#pragma once
#include "EasySize.h"
#include "tchart1.h"
#include "Si2dCfg.h"
// CSingle2dDlg �Ի���

class CSingle2dDlg : public CDialog
{
	DECLARE_DYNAMIC(CSingle2dDlg)
	DECLARE_EASYSIZE
public:
	CSingle2dDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CSingle2dDlg();

// �Ի�������
	enum { IDD = IDD_SINGLE2DDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	BOOL m_bFisrtEasySize;
	CTchart1 m_ChartSi;
    CSi2dCfg m_sSi2dCfgDlg;
	DECLARE_EVENTSINK_MAP()
	void OnDblClickTchartSingle();
};
