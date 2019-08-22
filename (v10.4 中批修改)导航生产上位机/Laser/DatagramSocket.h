#if !defined(AFX_DATAGRAMSOCKET_H__0290CFB3_3462_4B57_8693_C97EF1602E11__INCLUDED_)
#define AFX_DATAGRAMSOCKET_H__0290CFB3_3462_4B57_8693_C97EF1602E11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DatagramSocket.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CDatagramSocket command target

class CDatagramSocket : public CSocket
{
// Attributes
public:

// Operations
public:
	CDatagramSocket(HWND hWnd);
	virtual ~CDatagramSocket();

// Overrides
public:
	HWND m_hWnd;
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDatagramSocket)
	public:
	virtual void OnReceive(int nErrorCode);
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CDatagramSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAGRAMSOCKET_H__0290CFB3_3462_4B57_8693_C97EF1602E11__INCLUDED_)
