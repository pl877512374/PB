// ArmUartDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Laser.h"
#include "ArmUartDlg.h"
#include "afxdialogex.h"
#include "Mwic_32.h"
#include "MainFrm.h"
#include "InsideApI.h"
BOOL g_bOpenCom1 = FALSE;
// CArmUartDlg �Ի���
unsigned char g_u8ArmUartBuf[1024];
IMPLEMENT_DYNAMIC(CArmUartDlg, CDialogEx)

	UINT ThreadSearchArmUart(LPVOID pParam);

CArmUartDlg::CArmUartDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CArmUartDlg::IDD, pParent)
{

}

CArmUartDlg::~CArmUartDlg()
{
}

void CArmUartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_BUTTON_OPENARMUART, m_btnOpenArmUart);
	DDX_Control(pDX, IDC_COMBO_BAUDARM, m_CombBaudArm);
	DDX_Control(pDX, IDC_COMBO_COMARM, m_CombComArm);
	DDX_Control(pDX, IDC_EDIT_NET_IR, m_NetIR);
	//  DDX_Control(pDX, IDC_EDIT_NET_RXFSR, m_NetRxFsr);
	DDX_Control(pDX, IDC_EDIT_NET_RXRSR, m_Net_RxRsr);
	DDX_Control(pDX, IDC_EDIT_NET_SSR, m_Net_ssr);
	DDX_Control(pDX, IDC_EDIT_NET_TXFSR, m_Net_TxFsr);
	DDX_Control(pDX, IDC_EDIT_WIRE_ADDR, m_WireAddr);
	DDX_Control(pDX, IDC_EDIT_WIRE_DATA, m_WireData);
	DDX_Control(pDX, IDC_BUTTON_SEARCHARMUART, m_btnSearchArmUart);
}


BEGIN_MESSAGE_MAP(CArmUartDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_OPENARMUART, &CArmUartDlg::OnBnClickedButtonOpenarmuart)
	ON_BN_CLICKED(IDC_BUTTON_SEARCHARMUART, &CArmUartDlg::OnBnClickedButtonSearcharmuart)
END_MESSAGE_MAP()


// CArmUartDlg ��Ϣ�������


void CArmUartDlg::OnBnClickedButtonOpenarmuart()
{
	//// TODO: �ڴ���ӿؼ�֪ͨ����������
	int ret = 0;
	unsigned char str[5] = "com2";

	int com = 0;
	com = m_CombComArm.GetCurSel() + 1;
	str[3] = com+'0';
	static int ComState;
	if (ComState == 0)
	{
		ret = OpenComm(str);

		if (ret == 1)
		{
			//MessageBox(_T("�򿪴��ڳɹ�"));
			m_btnOpenArmUart.SetWindowText("�Ͽ�28E01");
			m_CombComArm.EnableWindow(FALSE);

			ComState = 1;
		}
		else
		{
			MessageBox(_T("�򿪴���ʧ��"));

		}
	}
	else
	{
		ret = CloseComm();
		if (ret == 0)
		{
			MessageBox(_T("�رմ��ڳɹ�"));
			m_CombComArm.EnableWindow(TRUE);
			ComState = 0;
			m_btnOpenArmUart.SetWindowText("����28E01");
		}
		else
		{
			MessageBox(_T("�رմ���ʧ��"));
		}
	}
}


void CArmUartDlg::OnBnClickedButtonSearcharmuart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������


}
extern HANDLE SearchArmUart_EVENT_rs; // add by penglei

unsigned char zIn_pkt_decode_uart(unsigned char *buf, int &len)
{
	unsigned int i=0, j=0;
	unsigned char chk = 0;	
	// ȥ��ͷ
	if (buf[0] == 0xff)
		i++;
	if (buf[1] == 0xff)
		i++;
	do 
	{
		if (buf[i] == 0xff) // �Ƿ�β
		{ 
			break;
		} 
		else 
		{
			buf[j] = buf[i];
			if (buf[i] == 0xfe) 
			{
				buf[j] |= buf[i+1];
				i++;	
			}
			chk ^= buf[j];
			i++;
			j++;
		}
	}while (1);

	// У����ȷ
	//if ((j > 1) && (chk == 0))//��ʱ���ε� ���λ
	if ((j > 1) )//��ʱ���ε� ���λ
	{
		len = j-1; // ȥ��У��λ

		for( i=0; i<len; i++)
		{
			g_com_rx_buf[i] = buf[i];
		}
		g_com_rx_len = len;

		return 0;
	} 
	else 
	{
		return 1; // ����
	}

}
UINT ThreadSearchArmUart(LPVOID pParam)
{

	CMainFrame *pMainDlg;
	CArmUartDlg *pDlg = (CArmUartDlg *)pParam;
	pMainDlg = (CMainFrame *)AfxGetApp()->m_pMainWnd;
	pDlg->m_btnSearchArmUart.EnableWindow(false);
	unsigned char *temp = new unsigned char[2];
	temp[0] = 0xaa;
	temp[1] = 0x55;

	int ret = Serial_TransChannelSend(temp,2);	
	//	page_data = new unsigned char [32];
	int TimeOut = 1000;
	while(1)
	{ResetEvent(SearchArmUart_EVENT_rs);
	DWORD dRet = WaitForSingleObject(SearchArmUart_EVENT_rs,INFINITE);
	if(dRet==WAIT_OBJECT_0)
	{

		ret = zIn_pkt_decode(g_u8ArmUartBuf, ComRecvLen); //����,У���
		if (ret == 0)
		{

			CString str;
			str.Format("0x%x",((g_u8ArmUartBuf[2] << 8) + g_u8ArmUartBuf[3]));
			pDlg->m_WireData.SetWindowText(str);
			str.Format("0x%x",((g_u8ArmUartBuf[4] << 8) + g_u8ArmUartBuf[5]));
			pDlg->m_WireAddr.SetWindowText(str);
			str.Format("0x%x",((g_u8ArmUartBuf[6] << 8) + g_u8ArmUartBuf[7]));
			pDlg->m_Net_ssr.SetWindowText(str);
			str.Format("0x%x",((g_u8ArmUartBuf[8] << 8) + g_u8ArmUartBuf[9]));
			pDlg->m_NetIR.SetWindowText(str);
			str.Format("0x%x",((g_u8ArmUartBuf[10] << 24) +(g_u8ArmUartBuf[11] << 16) +(g_u8ArmUartBuf[12] << 8) + g_u8ArmUartBuf[13]));
			pDlg->m_Net_TxFsr.SetWindowText(str);
			str.Format("0x%x",((g_u8ArmUartBuf[14] << 24) +(g_u8ArmUartBuf[15] << 16) +(g_u8ArmUartBuf[16] << 8) + g_u8ArmUartBuf[17]));
			pDlg->m_Net_RxRsr.SetWindowText(str);
		} 
	}
	}
	/*else
	{

	AfxMessageBox("��ѯʧ��");
	}*/
	pDlg->m_btnSearchArmUart.EnableWindow(true);
	return 1;
}

BOOL CArmUartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	AfxBeginThread(ThreadSearchArmUart,this,THREAD_PRIORITY_NORMAL,0,0,NULL);
	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SearchArmUart_EVENT_rs = CreateEvent(NULL,true,true,"SearchArmUart_EVENT_rs");
	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}
