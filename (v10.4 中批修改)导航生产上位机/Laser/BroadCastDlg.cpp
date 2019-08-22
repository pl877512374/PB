// BroadCastDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "BroadCastDlg.h"
#include "afxdialogex.h"
#include "NetParam.h"
#include "MainFrm.h"
#include "ServerIPDlg.h"
#include "NetWork\NetworkUtils.h"

//�㲥�߳̾�����㲥�߳�
HANDLE hthread_BroadcastThread;
DWORD WINAPI ThreadFuct_BroadCastThread(void *data);
WSADATA wsaData;									//ָ��WinSocket��Ϣ�ṹ��ָ��
SOCKET sockListener;
SOCKADDR_IN sin1,SockAddrIN_Client;							//����������ַ��sin1������
SOCKADDR_IN SockAddrIN_UdpServ;								//ָ��ͨ�Ŷ���Ľṹ��ָ��  
CMainFrame* pMainFrame;

HANDLE hthread_WriteLog_brst;
DWORD WINAPI ThreadFuct_WriteLog_brst(void *data);
int ExtString(CString strSrcString, CString strExt, CStringArray &strArray);

STRUCT_LOG structLog_brst;
//��־�ļ���
CString g_strLogfile_brst;
CString g_strMeasureDisfile_brst;  
//��־�ļ�ָ��
FILE *g_pfLogfile_brst;  
FILE *g_pfMeasureDisfile_brst;  
// CBroadCastDlg �Ի���
unsigned char macbuf[6];
unsigned char ipbuf[4];
int m_7TimeCount = 0;
IMPLEMENT_DYNAMIC(CBroadCastDlg, CDialogEx)

	CBroadCastDlg::CBroadCastDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CBroadCastDlg::IDD, pParent)
{

}

CBroadCastDlg::~CBroadCastDlg()
{
}

void CBroadCastDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BTN_BROAD, m_btnBroad);
	DDX_Control(pDX, IDC_COMBO_UDP_BROADCAST_GETIPADDR, m_cmbUdpGetIpAddr);
	DDX_Control(pDX, IDC_RICHEDIT21, m_recOperLog_brst);
	DDX_Control(pDX, IDC_BTN_IPRst, m_btnIPRst);
	DDX_Control(pDX, IDC_LIST_IP, m_ctrlIpList);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProCtrlIPRst);
}


BEGIN_MESSAGE_MAP(CBroadCastDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_BROAD, &CBroadCastDlg::OnBnClickedBtnBroad)
	ON_BN_CLICKED(IDC_BTN_IPRst, &CBroadCastDlg::OnBnClickedBtnIprst)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_BROAD2, &CBroadCastDlg::OnBnClickedBtnBroad2)
	ON_NOTIFY(NM_RCLICK, IDC_LIST_IP, &CBroadCastDlg::OnNMRClickListIp)
	ON_COMMAND(ID_IP32792, &CBroadCastDlg::OnIp32792)
END_MESSAGE_MAP()


// CBroadCastDlg ��Ϣ�������

