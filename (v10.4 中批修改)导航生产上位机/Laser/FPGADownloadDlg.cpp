// FPGADownloadDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Laser.h"
#include "FPGADownloadDlg.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "FileUtils.h"


#define FPGASEND_MAX 256

char FPGApbuffer[292]={0};
int FPGAfile_index=0;
int FPGAfile_indexLast=0;    //上一次的发送包序号 
bool FPGADownloadFlag=false;    //FPGA下载标志位
HANDLE g_FPGAevnet = CreateEvent(NULL,FALSE,FALSE,NULL);
CEvent Event_SUCCEED;//单包烧写成功
CEvent Event_ERROR;//单包烧写错误
HANDLE Event_FPGAList[2] = {Event_SUCCEED,Event_ERROR};
UINT SendFPGADataThreadProc(LPVOID lpParm);    //烧写FPGA数据线程函数
CMainFrame* pMainFrameFPGAThis;
int m_DownLoadType = 0;  //下载程序类型，  0：FPGA  1：BOOT

// CFPGADownloadDlg 对话框

IMPLEMENT_DYNAMIC(CFPGADownloadDlg, CDialogEx)

CFPGADownloadDlg::CFPGADownloadDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CFPGADownloadDlg::IDD, pParent)
	, m_FPGAfile_path(_T(""))
{

}

CFPGADownloadDlg::~CFPGADownloadDlg()
{
}

void CFPGADownloadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_FPGAPATH, m_FPGAfile_path);
	DDX_Control(pDX, IDC_COMBO_LOADCHOOSE, m_CombLoadChoose);
}


BEGIN_MESSAGE_MAP(CFPGADownloadDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_GETFILE, &CFPGADownloadDlg::OnBnClickedButtonGetfile)
	ON_BN_CLICKED(IDC_BUTTON_BEGINDOWNLOAD, &CFPGADownloadDlg::OnBnClickedButtonBegindownload)
	ON_BN_CLICKED(IDC_BUTTON_STOPDOWNLOAD, &CFPGADownloadDlg::OnBnClickedButtonStopdownload)
	ON_BN_CLICKED(IDC_BUTTON_FPGACLEARINFO, &CFPGADownloadDlg::OnBnClickedButtonFpgaclearinfo)
	ON_CBN_SELCHANGE(IDC_COMBO_LOADCHOOSE, &CFPGADownloadDlg::OnSelchangeComboLoadchoose)
END_MESSAGE_MAP()


// CFPGADownloadDlg 消息处理程序


BOOL CFPGADownloadDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	pMainFrameFPGAThis = (CMainFrame*)AfxGetMainWnd();
	SetDlgItemText(IDC_EDIT_FPGAPATH,"./data.rpd");
	m_FPGAfile_path = "./data.rpd";

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}


void CFPGADownloadDlg::OnBnClickedButtonGetfile()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CString file_path;
	char path_temp[1024];
	GetCurrentDirectory(128,path_temp);
	if (m_CombLoadChoose.GetCurSel()==0)
	{
		CFileDialog filedlg(TRUE,"*.rpd",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"rpd files(*.rpd)|*.rpd|All files(*.*)|*.*||");
		filedlg.m_ofn.lpstrInitialDir = path_temp;
		filedlg.DoModal();
		file_path = filedlg.GetPathName();
	}
	else if(m_CombLoadChoose.GetCurSel()==1)
	{
		CFileDialog filedlg(TRUE,"*.bin",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,"bin files(*.bin)|*.bin|All files(*.*)|*.*||");
		filedlg.m_ofn.lpstrInitialDir = path_temp;
		filedlg.DoModal();
		file_path = filedlg.GetPathName();
	}
	m_FPGAfile_path=file_path;
	SetDlgItemText(IDC_EDIT_FPGAPATH,file_path);
}


