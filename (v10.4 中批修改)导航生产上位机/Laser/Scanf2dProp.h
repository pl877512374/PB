#pragma once


// CScanf2dProp �Ի���

class CScanf2dProp : public CPropertyPage
{
	DECLARE_DYNAMIC(CScanf2dProp)

public:
	CScanf2dProp();
	virtual ~CScanf2dProp();

// �Ի�������
	enum { IDD = IDD_SCANF2DPROP };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
