// GraphForm2dState.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "GraphForm2dState.h"
#include "UserMsg.h"
#include "NetParam.h"
#include "MainFrm.h"
#include "File/CDMLogInfo.h"
UINT g_u32HeartCnt = 0;
// CGraphForm2dState

//д��־
CString m_SaveMonitorPath="";
CCDMLogInfo* m_pMyMonitorLog = NULL;
CMainFrame* pMainFrameMonitor;

IMPLEMENT_DYNCREATE(CGraphForm2dState, CFormView)

CGraphForm2dState::CGraphForm2dState()
	: CFormView(CGraphForm2dState::IDD)
{
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif

}

CGraphForm2dState::~CGraphForm2dState()
{
}

void CGraphForm2dState::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS_MOTORTEMPER, m_ProCtrl_MotorTemper);
	DDX_Control(pDX, IDC_PROGRESS_APDTEMPER, m_ProCtrl_APDTemper);
	DDX_Control(pDX, IDC_PROGRESS_APDHV, m_ProCtrl_APDHV);
	DDX_Text(pDX, IDC_EDIT_APDHV, m_fAPDHV);
	DDX_Text(pDX, IDC_EDIT_APDTEMPER, m_fAPDTemper);
	//  DDX_Control(pDX, IDC_EDIT_HUMIDITY, m_nHumm);
	DDX_Text(pDX, IDC_EDIT_MOTORTEMPER, m_fMotorTemper);
	DDX_Text(pDX, IDC_EDIT_LASEREN, m_strLaserEn);
	DDX_Text(pDX, IDC_EDIT_MOTORSTARTEN, m_strMotorEn);
	DDX_Text(pDX, IDC_EDIT_PROGRAMVERSION, m_strProgramVer);
	DDX_Text(pDX, IDC_EDIT_HEARTSTATE, m_strHeartState);
	DDX_Control(pDX, IDC_PROGRESS_NETCONNECT, m_ProCtrl_NetConnect);
	DDX_Text(pDX, IDC_STATIC_NETCONNECT, m_strNetConnect);

	DDX_Text(pDX, IDC_EDIT_PROGRAMVERSIONFPGA, m_strProgramVerFPGA);
	//  DDX_Text(pDX, IDC_EDIT_IWDG_RESTNUM, m_u32IWDGRstNum);
	//  DDX_Text(pDX, IDC_EDIT_IWDG_RESTNUM, m_u32IWDGRstNum);
	//DDX_Text(pDX, IDC_EDIT_IWDG_RESTNUM, m_u32IWDGRstNum);
	//DDX_Text(pDX, IDC_EDIT_PIN_RESTNUM, m_u32PinRstNum);
	//  DDX_Text(pDX, IDC_EDIT_FPGAERR, m_u32FPGA);
	//  DDX_Text(pDX, IDC_EDIT_FPGAERR, m_u32FPGAErr);

	//DDX_Control(pDX, IDC_PROGRESS_NETHEART, m_ProCtrl_NetHeart);
	//  DDX_Text(pDX, IDC_STATIC_NETHEART, m_strNetHeartFlag);
	//DDX_Text(pDX, IDC_EDIT_W5300_RESTNUM, m_u32W5300RstNum);
	//DDX_Text(pDX, IDC_EDIT_DIAODIAN_RESTNUM, m_n32DiaoDianRstNum);
	//DDX_Text(pDX, IDC_EDIT_SOFT_RESTNUM, m_u32SoftRstNum);
	//DDX_Text(pDX, IDC_EDIT_SYSPIBFLASH, m_u32SysPibFlash);
	//DDX_Text(pDX, IDC_EDIT_FACTORFLASH, m_u32FactorFlash);
	//DDX_Text(pDX, IDC_EDIT_5300_RESTNUM_CLOSE, m_u16W5300RstNum_Close);
	//DDX_Text(pDX, IDC_EDIT_5300_RESTNUM_HEART, m_u16W5300RstNum_Heart);
	//DDX_Text(pDX, IDC_EDIT_5300_RESTNUM_SENDOK, m_u16W5300RstNum_SendOk);
	DDX_Control(pDX, IDC_RESET_INFO, m_ResetInfo);
	DDX_Text(pDX, IDC_EDIT_HARDVERSION, m_HardVersion);
	DDX_Control(pDX, IDC_PROGRESS_WIRELESSVOLTAGE, m_ProCtrl_WirelessVoltage);
	DDX_Text(pDX, IDC_EDIT_WIRELESSVOLTAGE, m_fWirelessVoltage);
	DDX_Text(pDX, IDC_EDIT_PROGRAMVERSIONBOOT, m_strProgramVerBOOT);
	DDX_Text(pDX, IDC_EDIT_HEATINGSTATE, m_strHeatingState);
	DDX_Text(pDX, IDC_EDIT__INPUTVOLTAGE, m_fInputVoltage);
	DDX_Control(pDX, IDC_PROGRESS_INPUTVOLTAGE, m_ProCtrl_InputVoltage);
}

