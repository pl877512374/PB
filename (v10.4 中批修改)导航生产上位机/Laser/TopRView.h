#pragma once


// TopRView ��ͼ

class TopRView : public CView
{
	DECLARE_DYNCREATE(TopRView)

protected:
	TopRView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~TopRView();

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