void CBroadCastDlg::UpdateIP(void *lp,u8* Ip,int port)
{
	pMainFrame->m_NetConnection.dwServerIP=((Ip[26]<<24)+(Ip[27]<<16)+(Ip[28]<<8)+(Ip[29]));
	if(lp==NULL)
	{
		pMainFrame->m_NetConnection.dwServerIP=((Ip[0]<<24)+(Ip[1]<<16)+(Ip[2]<<8)+(Ip[3]));
	}
	//MAKEWORD()
	pMainFrame->m_NetConnection.unServerPort =port ;
	pMainFrame->m_ServerIpDlg.m_u32ServerPortSet=pMainFrame->m_NetConnection.unServerPort;
	
}
void CBroadCastDlg::OnBnClickedBtnBroad()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD thread;
	CString str;
	m_btnBroad.GetWindowText(str);
	if(str == "��ʼ�㲥")
	{
		m_btnBroad.SetWindowText("�رչ㲥");
		if(InitUdpSocket() == 0)
		{
			CString	m_sLog = "��UDP�㲥�˿ڳɹ���(*^__^*) ";
			WriteLog(&m_recOperLog_brst,m_sLog,0,0);
			SetTimer(7,2000,0);//�����㲥			
		}
		else
		{
			m_btnBroad.SetWindowText("��ʼ�㲥");
			m_btnIPRst.SetWindowText("IP��ԭ");
			m_bBroadcastIPRstEn = false;
			m_bBroadcastThread = false;
			m_bBroadcastSendRecvThread = false;
			return;
		}
		pMainFrame = (CMainFrame*)AfxGetMainWnd();
		hthread_BroadcastThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(ThreadFuct_BroadCastThread), this, 0, &thread);
	}
	else
	{
		DWORD dwexitcode;
		if (closesocket(sockListener) == 0)
		{
			KillTimer(7);
			KillTimer(8);
			m_btnBroad.SetWindowText("��ʼ�㲥");
			m_btnIPRst.SetWindowText("IP��ԭ");
			m_bBroadcastIPRstEn = false;
			CString	m_sLog = "�رչ㲥";
			WriteLog(&m_recOperLog_brst,m_sLog,0,0);
			GetExitCodeThread(hthread_BroadcastThread, &dwexitcode);
			if ((hthread_BroadcastThread != INVALID_HANDLE_VALUE)&&(hthread_BroadcastThread != 0))
			{
				BOOL retflag = TerminateThread( hthread_BroadcastThread,dwexitcode);
				CloseHandle(hthread_BroadcastThread);
				WaitForSingleObject(hthread_BroadcastThread,0);
				Sleep(200);	
			}
			else
			{
				CString m_sLog = "�ر�UDP�㲥�˿�ʧ�ܣ�o(����)o�� ";
				WriteLog(&m_recOperLog_brst,m_sLog,0,0);
			}
		}
	}
}

int CBroadCastDlg::InitUdpSocket()
{
	BOOL fBroadcast = TRUE;                             //����setsockopt(),��ʾ����
	m_nLocalUdpPort = 7000;//��¼ ���͹㲥֡ʱ����udpʹ�õĶ˿ں�
	int nBufSize = 8;
	//strcpy(pPack,"Tell me who are you ?");
	//saClient�����ӹ㲥��ַ������Ϣ
	int iAddrLen=sizeof(SockAddrIN_Client);
	if(WSAStartup(MAKEWORD( 1, 1 ), &wsaData )!=0)           //����WinSocket�ĳ�ʼ��
	{
		printf("Can't initiates windows socket!Program stop.\n");//��ʼ��ʧ�ܷ���-1
		return -1;
	}
	sockListener=socket(AF_INET, SOCK_DGRAM,0);
	sin1.sin_family = AF_INET;
	sin1.sin_port = htons(m_nLocalUdpPort);             //���Ͷ�ʹ�õķ��Ͷ˿ڣ����Ը�����Ҫ����
	sin1.sin_addr.s_addr = htonl(INADDR_ANY);
	if(bind( sockListener, (SOCKADDR FAR *)&sin1, sizeof(sin1))!=0)
	{
		// 		printf("Can't bind socket to local port!Program stop.\n");//��ʼ��ʧ�ܷ���-1
		return -1;
	}
	//	setsockopt�������������׽ӿ�ѡ��
	//	���ù㲥��ʽ�뽫��������������ΪSO_BROADCAST
	setsockopt ( sockListener,SOL_SOCKET,SO_BROADCAST, (CHAR *)&fBroadcast, sizeof (BOOL) );

	//  �������ã�ע��Ҫ��IP��ַ��ΪINADDR_BROADCAST����ʾ���͹㲥UDP���ݱ�
	SockAddrIN_UdpServ.sin_family = AF_INET;
	SockAddrIN_UdpServ.sin_addr.s_addr = htonl(INADDR_BROADCAST);
	SockAddrIN_UdpServ.sin_port = htons(6060);               //�����õĶ˿ڣ����Ը�����Ҫ����
	//	saUdpServ.sin_port = htons(7000);
	// 	if(bind( sockListener, (SOCKADDR FAR *)&saUdpServ, sizeof(saUdpServ))!=0)
	// 	{
	// 		printf("Can't bind socket to local port!Program stop.\n");//��ʼ��ʧ�ܷ���-1
	// 		return -1;
	// 	}

	return 0;
}
int g_nNumIP = 0;//��¼ip����
DWORD WINAPI ThreadFuct_BroadCastThread(void *data)
{
	char cRecvBuff[800];                               //������ջ�����
	int nSize,nbSize;
	CStringArray strarray;

	CBroadCastDlg *pdlg = (CBroadCastDlg *)data;

	int n = 0;
	pdlg->m_bBroadcastThread = TRUE;	
	pdlg->m_bBroadcastSendRecvThread = TRUE;
	while(pdlg->m_bBroadcastThread)
	{
		while (pdlg->m_bBroadcastSendRecvThread)
		{
			//**********************  ������ʹ��sendto��������ͨ��    *************************// 
			

			nSize = sizeof ( SOCKADDR_IN );
			if((nbSize=recvfrom (sockListener,cRecvBuff,800,0,(SOCKADDR FAR *) &SockAddrIN_Client,&nSize))==SOCKET_ERROR) //������ʧ������ʾ����
			{
			}
			else
			{
				if ((nbSize != 0xFFFFFFFF)&&(nbSize != 0))
				{
					//if (checkXor(cRecvBuff,nbSize))//���У��λ
					{
						if ((cRecvBuff[0] == (char)0xFF) && (cRecvBuff[1] == (char)0xAA) && (cRecvBuff[3] == (char)0x2E)) 
						{	
							u8 *pu8buf = new u8[50];
	                        memcpy(pu8buf,&cRecvBuff[0],50);
							BOOL Update=TRUE;
							CString strIp;
							strIp=inet_ntoa(SockAddrIN_Client.sin_addr);
							CString strPort;
							strPort.Format("%d",((BYTE)pu8buf[38]<<8)+((BYTE)(pu8buf[39]&0xFF))); 
							CString strMac;
							strMac.Format("%2.2X-%2.2X-%2.2X-%2.2X-%2.2X-%2.2X",pu8buf[40],pu8buf[41],pu8buf[42],pu8buf[43],pu8buf[44],pu8buf[45]) ;
							for(int i=0; i<pdlg->m_ctrlIpList.GetItemCount(); i++ )
							{
								if(pdlg->m_ctrlIpList.GetItemText(i,1)==strIp&&pdlg->m_ctrlIpList.GetItemText(i,2)==strPort&&pdlg->m_ctrlIpList.GetItemText(i,3)==strMac)
								{
								  Update=FALSE;
								}
								
							}
							if(Update)
							{
								//������ʾ����������ip��ַ���˿ں�
								CString strTmp;
								strTmp.Format("���յ�udp�㲥֡�Ļظ� ��ַ��%s:%d",inet_ntoa(SockAddrIN_Client.sin_addr),ntohs(SockAddrIN_Client.sin_port));
								//pdlg->WriteLog(&pdlg->m_recOperLog,strTmp);

								strarray.RemoveAll();
								//ip��ַ
								strarray.Add(inet_ntoa(SockAddrIN_Client.sin_addr));
								//�˿ں�
								strTmp.Format("%d",((BYTE)cRecvBuff[38]<<8)+((BYTE)(cRecvBuff[39]&0xFF)));
								strarray.Add(strTmp);
								//MAC��ַ
								strTmp.Format("%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X",
									(BYTE)cRecvBuff[40],(BYTE)cRecvBuff[41],
									(BYTE)cRecvBuff[42],(BYTE)cRecvBuff[43],
									(BYTE)cRecvBuff[44],(BYTE)cRecvBuff[45]);
								strarray.Add(strTmp);

								pdlg->UpdateIPInfo(1,strarray);

								//
								//g_nNumIP++;
								CString strNum;
								strNum.Format("%d",g_nNumIP+1);
								pdlg->m_ctrlIpList.InsertItem(g_nNumIP,strNum);
								pdlg->m_ctrlIpList.SetItemText(g_nNumIP,1,inet_ntoa(SockAddrIN_Client.sin_addr));
								strTmp.Format("%d",((BYTE)pu8buf[38]<<8)+((BYTE)(pu8buf[39]&0xFF)));
								pdlg->m_ctrlIpList.SetItemText(g_nNumIP,2,strTmp);
								/*strTmp.Format("%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X",
								(BYTE)cRecvBuff[12],(BYTE)cRecvBuff[13],
								(BYTE)cRecvBuff[14],(BYTE)cRecvBuff[15],
								(BYTE)cRecvBuff[16],(BYTE)cRecvBuff[17]);*/

								strTmp.Format("%2.2X-%2.2X-%2.2X-%2.2X-%2.2X-%2.2X",pu8buf[40],pu8buf[41],pu8buf[42],pu8buf[43],pu8buf[44],pu8buf[45]) ;
								pdlg->m_ctrlIpList.SetItemText(g_nNumIP,3,strTmp);
								int port=	((BYTE)pu8buf[38]<<8)+((BYTE)(pu8buf[39]&0xFF));
								pdlg->UpdateIP(pdlg,pu8buf,port);
								g_nNumIP++;
							}
							delete []pu8buf;

						}
						else if ((cRecvBuff[0] == (char)0x02) && (cRecvBuff[1] == (char)0x02) 
							&& (cRecvBuff[2] == (char)0x02) && (cRecvBuff[3] == (char)0x02)) 
						{	
							//������ʾ����������ip��ַ���˿ں�
							CString strTmp;
							strTmp.Format("���յ�udp�㲥֡�Ļظ� ��ַ��%s:%d",inet_ntoa(SockAddrIN_Client.sin_addr),ntohs(SockAddrIN_Client.sin_port));
							//pdlg->WriteLog(&pdlg->m_recOperLog,strTmp);

							strarray.RemoveAll();
							//ip��ַ
							strarray.Add(inet_ntoa(SockAddrIN_Client.sin_addr));
							//�˿ں�
							strTmp.Format("%d",(BYTE)cRecvBuff[10 + 24 - 6]+((BYTE)(cRecvBuff[11 + 24 - 6]&0xFF))*256);
							strarray.Add(strTmp);
							//MAC��ַ
							strTmp.Format("%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X",
								(BYTE)cRecvBuff[12 + 24 - 6],(BYTE)cRecvBuff[13 + 24 - 6],
								(BYTE)cRecvBuff[14 + 24 - 6],(BYTE)cRecvBuff[15 + 24 - 6],
								(BYTE)cRecvBuff[16 + 24 - 6],(BYTE)cRecvBuff[17 + 24 - 6]);
							strarray.Add(strTmp);

							pdlg->UpdateIPInfo(1,strarray);
							

							CString strNum;
							strNum.Format("%d",g_nNumIP+1);
							pdlg->m_ctrlIpList.InsertItem(g_nNumIP,strNum);
							strTmp.Format("%d.%d.%d.%d",(BYTE)cRecvBuff[24],(BYTE)cRecvBuff[25],(BYTE)cRecvBuff[26],(BYTE)cRecvBuff[27]) ;
							
							pdlg->m_ctrlIpList.SetItemText(g_nNumIP,1,strTmp);
							//pdlg->m_ctrlIpList.SetItemText(g_nNumIP,2,inet_ntoa(SockAddrIN_Client.sin_addr));
							/*strTmp.Format("%2.2X%2.2X%2.2X%2.2X%2.2X%2.2X",
								(BYTE)cRecvBuff[12],(BYTE)cRecvBuff[13],
								(BYTE)cRecvBuff[14],(BYTE)cRecvBuff[15],
								(BYTE)cRecvBuff[16],(BYTE)cRecvBuff[17]);*/
							pdlg->m_ctrlIpList.SetItemText(g_nNumIP,2,strTmp);
							strTmp.Format("%2.2X-%2.2X-%2.2X-%2.2X-%2.2X-%2.2X",cRecvBuff[30],cRecvBuff[31],cRecvBuff[32],cRecvBuff[33],(unsigned char)cRecvBuff[34],(unsigned char)cRecvBuff[35]) ;
							pdlg->m_ctrlIpList.SetItemText(g_nNumIP,3,strTmp);
							g_nNumIP++;

							if(cRecvBuff[9]  == (char)0xF1)
							{
								AfxMessageBox(_T("IP�ѳ�ʼ�����������Ѹ�λ��"));
								CString	m_sLog = "��ʼ��IPλ192.168.0.2\n";
								pdlg->WriteLog(&pdlg->m_recOperLog_brst,m_sLog,0,0);
								pdlg->m_bBroadcastIPRstEn = false;
								pdlg->KillTimer(8);
								pdlg->m_ProCtrlIPRst.SetPos(100);
							}
						}
					}
					nSize = sizeof (SOCKADDR_IN);
				}
			}
			cRecvBuff[nSize] = '\0';                              //�ַ�����ֹ
			Sleep(100);
		}
	} 
	//*********************   ���Ĳ��ر�socket  ***************************************//
	closesocket(sockListener);         //�رռ���socket
	WSACleanup();
	return 0;
}

