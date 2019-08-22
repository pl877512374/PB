// TopRView.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "TopRView.h"


// TopRView

IMPLEMENT_DYNCREATE(TopRView, CView)

TopRView::TopRView()
{

}

TopRView::~TopRView()
{
}

BEGIN_MESSAGE_MAP(TopRView, CView)
END_MESSAGE_MAP()


// TopRView 绘图

void TopRView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// TopRView 诊断

#ifdef _DEBUG
void TopRView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void TopRView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// TopRView 消息处理程序
