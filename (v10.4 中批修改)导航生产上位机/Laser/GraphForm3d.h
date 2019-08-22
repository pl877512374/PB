#pragma once
#include "tchart1.h"
#include "EasySize.h"
#include "Scanf3dDlg.h"

// GraphForm3d ������ͼ

class GraphForm3d : public CFormView
{
	DECLARE_DYNCREATE(GraphForm3d)
	DECLARE_EASYSIZE
protected:
	GraphForm3d();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~GraphForm3d();

public:
	enum { IDD = IDD_GRAPHFORM3D };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif
private:
	bool m_bFisrtEasySize;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	//CTchart1 m_draw3d;
	CScanf3dDlg m_scanf3ddlg;
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg LRESULT OnDraw3dChartSc(WPARAM wParam,LPARAM lParam);
	afx_msg void OnTimer(UINT nIDEvent); 
	CTabCtrl m_Tab3dGraphSet;
	virtual void OnInitialUpdate();
};