int CBroadCastDlg::UpdateIPInfo(int nType,CStringArray &strIpInfo)
{
	CString strTmp;
	int count = m_cmbUdpGetIpAddr.GetCount();
	strTmp = strIpInfo.GetAt(0);
	strTmp += " ";
	strTmp += strIpInfo.GetAt(1);
	strTmp += " ";
	strTmp += strIpInfo.GetAt(2);

	int nTmp = m_cmbUdpGetIpAddr.FindString(0,strTmp);
	if ((nTmp >= 0)&&(nTmp <count))
	{
		//		int ss =0;
	}
	else
	{
		strTmp = strIpInfo.GetAt(0);
		for (nTmp = 1; nTmp < strIpInfo.GetSize(); nTmp ++)
		{
			strTmp += " ";
			strTmp += strIpInfo.GetAt(nTmp);
		}
		m_cmbUdpGetIpAddr.AddString(strTmp);
	}
	m_cmbUdpGetIpAddr.SetCurSel(0);
	return 0;
}

void CBroadCastDlg::WriteLog(CRichEditCtrl *pREC, CString sLog, unsigned char *pbuf, unsigned short u16datalen)
{
	if (sLog != "")
	{
		structLog_brst.strLogInfo[structLog_brst.in] = sLog;	
	}
	else
	{
		structLog_brst.strLogInfo[structLog_brst.in] = "����ʾ��Ϣ";	
	}

	if (pbuf != NULL)
	{
		memcpy(&structLog_brst.au8buf[structLog_brst.in][0],pbuf,u16datalen);
		structLog_brst.au16buflen[structLog_brst.in] = u16datalen;
	}
	else
	{
		//memcpy(&strLog_Com.au8buf[strLog_Com.in][0],pbuf,u16datalen);
		structLog_brst.au16buflen[structLog_brst.in] = 0;
	}

	if((pbuf != NULL)||(sLog != ""))
	{
		structLog_brst.in ++ ;
		structLog_brst.in %= MAXLOGINFO;	
	}

	if(pREC->GetLineCount()>2000)
	{
		pREC->SetSel(0, -1);		
		pREC->ReplaceSel("");
	}
}