BEGIN_MESSAGE_MAP(CGraphForm2dState, CFormView)
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_MESSAGE(WM_MAIN2GR2DST_READ,OnStateQuery)
	ON_MESSAGE(WM_VERSION_READ,OnVersion)
	ON_MESSAGE(WM_SETAPP_RESETNUM_U,OnResetNumQuery)	
	ON_MESSAGE(WM_NETHEART_BEAT,OnHeartQuery)		
	ON_MESSAGE(WM_STATEDLG_STOPTIMER,OnStopTimer)
	ON_MESSAGE(WM_STATEDLG_STARTTIMER,OnStartTimer)
	ON_BN_CLICKED(IDC_BUTTON1, &CGraphForm2dState::OnBnClickedButton1)
	ON_WM_PAINT()
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BTN_CLEAR_RSTNUM, &CGraphForm2dState::OnBnClickedBtnClearRstnum)
	ON_BN_CLICKED(IDC_BUTTON2, &CGraphForm2dState::OnBnClickedButton2)
	
	
	ON_BN_CLICKED(IDC_BTN_SEARCH_RSTNUM, &CGraphForm2dState::OnBnClickedBtnSearchRstnum)
	ON_BN_CLICKED(IDC_BUTTON_AUTOMONITOR, &CGraphForm2dState::OnBnClickedButtonAutomonitor)
	ON_MESSAGE(WM_WRITELOG, &CGraphForm2dState::OnWritelog)
	ON_BN_CLICKED(IDC_BTN_CLEAR_DISPLAY, &CGraphForm2dState::OnBnClickedBtnClearDisplay)
	ON_BN_CLICKED(IDC_BTN_SAVEPROPARAM, &CGraphForm2dState::OnBnClickedBtnSaveproparam)
	ON_BN_CLICKED(IDC_BTN_RECOVERYPARAM, &CGraphForm2dState::OnBnClickedBtnRecoveryparam)
END_MESSAGE_MAP()

BEGIN_EASYSIZE_MAP(CGraphForm2dState)
	
END_EASYSIZE_MAP
// CGraphForm2dState ���

#ifdef _DEBUG
void CGraphForm2dState::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CGraphForm2dState::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CGraphForm2dState ��Ϣ�������

CTime CTimeCur,CTimeStart,CTimeStartSystem;

//
CTimeSpan spansystem;
		CString infosystem;
void CGraphForm2dState::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	INIT_EASYSIZE;
	m_bFisrtEasySize = true;

	m_ProCtrl_APDHV.SetRange(0,40000);

	m_ProCtrl_APDTemper.SetRange(0,14000);

	m_ProCtrl_NetConnect.SetRange(0,100);
	//m_ProCtrl_APDTemper.SetStep(1);

	m_ProCtrl_MotorTemper.SetRange(0,14000);

	m_ProCtrl_WirelessVoltage.SetRange(0,40);

	m_ProCtrl_InputVoltage.SetRange(0,40);

	m_strNetConnect = _T("ͨ��ʧ��");
	m_ProCtrl_NetConnect.SetBarColor(RGB(0,255,0));
		
 
	CTimeStartSystem = CTime::GetCurrentTime();
	UpdateData(false);

	pMainFrameMonitor = (CMainFrame*)AfxGetMainWnd();

	// TODO: �ڴ����ר�ô����/����û���
}

void CGraphForm2dState::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

      if (m_bFisrtEasySize && IsWindowVisible()&& GetWindow(GW_CHILD)!=NULL)
      {
            // INIT_EASYSIZE;
             m_bFisrtEasySize = FALSE;
            // SetScrollSizes(MM_TEXT,CSize(0,0));
             UPDATE_EASYSIZE;
      }else if (!m_bFisrtEasySize)
	 {
             //SetScrollSizes(MM_TEXT,CSize(0,0));
             UPDATE_EASYSIZE;

      }
	// TODO: �ڴ˴������Ϣ����������
}

