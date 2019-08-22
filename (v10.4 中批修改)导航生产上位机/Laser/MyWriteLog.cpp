#include "stdafx.h"
#include "MyWriteLog.h"
#include "Laser.h"
#include "Scan2dCfg.h"
#include "MainFrm.h"
#include "GraphForm2dState.h"
#include "SetFormView.h"
#include "GraphForm3d.h"
StructMyLog g_sMyLog;
CString g_strMyLogfile;
void WriteMyLog(StructLogFile l_sMyLog)
{
	memcpy(&g_sMyLog.m_sLogFile[g_sMyLog.m_u32in],&l_sMyLog,sizeof(StructLogFile));

	g_sMyLog.m_u32in = (g_sMyLog.m_u32in + 1)%MYLOGMAXLEN;
}

DWORD WINAPI ThreadFuct_MyWriteLog(void *data)
{
	CString strTmp; 
	GraphForm2d * pView	;
	CGraphForm2dState *pView1;
	CSetFormView *pSetView;
	//GraphForm3d *pView3d;
	CMainFrame *pMainDlg = (CMainFrame *)data;
	pView = (GraphForm2d*)pMainDlg->m_wndLeftSplitter.GetPane(0,0);
	pView1 = (CGraphForm2dState*) pMainDlg->m_wndSplitter.GetPane(0,0);
	pSetView = (CSetFormView*) pMainDlg->m_wndSplitter.GetPane(1,0);
	//pView3d = (GraphForm3d*) pMainDlg->m_wndSplitter.GetPane(1,0);
	
	//CScan2dCfg *pDlg = (CScan2dCfg *)pView->m_scanf2ddlg.m_sScan2dCfgDlg.GetSafeHwnd();
	
	while(1)
	{
		if(g_sMyLog.m_u32in == g_sMyLog.m_u32out)
		{
			Sleep(10);
			continue;
		}
		CString l_strInfoTxt;
		CString strLog_Time = CTime::GetCurrentTime().Format(_T("%Y-%m-%d %H:%M:%S\r\n"));
		l_strInfoTxt = strLog_Time;
		g_strMyLogfile = CTime::GetCurrentTime().Format(_T("%Y-%m-%d-%H.txt"));

		if (g_sMyLog.m_sLogFile[ g_sMyLog.m_u32out].m_strLog != "")
		{
			l_strInfoTxt += g_sMyLog.m_sLogFile[ g_sMyLog.m_u32out].m_strLog;
			l_strInfoTxt += _T("\r\n");
			/*l_strInfoTxt = "hello\r\n";
			pView->m_scanf2ddlg.m_sScan2dCfgDlgSetSel(-1, -1); 
			((CRichEditCtrl *)(pDlg.GetDlgItem(g_sMyLog.m_sLogFile[ g_sMyLog.m_u32out].m_n32id)))->ReplaceSel(l_strInfoTxt); 
			((CRichEditCtrl *)(pDlg.GetDlgItem(g_sMyLog.m_sLogFile[ g_sMyLog.m_u32out].m_n32id)))->PostMessage(WM_VSCROLL, SB_BOTTOM,0);*/
		}
	}
	return 1;
}