DWORD WINAPI ThreadFuct_WriteLog_brst(void *data)
{
	// 	WIN32_FIND_DATA fileInfo;
	// 	HANDLE hFind;
	// 	DWORD fileSize;
	structLog_brst.in = 0;	
	structLog_brst.out = 0;
	const char *fileName = "123";
	unsigned short i;
	CString strTmp; 
	CBroadCastDlg *pDlg = (CBroadCastDlg *)data;
	while(1)
	{
		if (structLog_brst.in == structLog_brst.out)
		{
			Sleep(10);
			continue;
		}

		CString l_strInfoTxt;
		//l_strInfoTxt.Format("%s", MSystemTime());
		CString strLog_Time = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S\r\n");
		l_strInfoTxt = strLog_Time;
		g_strLogfile_brst = CTime::GetCurrentTime().Format("%Y-%m-%d-%H.txt");
		if (structLog_brst.strLogInfo[structLog_brst.out] != "")
		{
			l_strInfoTxt += structLog_brst.strLogInfo[structLog_brst.out] + "\r\n";	
		}

		if (structLog_brst.au16buflen[structLog_brst.out] != 0)
		{
			for ( i = 0;  i < structLog_brst.au16buflen[structLog_brst.out];  i++)
			{
				strTmp.Format("%02X ", structLog_brst.au8buf[structLog_brst.out][i]);
				l_strInfoTxt += strTmp;
			}
			l_strInfoTxt += "\r\n";
		}

		pDlg->m_recOperLog_brst.SetSel(-1, -1);        
		pDlg->m_recOperLog_brst.ReplaceSel(l_strInfoTxt);        
		pDlg->m_recOperLog_brst.PostMessage(WM_VSCROLL, SB_BOTTOM,0);

		/*if((g_pfLogfile=fopen(g_strLogDir + g_strLogfile,"at+"))==NULL)
		{
		pDlg->m_recOperLog.SetSel(-1, -1);        
		pDlg->m_recOperLog.ReplaceSel("��¼��־����ʧ�ܣ���������!");        
		pDlg->m_recOperLog.PostMessage(WM_VSCROLL, SB_BOTTOM,0);
		structLog.out ++;
		structLog.out %= MAXLOGINFO;
		continue;
		}
		else
		{
		fseek(g_pfLogfile,0,SEEK_END);
		fwrite(l_strInfoTxt,strlen(l_strInfoTxt),1,g_pfLogfile);            
		fclose(g_pfLogfile);
		}*/
		structLog_brst.out ++;
		structLog_brst.out %= MAXLOGINFO;
	}
	return 0;
}