extern u8 g_u8DrawScFlag;
void CGraphForm2dState::OnTimer(UINT nIDEvent) 
{
	static u8 s_u8ProCtrlStep;
	static u8 s_u8FailCnt;
	static u8 s_u8tmp = 0;
	static u8 s_u8CmdSel;
	switch(nIDEvent)
	{
	case 0://1s��ʱ
		s_u8CmdSel++;
		if(s_u8CmdSel%2 == 0)
		{
			if(((CMainFrame *)(this->GetParent()->GetParent()))->m_NetConnection.bConnected);
			/*AfxGetMainWnd()->PostMessage(WM_SENDCMD_TIMER,g_n32App_QueryStatePibReadLen, (LPARAM)g_acApp_QueryStatePibReadFrameBuf);*/
		}
		else
		{
			if(((CMainFrame *)(this->GetParent()->GetParent()))->m_NetConnection.bConnected);
			//AfxGetMainWnd()->PostMessage(WM_SENDCMD_TIMER,nInqResetNumBufLength, (LPARAM)acInqResetNumBuf);
		}
		break;
	case 1://500ms��ʱ
		if(s_u8tmp != g_u8NetStateCnt)
		{
			if(s_u8ProCtrlStep < 100)
				s_u8ProCtrlStep+=25;
			else
				s_u8ProCtrlStep = 0;
			m_ProCtrl_NetConnect.SetPos(s_u8ProCtrlStep);
			m_strNetConnect = _T("ͨ������");
			s_u8FailCnt = 0;
		}
		else
		{
			s_u8FailCnt++;
			if(s_u8FailCnt >= 20)
			{
				m_strNetConnect = _T("ͨ��ʧ��");
				s_u8FailCnt = 0;
				f_CorrectionNetState = false;
				f_SlopMoveNetOn = false;
				f_MoveZeroNetOn = false;
			}
		}
			
		s_u8tmp = g_u8NetStateCnt;
		UpdateData(false);
		break;
	case 2://����//1s��ʱ
		
		if(m_strHeartState == "����")
		{
			//if(((CMainFrame *)(this->GetParent()->GetParent()))->m_NetConnection.bConnected)
				AfxGetMainWnd()->PostMessage(WM_SENDCMD_TIMER,acBeatFrameLength, (LPARAM)acBeatFrameBuf);
				if(g_u8DrawScFlag == 0)
				{
					Sleep(100);
					//AfxGetMainWnd()->SendMessage(WM_SENDCMD, nGetSickContinueFrameBufSize, (LPARAM)acGetSickContinueFrameBuf);
				}
			//if(!((CMainFrame *)(this->GetParent()->GetParent()))->m_NetConnection.bConnected)
			{
				g_u32HeartCnt++;
				if(g_u32HeartCnt >= 10)
				{
					printf("������\n");
					((CMainFrame *)(this->GetParent()->GetParent()))->NetConnect();
					if(g_u8DrawScFlag != 0xff)
					{
						g_u8DrawScFlag = 0;
					}
					else
						g_u8DrawScFlag = 0xff;
					g_u32HeartCnt = 0;
					Sleep(100);
					//AfxGetMainWnd()->SendMessage(WM_SENDCMD, nGetSickContinueFrameBufSize, (LPARAM)acGetSickContinueFrameBuf);	
					//GetDlgItem(IDC_BUTTON_SICKCONTINUEFRAME)->SetWindowText("ֹͣ��ȡ");	
				}
			}
		}
		break;
	case 3://1s��ʱ
		CTimeCur = CTime::GetCurrentTime();
		spansystem = CTimeCur - CTimeStartSystem;
		infosystem = spansystem.Format("%D %H:%M:%S");
		UpdateData(true);
		GetDlgItem(IDC_TIMECNTSYSTEM)->SetWindowText(infosystem);
		 if(((CMainFrame *)(this->GetParent()->GetParent()))->m_NetConnection.bConnected)
		 {
			 if(m_strNetConnect == _T("ͨ��ʧ��"))
			 {
				 KillTimer(0);
			 }
		 }
		 UpdateData(false);
	default:
		break;
	}

}

