// TopRView.cpp : ʵ���ļ�
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


// TopRView ��ͼ

void TopRView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: �ڴ���ӻ��ƴ���
}


// TopRView ���

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


// TopRView ��Ϣ�������
