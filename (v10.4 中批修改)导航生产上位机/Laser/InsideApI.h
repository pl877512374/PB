#ifndef INSIDEAPI
#define INSIDEAPI

#include "stdafx.h"
#include "Mwic_32.h"

/**************************接口函数返回值错误类型汇总******************************/

#define				SUCCESS					0		//命令执行成功
//串口打开关闭中的错误
#define				PARITY_ERROR			-1005	//设置奇偶校验错误
#define				CLOSE_ERROR				-1001	//关闭错误
//数据传输中的错误
#define				TIMEOUT_SEND			-1007	//发送时超时
#define				TIMEOUT_RECV			-1008	//接收时超时
#define				NOT_OPEN				-1009	//串口尚未打开
#define				BCC_ERROR				-1010	//接收数据校验位错误
#define				ERROR_CODE_VST			-1011	//命令执行不成功,一般是作为VST错误的返回
#define				ERROR_CMDTYPE			-1012	//返回命令号不正确
//根据ErrorCode可以有以下几种错误
#define				TIMEOUT_DSRC			-1013	//无线通讯超时
#define				ERROR_CODE_CMD			-1014	//命令执行不成功,作为功能性命令帧的返回
#define				OTHER_ERROR				-2000	//其他错误

#define ERR_ALREADY_OPEN	0
#define ERR_PARAM			-1000
#define ERR_OPEN			-1002
#define ERR_SET_PARAM		-2001
#define ERR_SET_TIMEOUT	-2002

#define ERR_NOT_OPEN			-1001

#define ERR_TIME_OUT			100		//读取超时时间
#define ERR_DATA_WRONG		10			// 接收到的数据错误



/* 分析接收信息帧 */
int ParsePkg(BYTE * buf,int len);


//打开设备端口
int OpenComm(unsigned char * dev);
int CloseComm();

int zIn_Com_Open(BYTE *com_num,DWORD dwBaud, char parity,int bytesize,char *stopbit);
int zIn_Comm_Send(BYTE *buf,int len);
int zIn_Comm_Rcv(BYTE *in_buff,int * recv_len);

unsigned char zIn_pkt_code(unsigned char *src_buf, unsigned char *target_buf,unsigned int len);
unsigned char zIn_pkt_code(unsigned char *src_buf, unsigned char *target_buf,unsigned int len);
unsigned char zIn_pkt_decode(unsigned char *buf, int &len);
int zIn_RecDataConver(unsigned char * ConverBuf,int BufCount);
int zIn_SendDataConver(unsigned char * ConverBuf,int BufCount);
int Serial_TransChannelSend(unsigned char *content,int length);

UINT __stdcall zIn_MyComThreadFunction( LPVOID pParam );


extern HANDLE GETFACTORYID_EVENT_rs;
extern HANDLE REPLACESKEY_EVENT_rs;
extern HANDLE CALCSENDRANDS_EVENT_rs;
extern HANDLE READPAGE_EVENT_rs;
extern HANDLE WRITEMEMO_EVENT_rs;
extern HANDLE CHECKKEYSTATE_EVENT_rs;
extern HANDLE GETFACTORYID_EVENT_NET_rs; // add by penglei
extern HANDLE REPLACESKEY_EVENT_NET_rs;
extern HANDLE CALCSENDRANDS_EVENT_NET_rs;
extern HANDLE READPAGE_EVENT_NET_rs;
extern HANDLE WRITEMEMO_EVENT_NET_rs;
extern HANDLE CHECKKEYSTATE_EVENT_NET_rs;
extern HANDLE ANALYSISDATA_EVENT_NET_rs;
extern BYTE ComRecvBuf[1024];
extern int ComRecvLen;


// 串口
extern BYTE ComRecvBuf[1024];
extern int ComRecvLen;
extern int	g_serial_fd;			// 串口文件描述符
extern unsigned int g_com_rx_len;			// 串口接收数据长度
extern unsigned int	g_com_tx_len;			// 串口发送数据长度
extern unsigned char 	g_com_rx_buf[512];			// 串口接收数据
extern unsigned char  	g_com_tx_buf[512];			// 串口发送数据 
extern unsigned int 	g_com_len;				
extern unsigned char 	g_com_buf[512];
extern unsigned short 	g_start_flag;//判断包头所用
extern unsigned int 	g_frame_flag;//串口组包标志
extern BOOL g_com_recv;

extern HANDLE GETFACTORYID_EVENT_NET_rs; // add by penglei
extern HANDLE REPLACESKEY_EVENT_NET_rs;
extern HANDLE CALCSENDRANDS_EVENT_NET_rs;
extern HANDLE READPAGE_EVENT_NET_rs;
extern HANDLE WRITEMEMO_EVENT_NET_rs;
extern HANDLE CHECKKEYSTATE_EVENT_NET_rs;
extern HANDLE ANALYSISDATA_EVENT_NET_rs;
#endif