//����״̬
LONG CGraphForm2dState::OnStateQuery(WPARAM wParam, LPARAM lParam)       //��Ӧ����ҳ�еĲ�ѯ�豸��������
{

	//������ͳһ������100����ϵ
	//���ڴ���ʱ�Ѿ�������100��������ﲻ��Ҫ����
	static u8 test = 0;
	char *pcBuf = (char *)wParam;
	int nBufSize = (int)lParam;
	u16 l_u16len;//���ݲ��ֳ��ȣ���ȥǰ2���ֽ�,֡ͷ
	l_u16len = nBufSize - 2;
	u16  checktype = 1;  // У������:0 ��У�� 1 ���У�� 2 CRCУ��
	u16  checkres=0;
	u8 *pu8buf = new u8[nBufSize];
	memcpy(pu8buf,&pcBuf[2],l_u16len);
	checktype = pu8buf[8];
	CString sLog="";

	switch(checktype) { // У�����
			case 0: checkres = TRUE; break;
		  case 1: checkres = CHECK_BCC(&pu8buf[0], l_u16len); break;
			//case 2: checkres = CHECK_CRC(&buf[2], l_u16len); break;
			default: checkres = FALSE; break;
			}
	if (!checkres)
	{
		return 0;
	}
	UpdateData(true);
	
	//���ƽ�������Ϣ
	if(((pu8buf[29]<<8)+(pu8buf[30]))/100.0 >= 250.0 || ((pu8buf[29]<<8)+(pu8buf[30]))/100.0 <= 40.0)
	{
		m_ProCtrl_APDHV.SetBarColor(RGB(255,0,0));
	}
	else
	{
		m_ProCtrl_APDHV.SetBarColor(RGB(0,255,0));
	}
	m_ProCtrl_APDHV.SetPos(((pu8buf[29]<<8)+(pu8buf[30])));
	m_fAPDHV = ((pu8buf[29]<<8)+(pu8buf[30]))/100.0;

	short int tmp= (short int)((pu8buf[31]<<8)+(pu8buf[32]));
	if(tmp/100.0 >= 85.0 || tmp/100.0 <= -40.0)
	{
		m_ProCtrl_APDTemper.SetBarColor(RGB(255,0,0));
	}
	else
	{
		m_ProCtrl_APDTemper.SetBarColor(RGB(0,255,0));
	}
	m_ProCtrl_APDTemper.SetPos(tmp+4000);
	m_fAPDTemper =tmp/100.0;

	tmp= (short int)((pu8buf[27]<<8)+(pu8buf[28]));
	if(tmp/100.0 >= 100.0 || tmp/100.0 <= -40.0)
	{
		m_ProCtrl_MotorTemper.SetBarColor(RGB(255,0,0));
	}
	else
	{
		m_ProCtrl_MotorTemper.SetBarColor(RGB(0,255,0));
	}
	m_ProCtrl_MotorTemper.SetPos(tmp+4000);	
	m_fMotorTemper = tmp/100.0;

	unsigned short int utmp= (unsigned short int)((pu8buf[43]<<8)+(pu8buf[44]));
	m_fWirelessVoltage = utmp/1000.00;
	if(m_fWirelessVoltage >= 25 || m_fWirelessVoltage <= 15)
	{
		m_ProCtrl_WirelessVoltage.SetBarColor(RGB(255,0,0));
	}
	else
	{
		m_ProCtrl_WirelessVoltage.SetBarColor(RGB(0,255,0));
	}
	m_ProCtrl_WirelessVoltage.SetPos(m_fWirelessVoltage);	
	
	utmp= (unsigned short int)((pu8buf[45]<<8)+(pu8buf[46]));
	m_fInputVoltage = utmp/1000.00;
	if(m_fInputVoltage >= 36 || m_fInputVoltage <= 9)
	{
		m_ProCtrl_InputVoltage.SetBarColor(RGB(255,0,0));
	}
	else
	{
		m_ProCtrl_InputVoltage.SetBarColor(RGB(0,255,0));
	}
	m_ProCtrl_InputVoltage.SetPos(m_fInputVoltage);	

	//����״̬��Ϣ
	m_strMotorEn =  ((pu8buf[24] == 1) ?"����":"δ����");
	m_strLaserEn = ((pu8buf[25] == 1) ?"����":"δ����");
	m_strHeartState = ((pu8buf[26] == 1) ?"����":"δ����");	
	m_strHeatingState = ((pu8buf[47] == 1) ?"����":"δ����");	

	//m_u32SysPibFlash = pu16Buf[13];
	//m_u32FactorFlash = pu16Buf[14];

	if ((g_bAutoMoniterRunning == true) && (g_bMoniterFaultedWave == true || (g_bMoniterFaultedMotor == true)))
	{
		sLog.Format("���ֹ��ϣ���֡����%d�������쳣����%d�����ת���쳣����%d, ��ǰ���ת��%.2f��APD��ѹ%.2f��APD�¶�%.2f������¶�%.2f�����߹���%.2f�������ѹ%.2f", g_nLostWaveCount, g_nFaultWaveCount, g_nFaultMSCount,g_nCurrentMotorSpeed,m_fAPDHV,m_fAPDTemper,m_fMotorTemper,m_fWirelessVoltage,m_ProCtrl_InputVoltage);
		SendMessage(WM_WRITELOG,0,(LPARAM)(LPCTSTR)sLog);
	}

	UpdateData(false);
	/*pu16Buf = NULL;*/
	delete []pu8buf;
	/*delete pu16Buf;*/
	return 1;
}

