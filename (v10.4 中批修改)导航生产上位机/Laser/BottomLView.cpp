// BottomLView.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "BottomLView.h"


// BottomLView

IMPLEMENT_DYNCREATE(BottomLView, CView)

BottomLView::BottomLView()
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif

}

BottomLView::~BottomLView()
{
}

BEGIN_MESSAGE_MAP(BottomLView, CView)
	ON_COMMAND(ID_CONNECT, &BottomLView::OnConnect)
	ON_UPDATE_COMMAND_UI(ID_CONNECT, &BottomLView::OnUpdateConnect)
	//ON_COMMAND(ID_LOAD, &BottomLView::OnLoad)
	//ON_UPDATE_COMMAND_UI(ID_LOAD, &BottomLView::OnUpdateLoad)
END_MESSAGE_MAP()


// BottomLView 绘图

void BottomLView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}


// BottomLView 诊断

#ifdef _DEBUG
void BottomLView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void BottomLView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// BottomLView 消息处理程序


void BottomLView::OnConnect()
{
	// TODO: 在此添加命令处理程序代码
	MessageBox(_T("hello"));
}


void BottomLView::OnUpdateConnect(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
	pCmdUI->Enable(false);
}


void BottomLView::OnLoad()
{
	// TODO: 在此添加命令处理程序代码
}


void BottomLView::OnUpdateLoad(CCmdUI *pCmdUI)
{
	// TODO: 在此添加命令更新用户界面处理程序代码
}
