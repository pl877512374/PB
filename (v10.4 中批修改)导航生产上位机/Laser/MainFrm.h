
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "MySplitter.h"
#include "ControlForm.h"
#include "TopLView.h"
#include "TopRView.h"
#include "BottomLView.h"
#include "BottomRView.h"
#include "GraphForm2d.h"
#include "GraphForm2dState.h"
#include "NetWork\NetworkUtils.h"
#include "BroadCastDlg.h"
#include "SystemLoad.h"
#include "ServerIPDlg.h"
#define WM_NET_RECV (WM_USER+100)
#define WM_NET_INI (WM_USER+101)

extern int g_nRecvNum_scanf;

extern bool g_bConnectting;
extern int g_nConnecttingCount;

extern bool g_bAutoMoniterRunning;
extern bool g_bMoniterFaultedLost;    //监控到丢帧异常的标志
extern bool g_bMoniterFaultedMotor;    //监控到电机异常的标志
extern bool g_bMoniterFaultedWave;    //监控到波形异常的标志
extern int g_nFaultWaveCount;
extern int g_nLostWaveCount;
extern int g_nSendWaveNum;
extern int g_nFaultMSCount;
extern float g_nCurrentMotorSpeed; //当前电机转速

class CMainFrame : public CFrameWndEx
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
	DECLARE_EASYSIZE
// 特性
protected:
	
public:
	CSplitterWnd m_wndSplitter;
	CSplitterWnd m_wndLeftSplitter;
	CMySplitter m_wndSplitter1,m_wndSplitter2;
// 操作
public:

// 重写
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 控件条嵌入成员
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;

// 生成的消息映射函数
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	DECLARE_MESSAGE_MAP()
private:
	bool m_wndSplitter_OK;
public:
	char m_cSendBufData[500];
	int m_nSendBufDataSize;
	int m_test;
	NET_CONNECTION m_NetConnection;
	NET_CONNECTION m_NetConnection7070;
	afx_msg void OnConnect();
	void NetConnect();
	afx_msg void OnUpdateConnect(CCmdUI *pCmdUI);
	afx_msg LRESULT OnRecvNetData(WPARAM wParam,LPARAM lParam);
	afx_msg LONG OnNetSendCmd(WPARAM wParam, LPARAM lParam);       //响应属性页中的查询设备参数命令
	afx_msg LONG OnNetSendCmdTimer(WPARAM wParam, LPARAM lParam);  	
	afx_msg LONG OnConnect7070(WPARAM wParam, LPARAM lParam) ;
	afx_msg void OnClose();
	afx_msg void OnUdpBrod();
	afx_msg void OnUpdateUdpBrod(CCmdUI *pCmdUI);
	BOOL g_bMenuConnectClicked;
	BOOL g_bMenuBroadClicked;
	BOOL g_bMenuLoadClicked;
	CSystemLoad m_sSystemLoadDlg;
	CBroadCastDlg m_sBroadCastDlg;
	CServerIPDlg m_ServerIpDlg;
	afx_msg void OnLoad();
	afx_msg void OnUpdateLoad(CCmdUI *pCmdUI);
	afx_msg long FD_Cal_DataSum(char *pcDataBuf, int nFirstByte);
	void OnTimer(UINT nIDEvent); 
	afx_msg void OnServerip();
	afx_msg LRESULT OnINI(WPARAM wParam,LPARAM lParam);
	CString GetMacByGetAdaptersAddresses(PIP_ADAPTER_INFO pAdapter);
	unsigned short CRC16(unsigned char *puchMsg, unsigned short usDataLen);
	void CheckLicense();
};