void CFPGADownloadDlg::OnBnClickedButtonBegindownload()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	GetDlgItem(IDC_BUTTON_BEGINDOWNLOAD)->EnableWindow(FALSE);
	GetDlgItem(IDC_COMBO_LOADCHOOSE)->EnableWindow(FALSE);
	CString l_sNameTmp;//打开文件的后缀名
	l_sNameTmp = m_FPGAfile_path.Right(m_FPGAfile_path.GetLength()-m_FPGAfile_path.ReverseFind('.')-1);
	CRichEditCtrl *p_rich = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT_FPGADN);
	CString str_rich="";
	m_DownLoadType=m_CombLoadChoose.GetCurSel();

	//检查文件类型
	if (m_DownLoadType == 0)
	{
		if(l_sNameTmp != "rpd")
		{
			str_rich = "打开文件失败o(>﹏<)o！\n请选择正确的FPGA程序文件！\n";
			p_rich->SetSel(-1, -1);		
			p_rich->ReplaceSel(str_rich);
			p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
			GetDlgItem(IDC_BUTTON_BEGINDOWNLOAD)->EnableWindow(TRUE);
			GetDlgItem(IDC_COMBO_LOADCHOOSE)->EnableWindow(TRUE);
			return;
		}
	}
	else if (m_DownLoadType == 1)
	{
		if(l_sNameTmp != "bin")
		{
			str_rich = "打开文件失败o(>﹏<)o！\n请选择正确的boot程序文件！\n";
			p_rich->SetSel(-1, -1);		
			p_rich->ReplaceSel(str_rich);
			p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
			GetDlgItem(IDC_BUTTON_BEGINDOWNLOAD)->EnableWindow(TRUE);
			GetDlgItem(IDC_COMBO_LOADCHOOSE)->EnableWindow(TRUE);
			return;
		}
	}
	
	//检查网口连接状态
	if (pMainFrameFPGAThis->m_NetConnection.bConnected)
	{
		str_rich = "开始发送数据\n";
		p_rich->SetSel(-1, -1);		
		p_rich->ReplaceSel(str_rich);
		p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);

		pMainFrameFPGAThis->KillTimer(0);      //停止循环发状态查询指令

		GetDlgItem(IDC_BUTTON_FPGACLEARINFO)->EnableWindow(FALSE);
		CWinThread* pThread;
		FPGADownloadFlag = true;
		pThread = AfxBeginThread(SendFPGADataThreadProc,this);
	}
	else
	{
		str_rich = "网络已断开，请连接网络！\n";
		p_rich->SetSel(-1, -1);		
		p_rich->ReplaceSel(str_rich);
		p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		GetDlgItem(IDC_BUTTON_BEGINDOWNLOAD)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_LOADCHOOSE)->EnableWindow(TRUE);
		return;
	} 
}

