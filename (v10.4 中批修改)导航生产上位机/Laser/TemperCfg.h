#pragma once


// CTemperCfg �Ի���

class CTemperCfg : public CDialogEx
{
	DECLARE_DYNAMIC(CTemperCfg)

public:
	CTemperCfg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CTemperCfg();

// �Ի�������
	enum { IDD = IDD_TEMPER_CFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonSearchtemp();
	afx_msg void OnBnClickedButtonCleartemp();
};