BOOL CBroadCastDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_ctrlIpList.InsertColumn(0,_T("���"),LVCFMT_CENTER,100,-1);
	m_ctrlIpList.InsertColumn(1,"IP",LVCFMT_CENTER,100,-1);
	m_ctrlIpList.InsertColumn(2,"�˿ں�",LVCFMT_CENTER,50,-1);
	m_ctrlIpList.InsertColumn(3,"Mac��ַ",LVCFMT_CENTER,150,-1);

	m_ctrlIpList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT|LVS_SHOWSELALWAYS|LVS_EDITLABELS);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	if (hthread_WriteLog_brst <= 0)
	{
		DWORD thread1;
		hthread_WriteLog_brst = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(ThreadFuct_WriteLog_brst), this, 0, &thread1);
	}
	m_bBroadcastIPRstEn = false;
	m_ProCtrlIPRst.SetRange(0,100);
	m_ProCtrlIPRst.SetBarColor(RGB(0,255,0));

	//long long a= _strtol_l(strNum,NULL,16);
	////CString strNum;
	////strNum = "192.168.1.1";//inet_ntoa(SockAddrIN_Client.sin_addr);
	////long a  = inet_addr(strNum);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CBroadCastDlg::OnBnClickedBtnIprst()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	DWORD thread;
	CString str;
	m_btnIPRst.GetWindowText(str);

	if(!(m_bBroadcastSendRecvThread && m_bBroadcastThread))
	{
		AfxMessageBox(_T("���ȿ����㲥��"));
		m_btnIPRst.SetWindowText("IP��ԭ");
		return;
	}

	if(str == "IP��ԭ")
	{
		m_btnIPRst.SetWindowText("ͨ����");
		m_bBroadcastIPRstEn = true;
		//hthread_BroadcastThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(ThreadFuct_BroadCastThread), this, 0, &thread);
	}
	else
	{
		m_btnIPRst.SetWindowText("IP��ԭ");
		m_bBroadcastIPRstEn = false;
	}
}

