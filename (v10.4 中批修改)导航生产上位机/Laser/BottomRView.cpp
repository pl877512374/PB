// BottomRView.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "BottomRView.h"


// BottomRView

IMPLEMENT_DYNCREATE(BottomRView, CView)

BottomRView::BottomRView()
{

}

BottomRView::~BottomRView()
{
}

BEGIN_MESSAGE_MAP(BottomRView, CView)
END_MESSAGE_MAP()


// BottomRView 绘图

void BottomRView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// BottomRView 诊断

#ifdef _DEBUG
void BottomRView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void BottomRView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// BottomRView 消息处理程序
