// TopLView.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "TopLView.h"


// TopLView

IMPLEMENT_DYNCREATE(TopLView, CView)

TopLView::TopLView()
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif

}

TopLView::~TopLView()
{
}

BEGIN_MESSAGE_MAP(TopLView, CView)
END_MESSAGE_MAP()


// TopLView 绘图

void TopLView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// TopLView 诊断

#ifdef _DEBUG
void TopLView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void TopLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// TopLView 消息处理程序