void CBroadCastDlg::OnTimer(UINT nIDEvent) 
{
	static u8 s_u8ProCtrlStep;
	switch(nIDEvent)
	{
	case 7:
	if(m_bBroadcastSendRecvThread && m_bBroadcastThread)//�жϸ��߳��Ƿ��ڼ���
	{
		if (m_7TimeCount >= 4)
		{
			m_cmbUdpGetIpAddr.ResetContent();
			//m_strServerIp = "";
			//m_dwServerPort = 0;
			m_7TimeCount = 0;
			UpdateData(FALSE);
		}
		m_7TimeCount ++;	
		GetLocalIp(m_acLocalIp);

		int nstart = 0;
		if(m_bBroadcastIPRstEn == false)
		{
			nstart = 24 - 6;
			acBroadcastFrameBuf[nstart + 6] = m_acLocalIp[0];
			acBroadcastFrameBuf[nstart + 7] = m_acLocalIp[1];
			acBroadcastFrameBuf[nstart + 8] = m_acLocalIp[2];
			acBroadcastFrameBuf[nstart + 9] = m_acLocalIp[3];


			m_nLocalUdpPort = 7000;
			acBroadcastFrameBuf[nstart + 10] = (char)(m_nLocalUdpPort&0xFF);
			acBroadcastFrameBuf[nstart + 11] = (char)((m_nLocalUdpPort>>8)&0xFF);

			//**********************  ������ʹ��sendto��������ͨ��    *************************//
			Xor(acBroadcastFrameBuf,nBroadcastFrameLength-1);
			sendto(sockListener,Broad, nBroad, 0, (SOCKADDR *) &SockAddrIN_UdpServ, sizeof (SOCKADDR_IN));
		}
		else
		{
			nstart = 0;
			nstart = 24 - 6;
			acBroadcastIPRstFrameBuf[nstart + 6] = ipbuf[0];
			acBroadcastIPRstFrameBuf[nstart + 7] = ipbuf[1];
			acBroadcastIPRstFrameBuf[nstart + 8] = ipbuf[2];
			acBroadcastIPRstFrameBuf[nstart + 9] = ipbuf[3];

			m_nLocalUdpPort = 7000;
			acBroadcastIPRstFrameBuf[nstart + 10] = (char)(m_nLocalUdpPort&0xFF);
			acBroadcastIPRstFrameBuf[nstart + 11] = (char)((m_nLocalUdpPort>>8)&0xFF);


			acBroadcastIPRstFrameBuf[nstart + 12] = macbuf[0];
			acBroadcastIPRstFrameBuf[nstart + 13] = macbuf[1];
			acBroadcastIPRstFrameBuf[nstart + 14] = macbuf[2];
			acBroadcastIPRstFrameBuf[nstart + 15] = macbuf[3];
			acBroadcastIPRstFrameBuf[nstart + 16] = macbuf[4];
			acBroadcastIPRstFrameBuf[nstart + 17] = macbuf[5];
			//**********************  ������ʹ��sendto��������ͨ��    *************************//
		Xor(acBroadcastIPRstFrameBuf,nBroadcastIPRstFrameLength-1);
			sendto(sockListener,acBroadcastIPRstFrameBuf, nBroadcastIPRstFrameLength, 0, (SOCKADDR *) &SockAddrIN_UdpServ, sizeof (SOCKADDR_IN));
		}
	}
	break;
	case 8:
		case 1:

			if(s_u8ProCtrlStep < 100)
				s_u8ProCtrlStep+=5;
			else
			s_u8ProCtrlStep = 0;
			
			m_ProCtrlIPRst.SetPos(s_u8ProCtrlStep);

		break;
	default:
		break;
	}
}

CString CBroadCastDlg::GetLocalIp(char *acSelfIPBuf)
{
	CStringArray strArrayTmp;
	struct hostent * phost;
	char acLocalIp[20];
	char hostname[50];
	WSADATA wsData;  
	::WSAStartup(MAKEWORD(2,2), &wsData);  

	if (gethostname(hostname, 50) != 0)
	{
		return "";
	}

	phost = gethostbyname(hostname);
	char **names;
	names = phost->h_aliases;
	char **iplist;
	iplist = phost->h_addr_list;
	while (*iplist)
	{
		strcpy(acLocalIp, inet_ntoa(*(struct in_addr *)*iplist));
		iplist++;
	}

	CString strLocalIp = acLocalIp;

	ExtString(acLocalIp,".",strArrayTmp);
	acSelfIPBuf[0] = atoi(strArrayTmp.GetAt(0));
	acSelfIPBuf[1] = atoi(strArrayTmp.GetAt(1));
	acSelfIPBuf[2] = atoi(strArrayTmp.GetAt(2));
	acSelfIPBuf[3] = atoi(strArrayTmp.GetAt(3));

	return strLocalIp;
}

int ExtString(CString strSrcString, CString strExt, CStringArray &strArray)
{
	CString strTmp;
	int nCount = 0;
	strArray.RemoveAll();
	while (strSrcString.Find(_T(strExt)) != -1) //�����ַ������Ƿ��С� ������
	{
		strTmp = strSrcString.Left(strSrcString.Find(_T(strExt)));//��ȡ�ַ���
		strSrcString = strSrcString.Right(strSrcString.GetLength() - strSrcString.Find(_T(strExt)) - 1);//ʣ�µ��ַ���
		strArray.Add(strTmp);//��ȡ�����ַ����붯̬������
		nCount ++;
	}

	if (strSrcString.GetLength()>0)
	{
		strArray.Add(strSrcString);//ʣ����ַ����붯̬������
		nCount ++;
	}
	return nCount;
}

void CBroadCastDlg::OnBnClickedBtnBroad2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_ctrlIpList.DeleteAllItems();
	g_nNumIP = 0;
}

CString g_strRstIP,g_strRstMac,g_strRstPort;

void CBroadCastDlg::OnNMRClickListIp(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if(pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point( LOWORD(dwPos), HIWORD(dwPos) );
   
		CMenu menu;
		VERIFY( menu.LoadMenu( IDR_MENU1 ) );
		CMenu* popup = menu.GetSubMenu(0);
		ASSERT( popup != NULL );
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this );
		int row = pNMItemActivate->iItem;
		g_strRstIP = m_ctrlIpList.GetItemText(row,1);
		g_strRstPort = m_ctrlIpList.GetItemText(row,2);
		g_strRstMac = m_ctrlIpList.GetItemText(row,3);
	}
	*pResult = 0;
	*pResult = 0;
}

byte ip_write[4]={0};
void CBroadCastDlg::OnIp32792()
{
	// TODO: �ڴ���������������
	
//	if(!(m_bBroadcastSendRecvThread && m_bBroadcastThread))
//	{
//		AfxMessageBox(_T("���ȿ����㲥��"));
//		m_btnIPRst.SetWindowText("IP��ԭ");
//		return;
//	}
//	INT_PTR nRes = MessageBox(_T("��ԭ��ʹIP��ַ���г�ʼ������ȷ����Ҫ���л�ԭ��"),"��ʼ��",MB_OKCANCEL|MB_ICONQUESTION);
//
//	if(nRes == IDCANCEL)
//		return;
//	SetTimer(8,2000,0);//�����㲥
//	long ip  = inet_addr(g_strRstIP);
//	
//	ipbuf[0] = ip & 0xff;
//	ipbuf[1] = (ip >> 8) & 0xff;
//	ipbuf[2] = (ip >> 16) & 0xff;
//	ipbuf[3] = (ip >> 24) & 0xff;
//
//	
//	
//	CString stra[6];
//	stra[0] = g_strRstMac.Mid(0,2);
//	stra[1] = g_strRstMac.Mid(3,2);
//	stra[2] = g_strRstMac.Mid(6,2);
//	stra[3] = g_strRstMac.Mid(9,2);
//	stra[4] = g_strRstMac.Mid(12,2);
//	stra[5] = g_strRstMac.Mid(15,2);
//\
//	macbuf[0] = strtol(stra[0],NULL,16);
//	macbuf[1] = strtol(stra[1],NULL,16);
//	macbuf[2] = strtol(stra[2],NULL,16);
//	macbuf[3] = strtol(stra[3],NULL,16);
//	macbuf[4] = strtol(stra[4],NULL,16);
//	macbuf[5] = strtol(stra[5],NULL,16);
//
//	m_bBroadcastIPRstEn = true;

	CString str;
	m_btnBroad.GetWindowText(str);
	if(str=="�رչ㲥")
	{
		AfxMessageBox(_T("��ȷ���㲥�ѹرգ�"));
		return;
	}
	long ip  = inet_addr(g_strRstIP);
	
	ip_write[0] = ip & 0xff;
	ip_write[1] = (ip >> 8) & 0xff;
	ip_write[2] = (ip >> 16) & 0xff;
	ip_write[3] = (ip >> 24) & 0xff;
	int port= atoi(g_strRstPort);  
	UpdateIP(NULL,ip_write,port);
}
