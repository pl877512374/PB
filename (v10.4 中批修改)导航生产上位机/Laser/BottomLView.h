#pragma once


// BottomLView 视图

class BottomLView : public CView
{
	DECLARE_DYNCREATE(BottomLView)

protected:
	BottomLView();           // 动态创建所使用的受保护的构造函数
	virtual ~BottomLView();

public:
	virtual void OnDraw(CDC* pDC);      // 重写以绘制该视图
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


