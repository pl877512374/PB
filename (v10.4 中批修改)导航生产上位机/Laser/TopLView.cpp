// TopLView.cpp : ʵ���ļ�
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


// TopLView ��ͼ

void TopLView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: �ڴ���ӻ��ƴ���
}


// TopLView ���

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


// TopLView ��Ϣ�������
