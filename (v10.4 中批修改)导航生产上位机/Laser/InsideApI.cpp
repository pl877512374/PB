#include "stdafx.h"
#include "InsideApI.h"
#include <windows.h>
#include <process.h>

HANDLE hCom = 0;	//全局变量，串口句柄

extern unsigned char g_u8ArmUartBuf[1024];
HANDLE GETFACTORYID_EVENT_rs; // add by penglei
HANDLE REPLACESKEY_EVENT_rs;
HANDLE CALCSENDRANDS_EVENT_rs;
HANDLE READPAGE_EVENT_rs;
HANDLE WRITEMEMO_EVENT_rs;
HANDLE CHECKKEYSTATE_EVENT_rs;

HANDLE GETFACTORYID_EVENT_NET_rs; // add by penglei
HANDLE REPLACESKEY_EVENT_NET_rs;
HANDLE CALCSENDRANDS_EVENT_NET_rs;
HANDLE READPAGE_EVENT_NET_rs;
HANDLE WRITEMEMO_EVENT_NET_rs;
HANDLE CHECKKEYSTATE_EVENT_NET_rs;
HANDLE ANALYSISDATA_EVENT_NET_rs;


HANDLE SearchArmUart_EVENT_rs; // add by penglei

 unsigned int g_com_rx_len = 0;			// 串口接收数据长度
 unsigned int	g_com_tx_len;			// 串口发送数据长度
 unsigned char 	g_com_rx_buf[512];			// 串口接收数据
 unsigned char  	g_com_tx_buf[512];			// 串口发送数据 
 unsigned int 	g_com_len = 0;				
 unsigned char 	g_com_buf[512];
 unsigned short 	g_start_flag = 0;//判断包头所用
 unsigned int 	g_frame_flag = 0;//串口组包标志







/*
* 函数名称：	pkt_code()
* 功能描述：	发送数据转义
* 参数列表：	
*			src_buf			--	转义前数据
*			target_buf		--	转义后数据
*			len				--	转义前数据长度
* 返回结果：	
*			转义后数据长度	
*/
unsigned char zIn_pkt_code(unsigned char *src_buf, unsigned char *target_buf,unsigned int len)
{
	unsigned char i = 0;
	unsigned char j = 0;

	// 帧头
	target_buf[i++]=src_buf[0];
	target_buf[i++]=src_buf[1];
	// 数据
	for (j=2; j<len-1; j++)
	{
		if (src_buf[j]==0xFF)
		{
			target_buf[i++]=0xFE;
			target_buf[i++]=0x01;
		}
		else if (src_buf[j]==0xFE)
		{
			target_buf[i++]=0xFE;
			target_buf[i++]=0x00;
		}
		else
		{
			target_buf[i++]=src_buf[j];
		}
	}
	// 帧尾
	target_buf[i++]=0xFF;
	return i;	
}	


/*
* 函数名称：	pkt_decode()
* 功能描述：	接收数据包的解码工作
* 参数列表：	
*			buf			--	接收到的数据
*			len			--	数据长度
*			port		--  端口类型，串口或网口
* 返回结果：	
*			ERR_NO		--	数据校验成功	
*			ERR_CHECK_WRONG	--	数据校验失败	
*/
unsigned char zIn_pkt_decode(unsigned char *buf, int &len)
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

/* 分析接收信息帧 */
int zIn_ParsePkg(BYTE * buf,int len)
{
	int start;
	int i,j;
	BYTE chk = 0;

	//分析包头的正确性
	if( buf[0] != 0xff  )
		return OTHER_ERROR;

	if(buf[1] == 0xff)
		start = 2;
	else
		start = 1;

	j = 0;
	//去掉了开始位和停止位
	for(i = start;i < len -1;i++)
	{
		if(buf[i] == 0xff)
			return OTHER_ERROR;

		buf[j] = buf[i];
		//还原为0xff的情况
		if( buf[i] == 0xfe )
		{
			buf[j] |= buf[i + 1];
			i++;
		}
		j++;
	}

	len = j;

	//检验异或校验值
	for(i = 0;i < len-1;i++)
		chk ^= buf[i];
	if(chk != buf[len - 1])
		return BCC_ERROR;

	return SUCCESS;

}

