// ControlForm.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "ControlForm.h"


// CControlForm

IMPLEMENT_DYNCREATE(CControlForm, CFormView)

CControlForm::CControlForm()
	: CFormView(CControlForm::IDD)
{

}

CControlForm::~CControlForm()
{
}

void CControlForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CControlForm, CFormView)
END_MESSAGE_MAP()


// CControlForm 诊断

#ifdef _DEBUG
void CControlForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CControlForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CControlForm 消息处理程序
