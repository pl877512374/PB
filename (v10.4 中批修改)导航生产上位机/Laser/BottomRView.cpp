// BottomRView.cpp : ʵ���ļ�
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


// BottomRView ��ͼ

void BottomRView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: �ڴ���ӻ��ƴ���
}


// BottomRView ���

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


// BottomRView ��Ϣ�������
