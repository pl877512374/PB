// ArmUartDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "ArmUartDlg.h"
#include "afxdialogex.h"
#include "Mwic_32.h"
#include "MainFrm.h"
#include "InsideApI.h"
BOOL g_bOpenCom1 = FALSE;
// CArmUartDlg 对话框
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


// CArmUartDlg 消息处理程序


void CArmUartDlg::OnBnClickedButtonOpenarmuart()
{
	//// TODO: 在此添加控件通知处理程序代码
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
			//MessageBox(_T("打开串口成功"));
			m_btnOpenArmUart.SetWindowText("断开28E01");
			m_CombComArm.EnableWindow(FALSE);

			ComState = 1;
		}
		else
		{
			MessageBox(_T("打开串口失败"));

		}
	}
	else
	{
		ret = CloseComm();
		if (ret == 0)
		{
			MessageBox(_T("关闭串口成功"));
			m_CombComArm.EnableWindow(TRUE);
			ComState = 0;
			m_btnOpenArmUart.SetWindowText("连接28E01");
		}
		else
		{
			MessageBox(_T("关闭串口失败"));
		}
	}
}


void CArmUartDlg::OnBnClickedButtonSearcharmuart()
{
	// TODO: 在此添加控件通知处理程序代码


}
extern HANDLE SearchArmUart_EVENT_rs; // add by penglei

unsigned char zIn_pkt_decode_uart(unsigned char *buf, int &len)
{
	unsigned int i=0, j=0;
	unsigned char chk = 0;	
	// 去掉头
	if (buf[0] == 0xff)
		i++;
	if (buf[1] == 0xff)
		i++;
	do 
	{
		if (buf[i] == 0xff) // 是否到尾
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

	// 校验正确
	//if ((j > 1) && (chk == 0))//暂时屏蔽掉 检查位
	if ((j > 1) )//暂时屏蔽掉 检查位
	{
		len = j-1; // 去掉校验位

		for( i=0; i<len; i++)
		{
			g_com_rx_buf[i] = buf[i];
		}
		g_com_rx_len = len;

		return 0;
	} 
	else 
	{
		return 1; // 出错
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

		ret = zIn_pkt_decode(g_u8ArmUartBuf, ComRecvLen); //解码,校验等
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

	AfxMessageBox("查询失败");
	}*/
	pDlg->m_btnSearchArmUart.EnableWindow(true);
	return 1;
}

BOOL CArmUartDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	AfxBeginThread(ThreadSearchArmUart,this,THREAD_PRIORITY_NORMAL,0,0,NULL);
	// TODO:  在此添加额外的初始化
	SearchArmUart_EVENT_rs = CreateEvent(NULL,true,true,"SearchArmUart_EVENT_rs");
	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}