//烧写FPGA数据线程函数
UINT SendFPGADataThreadProc(LPVOID lpParm)    
{
	CFPGADownloadDlg* pDlg = (CFPGADownloadDlg*)lpParm;
	CRichEditCtrl *p_rich = (CRichEditCtrl *)pDlg->GetDlgItem(IDC_RICHEDIT_FPGADN);
	CString str_rich="";

	memset(FPGApbuffer,'\0',292);
	long FPGAfile_len=0;  //总长字节数
	FILE *FPGAfp=NULL;   
	int FPGAsend_len=0;  //每包中发送FPGA内容字节数
	int RetrySendNum=0;  //重发次数
	DWORD dw;
	FPGAfile_index=0;
	FPGAfile_indexLast=-1;
	int TotalPack=0;   //总包数
	char zhiling[36]={0xFF,0xAA,0x01,0x20,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x00,0x07,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x14,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	FPGAfp=fopen(pDlg->m_FPGAfile_path,"rb");
	if(FPGAfp==NULL)
	{
		str_rich = "程序文件为空\n";
		p_rich->SetSel(-1, -1);		
		p_rich->ReplaceSel(str_rich);
		p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		pDlg->GetDlgItem(IDC_BUTTON_BEGINDOWNLOAD)->EnableWindow(TRUE);
		pDlg->GetDlgItem(IDC_COMBO_LOADCHOOSE)->EnableWindow(TRUE);
		pDlg->GetDlgItem(IDC_BUTTON_FPGACLEARINFO)->EnableWindow(TRUE);
		return 0;
	}
	fseek(FPGAfp,0,SEEK_END);
	FPGAfile_len = ftell(FPGAfp);
	fseek(FPGAfp,0,SEEK_SET);
	
	if (FPGAfile_len%FPGASEND_MAX==0)
	{
		TotalPack=FPGAfile_len/FPGASEND_MAX;
	} 
	else
	{
		TotalPack=1+FPGAfile_len/FPGASEND_MAX;
	}		

	while(FPGADownloadFlag==true)
	{
		if (FPGAfile_indexLast != FPGAfile_index)
		{
			if (FPGAfile_index < (TotalPack-1))
			{
				FPGAsend_len=FPGASEND_MAX;
				fread(FPGApbuffer+30,FPGASEND_MAX,1,FPGAfp);
			}
			else if (FPGAfile_index == (TotalPack-1))
			{
				if (m_DownLoadType == 0)
				{
					memset(FPGApbuffer,0xFF,292);
				}
				else if (m_DownLoadType == 1)
				{
					memset(FPGApbuffer,0x00,292);
				}
				
				FPGAsend_len=(FPGAfile_len-FPGAfile_index*FPGASEND_MAX);
				fread(FPGApbuffer+30,FPGAsend_len,1,FPGAfp);
			}
			else
			{
				//结束
				str_rich.Format("%d",TotalPack);
				str_rich = "烧写完成，总共烧写 " + str_rich + " 包\n";
				p_rich->SetSel(-1, -1);		
				p_rich->ReplaceSel(str_rich);
				p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
				FPGADownloadFlag=false;
				fclose(FPGAfp);
				pDlg->GetDlgItem(IDC_BUTTON_BEGINDOWNLOAD)->EnableWindow(TRUE);
				pDlg->GetDlgItem(IDC_COMBO_LOADCHOOSE)->EnableWindow(TRUE);
				pDlg->GetDlgItem(IDC_BUTTON_FPGACLEARINFO)->EnableWindow(TRUE);
				break;
			}

			//数据组包
			CTime time=CTime::GetCurrentTime();
			char Hour= time.GetHour();
			char Miu= time.GetMinute();
			char Sec= time.GetSecond();
			zhiling[7]=Hour;
			zhiling[8]=Miu;
			zhiling[9]=Sec;
			if(m_DownLoadType == 1)
			{
				zhiling[23]=0x15;
			}
			zhiling[26]=(TotalPack>>8);
			zhiling[27]=(TotalPack & 0xff);
			zhiling[28]=(FPGAfile_index>>8);
			zhiling[29]=(FPGAfile_index & 0xff);
			memcpy(&FPGApbuffer[0],zhiling,30);
			FPGApbuffer[286]=0x00;
			FPGApbuffer[287]=Xor_download(0x00,&FPGApbuffer[30],FPGASEND_MAX);
			FPGApbuffer[290]=0xEE;
			FPGApbuffer[291]=0xEE;
		}
		
		if (RetrySendNum < 10)
		{
			//发送数据
			//CMainFrame *pMainDlg;
			//pMainDlg = (CMainFrame *)AfxGetApp()->m_pMainWnd;
			pMainFrameFPGAThis->m_NetConnection.pcSendDataBuf = FPGApbuffer;
			pMainFrameFPGAThis->m_NetConnection.nSendDataBufSize = 292;
			if (pMainFrameFPGAThis->m_NetConnection.bConnected)
				SendData(&pMainFrameFPGAThis->m_NetConnection);
			FPGAfile_indexLast = FPGAfile_index; 

			str_rich.Format("共烧写 %d 包，发送第 %d 包\n",TotalPack,FPGAfile_index);
			p_rich->SetSel(-1, -1);		
			p_rich->ReplaceSel(str_rich);
			p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
		}
		else
		{
			str_rich = "本次烧写失败，请重新开始烧写\n";
			p_rich->SetSel(-1, -1);		
			p_rich->ReplaceSel(str_rich);
			p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
			pDlg->GetDlgItem(IDC_BUTTON_BEGINDOWNLOAD)->EnableWindow(TRUE);
			pDlg->GetDlgItem(IDC_COMBO_LOADCHOOSE)->EnableWindow(TRUE);
			pDlg->GetDlgItem(IDC_BUTTON_FPGACLEARINFO)->EnableWindow(TRUE);
			return 0;
		}

		dw = WaitForMultipleObjects(2,Event_FPGAList,false,10000);	//等待超时 //等事件  调试用 INFINITE   10000
		if (dw == WAIT_TIMEOUT)
		{
			//超时未收到回复
			str_rich.Format("%d",FPGAfile_index);
			str_rich = "烧写第 " + str_rich + " 包程序超时，重新烧写本包\n";
			p_rich->SetSel(-1, -1);		
			p_rich->ReplaceSel(str_rich);
			p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
			RetrySendNum++;
		} 
		else if(dw == WAIT_OBJECT_0)  //Event_SUCCEED
		{ 
			/*str_rich.Format("%d",FPGAfile_index);
			str_rich = "烧写第 " + str_rich + " 包FPGA程序成功\n";
			p_rich->SetSel(-1, -1);		
			p_rich->ReplaceSel(str_rich);*/
			FPGAfile_index++;
			RetrySendNum=0;
		}
		else if(dw == (WAIT_OBJECT_0 + 1))   //Event_ERROR
		{
			/*str_rich.Format("%d",FPGAfile_index);
			str_rich = "烧写第 " + str_rich + " 包FPGA程序错误，重新烧写本包\n";
			p_rich->SetSel(-1, -1);		
			p_rich->ReplaceSel(str_rich);*/
			RetrySendNum++;
		}
		Sleep(1);
	}

	pMainFrameFPGAThis->SetTimer(0,500,NULL);       //循环发状态查询指令
	return 0;
}

void CFPGADownloadDlg::OnBnClickedButtonStopdownload()
{
	// TODO: 在此添加控件通知处理程序代码
	FPGADownloadFlag=false;
	Sleep(500);
	CRichEditCtrl *p_rich = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT_FPGADN);
	CString str_rich="";
	str_rich = "停止烧写成功\n";
	p_rich->SetSel(-1, -1);		
	p_rich->ReplaceSel(str_rich);
	p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
	GetDlgItem(IDC_BUTTON_BEGINDOWNLOAD)->EnableWindow(TRUE);
	GetDlgItem(IDC_COMBO_LOADCHOOSE)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_FPGACLEARINFO)->EnableWindow(TRUE);
}


void CFPGADownloadDlg::OnBnClickedButtonFpgaclearinfo()
{
	// TODO: 在此添加控件通知处理程序代码
	CRichEditCtrl *p_rich = (CRichEditCtrl *)GetDlgItem(IDC_RICHEDIT_FPGADN);
	p_rich->SetWindowText(" ");
	p_rich->PostMessage(WM_VSCROLL, SB_BOTTOM, 0);
}


void CFPGADownloadDlg::OnSelchangeComboLoadchoose()
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_CombLoadChoose.GetCurSel() == 0)
	{
		SetDlgItemText(IDC_EDIT_FPGAPATH,"./data.rpd");
		m_FPGAfile_path = "./data.rpd";
	}
	else if(m_CombLoadChoose.GetCurSel() == 1)
	{
		SetDlgItemText(IDC_EDIT_FPGAPATH,"./data.bin");
		m_FPGAfile_path = "./data.bin";
	}
}
