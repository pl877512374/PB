#pragma once

#include "NetParam.h"
#include "OperateExcelFile.h"
#include "afxwin.h"
// CScan2dCfg 对话框
extern HANDLE hthread_WriteLog_Sc;
extern DWORD WINAPI ThreadFuct_WriteLog_Sc(void *data);



//日志文件名
extern CString g_strLogfile_Sc;
extern CString g_strMeasureDisfile_Sc;  
//日志文件指针
extern FILE *g_pfLogfile_Sc;  
//extern FILE *g_pfMeasureDisfile_Sc;
extern STRUCT_LOG structLog_Sc;
//当前应用程序目录
extern CString g_strCurDir_Sc;
//日志文件目录
extern CString g_strLogDir_Sc;
class CScan2dCfg : public CDialog
{
	DECLARE_DYNAMIC(CScan2dCfg)

public:
	CScan2dCfg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CScan2dCfg();

// 对话框数据
	enum { IDD = IDD_SCAN2DCFG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
//	BOOL m_dealnegative;
//	BOOL m_mesurectl;
	int m_nangle_num;
	int m_nDrawNum;
	int m_nFrameID;
	int m_nInterval;
//	int m_nMaxDistance;
//	int m_nOneChartDistance;
//	int m_RecvNum;
	int m_nRecvNum;
	int m_nSaveNum;
	int m_nZeroX;
//	int m_nMaxDis;
	int m_nMaxDistance;
	int m_nMaxDis;
	int m_nMinDis;
	int m_mesureinterval;
	int m_nAVGDis;
	int m_nOneChartNum;
	int     m_nNetorFile;
	BOOL m_bSaveFlag;	//记录 当前波形是否需要保存的标志
	CString m_strFiles; //当前文件夹下+NetFile文件夹，用于保存文件
	CString m_strNetFileName;
	CFile m_NetDataFile;
	CString m_sLog;		//记录 显示提示信息时的字符串
	CString m_sFilePath;
	CStringList m_slFileName;
	CString m_sIniFilePath;
	unsigned int m_unFilePlayTimer;
	int m_nCurFileInd;
	void WriteLog(CRichEditCtrl *pREC, CString sLog, unsigned char *pbuf = NULL, unsigned short u16datalen = 0);
	afx_msg bool DrawWave(char *pcDataBuf,int nDataBufSize,int nBufCount);
	afx_msg bool AnalyLWDFile(CString sFileName);
	afx_msg void OnBnClickedButtonSickOneframe();
	afx_msg void OnBnClickedButtonSickcontinueframe();
//	afx_msg void OnEnChangemesureinterval();
	afx_msg void OnBnClickedBtnHtSelect();
	afx_msg void OnEnChangeEditOnechart();
	afx_msg void OnBnClickedCheck2();
	CRichEditCtrl m_recOperLogSc;
//	afx_msg void OnEnChangeEditAnglenum();
	afx_msg void OnBnClickedButtonSavemeasuredis();
	afx_msg void OnEnChangeEditInterval();
	afx_msg void OnEnChangeEditMaxdistance();
	afx_msg void OnEnChangeEditZerox();
	virtual BOOL OnInitDialog();
	CButton m_mesurectl;
	afx_msg void OnBnClickedButtonClear();
	afx_msg void OnBnClickedButtonSavenetdata();
	afx_msg void OnBnClickedBtnHtPrev();
	afx_msg void OnBnClickedBtnHtNext();
	afx_msg void OnBnClickedButtonFilePlay();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void StopTimer(UINT &nTimer);
	afx_msg void StartTimer(UINT &nTimer,UINT nIDEvent,UINT nElapse);
	afx_msg void OnBnClickedButtonOpenOneFrame();
	static DWORD ThreadSaveExcel(LPVOID pParam);
	BOOL m_dealnegative;
	int m_n32LostDataNum;
	IllusionExcelFile  m_OperExcel;
	afx_msg void OnBnClickedButtonRecord();
	afx_msg void OnEnChangeAveragedistance2();
	int m_nRealDisc;
	float m_fjiaodu;
	afx_msg void OnBnClickedButtonRecordDel();
	CButton m_btnRecord;
	CButton m_btnRecordDel;
	CButton m_btnSaveExcel;
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButtonSave();
	int m_editValRef;
	int m_editDispersion;
	float m_ediProbability;
//	afx_msg void OnEnChangeEditDispersion();
	afx_msg void OnEnChangeEditValref();
	CButton m_btnSave;
	int m_nSigDataCmdCnt;
	afx_msg void OnBnClickedButtonClearcnt();
	afx_msg void OnBnClickedCheck20mssigdatacmd();
	CButton m_btn_sick_oneframe;
	CButton m_btn_sick_continueframe;
	int m_JianGe_Point;
	CComboBox m_WorkModeSel;
	afx_msg void OnBnClickedButtonWorkmodeset();
	afx_msg void OnKillfocusRealdisc();
	afx_msg void OnKillfocusEditAnglenum();
	afx_msg void OnKillfocusMesureinterval();
	afx_msg void OnKillfocusEditDispersion();
};
