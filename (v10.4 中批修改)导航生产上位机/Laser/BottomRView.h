#pragma once


// BottomRView ��ͼ

class BottomRView : public CView
{
	DECLARE_DYNCREATE(BottomRView)

protected:
	BottomRView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~BottomRView();

public:
	virtual void OnDraw(CDC* pDC);      // ��д�Ի��Ƹ���ͼ
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
};


