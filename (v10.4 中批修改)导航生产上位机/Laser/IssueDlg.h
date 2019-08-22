#pragma once
#pragma comment (lib,"NetPublish.lib")
#pragma comment (linker,"/NODEFAULTLIB:libc.lib")   //低版本编译的静态库加载到高版本中的时候会报找不到libc.lib的错误，使用该语句忽略加载
#pragma comment(lib,"ws2_32.lib")
//常量定义
#define ICC_COMMAND_SUCCESS 0x9000
#define ICC_COMMAN_BUF_LEN      64 
#define MAX_LEADERNUM			2

//卡座设备标识符
#define CARD_TYPE_UP	  0
#define CARD_TYPE_SAM1    1
#define CARD_TYPE_SAM2    2
#define CARD_TYPE_SAM3    3
#define CARD_TYPE_DEFAULT  2
#define CARD_TYPE_WATCH   5

#define USER_SUCCESS 1
#define USER_FAILED  -1
#define USER_READBINARYERR  -3
#define USER_MAXISSUEERR   -4
//卡机与卡片的通讯波特率
#define CARD_BAUD_9600    0
#define CARD_BAUD_55800   1
#define CARD_BAUD_115200  2
#define CARD_BAUD_DEFAULT 0
extern WSADATA wsaData;									//指向WinSocket信息结构的指针
extern SOCKADDR_IN sin1,SockAddrIN_Client;							//设置两个地址，sin1用来绑定
extern SOCKADDR_IN SockAddrIN_UdpServ;								//指向通信对象的结构体指针 
extern SOCKET sockListener;
extern int m_nLocalUdpPort;//记录 发送广播帧时本机udp使用的端口号

//网络程序下载
#define GET_CHIPID_NET_RQ	(0x3073)
#define GET_CHIPID_NET_RS	((unsigned char)0x80)
#define REPLACEKEY_NET_RQ	(0x3173)
#define REPLACEKEY_NET_RS	((unsigned char)0x81)
#define CALRAND_NET_RQ	(0x3273)
#define CALRAND_NET_RS	((unsigned char)0x82)
#define READPAGE_NET_RQ	(0x3373)
#define READPAGE_NET_RS	((unsigned char)0x83)
#define WRITEMEMO_NET_RQ	(0x3473)
#define WRITEMEMO_NET_RS	((unsigned char)0x84)
#define CHECKKEYSTATE_NET_RQ	(0x3573)
#define CHECKKEYSTATE_NET_RS	((unsigned char)0x85)

//广播线程句柄及广播线程
extern HANDLE hthread_BroadcastThread;
extern DWORD WINAPI ThreadFuct_BroadCastThread(void *data);
// CIssueDlg 对话框
typedef struct SAVE_DATA 
{

	unsigned char st_chipID[8];      //芯片的ID号
	unsigned char st_subkey[8];   //密钥明文
	unsigned char st_sect_subkey[8]; //密钥密文
	unsigned char st_rand[8];    //计算出来的随机数
	int st_page_rand;			//取0-3页中的一组数据
	int st_page_read;
	int st_page_sub;
	unsigned char st_hard_mac[21];  // DS28返回的MAC值
	unsigned char st_page_data[32]; //从芯片存储器中读取的32字节的数据
	unsigned char st_soft_mac[21];  //算法计算出的MAC值
	int st_page_write;
	int st_page_sub_write;
}REP_DATA;
static unsigned char SHAVM_Message[64];
static signed long SHAVM_KTN[4];
static signed short  SHAVM_cnt ;
static unsigned long   SHAVM_MTword[80];
static signed long   SHAVM_Temp;
static unsigned long   SHAVM_Hash[5];
static unsigned char SHAVM_MAC[20];
// CIssueDlg 对话框
#define ISSUELOGNUM 1000
typedef struct TagIssueLogInfo
{
	CString m_strbuf[ISSUELOGNUM];
	int m_n32In;
	int m_n32Out;
}IssueLogInfo;
class CIssueDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CIssueDlg)

public:
	CIssueDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CIssueDlg();

// 对话框数据
	enum { IDD = IDD_ISSUEDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonIssue();
	void ShowInfomation(CString& info);
	long CheckIssueMaxCnt(HANDLE iDevID, unsigned char CardType, unsigned char *Key);
	////网络下载密钥
	int  GetFactoryID_Net_rq(void); //1-1
	int  GetFactoryID_Net_rs(char* ID);
	int  ReplacesKey_Net_rq(unsigned char *chipID,unsigned char *srckey,unsigned char *subkey); //2
	int  ReplacesKey_Net_rs(void);  //2-2
	int  CalcSendRands_Net_rq(int page,unsigned char * rands);  //3
	int  CalcSendRands_Net_rs(unsigned char *hsmac);  //3
	int  ReadPage_Net_rq(int page);
	int  ReadPage_Net_rs(unsigned char *page_data);
	int  WriteMemo_Net_rq(int page, int subpage);
	int  WriteMemo_Net_rs(void);
	int  CheckLatestKeyVersion(REP_DATA sdata,int mode);
	long UpdateIssueMaxCnt(HANDLE iDevId, unsigned char CarType, unsigned char *Key);
	int WriteToBLog(CString strPath);
	int GetKeyState_rq();
	long IC_Command(HANDLE icdev, unsigned char CardType, int slen, unsigned char *send_data, int *rlen, unsigned char *receive_data);
	void SHAVM_Compute(void);
	int GetKeyState_rs();
	int CalcSendRands(int page,unsigned char * rands);
	int CIssueDlg::CalcSendRands_rs(unsigned char *hsmac);
	int ReadPage(int page);
	int ReadPage_rs(unsigned char *page_data);
	int GetKeyState_Net_rs();
	int GetKeyState_Net_rq();
	afx_msg LONG OnIssueLog(WPARAM wParam, LPARAM lParam); 
	CRichEditCtrl m_ShowInfo;
	HANDLE m_iDevId;
	CButton m_btnIssue;
	afx_msg void OnBnClickedButtonClearlist();
	afx_msg void OnBnClickedButtonOpenct();
	CButton m_OpenCom;
	CComboBox m_ComNum;
	CComboBox m_CombBaud;
	//HANDLE m_iDevId;
	long m_lBaud;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButtonCheckstate();
	CComboBox m_CombEquipSel;
	afx_msg void OnBnClickedButtonIssue2();
	CButton m_btnCheckState;
	afx_msg void OnTimer(UINT nIDEvent);
	
	
};
