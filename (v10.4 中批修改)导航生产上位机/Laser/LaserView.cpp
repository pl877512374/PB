
// LaserView.cpp : CLaserView ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "Laser.h"
#endif

#include "LaserDoc.h"
#include "LaserView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLaserView

IMPLEMENT_DYNCREATE(CLaserView, CView)

BEGIN_MESSAGE_MAP(CLaserView, CView)
	// ��׼��ӡ����
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CLaserView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CLaserView ����/����

CLaserView::CLaserView()
{
	EnableActiveAccessibility();
	//DragAcceptFiles(TRUE);
	// TODO: �ڴ˴���ӹ������

}

CLaserView::~CLaserView()
{
}

BOOL CLaserView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	return CView::PreCreateWindow(cs);
}

// CLaserView ����

void CLaserView::OnDraw(CDC* /*pDC*/)
{
	CLaserDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: �ڴ˴�Ϊ����������ӻ��ƴ���
}


// CLaserView ��ӡ


void CLaserView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CLaserView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Ĭ��׼��
	return DoPreparePrinting(pInfo);
}

void CLaserView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӷ���Ĵ�ӡǰ���еĳ�ʼ������
}

void CLaserView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: ��Ӵ�ӡ����е��������
}

void CLaserView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CLaserView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CLaserView ���

#ifdef _DEBUG
void CLaserView::AssertValid() const
{
	CView::AssertValid();
}

void CLaserView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLaserDoc* CLaserView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLaserDoc)));
	return (CLaserDoc*)m_pDocument;
}
#endif //_DEBUG


// CLaserView ��Ϣ�������
