// ProgramDownDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "ProgramDownDlg.h"
#include "afxdialogex.h"
#include "process.h"
#include "MainFrm.h"
#include "UserMsg.h"

volatile int mode=0; // 0:����״̬ 1: �����������󣬵ȴ��ظ�  2: �յ��ظ�����������  3: ��ʼ���ݴ���
volatile int Start=0;
int Bcc_CHeck=0;
CString ipstr=""; 
volatile UINT d_port=0;
volatile UINT r_flag;
volatile UINT r_flag2;
//BYTE pbuffer[1024]={0};
char pbuffer[1064]={0};//socket���ͻ���

volatile int send_len=0;			 //socket���ͳ���
int file_index=0;
int chongfa=0;
int chongfa2=0;
#define SEND_MAX 1024
//#define SEND_MAX 1000
int g_n32ProgramFlag = 0;
// CProgramDownDlg �Ի���

IMPLEMENT_DYNAMIC(CProgramDownDlg, CDialogEx)

	CProgramDownDlg::CProgramDownDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProgramDownDlg::IDD, pParent)
{
	m_file_path = _T("");
	m_port = 0;
}

CProgramDownDlg::~CProgramDownDlg()
{
}

void CProgramDownDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_DOWNPATH, m_file_path);
	DDX_Control(pDX, IDC_IPADDRESS1, m_ipadd);
	DDX_Text(pDX, IDC_EDIT1, m_port);
}


BEGIN_MESSAGE_MAP(CProgramDownDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CProgramDownDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON_SELFILE, &CProgramDownDlg::OnBnClickedButtonSelfile)
	ON_BN_CLICKED(IDC_BUTTON3, &CProgramDownDlg::OnBnClickedButton3)
	ON_EN_CHANGE(IDC_EDIT_DOWNPATH, &CProgramDownDlg::OnEnChangeEditDownpath)
	ON_MESSAGE(WM_RECEIVE, OnReceive)
	ON_MESSAGE(WM_MES, MES_reply)
	ON_BN_CLICKED(IDC_BUTTON4, &CProgramDownDlg::OnBnClickedButton4)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_CLEARINFO, &CProgramDownDlg::OnBnClickedButtonClearinfo)
END_MESSAGE_MAP()


// CProgramDownDlg ��Ϣ�������
char bcc_packs=0;
int send_over=0;		//������ɱ�־
/*�������̺���*/
void send_process(LPVOID p)
{
	CProgramDownDlg * ps = (CProgramDownDlg *)p;
	CDatagramSocket * m_pDatagramSocket;
	m_pDatagramSocket = ps->m_pDatagramSocket;
	memset(pbuffer,'\0',1024);
	long file_len=0;
	BYTE send_flag=0;
	FILE *fp=NULL;
	int sum_c=0;


	file_index=0;

	while(g_n32ProgramFlag)
	{
		if(mode==0)								//����״̬
		{
			file_index=0;
			sum_c=0;
			fp=NULL;
			memset(pbuffer,'\0',1024);
			file_len=0;
			send_len=0;
			r_flag=0;
			//r_flag2=0;
			send_over=0;
		}
		else if(mode==1)							//������������״̬
		{
			send_len=0;
			//pbuffer[0]=0x30;			//֡ͷ
			//pbuffer[1]=0x24;
			//pbuffer[2]=(send_len+9)>>8; //֡��
			//pbuffer[3]=(send_len+9) & 0xFF;
			//pbuffer[4]=0xCD;
			//pbuffer[5]=0x45;		//�����
			//pbuffer[6]=0;	//���
			//pbuffer[7]=0;
			//sum_c=0;
			//for(int i=0;i<8+send_len;i++)
			//{
			//	sum_c = (sum_c + pbuffer[i]) & 0xFF;
			//}
			/*pbuffer[8+send_len]=sum_c;*/

			CTime time=CTime::GetCurrentTime();
			char Hour= time.GetHour();
			char Miu= time.GetMinute();
			char Sec= time.GetSecond();
			static char zhiling[34]={0xFF,0xAA,0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x01,0x00,0x00,0x00,0x00,0x00,0x00};//����ָ��
			Xor(&zhiling[0],34);
			memcpy(pbuffer,zhiling,34);
			send_len+=34;  //����һЩ֡����

			SendMessage(ps->m_hWnd, WM_MES, 0, 0);  //֪ͨ���̷߳���
			Sleep(50);     //�����������������������������500��Ϊ50����
		}
		else if((mode==3)&&(r_flag==1)&&(send_over==0))  //��ʼ��������״̬
		{
			r_flag=0;
			//r_flag2=1;

			if(file_index==0&&Start==0)
			{
				fp=fopen(ps->m_file_path,"rb");
				if(fp==NULL)
				{
					return ;
				}
				fseek(fp,0,SEEK_END);
				file_len = ftell(fp);
				fseek(fp,0,SEEK_SET);
				/*fread(pbuffer1,file_len,1,fp);
				bcc_packs=	Xor_download(0,&pbuffer1[0],file_len);*/
				//���͸�����Ϣ
				CTime time=CTime::GetCurrentTime();
				char Hour= time.GetHour();
				char Miu= time.GetMinute();
				char Sec= time.GetSecond();
				static char zhiling[36]={0xFF,0xAA,0x00,0x20,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x02,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
				int size=file_len;
				zhiling[24]=(size>>24);
				zhiling[25]=(size>>16);
				zhiling[26]=(size>>8);
				zhiling[27]=(size& 0xff);
				int pack=1+file_len/SEND_MAX;//�ܰ���
				zhiling[28]=(pack>>24);
				zhiling[29]=(pack>>16);
				zhiling[30]=(pack>>8);
				zhiling[31]=(pack& 0xff);
				Xor(&zhiling[0],36);
				memcpy(pbuffer,zhiling,36);
				send_len=36;  

				SendMessage(ps->m_hWnd, WM_MES, 0, 0);  //֪ͨ���̷߳���
				Sleep(100);
			}
			if(Start==1)//�������������Ϣ�����յ�������Ϣ֮��
			{
				if((file_len-file_index*SEND_MAX)>=SEND_MAX)
				{
					send_len=SEND_MAX;
					fread(pbuffer+28,send_len,1,fp);
					send_flag=0x02;
				}
				else if((file_len-file_index*SEND_MAX)>0)
				{
					send_len=(file_len-file_index*SEND_MAX);
					fread(pbuffer+28,send_len,1,fp);
					send_flag=0x02;
					if(send_len<SEND_MAX)
					{

						send_flag=0x03;
						send_over=1;
						fclose(fp);
					}
				}
				else
				{
					send_len=0;
					send_flag=0x03;
					send_over=1;
					fclose(fp);
				}

				CTime time=CTime::GetCurrentTime();
				char Hour= time.GetHour();
				char Miu= time.GetMinute();
				char Sec= time.GetSecond();
				static char zhiling[36]={0xFF,0xAA,0x00,0x20,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
				int size=file_index;
				zhiling[24]=(size>>24);
				zhiling[25]=(size>>16);
				zhiling[26]=(size>>8);
				zhiling[27]=(size& 0xff);
				memcpy(&pbuffer[0],zhiling,28);
				int plenth=28+send_len;
				pbuffer[2]=(plenth>>8);
				pbuffer[3]=plenth;
				Xor(&pbuffer[0],32+send_len);

				file_index++;
				//

				bcc_packs=Xor_download(bcc_packs,&pbuffer[28],send_len);
				send_len+=32;
				SendMessage(ps->m_hWnd, WM_MES, 1, 0);  //֪ͨ���̷߳���
				Sleep(100);
			}
			//r_flag2=0;

		}else if((mode==3)&&(r_flag==1)&&(Start==2))
		{
			//���͸������
			CTime time=CTime::GetCurrentTime();
			char Hour= time.GetHour();
			char Miu= time.GetMinute();
			char Sec= time.GetSecond();
			static char zhiling[34]={0xFF,0xAA,0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x05,0x00,0x00,0x00,0x00,0x00,0x00};
			zhiling[27]=(bcc_packs& 0xff);
			memcpy(pbuffer,zhiling,34);
			Xor(&pbuffer[0],34);
			send_len=34;  
			send_over=0;
			SendMessage(ps->m_hWnd, WM_MES, 0, 0);  //֪ͨ���̷߳���
			Sleep(100);
		}
		else if((mode==3)&&(r_flag==1)&&Start==3)//���Ͱ��������·�
		{
			r_flag=0;
			CTime time=CTime::GetCurrentTime();
			char Hour= time.GetHour();
			char Miu= time.GetMinute();
			char Sec= time.GetSecond();
			static char zhiling[36]={0xFF,0xAA,0x00,0x20,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
			int size=file_index-1;
			zhiling[24]=(size>>24);
			zhiling[25]=(size>>16);
			zhiling[26]=(size>>8);
			zhiling[27]=(size& 0xff);
			memcpy(&pbuffer[0],zhiling,28);
			send_len=send_len-32;
			int plenth=28+send_len;
			pbuffer[2]=(plenth>>8);
			pbuffer[3]=plenth;
			Xor(&pbuffer[0],32+send_len);

			//bcc_packs=Xor_download(bcc_packs,&pbuffer[28],send_len);
			send_len+=32;
			SendMessage(ps->m_hWnd, WM_MES, 1, 0);  //֪ͨ���̷߳���
			Sleep(100);
		}
		Sleep(1);
		chongfa++;
		if((mode == 3)&&(r_flag==0)&&Start==1)//δ�ظ��ط�
		{
			if(chongfa>2000)
			{
				chongfa=0;
				r_flag=0;
				CTime time=CTime::GetCurrentTime();
				char Hour= time.GetHour();
				char Miu= time.GetMinute();
				char Sec= time.GetSecond();
				static char zhiling[36]={0xFF,0xAA,0x00,0x20,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
				int size=file_index-1;
				zhiling[24]=(size>>24);
				zhiling[25]=(size>>16);
				zhiling[26]=(size>>8);
				zhiling[27]=(size& 0xff);
				memcpy(&pbuffer[0],zhiling,28);

				send_len=send_len-32;

				chongfa2++;
				int plenth=28+send_len;
				pbuffer[2]=(plenth>>8);
				pbuffer[3]=plenth;
				Xor(&pbuffer[0],32+send_len);
				//bcc_packs=Xor_download(bcc_packs,&pbuffer[28],send_len);//
				send_len+=32;
				SendMessage(ps->m_hWnd, WM_MES, 1, 0);  //֪ͨ���̷߳���
				Sleep(100);
			}
		}
	}
}


void CProgramDownDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	if(pMainFrame->m_NetConnection.bConnected)
	{
		AfxMessageBox("����������");
		return;
	}

	GetDlgItem(IDC_BUTTON_CLEARINFO)->EnableWindow(FALSE);

	/*int nBufSize = 25;
	char cBuf[26] = {(char)0x02, (char)0x02, (char)0x02, (char)0x02, (char)0x00, (char)0x00, (char)0x00, (char)0x11,(char)0x73,(char)0x47, (char)0x4E, 
	(char)0x20, (char)0x4C, (char)0x4D, (char)0x44, (char)0x73, (char)0x63, (char)0x61, (char)0x6E, (char)0x64, (char)0x61, (char)0x74, (char)0x61, (char)0x20, (char)0x00};*/
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x04,0x05};
	ZhiLing(Reset_zhiling,pn,0);
	SendMessage(WM_SENDCMD,Reset_zhilingSize, (LPARAM)Reset_zhiling);

	g_n32ProgramFlag = 1;
	AfxBeginThread((AFX_THREADPROC)(send_process),(LPVOID)this,0,0,0,NULL);
	UpdateData(true);
	CRichEditCtrl *p_rich = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT_PROGRAMDN);
	CString	str_rich = "\r\n����������!\r\n";
	p_rich->SetSel(-1, -1);		
	p_rich->ReplaceSel(str_rich);
	d_port=m_port;

	BYTE ip_field[4];
	m_ipadd.GetAddress(ip_field[0],ip_field[1],ip_field[2],ip_field[3]);
	ipstr.Format("%d.%d.%d.%d",ip_field[0],ip_field[1],ip_field[2],ip_field[3]);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	//�ñ�־λ
	mode=1;
	str_rich = "������Ѱ�豸";
	p_rich->SetSel(-1, -1);		
	p_rich->ReplaceSel(str_rich);
	UpdateData(false);
}


void CProgramDownDlg::OnBnClickedButtonSelfile()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	char path_temp[1024];
	GetCurrentDirectory(128,path_temp);
	CFileDialog filedlg(TRUE,"*.bin",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"bin files(*.bin)|*.bin|All files(*.*)|*.*||");
	//	filedlg.m_ofn.lpstrInitialDir = "C:\\";
	filedlg.m_ofn.lpstrInitialDir = path_temp;
	CString file_path;
	filedlg.DoModal();
	file_path = filedlg.GetPathName();
	SetDlgItemText(IDC_EDIT_DOWNPATH,file_path);
}


void CProgramDownDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CRichEditCtrl *p_rich = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT_PROGRAMDN);

	mode=3; //��ʼˢд
	r_flag=1;

	CString str_rich = "��ʼˢд";
	p_rich->SetSel(-1, -1);		
	p_rich->ReplaceSel(str_rich);
	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_CLEARINFO)->EnableWindow(FALSE);
}


void CProgramDownDlg::OnEnChangeEditDownpath()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	GetDlgItemText(IDC_EDIT_DOWNPATH,m_file_path);

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}

LRESULT CProgramDownDlg::MES_reply(WPARAM wp,LPARAM lp)
{
	CRichEditCtrl *p_rich = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT_PROGRAMDN);
	CString str_rich="";
	if(wp==0)  //��������Ϊ0������socket������
	{
		//����������SOCKET
		if(m_pDatagramSocket != NULL)
		{
			delete m_pDatagramSocket;
			m_pDatagramSocket = NULL;
		}
		if (m_pDatagramSocket == NULL)
		{
			m_pDatagramSocket = new CDatagramSocket(m_hWnd);
			if (!m_pDatagramSocket->Create(3010, SOCK_DGRAM))
			{
				delete m_pDatagramSocket;
				m_pDatagramSocket = NULL;
				//AfxMessageBox("�׽��ִ���ʧ�ܡ�");
				return 0;
			}
		}

		m_pDatagramSocket->SendTo(pbuffer, send_len, d_port, ipstr);
		str_rich = "- ";
		p_rich->SetSel(-1, -1);		
		p_rich->ReplaceSel(str_rich);
	}
	else if(wp==1)
	{
		m_pDatagramSocket->SendTo(pbuffer, send_len, d_port, ipstr);
	}


	return 0;
}

