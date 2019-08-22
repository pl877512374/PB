#pragma once
#include "tchart1.h"
#include "Scan3dCfg.h"

// CScanf3dDlg �Ի���

class CScanf3dDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CScanf3dDlg)

public:
	CScanf3dDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CScanf3dDlg();

// �Ի�������
	enum { IDD = IDD_SCANF3DDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CTchart1 m_draw3d;
	CScan3dCfg m_sScan3dCfgDlg;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_EVENTSINK_MAP()
	void OnDblClickTchart3d();
	virtual BOOL OnInitDialog();
};
