#pragma once
#include "NetParam.h"

// CBroadCastDlg 对话框

class CBroadCastDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBroadCastDlg)

public:
	CBroadCastDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CBroadCastDlg();

// 对话框数据
	enum { IDD = IDD_BROADDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	//广播线程开关

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnBroad();
	int InitUdpSocket();
	void WriteLog(CRichEditCtrl *pREC, CString sLog, unsigned char *pbuf, unsigned short u16datalen);
	CButton m_btnBroad;
		BOOL m_bBroadcastThread;
	//广播线程（发送接收数据）开关
	BOOL m_bBroadcastSendRecvThread;
	BOOL m_bBroadcastIPRstEn;
	int m_nLocalUdpPort;//记录 发送广播帧时本机udp使用的端口号
	int UpdateIPInfo(int nType,CStringArray &strIpInfo);
	char m_acLocalIp[5];//记录 本地ip地址
	CComboBox m_cmbUdpGetIpAddr;
	CRichEditCtrl m_recOperLog_brst;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBtnIprst();
	CString GetLocalIp(char *acSelfIPBuf);
	void OnTimer(UINT nIDEvent);
	CButton m_btnIPRst;
	CListCtrl m_ctrlIpList;
	afx_msg void OnBnClickedBtnBroad2();
	afx_msg void OnNMRClickListIp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnIp32792();
	CProgressCtrl m_ProCtrlIPRst;
	void UpdateIP(void *lp,u8* Ip,int port);
};