//Ӳ�����汾��
LONG CGraphForm2dState::OnVersion(WPARAM wParam, LPARAM lParam)
{
	char *pcBuf = (char *)wParam;
	int nBufSize = (int)lParam;
	u16 l_u16len;//���ݲ��ֳ��ȣ���ȥǰ2���ֽ�,֡ͷ
	l_u16len = nBufSize - 2;
	u16  checktype = 1;  // У������:0 ��У�� 1 ���У�� 2 CRCУ��
	u16  checkres=0;
	u8 *pu8buf = new u8[nBufSize];
	memcpy(pu8buf,&pcBuf[2],l_u16len);
	checktype = pu8buf[8];
	switch(checktype) 
	{ // У�����
		case 0: checkres = TRUE; break;
		case 1: checkres = CHECK_BCC(&pu8buf[0], l_u16len); break;
		//case 2: checkres = CHECK_CRC(&buf[2], l_u16len); break;
		default: checkres = FALSE; break;
	}
	if (!checkres)
	{
		return 0;
	}
	m_HardVersion="";
	m_strProgramVer="";
	m_strProgramVerFPGA="";
	m_strProgramVerBOOT="";
	CString strtmp;
	
	//Ӳ���汾��
	for (u8 l_u8i = 24;l_u8i < 44; l_u8i++)
	{
		strtmp.Format("%c",pu8buf[l_u8i]);
		m_HardVersion += strtmp;
	}
	//ARM����汾��
	for (u8 l_u8i = 44;l_u8i < 69; l_u8i++)
	{
		strtmp.Format("%c",pu8buf[l_u8i]);
		m_strProgramVer += strtmp;
	}
	//FPGA����汾��
	for (u8 l_u8i = 69;l_u8i < 94; l_u8i++)
	{
		strtmp.Format("%c",pu8buf[l_u8i]);
		m_strProgramVerFPGA += strtmp;
	}
	//BOOT����汾��
	for (u8 l_u8i = 94;l_u8i < 119; l_u8i++)
	{
		strtmp.Format("%c",pu8buf[l_u8i]);
		m_strProgramVerBOOT += strtmp;
	}
	UpdateData(false);
	delete []pu8buf;
}

LONG CGraphForm2dState::OnHeartQuery(WPARAM wParam, LPARAM lParam) 
{
	static u8 s_HeartCnt;
	if(s_HeartCnt >= 20)
	{
		s_HeartCnt = 0;		
	}
	else
	{
		s_HeartCnt++;
	}
	//m_ProCtrl_NetHeart.SetPos(s_HeartCnt);
	g_u32HeartCnt = 0;
	return 1;
}

