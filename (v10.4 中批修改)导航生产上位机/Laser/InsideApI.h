#ifndef INSIDEAPI
#define INSIDEAPI

#include "stdafx.h"
#include "Mwic_32.h"

/**************************�ӿں�������ֵ�������ͻ���******************************/

#define				SUCCESS					0		//����ִ�гɹ�
//���ڴ򿪹ر��еĴ���
#define				PARITY_ERROR			-1005	//������żУ�����
#define				CLOSE_ERROR				-1001	//�رմ���
//���ݴ����еĴ���
#define				TIMEOUT_SEND			-1007	//����ʱ��ʱ
#define				TIMEOUT_RECV			-1008	//����ʱ��ʱ
#define				NOT_OPEN				-1009	//������δ��
#define				BCC_ERROR				-1010	//��������У��λ����
#define				ERROR_CODE_VST			-1011	//����ִ�в��ɹ�,һ������ΪVST����ķ���
#define				ERROR_CMDTYPE			-1012	//��������Ų���ȷ
//����ErrorCode���������¼��ִ���
#define				TIMEOUT_DSRC			-1013	//����ͨѶ��ʱ
#define				ERROR_CODE_CMD			-1014	//����ִ�в��ɹ�,��Ϊ����������֡�ķ���
#define				OTHER_ERROR				-2000	//��������

#define ERR_ALREADY_OPEN	0
#define ERR_PARAM			-1000
#define ERR_OPEN			-1002
#define ERR_SET_PARAM		-2001
#define ERR_SET_TIMEOUT	-2002

#define ERR_NOT_OPEN			-1001

#define ERR_TIME_OUT			100		//��ȡ��ʱʱ��
#define ERR_DATA_WRONG		10			// ���յ������ݴ���



/* ����������Ϣ֡ */
int ParsePkg(BYTE * buf,int len);


//���豸�˿�
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


// ����
extern BYTE ComRecvBuf[1024];
extern int ComRecvLen;
extern int	g_serial_fd;			// �����ļ�������
extern unsigned int g_com_rx_len;			// ���ڽ������ݳ���
extern unsigned int	g_com_tx_len;			// ���ڷ������ݳ���
extern unsigned char 	g_com_rx_buf[512];			// ���ڽ�������
extern unsigned char  	g_com_tx_buf[512];			// ���ڷ������� 
extern unsigned int 	g_com_len;				
extern unsigned char 	g_com_buf[512];
extern unsigned short 	g_start_flag;//�жϰ�ͷ����
extern unsigned int 	g_frame_flag;//���������־
extern BOOL g_com_recv;

extern HANDLE GETFACTORYID_EVENT_NET_rs; // add by penglei
extern HANDLE REPLACESKEY_EVENT_NET_rs;
extern HANDLE CALCSENDRANDS_EVENT_NET_rs;
extern HANDLE READPAGE_EVENT_NET_rs;
extern HANDLE WRITEMEMO_EVENT_NET_rs;
extern HANDLE CHECKKEYSTATE_EVENT_NET_rs;
extern HANDLE ANALYSISDATA_EVENT_NET_rs;
#endif