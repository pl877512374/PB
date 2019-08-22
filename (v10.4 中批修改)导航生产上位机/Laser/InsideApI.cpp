#include "stdafx.h"
#include "InsideApI.h"
#include <windows.h>
#include <process.h>

HANDLE hCom = 0;	//ȫ�ֱ��������ھ��

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

 unsigned int g_com_rx_len = 0;			// ���ڽ������ݳ���
 unsigned int	g_com_tx_len;			// ���ڷ������ݳ���
 unsigned char 	g_com_rx_buf[512];			// ���ڽ�������
 unsigned char  	g_com_tx_buf[512];			// ���ڷ������� 
 unsigned int 	g_com_len = 0;				
 unsigned char 	g_com_buf[512];
 unsigned short 	g_start_flag = 0;//�жϰ�ͷ����
 unsigned int 	g_frame_flag = 0;//���������־







/*
* �������ƣ�	pkt_code()
* ����������	��������ת��
* �����б�	
*			src_buf			--	ת��ǰ����
*			target_buf		--	ת�������
*			len				--	ת��ǰ���ݳ���
* ���ؽ����	
*			ת������ݳ���	
*/
unsigned char zIn_pkt_code(unsigned char *src_buf, unsigned char *target_buf,unsigned int len)
{
	unsigned char i = 0;
	unsigned char j = 0;

	// ֡ͷ
	target_buf[i++]=src_buf[0];
	target_buf[i++]=src_buf[1];
	// ����
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
	// ֡β
	target_buf[i++]=0xFF;
	return i;	
}	


/*
* �������ƣ�	pkt_decode()
* ����������	�������ݰ��Ľ��빤��
* �����б�	
*			buf			--	���յ�������
*			len			--	���ݳ���
*			port		--  �˿����ͣ����ڻ�����
* ���ؽ����	
*			ERR_NO		--	����У��ɹ�	
*			ERR_CHECK_WRONG	--	����У��ʧ��	
*/
unsigned char zIn_pkt_decode(unsigned char *buf, int &len)
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

/* ����������Ϣ֡ */
int zIn_ParsePkg(BYTE * buf,int len)
{
	int start;
	int i,j;
	BYTE chk = 0;

	//������ͷ����ȷ��
	if( buf[0] != 0xff  )
		return OTHER_ERROR;

	if(buf[1] == 0xff)
		start = 2;
	else
		start = 1;

	j = 0;
	//ȥ���˿�ʼλ��ֹͣλ
	for(i = start;i < len -1;i++)
	{
		if(buf[i] == 0xff)
			return OTHER_ERROR;

		buf[j] = buf[i];
		//��ԭΪ0xff�����
		if( buf[i] == 0xfe )
		{
			buf[j] |= buf[i + 1];
			i++;
		}
		j++;
	}

	len = j;

	//�������У��ֵ
	for(i = 0;i < len-1;i++)
		chk ^= buf[i];
	if(chk != buf[len - 1])
		return BCC_ERROR;

	return SUCCESS;

}

/* ******************************************************************************************** */
/* *************************************���ڵײ�ӿ�******************************************* */
/* ******************************************************************************************** */

//�򿪴���
int zIn_Com_Open(BYTE *com_num,DWORD dwBaud, char parity,int bytesize,char *stopbit)
{
	DCB dcb;
	
	hCom=CreateFile((char *)com_num,//COM1��
		GENERIC_READ|GENERIC_WRITE, //�������д
		0, //��ռ��ʽ
		NULL,
		OPEN_EXISTING, //�򿪶����Ǵ���
		0, 
		NULL);
	
	if(hCom==INVALID_HANDLE_VALUE)
	{
		//�豸��ռ��
		if(GetLastError() == 5)
		{
			return ERR_ALREADY_OPEN;
		}
		//�豸��ʧ�ܣ�ͨ��ָ�޴��豸
		if(GetLastError() == 2)
		{
			return ERR_OPEN;
		}
		//��������
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

		//���ô������Դ���
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
						
			return 1; //���ڷ���1
		}
	
	}
	else//��ȡ�������Դ��󣬹鵽��������
	{
		CloseHandle(hCom);
		hCom = 0;
		return ERR_SET_PARAM;
	}

}

//���ڷ������ݰ�
int zIn_Comm_Send(BYTE *buf,int len)
{
	OVERLAPPED m_Write;
	COMSTAT ComStat;
	BOOL bWriteStat;
	DWORD real_len;
	DWORD dwErrorFlags;

	PurgeComm(hCom, PURGE_TXABORT|PURGE_RXABORT|PURGE_TXCLEAR|PURGE_RXCLEAR);

	/* *****************���ڷ���***************** */
	memset(&m_Write,0,sizeof(OVERLAPPED));
	//�����¼�
	m_Write.hEvent=CreateEvent(NULL,TRUE,FALSE,NULL);

	ClearCommError(hCom,&dwErrorFlags,&ComStat);
	bWriteStat=WriteFile(hCom,buf,len,&real_len,&m_Write);

	if(!bWriteStat)
	{
		//�����������ڽ���д����
		if(GetLastError()==ERROR_IO_PENDING)
		{
			//�涨ʱ������δ���ͣ��򷵻ش�����Ϣ
			if( WaitForSingleObject(m_Write.hEvent,1000) == WAIT_TIMEOUT )
				return 2;    //ʧ��
		}
		//��Ч��������������Ѿ��ر�
		else if(GetLastError() == 6)
			return 4;        //����δ��
		else
			return -1;
	}
	return SUCCESS;
}

//���ڽ������ݰ���inbuff ���յ������ݳ���
int  zIn_Comm_Rcv(BYTE *in_buff,int * recv_len)
{
	unsigned int i;
	COMSTAT ComStat;
	BOOL bReadStat;
	DWORD dwErrorFlags;
	DWORD dwBytesRead;//��ȡ���ֽ���
	int k = 0;
	BYTE *pBuf = (BYTE *)in_buff;


	*recv_len = 0;//��ʼ����

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
				if(g_start_flag == 0xffff)		// ֡ͷ
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
				if( *pBuf == 0xff)	// ֡β
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
					//case 0x00:	//����оƬID					
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
/* *************************************����ײ�ӿ�******************************************* */
/* ******************************************************************************************** */

/*������Ϣ��ת�����*/
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

//��������ת��
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




//���ڽ��մ�����
UINT __stdcall zIn_MyComThreadFunction( LPVOID pParam )
{
	unsigned char rbuf[512];
	int rlen;
	//���մ���
	g_com_recv = TRUE;
	while(g_com_recv)
	{
		Sleep(5);
		zIn_Comm_Rcv(rbuf,&rlen);
	}

	return 0;
}

/*****************************
���ܣ��򿪴��ڣ������������߳�

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
	HANDLE hComThread;//�߳̾��
	unsigned int comThreadId;

	dwBaud = 9600;
	parity = 'N';
	bytesize = 8;
	stopbit[0] = '1';
	stopbit[1] = '\0';
	ret = zIn_Com_Open(dev,dwBaud,parity,bytesize,stopbit);//�����ȷ����1

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

////// �رմ���
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
