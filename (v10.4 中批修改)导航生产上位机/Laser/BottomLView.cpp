// BottomLView.cpp : ʵ���ļ�
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


// BottomLView ��ͼ

void BottomLView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: �ڴ���ӻ��ƴ���
}


// BottomLView ���

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


// BottomLView ��Ϣ�������


void BottomLView::OnConnect()
{
	// TODO: �ڴ���������������
	MessageBox(_T("hello"));
}


void BottomLView::OnUpdateConnect(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
	pCmdUI->Enable(false);
}


void BottomLView::OnLoad()
{
	// TODO: �ڴ���������������
}


void BottomLView::OnUpdateLoad(CCmdUI *pCmdUI)
{
	// TODO: �ڴ������������û����洦��������
}
