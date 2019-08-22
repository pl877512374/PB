#pragma once


// CTemperCfg 对话框

class CTemperCfg : public CDialogEx
{
	DECLARE_DYNAMIC(CTemperCfg)

public:
	CTemperCfg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTemperCfg();

// 对话框数据
	enum { IDD = IDD_TEMPER_CFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSearchtemp();
	afx_msg void OnBnClickedButtonCleartemp();
};
