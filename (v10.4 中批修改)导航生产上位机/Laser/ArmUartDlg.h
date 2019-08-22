#pragma once


// CArmUartDlg 对话框

class CArmUartDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CArmUartDlg)

public:
	CArmUartDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CArmUartDlg();

// 对话框数据
	enum { IDD = IDD_ARMUARTDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CButton m_btnOpenArmUart;
	CComboBox m_CombBaudArm;
	CComboBox m_CombComArm;
	CEdit m_NetIR;
//	CEdit m_NetRxFsr;
	CEdit m_Net_RxRsr;
	CEdit m_Net_ssr;
	CEdit m_Net_TxFsr;
	HANDLE m_iDevId;
	afx_msg void OnBnClickedButtonOpenarmuart();
	afx_msg void OnBnClickedButtonSearcharmuart();
	virtual BOOL OnInitDialog();
	CEdit m_WireAddr;
	CEdit m_WireData;
	CButton m_btnSearchArmUart;
};