LRESULT CProgramDownDlg::OnReceive(WPARAM wp,LPARAM lp)
{
	BYTE RBuffer[2048] = {0} ;
	memset(RBuffer,'\0',2048);
	CProgramDownDlg * ps = (CProgramDownDlg *)lp;

	//��������
	CString strAddr = _T("");
	UINT nPort = 0;
	int r_len = m_pDatagramSocket->ReceiveFrom(RBuffer, 2048, strAddr, nPort);
	strAddr.Format("%d",r_len);
	//MessageBox(strAddr);
	static CString strSData=_T("");
	CRichEditCtrl *p_rich = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT_PROGRAMDN);
	CString str_rich="";

	if( (r_len==34)&&(RBuffer[29]==0x01)&& mode == 1)  //�յ��ظ�
	{
		mode=2;     //�յ��ظ�����������

		str_rich = "\n\r��⵽ 1 ���豸!\r\n";
		p_rich->SetSel(-1, -1);		
		p_rich->ReplaceSel(str_rich);
		str_rich = "������......\r\n";
		p_rich->SetSel(-1, -1);		
		p_rich->ReplaceSel(str_rich);
		str_rich = "���ӳɹ�!\r\n";
		p_rich->SetSel(-1, -1);		
		p_rich->ReplaceSel(str_rich);
		GetDlgItem(IDC_BUTTON3)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_CLEARINFO)->EnableWindow(TRUE);
		p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

	}
	else if(r_len==35&&RBuffer[30]==0x01&& mode == 3&&Start==1&&send_over==1)//д�����һ�����������
	{
		Start=2;
		r_flag=1;
	}
	/*else if((r_len==1)&&(RBuffer[0]==0x55)&&(mode == 3)&&(r_flag2==0))*/
	else if((r_len==35)&&(RBuffer[30]==0x01)&&(mode == 3)&&(r_flag==0))//д������
	{
		r_flag=1;
		chongfa2=0;
		//file_index++; 
		str_rich = "> ";
		p_rich->SetSel(-1, -1);		
		p_rich->ReplaceSel(str_rich);
		p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	}
	else if((r_len==34)&&(RBuffer[29]==0x00)&& mode == 3&&r_flag==0)//д�����ݷ���ʧ��
	{
		Start=3;
		r_flag=1;
	}
	else if((r_len==34)&&(RBuffer[29]==0x01)&& mode == 3&&Start==0)//���͸�����Ϣ�󷵻�
	{
		Start=1;
		r_flag=1;
	}
	else if((r_len==34)&&(RBuffer[29]==0x00)&&(mode==3)&&(Start==2))//��дʧ��
	{
		//str_rich = "\nͨ�Ŵ�����дʧ�ܣ�\n";
		str_rich.Format("\n��%d��дʧ�ܣ�\r\n",file_index) ;
		p_rich->SetSel(-1, -1);			
		p_rich->ReplaceSel(str_rich);
		p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

		//һ����д��ɣ���λ����
		//		mode=0;
		//		GetDlgItem(IDC_IPADDRESS1)->EnableWindow(TRUE);
		//		GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
		//		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		//		GetDlgItem(IDC_BUTTON_SEND_DATA)->EnableWindow(TRUE);
		//		OnBnClickedButton4();
		//GetDlgItem(IDC_EDIT2)->EnableWindow(true);
		//GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	}

	if((r_len==34)&&(RBuffer[29]==0x01)&&(mode==3)&&(Start==2))
	{
		str_rich = "\n��д�ɹ���\n";
		p_rich->SetSel(-1, -1);		
		p_rich->ReplaceSel(str_rich);
		p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

		//һ����д��ɣ���λ����
		mode=0;
		Start=0;
		send_over=0;
		GetDlgItem(IDC_IPADDRESS1)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT1)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_SELFILE)->EnableWindow(TRUE);
		//GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
		OnBnClickedButton4();
		//GetDlgItem(IDC_EDIT2)->EnableWindow(true);

	}

	return 0;

}


BOOL CProgramDownDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	BYTE b_ip[4]={192,168,0,236};
	m_ipadd.SetAddress(b_ip[0],b_ip[1],b_ip[2],b_ip[3]);	
	m_port1 = 4000;
	m_port = 4000;
	SetDlgItemText(IDC_EDIT1,"4000");
	SetDlgItemText(IDC_EDIT_DOWNPATH,"./data.bin");
	m_file_path = "./data.bin";
	m_pDatagramSocket=NULL;

	CMainFrame *pMainFrame =  (CMainFrame*)AfxGetMainWnd();
	m_ipadd.SetAddress(pMainFrame->m_NetConnection.dwServerIP);
	m_port1 = 6060;
	m_port = 6060;
	CString str;
	str.Format("%d",m_port);
	SetDlgItemText(IDC_EDIT1,str);

	//AfxBeginThread((AFX_THREADPROC)(send_process),(LPVOID)this,0,0,0,NULL);

	UpdateData(true);

	GetDlgItem(IDC_BUTTON3)->EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CProgramDownDlg::OnBnClickedButton4()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	g_n32ProgramFlag = 0;
	Start=0;
	send_over=0;
	bcc_packs=0;
	Sleep(10);
	CRichEditCtrl *p_rich = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT_PROGRAMDN);
	CString	str_rich = "\r\n��ֹͣ!\r\n";
	p_rich->SetSel(-1, -1);		
	p_rich->ReplaceSel(str_rich);
	GetDlgItem(IDC_BUTTON1)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_CLEARINFO)->EnableWindow(TRUE);
}


void CProgramDownDlg::OnDropFiles(HDROP hDropInfo)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	WCHAR wcStr[MAX_PATH];
	int DropCount = DragQueryFile(hDropInfo,-1,NULL,0);
	//for(int i = 0;i<DropCount;i++)
	//{
	//	
	//	DragQueryFile(hDropInfo,i,(LPSTR)wcStr,MAX_PATH);
	//}
	DragQueryFile(hDropInfo,0,(LPSTR)wcStr,MAX_PATH);
	DragFinish(hDropInfo);
	CString file_path = (LPSTR)wcStr;
	m_file_path = file_path;
	SetDlgItemText(IDC_EDIT_DOWNPATH,file_path);
	//m_sScan2dCfgDlg.m_nCurFileInd = 0;
	CDialogEx::OnDropFiles(hDropInfo);
}


void CProgramDownDlg::OnBnClickedButtonClearinfo()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CRichEditCtrl *p_rich = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT_PROGRAMDN);
	p_rich->SetWindowText(" ");
}