//��ѯ��λ
LONG CGraphForm2dState::OnResetNumQuery(WPARAM wParam, LPARAM lParam) 
{
	char *pcBuf = (char *)wParam;
	int nBufSize = (int)lParam;
	u16 l_u16len;//���ݲ��ֳ��ȣ���ȥǰ2���ֽ�,֡ͷ
	l_u16len = nBufSize - 2;
	u16  checktype = 1;  // У������:0 ��У�� 1 ���У�� 2 CRCУ��
	u16  checkres=0;
	u8 *pu8buf = new u8[nBufSize];
	memcpy(pu8buf,&pcBuf[2],l_u16len);
	checktype = pu8buf[8];
	switch(checktype) { // У�����
			case 0: checkres = TRUE; break;
		  case 1: checkres = CHECK_BCC(&pu8buf[0], l_u16len); break;
			//case 2: checkres = CHECK_CRC(&buf[2], l_u16len); break;
			default: checkres = FALSE; break;
			}
	if (!checkres)
	{
		return 0;
	}
	CRichEditCtrl *p_rich = (CRichEditCtrl *)GetDlgItem(IDC_RESET_INFO);
	p_rich->SetSel(0,-1);
	p_rich->Clear();

	CString TimesTxt = "";
	CString RestartInfoTxt="";
	TimesTxt.Format(_T("%d"), ((pu8buf[24]<<8)+pu8buf[25]));
	RestartInfoTxt="SOFT��λ������"+TimesTxt+"\r\n";
	p_rich->SetSel(-1, -1);	
	p_rich->ReplaceSel(RestartInfoTxt);

	TimesTxt.Format(_T("%d"), ((pu8buf[26]<<8)+pu8buf[27]));
	RestartInfoTxt="IWDG��λ������"+TimesTxt+"\r\n";
	p_rich->SetSel(-1, -1);	
	p_rich->ReplaceSel(RestartInfoTxt);

	TimesTxt.Format(_T("%d"), ((pu8buf[28]<<8)+pu8buf[29]));
	RestartInfoTxt="WWDG��λ������"+TimesTxt+"\r\n";
	p_rich->SetSel(-1, -1);	
	p_rich->ReplaceSel(RestartInfoTxt);

	TimesTxt.Format(_T("%d"), ((pu8buf[30]<<8)+pu8buf[31]));
	RestartInfoTxt="POR��λ������"+TimesTxt+"\r\n";
	p_rich->SetSel(-1, -1);	
	p_rich->ReplaceSel(RestartInfoTxt);

	TimesTxt.Format(_T("%d"), ((pu8buf[32]<<8)+pu8buf[33]));
	RestartInfoTxt="NRST��λ������"+TimesTxt+"\r\n";
	p_rich->SetSel(-1, -1);	
	p_rich->ReplaceSel(RestartInfoTxt);

	TimesTxt.Format(_T("%d"), ((pu8buf[34]<<8)+pu8buf[35]));
	RestartInfoTxt="W5500��λ������"+TimesTxt+"\r\n";
	p_rich->SetSel(-1, -1);	
	p_rich->ReplaceSel(RestartInfoTxt);

	TimesTxt.Format(_T("%d"), ((pu8buf[36]<<8)+pu8buf[37]));
	RestartInfoTxt="W5500������λ������"+TimesTxt+"\r\n";
	p_rich->SetSel(-1, -1);	
	p_rich->ReplaceSel(RestartInfoTxt);

	TimesTxt.Format(_T("%d"), ((pu8buf[38]<<8)+pu8buf[39]));
	RestartInfoTxt="W5500�رն˿ڴ�����"+TimesTxt+"\r\n";
	p_rich->SetSel(-1, -1);	
	p_rich->ReplaceSel(RestartInfoTxt);

	p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	return 1;
	delete []pu8buf;
}

void CGraphForm2dState::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	AfxGetMainWnd()->PostMessage(WM_SENDCMD,g_n32App_QueryStatePibReadLen, (LPARAM)g_acApp_QueryStatePibReadFrameBuf);
}

LONG CGraphForm2dState::OnStopTimer(WPARAM wParam, LPARAM lParam) 
{
	KillTimer(0);
	return 1;
}

LONG CGraphForm2dState::OnStartTimer(WPARAM wParam, LPARAM lParam) 
{
	SetTimer(0,1000,0);
	return 1;
}

void CGraphForm2dState::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CFormView::OnPaint()
        //CRect   myrect;   
        //GetClientRect(&myrect);   
        //CDC   dcMem;   
        //dcMem.CreateCompatibleDC(&dc);   
        //CBitmap   bmpBackground;   
        //bmpBackground.LoadBitmap(IDB_BITMAP2);   //IDB_BITMAP_TOOL�����Լ���ͼ��Ӧ��ID 
        //BITMAP   bitmap;   
        //bmpBackground.GetBitmap(&
        //bitmap);   
        //CBitmap   *pbmpOld=dcMem.SelectObject(&bmpBackground);   
        //dc.StretchBlt(0,0,myrect.Width(),myrect.Height(),&dcMem,0,0,   
        //bitmap.bmWidth,bitmap.bmHeight,SRCCOPY); 
}

