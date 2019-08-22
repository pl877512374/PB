#pragma once


// CUartCfg 对话框

class CUartCfg : public CDialogEx
{
	DECLARE_DYNAMIC(CUartCfg)

public:
	CUartCfg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CUartCfg();

// 对话框数据
	enum { IDD = IDD_UARTCFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	double m_fAPDHV;
//	float m_DataHUp;
	float m_fAPDHv;
	float m_fDataHDn;
	float m_fDATAHUp;
	float m_fDataLDn;
	float m_fDataLUp;
	float m_fHumm;
	float m_fLaserHv;
	float m_fSetH;
	float m_fSetL;
	float m_fStart;
	float m_fMotorDa;
	float m_fScanfEnd;
//	CEdit m_fScanfSta;
	float m_fScanfSta;
	afx_msg void OnBnClickedBtnUarten();
	CButton m_btnUartEn;
	afx_msg void OnBnClickedBtnUartclear();
//	CEdit m_fPID_P;
	float m_fPID_P;
//	CEdit m_fPID_I;
	float m_fPID_I;
	float m_fPID_D;
	float m_fZeroPluseNum;
	float m_fUartApdTemper;
	float m_fUartMotorTemper;
};
