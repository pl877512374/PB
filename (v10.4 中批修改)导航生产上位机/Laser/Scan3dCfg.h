#pragma once

// CScan3dCfg �Ի���

class CScan3dCfg : public CDialogEx
{
	DECLARE_DYNAMIC(CScan3dCfg)

public:
	CScan3dCfg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CScan3dCfg();

// �Ի�������
	enum { IDD = IDD_SCANF3DCFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int m_drawflag;
	UINT m_high;
//	UINT m_xmax;
	int m_xmin;
	int m_xmax;
//	UINT m_ymax;
//	int m_ymax;
	UINT m_ymax;
	UINT m_ymin;
	
	
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonDraw();
	int m_n32jiaoduSta;
	int m_n32jiaoduEnd;
};
