
// LaserView.cpp : CLaserView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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
	// 标准打印命令
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CLaserView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CLaserView 构造/析构

CLaserView::CLaserView()
{
	EnableActiveAccessibility();
	//DragAcceptFiles(TRUE);
	// TODO: 在此处添加构造代码

}

CLaserView::~CLaserView()
{
}

BOOL CLaserView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

// CLaserView 绘制

void CLaserView::OnDraw(CDC* /*pDC*/)
{
	CLaserDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
}


// CLaserView 打印


void CLaserView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CLaserView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 默认准备
	return DoPreparePrinting(pInfo);
}

void CLaserView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加额外的打印前进行的初始化过程
}

void CLaserView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 添加打印后进行的清理过程
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


// CLaserView 诊断

#ifdef _DEBUG
void CLaserView::AssertValid() const
{
	CView::AssertValid();
}

void CLaserView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLaserDoc* CLaserView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLaserDoc)));
	return (CLaserDoc*)m_pDocument;
}
#endif //_DEBUG


// CLaserView 消息处理程序
