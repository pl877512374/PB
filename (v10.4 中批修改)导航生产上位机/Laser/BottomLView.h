#pragma once


// BottomLView ��ͼ

class BottomLView : public CView
{
	DECLARE_DYNCREATE(BottomLView)

protected:
	BottomLView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~BottomLView();

public:
	virtual void OnDraw(CDC* pDC);      // ��д�Ի��Ƹ���ͼ
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnConnect();
	afx_msg void OnUpdateConnect(CCmdUI *pCmdUI);
	
	afx_msg void OnLoad();
	afx_msg void OnUpdateLoad(CCmdUI *pCmdUI);
};