/* ******************************************************************************************** */
/* *************************************串口底层接口******************************************* */
/* ******************************************************************************************** */

//打开串口
int zIn_Com_Open(BYTE *com_num,DWORD dwBaud, char parity,int bytesize,char *stopbit)
{
	DCB dcb;
	
	hCom=CreateFile((char *)com_num,//COM1口
		GENERIC_READ|GENERIC_WRITE, //允许读和写
		0, //独占方式
		NULL,
		OPEN_EXISTING, //打开而不是创建
		0, 
		NULL);
	
	if(hCom==INVALID_HANDLE_VALUE)
	{
		//设备被占用
		if(GetLastError() == 5)
		{
			return ERR_ALREADY_OPEN;
		}
		//设备打开失败，通常指无此设备
		if(GetLastError() == 2)
		{
			return ERR_OPEN;
		}
		//其他错误
		return ERR_OPEN;
	}

	
	SetupComm(hCom,1024,1024); 

	if(GetCommState(hCom,&dcb))
	{
		dcb.BaudRate=dwBaud;
		dcb.ByteSize=bytesize;
		//Setup the Parity
  
		switch (parity)
		{
			case 'E':  
				dcb.Parity = EVENPARITY;  
				break;
			case 'N':    
				dcb.Parity = NOPARITY;    
				break;
			case 'O':   
				dcb.Parity = ODDPARITY;   
				break;
			default:          
				return PARITY_ERROR;          
		}

		if(strlen(stopbit) > 1)
			dcb.StopBits=ONE5STOPBITS;
		else if(atoi(stopbit) == 1)
			dcb.StopBits = ONESTOPBIT;
		else
			dcb.StopBits = TWOSTOPBITS;

		//设置串口属性错误
		if(!SetCommState(hCom,&dcb))
		{
			CloseHandle(hCom);
			hCom = 0;
			return ERR_SET_PARAM;
		}
		else 
		{
			PurgeComm(hCom,PURGE_TXCLEAR|PURGE_RXCLEAR);
	//		ret = atoi(&com_num[3]);
						
			return 1; //串口返回1
		}
	
	}
	else//读取串口属性错误，归到其他错误
	{
		CloseHandle(hCom);
		hCom = 0;
		return ERR_SET_PARAM;
	}

}

