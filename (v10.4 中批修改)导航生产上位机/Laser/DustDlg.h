#pragma once
#include "afxwin.h"


// CDustDlg 对话框

class CDustDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CDustDlg)

public:
	CDustDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDustDlg();

// 对话框数据
	enum { IDD = IDD_DUSTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	float m_fDustCh1;
	float m_fDustCh10;
	float m_fDustCh2;
	float m_fDustCh3;
	float m_fDustCh4;
	float m_fDustCh5;
	float m_fDustCh6;
	float m_fDustCh7;
	float m_fDustCh8;
	float m_fDustCh9;
	float m_fDust_M;
	CButton m_ChBxDust1;
	CButton m_ChBxDust2;
	CButton m_btnDustSearch;
	CButton m_btnDustInit;
	afx_msg void OnBnClickedBtnDustinit();
	afx_msg void OnBnClickedBtnDustsearch();
	LONG OnDustR(WPARAM wParam, LPARAM lParam);       //响应基本参数查询
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedCheck2();
	CButton m_btn_chn1test;
	CButton m_btn_chn2test;
	CButton m_btn_chn3test;
	CButton m_btn_chn4test;
	CButton m_btn_chn5test;
	CButton m_btn_chn6test;
	afx_msg void OnBnClickedButton4();
	CButton m_btn_DustTest;
	afx_msg void OnBnClickedBtnChn1test();
	afx_msg void OnBnClickedBtnChn2test();
	CButton m_btn_chnltest;
	CButton m_btn_chnrtest;
	afx_msg void OnBnClickedBtnChn3test();
	afx_msg void OnBnClickedBtnChn4test();
	afx_msg void OnBnClickedBtnChn5test();
	afx_msg void OnBnClickedBtnChn6test();
	afx_msg void OnBnClickedBtnChnltest();
	afx_msg void OnBnClickedBtnChnytest();
};
