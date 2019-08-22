#pragma once
#include "NetParam.h"

// CBroadCastDlg �Ի���

class CBroadCastDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CBroadCastDlg)

public:
	CBroadCastDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CBroadCastDlg();

// �Ի�������
	enum { IDD = IDD_BROADDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	//�㲥�߳̿���

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBtnBroad();
	int InitUdpSocket();
	void WriteLog(CRichEditCtrl *pREC, CString sLog, unsigned char *pbuf, unsigned short u16datalen);
	CButton m_btnBroad;
		BOOL m_bBroadcastThread;
	//�㲥�̣߳����ͽ������ݣ�����
	BOOL m_bBroadcastSendRecvThread;
	BOOL m_bBroadcastIPRstEn;
	int m_nLocalUdpPort;//��¼ ���͹㲥֡ʱ����udpʹ�õĶ˿ں�
	int UpdateIPInfo(int nType,CStringArray &strIpInfo);
	char m_acLocalIp[5];//��¼ ����ip��ַ
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