//串口发送数据包
int zIn_Comm_Send(BYTE *buf,int len)
{
	OVERLAPPED m_Write;
	COMSTAT ComStat;
	BOOL bWriteStat;
	DWORD real_len;
	DWORD dwErrorFlags;

	PurgeComm(hCom, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	/* *****************串口发包***************** */
	memset(&m_Write,0,sizeof(OVERLAPPED));
	//设置事件
	m_Write.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

	ClearCommError(hCom,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(hCom,buf,len,&real_len,&m_Write);

	if(!bWriteStat)
	{
		//表明串口正在进行写操作
		if(GetLastError()==ERROR_IO_PENDING)
		{
			//规定时间内仍未发送，则返回错误信息
			if( WaitForSingleObject(m_Write.hEvent,1000) == WAIT_TIMEOUT )
				return 2;    //失败
		}
		//无效句柄，表明串口已经关闭
		else if(GetLastError() == 6)
			return 4;        //串口未打开
		else
			return -1;
	}
	return SUCCESS;
}

//串口接收数据包，inbuff 接收到的数据长度
int  zIn_Comm_Rcv(BYTE *in_buff,int * recv_len)
{
	unsigned int i;
	COMSTAT ComStat;
	BOOL bReadStat;
	DWORD dwErrorFlags;
	DWORD dwBytesRead;//读取的字节数
	int k = 0;
	BYTE *pBuf = (BYTE *)in_buff;


	*recv_len = 0;//初始置零

	ClearCommError(hCom,&dwErrorFlags,&ComStat);
	if(ComStat.cbInQue>=1024)  ComStat.cbInQue=0;
	if(ComStat.cbInQue>0)
	{
		bReadStat=ReadFile(hCom,in_buff,ComStat.cbInQue,&dwBytesRead,NULL);
		
		for (i=0; i<ComStat.cbInQue; i++)
		{	
			if( g_frame_flag == 0 )
			{
				g_start_flag = (g_start_flag<<8) + (*pBuf);
				if(g_start_flag == 0xffff)		// 帧头
				{
					g_frame_flag = 1;
					ComRecvBuf[0] = 0xff;
					ComRecvBuf[1] = 0xff;
					ComRecvLen = 2;
				}
			}
			else if( g_frame_flag == 1 )
			{
				ComRecvBuf[ComRecvLen++] = *pBuf;
				if (ComRecvLen > 255)
				{
					g_start_flag = 0;
					g_frame_flag = 0;
					ComRecvLen =0;
					return ERR_DATA_WRONG;
				}
				if( *pBuf == 0xff)	// 帧尾
				{
					g_start_flag = 0;
					g_frame_flag = 0;
					ComRecvLen =0;
					if(ComRecvBuf[2] == 0xaa && ComRecvBuf[3] == 0x55)
					{
						memcpy(g_u8ArmUartBuf,ComRecvBuf,50);
						SetEvent(SearchArmUart_EVENT_rs);
					}
					//switch(ComRecvBuf[3])
					//{
					//case 0x00:	//接收芯片ID					
					//	SetEvent(GETFACTORYID_EVENT_rs);
					//	break;
					//case 0x01:
					//	SetEvent(REPLACESKEY_EVENT_rs);
					//	break;
					//case  0x02:
					//	SetEvent(CALCSENDRANDS_EVENT_rs);
					//	break;
					//case 0x03:
					//	SetEvent(READPAGE_EVENT_rs);
					//	break;
					//case 0x04:
					//	SetEvent(WRITEMEMO_EVENT_rs);
					//	break;
					//case 0xaa:
					//	SetEvent(CHECKKEYSTATE_EVENT_rs);
					//	break;

					//default:
					//	break;
					//}
				}
			}
			pBuf++;
		}
	}

	return SUCCESS;

}

/* ******************************************************************************************** */
/* *************************************网络底层接口******************************************* */
/* ******************************************************************************************** */

/*发送信息串转义组合*/
int zIn_SendDataConver(unsigned char * ConverBuf,int BufCount)
{
	unsigned char tmpbuf[1024];
	int tmpcount,i,j;
	unsigned char byteTemp;
	
	tmpbuf[0]=0xFF;
	tmpbuf[1]=0xFF;
	i=2;
	for (j=0;j<BufCount;j++)
	{
		if (ConverBuf[j]==0xFF)
		{
			tmpbuf[i++]=0xFE;
			tmpbuf[i++]=0x01;
		} 
		else if (ConverBuf[j]==0xFE)
		{
			tmpbuf[i++]=0xFE;
			tmpbuf[i++]=0x00;
		}
		else
		{
			tmpbuf[i++]=ConverBuf[j];
		}		
	}
	
	tmpcount = i;
	unsigned char crc16;
	crc16=0;
	for(i=2;i<tmpcount;i++)
	{
		crc16^=(tmpbuf[i]);
	}
	byteTemp = (crc16&0xFF);
	if (byteTemp==0xff)
	{
		tmpbuf[tmpcount++]=0xfe;
		tmpbuf[tmpcount++]=0x01;
	}
	else if (byteTemp==0xfe)
	{
		tmpbuf[tmpcount++]=0xfe;
		tmpbuf[tmpcount++]=0x00;
	} 
	else
	{
		tmpbuf[tmpcount++]=byteTemp;
	}
    
	tmpbuf[tmpcount++]=0xFF;
    
	memset(ConverBuf,0,tmpcount);
	memcpy(ConverBuf,tmpbuf,tmpcount);
	return tmpcount;
}

//接收数据转义
int zIn_RecDataConver(unsigned char * ConverBuf,int BufCount)
{
	unsigned char tmpbuf[1024];
	int tmpcount;
	tmpcount=BufCount;
    int j=2,k=0;
	tmpbuf[0]=ConverBuf[0];
	tmpbuf[1]=ConverBuf[1];
	for (int i=2;i<BufCount-1;i++)
	{
		if (k==1)
		{
			k=0;
			tmpcount--;
			continue;
		}
		if (ConverBuf[i]==0xFE)
		{
			if ((ConverBuf[i+1]==0x00)|(ConverBuf[i+1]==0x01))
			{
				if (ConverBuf[i+1]==0x00)
				{
					tmpbuf[j++]=0xFE;
					k=1;
				} 
				else
				{
					tmpbuf[j++]=0xFF;
					k=1;
				}
			} 
			else
			{
				tmpbuf[j++]=ConverBuf[i];
			}
		} 
		else
		{
			tmpbuf[j++]=ConverBuf[i];
			
		}
	}
	tmpbuf[j]=ConverBuf[BufCount-1];
	memset(ConverBuf,0,BufCount);
	memcpy(ConverBuf,tmpbuf,tmpcount);
	return tmpcount;
}




//串口接收处理函数
UINT __stdcall zIn_MyComThreadFunction( LPVOID pParam )
{
	unsigned char rbuf[512];
	int rlen;
	//接收处理
	g_com_recv = TRUE;
	while(g_com_recv)
	{
		Sleep(5);
		zIn_Comm_Rcv(rbuf,&rlen);
	}

	return 0;
}

/*****************************
功能：打开串口，并开启接收线程

*****************************/
int OpenComm(unsigned char * dev)
{
	int ret;
	int i = 0;
	char str[10];
	unsigned long dwBaud;
	char parity;
	int bytesize;
	char stopbit[3];
	HANDLE hComThread;//线程句柄
	unsigned int comThreadId;

	dwBaud = 9600;
	parity = 'N';
	bytesize = 8;
	stopbit[0] = '1';
	stopbit[1] = '\0';
	ret = zIn_Com_Open(dev,dwBaud,parity,bytesize,stopbit);//如果正确返回1

	GETFACTORYID_EVENT_rs = CreateEvent(NULL,true,true,"GETFACTORYID_EVENT_rs");
	REPLACESKEY_EVENT_rs = CreateEvent(NULL,true,true,"REPLACESKEY_EVENT_rs");
	CALCSENDRANDS_EVENT_rs = CreateEvent(NULL,true,true,"CALCSENDRANDS_EVENT_rs");
	READPAGE_EVENT_rs = CreateEvent(NULL,true,true,"READPAGE_EVENT_rs");
	WRITEMEMO_EVENT_rs = CreateEvent(NULL,true,true,"WRITEMEMO_EVENT_rs");
	CHECKKEYSTATE_EVENT_rs = CreateEvent(NULL,true,true,"CHECKKEYSTATE_EVENT_rs");
	ANALYSISDATA_EVENT_NET_rs = CreateEvent(NULL,true,true,"ANALYSISDATA_EVENT_NET_rs");
	



	if (ret != 1)
	{
		return ret;
	}
	else
	{
		hComThread = NULL;
		hComThread = (HANDLE)_beginthreadex( NULL,0, &zIn_MyComThreadFunction,NULL,NULL,&comThreadId );
		
// 		unsigned char heart[24]  = {0x00,0x00,0xab,0xba};
// 		int len=0;
// 		len = zIn_SendDataConver(heart,4);
//		zIn_Comm_Send(heart,len);
		
		return ret;	//ret=1
	}
}

////// 关闭串口
int CloseComm()
{
	if (hCom == 0)
	{
		return ERR_NOT_OPEN;
	}

	g_com_recv = FALSE;
	Sleep(100);

	if (CloseHandle(hCom) != 0)
	{
		hCom = 0;

		return SUCCESS;
	}
	else
	{
		return ERR_TIME_OUT;
	}

}



int Serial_TransChannelSend(unsigned char *content,int length)
{
	int ret = 0;
	unsigned char *temp = new unsigned char[length];
	memset(temp,0,length);
	memcpy(temp,content,length);
	int conver_len=0;
    conver_len = zIn_SendDataConver(temp,length);
	ret = zIn_Comm_Send(temp,conver_len);	



	return 0;

}