HBRUSH CGraphForm2dState::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
 //
 //if(nCtlColor == CTLCOLOR_STATIC )
 //{
 // pDC->SetBkMode(TRANSPARENT);//���ñ���͸��
 // pDC->SetTextColor(RGB(255,255,0));//��������Ϊ��ɫ
 // return (HBRUSH)::GetStockObject(NULL_BRUSH);
 //} 
	// TODO:  �ڴ˸��� DC ���κ�����

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}

//�����λ
void CGraphForm2dState::OnBnClickedBtnClearRstnum()
{
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x04,0x04};
	ZhiLing(Restar_zhiling,pn,0);
	AfxGetMainWnd()->PostMessage(WM_SENDCMD, Restar_zhilingSize, (LPARAM)Restar_zhiling);	
}

void CGraphForm2dState::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CTimeStartSystem = CTime::GetCurrentTime();
}

void CGraphForm2dState::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}

void CGraphForm2dState::OnBnClickedBtnClearRstnum2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	AfxGetMainWnd()->PostMessage(WM_SENDCMD, nSystemFlashRestLength, (LPARAM)acSystemFlashResetNumBuf);	
}

void CGraphForm2dState::OnBnClickedBtnClearRstnum3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	AfxGetMainWnd()->PostMessage(WM_SENDCMD, nFactorFlashLength, (LPARAM)acFactorFlashResetNumBuf);	
}

//��ѯ��λ��Ϣ
void CGraphForm2dState::OnBnClickedBtnSearchRstnum()
{
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x04,0x03};
	ZhiLing(Restar_zhiling,pn,0);
	AfxGetMainWnd()->PostMessage(WM_SENDCMD, Restar_zhilingSize, (LPARAM)Restar_zhiling);
}

//�����������
void CGraphForm2dState::OnBnClickedBtnSaveproparam()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x1B};
	ZhiLing(Restar_zhiling,pn,0);
	AfxGetMainWnd()->PostMessage(WM_SENDCMD, Restar_zhilingSize, (LPARAM)Restar_zhiling);
}

//�ָ���������
void CGraphForm2dState::OnBnClickedBtnRecoveryparam()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x06,0x18};
	ZhiLing(Restar_zhiling,pn,0);
	AfxGetMainWnd()->PostMessage(WM_SENDCMD, Restar_zhilingSize, (LPARAM)Restar_zhiling);
}

void CGraphForm2dState::OnCreateMonitorFile(CString fileName)  //�����ļ�
{
	CFile file;
	CString path;

	//�����ļ���
	GetModuleFileName(NULL,path.GetBufferSetLength(MAX_PATH+1),MAX_PATH);
	path.ReleaseBuffer();
	int pos = path.ReverseFind('\\');
	path = path.Left(pos);
	m_SaveMonitorPath=path + _T("\\") + "�Զ���ؽ��";
	if(!PathIsDirectory(m_SaveMonitorPath))
	{
		BOOL bRet = CreateDirectory(m_SaveMonitorPath, NULL);//�����ļ���
	}

	m_SaveMonitorPath=m_SaveMonitorPath+"\\"+fileName;

	file.Open(m_SaveMonitorPath,CFile::modeCreate|CFile::modeWrite);//�����ļ�
	file.Close();  //�ر��ļ�
}

void CGraphForm2dState::OnPrepareStartMonitor(CString TestName)  //��ʼ����
{
	//д��־
	CString FileName="";
	CString strTemp="";
	FileName=CTime::GetCurrentTime().Format("������־_%Y-%m-%d-%H-%M-%S.txt_Monitor");
	OnCreateMonitorFile(FileName);

	m_pMyMonitorLog = CCDMLogInfo::GetInstance(m_SaveMonitorPath.GetBuffer(m_SaveMonitorPath.GetLength()));//��ȡ��־�ļ�
	strTemp=  "\r\n========" + CTime::GetCurrentTime().Format("%H:%M:%S ") + TestName + "========\r\n";
	if(m_pMyMonitorLog)	//д����־
		m_pMyMonitorLog->SetNotify(strTemp.GetBuffer(strTemp.GetLength()));
}

afx_msg LRESULT CGraphForm2dState::OnWritelog(WPARAM wParam, LPARAM lParam)
{
	CString str=(char*)lParam; 
	CString getData = "";
	CTime t=CTime::GetCurrentTime(); 
	getData=t.Format("%H:%M:%S ") + str;
	if(m_pMyMonitorLog)	//д����־
		m_pMyMonitorLog->SetNotify(getData.GetBuffer(getData.GetLength()));
	getData.ReleaseBuffer();

	return 0;
}

void CGraphForm2dState::OnBnClickedButtonAutomonitor()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!pMainFrameMonitor->m_NetConnection.bConnected)
	{
		MessageBox("����δ�򿪻�����û����!");
		return;
	}

	GraphForm2d * pView	;
	CString strBtTemp;
	CString l_strBtnName;
	pView = (GraphForm2d*) pMainFrameMonitor->m_wndLeftSplitter.GetPane(0,0);

	GetDlgItem(IDC_BUTTON_AUTOMONITOR)->GetWindowText(strBtTemp);
	if (strBtTemp == "�����Զ����")
	{
		//�����ļ���
		OnPrepareStartMonitor("��ʼ�Զ����");
		g_bAutoMoniterRunning = true;

		//��ʼ������
		g_bMoniterFaultedLost=false;    //��ص���֡�쳣�ı�־
		g_bMoniterFaultedMotor=false;    //��ص�����쳣�ı�־
		g_bMoniterFaultedWave=false;    //��ص������쳣�ı�־
		g_nFaultWaveCount=0;     //���ϲ��δ���
		g_nLostWaveCount=0; //��֡ͳ�ƴ���
		g_nSendWaveNum = 0;   //���ͼ���
		g_nFaultMSCount=0;  //���ת���쳣����

		pView->m_scanf2ddlg.m_sScan2dCfgDlg.GetDlgItem(IDC_BUTTON_SICKCONTINUEFRAME)->GetWindowText(l_strBtnName);
		if (l_strBtnName == "������ȡ")
		{
			pView->m_scanf2ddlg.m_sScan2dCfgDlg.OnBnClickedButtonSickcontinueframe();
		}

		pView->m_SetRdDlg.GetDlgItem(IDC_BTN_PIDSTART_CHECK)->GetWindowText(l_strBtnName);
		if (l_strBtnName == "��������")
		{
			pView->m_SetRdDlg.OnBnClickedBtnPidstartCheck();
		}

		GetDlgItem(IDC_BUTTON_AUTOMONITOR)->SetWindowText("�ر��Զ����");
	}
	else if(strBtTemp == "�ر��Զ����")
	{
		pView->m_scanf2ddlg.m_sScan2dCfgDlg.GetDlgItem(IDC_BUTTON_SICKCONTINUEFRAME)->GetWindowText(l_strBtnName);
		if (l_strBtnName == "ֹͣ��ȡ")
		{
			pView->m_scanf2ddlg.m_sScan2dCfgDlg.OnBnClickedButtonSickcontinueframe();
		}

		pView->m_SetRdDlg.GetDlgItem(IDC_BTN_PIDSTART_CHECK)->GetWindowText(l_strBtnName);
		if (l_strBtnName == "�رղ���")
		{
			pView->m_SetRdDlg.OnBnClickedBtnPidstartCheck();
		}

		g_bAutoMoniterRunning=false;    //�Զ���ر�־
		CString sLog="";
		sLog.Format("����ͳ���������֡����%d�������쳣����%d�����ת���쳣����%d", g_nLostWaveCount, g_nFaultWaveCount, g_nFaultMSCount);
		SendMessage(WM_WRITELOG,0,(LPARAM)(LPCTSTR)sLog);
		
		g_bMoniterFaultedLost=false;    //��ص���֡�쳣�ı�־
		g_bMoniterFaultedMotor=false;    //��ص�����쳣�ı�־
		g_bMoniterFaultedWave=false;    //��ص������쳣�ı�־

		GetDlgItem(IDC_BUTTON_AUTOMONITOR)->SetWindowText("�����Զ����");
	}


}

void CGraphForm2dState::OnBnClickedBtnClearDisplay()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CRichEditCtrl *p_rich = (CRichEditCtrl *)GetDlgItem(IDC_RESET_INFO);
	p_rich->SetWindowText(" ");
	p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}

