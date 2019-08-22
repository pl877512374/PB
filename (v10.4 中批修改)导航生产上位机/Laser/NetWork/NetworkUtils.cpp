
#include "stdafx.h"
//#include "../MainFrm.h"
#include "resource.h"
#include "MainFrm.h"
#include "NetworkUtils.h"
//#define __NETPARAM_C
#include "NetParam.h"
//#include <WINSOCK2.H>

#pragma comment(lib,"ws2_32.lib")
#define TIME_OUT_SECOND 2
static VOID CALLBACK TimeProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime);//定时器处理函数
SOCKET sockClient;
SOCKET sockClient7070;
typedef struct tagFullNetConnection
{
	NET_CONNECTION netConnection;
	SOCKET sckClient;	
	
	//结构体默认构造函数，初始化变量
	void NET_CONNECTION_FULL()	
	{
		void Init();
	}
	
	void Init()
	{
		this->netConnection.Init();
	}
}NET_CONNECTION_FULL;

NET_CONNECTION_FULL g_aNetConnection[MAX_CONNECT_NUM];
CRITICAL_SECTION g_netcs;
int g_nConnectNum = 0;

char acRecvDataBuf[MAX_BLOCK];
char acRecvDataBuftmp[MAX_BLOCK];
int nRecvSizetmp = 0;


//UINT RecvThreadProc1(LPVOID lpParam)	// 接收线程处理函数
//{
//	int nID = *((int *)lpParam);
//	int nRecvSize = 0;
//	char l_cNetBuf[MAX_BLOCK];
//	InitializeCriticalSection(&g_netcs);
//	PFCB_NET_RECV recvFun = (PFCB_NET_RECV)g_aNetConnection[nID].netConnection.lpRecvFun;
//	int l_n32Tcp_02_ff = 0;//0正常，1粘帧，2断帧
////	int l_n32Tcpff = 0;//0正常，1粘帧，2断帧
//	while(g_aNetConnection[nID].netConnection.bConnected)
//	{  //acRecvDataBuf
//		EnterCriticalSection(&g_netcs);
//		nRecvSize = recv(g_aNetConnection[nID].sckClient,l_cNetBuf,MAX_BLOCK,0);
//		if(nRecvSize < 0)
//		{
//			continue;
//		}
//		u32 l_u32reallen = 0;
//		for(int i = 0;i<nRecvSize;)
//		{
//			if((l_cNetBuf[i] == 0xff && l_cNetBuf[i+1] == 0xff))
//			{
//				l_u32reallen = ((u8)l_cNetBuf[i+4] << 0) | 
//						       ((u8)l_cNetBuf[i+5] << 8);
//				if(l_u32reallen <= nRecvSize)
//				{
//					(*recvFun)(g_aNetConnection[nID].netConnection.lpWnd,&l_cNetBuf[i],l_u32reallen);
//					i+=l_u32reallen;
//				}
//				else
//				{
//					i=nRecvSize;
//				}
//			}
//			else if((l_cNetBuf[i] == 0x02 && l_cNetBuf[i+1] == 0x02&& l_cNetBuf[i+2] == 0x02&& l_cNetBuf[i+3] == 0x02))
//			{
//				
//				l_u32reallen = ((u8)l_cNetBuf[i+4] << 24) | 
//						           ((u8)l_cNetBuf[i+5] << 16) |
//								   ((u8)l_cNetBuf[i+6] << 8)  |
//								   ((u8)l_cNetBuf[i+7] << 0);
//				l_u32reallen = l_u32reallen + 9;
//				if(l_u32reallen <= nRecvSize)
//				{
//					(*recvFun)(g_aNetConnection[nID].netConnection.lpWnd,&l_cNetBuf[i],l_u32reallen);
//					i+=l_u32reallen;
//				}
//				else
//				{
//					i=nRecvSize;
//				}
//			}
//			else
//			{
//				i++;
//			}
//		}//for(int i = 0;i<nRecvSize;)
//		LeaveCriticalSection(&g_netcs);
//	}//end of while
//
//	return 1;
//}

typedef struct TagStructNetBuf
{
	char m_acbuf[MAX_BLOCK*10];
	unsigned int m_u32in;
	unsigned int m_u32out;
	unsigned int m_u32size;
}StructNetBuf;
StructNetBuf g_sNetBuf;
int xorflag = 0;
UINT RecvThreadProc(LPVOID lpParam)	// 接收线程处理函数
{
	int nID = *((int *)lpParam);
	int nRecvSize = 0;
	InitializeCriticalSection(&g_netcs);
	PFCB_NET_RECV recvFun = (PFCB_NET_RECV)g_aNetConnection[nID].netConnection.lpRecvFun;
	int l_n32TcpErr = 0;//0正常，1粘帧，2断帧
	memset(&g_sNetBuf,0,sizeof(g_sNetBuf));
	while(g_aNetConnection[nID].netConnection.bConnected)
	{
		EnterCriticalSection(&g_netcs);
		nRecvSize = recv(g_aNetConnection[nID].sckClient,acRecvDataBuf,MAX_BLOCK,0);
		//printf("%d,%d,%d,%d,%d\n",acRecvDataBuf[0],acRecvDataBuf[1],acRecvDataBuf[2],acRecvDataBuf[3],acRecvDataBuf[4]);

		if(nRecvSize < 0)
		{
			continue;
		}
		//if(acRecvDataBuf[16] == acRecvDataBuf[20])
		//{
		//	acRecvDataBuf[16] = 0x40;
		//}
		// recv 函数返回值
		//   <0 出错 
		//   =0 连接关闭 
		//   >0 接收到数据大小，
		//由于每包最大只能有1460字节，72k数据分两个36k包发送
		if((u32)nRecvSize > MAX_BLOCK)
		{
			printf("nRecvSize > MAX_BLOCK");
			continue;
		}
//-----------------------新解析------------------------------------------
		if ((g_sNetBuf.m_u32in + nRecvSize)>=MAX_BLOCK*10)
		{
			printf("(g_sNetBuf.m_u32in + nRecvSize)>=MAX_BLOCK*10\n");
			memset(&g_sNetBuf,0,sizeof(g_sNetBuf));//大于最大缓存，原来的数全清0
			continue;
		}
		memcpy(&g_sNetBuf.m_acbuf[g_sNetBuf.m_u32in],acRecvDataBuf,nRecvSize*sizeof(char));
		g_sNetBuf.m_u32in += nRecvSize;
		if (g_sNetBuf.m_u32in >= MAX_BLOCK*10)
		{
			printf("g_sNetBuf.m_u32in >= MAX_BLOCK\n");
			memset(&g_sNetBuf,0,sizeof(g_sNetBuf));
			//memcpy(&g_sNetBuf.m_acbuf[g_sNetBuf.m_u32in],acRecvDataBuf,nRecvSize*sizeof(char));
			break;
		}
		while(g_sNetBuf.m_u32out < g_sNetBuf.m_u32in)
		{
			if(((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out] == 0xff && 
				(unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+1] == 0xaa) || 
				((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out] == 0x02 &&
				(unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+1] == 0x02 && 
				(unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+2] == 0x02 && 
			   (unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+3] == 0x02 ))
			{
				//计算包长度
				unsigned int l_u32reallen = 0;
				if((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out] == 0x02 )
				{
					l_u32reallen = ((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+4] << 24) | 
						((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+5] << 16) |
						((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+6] << 8) |
						((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+7] << 0);
					l_u32reallen = l_u32reallen + 9;
				}
				else if((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out] == 0xff)
				{
					l_u32reallen = ((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+3] << 0) | 
						((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+2] << 8);
					l_u32reallen = l_u32reallen + 4;
				}
				else
				{
					g_sNetBuf.m_u32out++;
					continue;
				}

				//判断包长度与实际缓存长度之间的关系
				if (l_u32reallen <= (g_sNetBuf.m_u32in - g_sNetBuf.m_u32out+1))
				{
					(*recvFun)(g_aNetConnection[nID].netConnection.lpWnd,&g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out],l_u32reallen);
					if(xorflag == 1)   //校验失败
					{
						printf("校验 failed!\n");
						int i;
						for( i = 1;i< l_u32reallen;i++)
						{
							if((g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+i] == 0x02 && g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+i+1] == 0x02 &&g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+i+2] == 0x02 &&g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+i+3] == 0x02)
								|| (g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+i] == 0xff && g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+i+1] == 0xff))
							{
								g_sNetBuf.m_u32out += i;
								//memset(&g_sNetBuf,0,sizeof(gti_sNetBuf));
								printf("！！！连帧 failed!\n");
								xorflag = 0;
								break;
							}							
						}
						if(i== l_u32reallen && xorflag == 1)
						{
							g_sNetBuf.m_u32out += l_u32reallen;
						}
						xorflag = 0;
					}
					else
					{
						g_sNetBuf.m_u32out += l_u32reallen;
					}					
				}
				else if(l_u32reallen >= MAX_BLOCK)
				{
					printf("l_u32reallen err %d\n",l_u32reallen);
					memset(&g_sNetBuf,0,sizeof(g_sNetBuf));//大于最大缓存，原来的数全清0
					break;
				}
				else
				{
					break;
				}
			}
			else if((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out] == 0x02 && (unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out+1] != 0x02 )
			{
				//计算包长度
				unsigned int l_u32reallen = 0;
				if (g_sNetBuf.m_u32in-g_sNetBuf.m_u32out<=nRecvSize)
				{
					for (int i=g_sNetBuf.m_u32out;i<nRecvSize;i++)
					{
						if ((unsigned char)g_sNetBuf.m_acbuf[i] == 0x03)
						{
							l_u32reallen=i-g_sNetBuf.m_u32out+1;
							(*recvFun)(g_aNetConnection[nID].netConnection.lpWnd,&g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out],l_u32reallen);
							g_sNetBuf.m_u32out += l_u32reallen;	
							break;
						}
					}
				} 
				else
				{
					for (int i=0;i<nRecvSize;i++)
					{
						if ((unsigned char)g_sNetBuf.m_acbuf[g_sNetBuf.m_u32in-nRecvSize+i] == 0x03)
						{
							l_u32reallen=g_sNetBuf.m_u32in-nRecvSize+i+1;
							(*recvFun)(g_aNetConnection[nID].netConnection.lpWnd,&g_sNetBuf.m_acbuf[g_sNetBuf.m_u32out],l_u32reallen);
							g_sNetBuf.m_u32out += l_u32reallen;	
							break;
						}
					}
				}

				//判断包长度大于最大包长后依然没找到03，就清零
				if((g_sNetBuf.m_u32in-g_sNetBuf.m_u32out) >= 1460 && (l_u32reallen==0))
				{
					printf("l_u32reallen err %d\n",l_u32reallen);
					memset(&g_sNetBuf,0,sizeof(g_sNetBuf));//大于最大缓存，原来的数全清0
					break;
				}
				else
				{
					continue;
				}
			}
			else
			{
				g_sNetBuf.m_u32out++;
			}

		}

		if ((g_sNetBuf.m_u32in + nRecvSize)>=MAX_BLOCK*10)
		{
			printf("！！！(g_sNetBuf.m_u32in + nRecvSize)>=MAX_BLOCK*10\n");
			printf("g_sNetBuf.m_u32in is %d,g_sNetBuf.m_u32out is%d\n",g_sNetBuf.m_u32in,g_sNetBuf.m_u32out);
			memset(&g_sNetBuf,0,sizeof(g_sNetBuf));//大于最大缓存，原来的数全清0
			continue;
		}
		if (g_sNetBuf.m_u32out >= g_sNetBuf.m_u32in)
		{
			memset(&g_sNetBuf,0,sizeof(g_sNetBuf));
		}	
		LeaveCriticalSection(&g_netcs);
	}	
	return 0;
}

//UINT RecvThreadProc4(LPVOID lpParam)	// 接收线程处理函数
//{
//	int nID = *((int *)lpParam);
//	int nRecvSize = 0;
//
//	InitializeCriticalSection(&g_netcs);
//	PFCB_NET_RECV recvFun = (PFCB_NET_RECV)g_aNetConnection[nID].netConnection.lpRecvFun;
//	int l_n32TcpErr = 0;//0正常，1粘帧，2断帧
//	while(g_aNetConnection[nID].netConnection.bConnected)
//	{
//		EnterCriticalSection(&g_netcs);
//		nRecvSize = recv(g_aNetConnection[nID].sckClient,acRecvDataBuf,MAX_BLOCK,0);
//		if(nRecvSize < 0)
//		{
//			continue;
//		}
//		//if(acRecvDataBuf[16] == acRecvDataBuf[20])
//		//{
//		//	acRecvDataBuf[16] = 0x40;
//		//}
//		// recv 函数返回值
//		//   <0 出错 
//		//   =0 连接关闭 
//		//   >0 接收到数据大小，
//		//由于每包最大只能有1460字节，72k数据分两个36k包发送
//		if(((u8)acRecvDataBuf[0] == 0xff && 
//		   (u8)acRecvDataBuf[1] == 0xff) || 
//		   ((u8)acRecvDataBuf[0] == 0x02 &&
//		   (u8)acRecvDataBuf[1] == 0x02 && 
//		   (u8)acRecvDataBuf[2] == 0x02 && 
//		   (u8)acRecvDataBuf[3] == 0x02 ))
//		{
//			int l_n32i = 0;
//			for(l_n32i = 0;l_n32i<nRecvSize;)
//			{
//				u32 l_u32reallen = 0;
//				if((u8)acRecvDataBuf[l_n32i] == 0x02)
//				{
//					l_u32reallen = ((u8)acRecvDataBuf[l_n32i+4] << 24) | 
//						           ((u8)acRecvDataBuf[l_n32i+5] << 16) |
//								   ((u8)acRecvDataBuf[l_n32i+6] << 8) |
//								   ((u8)acRecvDataBuf[l_n32i+7] << 0);
//					l_u32reallen = l_u32reallen + 9;
//				}
//				else if((u8)acRecvDataBuf[l_n32i] == 0xff)
//				{
//					l_u32reallen = ((u8)acRecvDataBuf[l_n32i+4] << 0) | 
//						           ((u8)acRecvDataBuf[l_n32i+5] << 8);
//				}
//				else
//				{
//					l_n32i++;
//				}
//				if((l_n32i + l_u32reallen) <= nRecvSize && l_u32reallen !=0)
//				{
//					(*recvFun)(g_aNetConnection[nID].netConnection.lpWnd,&acRecvDataBuf[l_n32i],l_u32reallen);
//					l_n32i = l_n32i + l_u32reallen;
//				}
//				else
//				{
//					break;
//				}
//			}
//			if(l_n32i != nRecvSize && 
//			   (nRecvSize-l_n32i) > 0 &&
//			   nRecvSizetmp < (MAX_BLOCK))//有粘帧
//			{
//				nRecvSizetmp = (nRecvSize-l_n32i);
//				memcpy(&acRecvDataBuftmp[0],&acRecvDataBuf[l_n32i],nRecvSizetmp);
//
//				l_n32TcpErr = 1;
//			}
//		}
//		else if(l_n32TcpErr == 1)
//		{
//			if((nRecvSizetmp + nRecvSize)>=MAX_BLOCK || nRecvSizetmp < 0 || nRecvSize < 0)
//			{
//				continue;
//			}
//			memcpy(&acRecvDataBuftmp[nRecvSizetmp],&acRecvDataBuf[0],nRecvSize);
//			nRecvSizetmp += nRecvSize;
//			int l_n32i = 0;
//			for(l_n32i = 0;l_n32i<nRecvSizetmp;)
//			{
//				u32 l_u32reallen = 0;
//				if((u8)acRecvDataBuftmp[l_n32i] == 0x02)
//				{
//					l_u32reallen = ((u8)acRecvDataBuftmp[l_n32i+4] << 24) | 
//						           ((u8)acRecvDataBuftmp[l_n32i+5] << 16) |
//								   ((u8)acRecvDataBuftmp[l_n32i+6] << 8) |
//								   ((u8)acRecvDataBuftmp[l_n32i+7] << 0);
//					l_u32reallen = l_u32reallen + 9;
//				}
//				else if((u8)acRecvDataBuftmp[l_n32i] == 0xff)
//				{
//					l_u32reallen = ((u8)acRecvDataBuftmp[l_n32i+4] << 0) | 
//						           ((u8)acRecvDataBuftmp[l_n32i+5] << 8);
//				}
//				else
//				{
//					l_n32i++;
//				}
//				if((l_n32i + l_u32reallen) <= nRecvSizetmp && ((u8)acRecvDataBuftmp[l_n32i] == 0x02 || (u8)acRecvDataBuftmp[l_n32i] == 0xff))
//				{
//					if((u8)acRecvDataBuftmp[l_n32i] != 0x02 && (u8)acRecvDataBuftmp[l_n32i] != 0xff)
//					{
//						break;
//					}
//				//	if(acRecvDataBuftmp[l_n32i+16] == acRecvDataBuftmp[l_n32i+20])
//				//{
//				//	acRecvDataBuftmp[l_n32i+16] = 0x40;
//				//}
//					(*recvFun)(g_aNetConnection[nID].netConnection.lpWnd,&acRecvDataBuftmp[l_n32i],l_u32reallen);
//					l_n32i = l_n32i + l_u32reallen;
//					//nRecvSizetmp = nRecvSizetmp - l_n32i-1;
//					
//				}
//				else
//				{
//					break;
//				}
//			}
//			if(l_n32i != nRecvSizetmp && 
//			   (nRecvSizetmp-l_n32i) > 0 &&
//			   nRecvSizetmp < (MAX_BLOCK))//有粘帧
//			{
//				nRecvSizetmp = (nRecvSizetmp-l_n32i);
//				memcpy(&acRecvDataBuftmp[0],&acRecvDataBuftmp[l_n32i],nRecvSizetmp);
//				//if(acRecvDataBuftmp[16] == acRecvDataBuftmp[20])
//				//{
//				//	acRecvDataBuftmp[16] = 0x40;
//				//}
//			}
//			else
//			{
//				l_n32TcpErr = 0;
//			}
//		}
//		//else if(nRecvSizetmp == 0)
//		//{
//		//	if(((u8)acRecvDataBuf[0] == 0xff && 
//		//   (u8)acRecvDataBuf[1] == 0xff) || 
//		//   ((u8)acRecvDataBuf[0] == 0x02 &&
//		//   (u8)acRecvDataBuf[1] == 0x02 && 
//		//   (u8)acRecvDataBuf[2] == 0x02 && 
//		//   (u8)acRecvDataBuf[3] == 0x02 ))
//		//	{
//		//		u32 l_u32reallen = 0;
//		//		if((u8)acRecvDataBuf[0] == 0x02)
//		//		{
//		//			l_u32reallen = ((u8)acRecvDataBuf[0+4] << 24) | 
//		//				           ((u8)acRecvDataBuf[0+5] << 16) |
//		//						   ((u8)acRecvDataBuf[0+6] << 8) |
//		//						   ((u8)acRecvDataBuf[0+7] << 0);
//		//			l_u32reallen = l_u32reallen + 9;
//		//		}
//		//		else if((u8)acRecvDataBuf[0] == 0xff)
//		//		{
//		//			l_u32reallen = ((u8)acRecvDataBuf[0+4] << 0) | 
//		//				           ((u8)acRecvDataBuf[0+5] << 8);
//		//		}
//		//		if(l_u32reallen == nRecvSize || nRecvSize == (nRecvSize + 9))
//		//		{
//		//		//	if(acRecvDataBuf[16] == acRecvDataBuf[20])
//		//		//{
//		//		//	acRecvDataBuf[16] = 0x40;
//		//		//}
//		//			(*recvFun)(g_aNetConnection[nID].netConnection.lpWnd,&acRecvDataBuf[0],nRecvSize);
//		//		}
//		//	}
//		//}
//		LeaveCriticalSection(&g_netcs);
//		
//	}	
//	return 0;
//}
//int aa = 0;
//int bb = 0;
//int cc = 0;
void OnNetRecv(LPVOID lpParam, char *pDataBuf, int nDataBufSize)
{
	
	EnterCriticalSection(&g_netcs);
	CMainFrame *pMainFrame = (CMainFrame *)lpParam;
	//printf("ok\n");
	if(nDataBufSize > 0)
	{
		if ((*pDataBuf)==0x02 && (*(pDataBuf+1))!=0x02)
		{
			if (nDataBufSize>25)
			{
				memcpy(g_cNetRecvBuf[g_nNetRecvInd],pDataBuf,nDataBufSize);
				g_nNetRecvSize[g_nNetRecvInd] = nDataBufSize;

				pMainFrame->PostMessage(WM_NET_RECV,g_nNetRecvInd);
				g_nNetRecvInd = (g_nNetRecvInd+1)%NET_BUF_NUM;
			}	
		}
		else
		{
			if (checkXor(pDataBuf, nDataBufSize) || (*pDataBuf) == 0x02)
			{
				memcpy(g_cNetRecvBuf[g_nNetRecvInd],pDataBuf,nDataBufSize);
				g_nNetRecvSize[g_nNetRecvInd] = nDataBufSize;

				pMainFrame->PostMessage(WM_NET_RECV,g_nNetRecvInd);
				g_nNetRecvInd = (g_nNetRecvInd+1)%NET_BUF_NUM;
			}
			else if (unsigned char(*pDataBuf) == 0xFF && unsigned char(*(pDataBuf + 22)) == 0x05 && unsigned char(*(pDataBuf + 23)) == 0x1A)//偏心获取靶标
			{
				memcpy(g_cNetRecvBuf[g_nNetRecvInd], pDataBuf, nDataBufSize);
				g_nNetRecvSize[g_nNetRecvInd] = nDataBufSize;

				pMainFrame->PostMessage(WM_NET_RECV, g_nNetRecvInd);
				g_nNetRecvInd = (g_nNetRecvInd + 1) % NET_BUF_NUM;
			}
			else
			{
				xorflag = 1;			
			}
		}		

	}

	LeaveCriticalSection(&g_netcs);
}

VOID CALLBACK TimeProc(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime)
{
	
}

UINT ConnectThreadProc(LPVOID lpParam)
{
	CMainFrame *pMainFrame = (CMainFrame *)lpParam;

	while (g_bConnectting == true)
	{
		if (!(pMainFrame->m_NetConnection.bConnected))  //未连接
		{
			g_nConnecttingCount++;
			if (sockClient != INVALID_SOCKET)
			{
				closesocket(sockClient);
			}
			pMainFrame->m_NetConnection.lpWnd = lpParam;
			pMainFrame->m_NetConnection.lpRecvFun = (LPVOID)OnNetRecv;
			if (ConnectServer(&(pMainFrame->m_NetConnection)))
			{
				f_CorrectionNetState = true;
				f_SlopMoveNetOn = true;
				f_MoveZeroNetOn = true;
				g_bConnectting = false;
				g_nConnecttingCount = 0;
				pMainFrame->PostMessage(WM_NET_INI,(WPARAM)lpParam);
			}
			else
			{
				;
			}
		}
		else
		{
			if(DisconnectServer(&(pMainFrame->m_NetConnection)))
			{
				g_bConnectting = false;
				g_nConnecttingCount = 0;
			}
			else
				pMainFrame->MessageBox(_T("error"));
			NET_CONNECTION l_NetConnection;

			l_NetConnection.dwServerIP = pMainFrame->m_NetConnection.dwServerIP;
			l_NetConnection.unServerPort = pMainFrame->m_NetConnection.unServerPort;

			pMainFrame->m_NetConnection.Init();

			pMainFrame->m_NetConnection.dwServerIP = l_NetConnection.dwServerIP;
			pMainFrame->m_NetConnection.unServerPort = l_NetConnection.unServerPort;
		}

		if (g_nConnecttingCount>10)
		{
			g_bConnectting = false;
			g_nConnecttingCount = 0;
		}
	}
	
	return 1;
}

UINT ConnectThread7070(LPVOID lpParam)
{
	
	CMainFrame *pMainFrame = (CMainFrame *)lpParam;
	
	if (!(pMainFrame->m_NetConnection7070.bConnected))  //未连接
	{
		if (sockClient7070 != INVALID_SOCKET)
		{
			closesocket(sockClient7070);
		}
		pMainFrame->m_NetConnection7070.lpWnd = lpParam;
		if (ConnectServer7070(&(pMainFrame->m_NetConnection7070)))
		{
			
		}
		else
		{
			;
		}
		
	}
	else
	{
		if(DisconnectServer(&(pMainFrame->m_NetConnection7070)))
		{

		}
		else
			pMainFrame->MessageBox(_T("error"));
		NET_CONNECTION l_NetConnection;

		l_NetConnection.dwServerIP = pMainFrame->m_NetConnection7070.dwServerIP;
		l_NetConnection.unServerPort = pMainFrame->m_NetConnection7070.unServerPort;

		pMainFrame->m_NetConnection7070.Init();

		pMainFrame->m_NetConnection7070.dwServerIP = l_NetConnection.dwServerIP;
		pMainFrame->m_NetConnection7070.unServerPort = l_NetConnection.unServerPort;
	}
	return 1;
}
BOOL ConnectServer7070(NET_CONNECTION *pNetConnection){
	WORD wVersionRequested;
	WSADATA wsaData;
    CWinThread *pThread;
	int err;
	int len = sizeof(int);
	timeval tm;
	fd_set set;
	unsigned long ul = 1;
	bool ret = false;
	wVersionRequested = MAKEWORD(1,1);
	err = WSAStartup(wVersionRequested,&wsaData);
	if (err != 0)
		return FALSE;
	
	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion != 1))
	{
		WSACleanup();
		return FALSE;
	}
	// 创建套接字
	sockClient7070 = socket(AF_INET,SOCK_STREAM,0);
	
	if (sockClient7070 == INVALID_SOCKET)
	{
		return FALSE;
	}
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(pNetConnection->dwServerIP);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(pNetConnection->unServerPort);
	ioctlsocket(sockClient7070,FIONBIO,&ul);
	// 向服务器发出连接请求
	int iConnect = connect(sockClient7070,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	if(iConnect == -1)
	{   
		tm.tv_sec = TIME_OUT_SECOND;
		tm.tv_usec = 0;
		FD_ZERO(&set);
		FD_SET(sockClient7070,&set);
		if (select(sockClient7070+1,NULL,&set,NULL,&tm) > 0)
		{
			getsockopt(sockClient7070,SOL_SOCKET,SOCKET_ERROR,(char*)err,&len);
			if (err == 0)
			{
				ret = true;
			} 
			else
			{
				ret = false;
			}
		}
		else
			ret = false;		
	}
	else
		ret = false;
	ul = 0;
	ioctlsocket(sockClient7070,FIONBIO,&ul);
	if (!ret)
	{
		return false;
	}
	g_aNetConnection[g_nConnectNum].sckClient = sockClient7070;
	pNetConnection->nID = g_nConnectNum;
	pNetConnection->bConnected = TRUE;
	g_aNetConnection[g_nConnectNum].netConnection = *pNetConnection;
	g_nConnectNum++;	 
	pThread = ::AfxBeginThread(RecvThreadProc,&g_aNetConnection[pNetConnection->nID].netConnection.nID);
}

UINT UDP_RecvThreadProc(LPVOID lpParam)	// UPD接收线程处理函数
{
	return 1;
}

UINT SendThreadProc(LPVOID lpParam)
{
	int nID = *((int *)lpParam);
	int nSendSize = 0;
	
	while(nSendSize < g_aNetConnection[nID].netConnection.nSendDataBufSize)
	{
		nSendSize += send(g_aNetConnection[nID].sckClient,g_aNetConnection[nID].netConnection.pcSendDataBuf+nSendSize,
						  g_aNetConnection[nID].netConnection.nSendDataBufSize-nSendSize,0);
		if (nSendSize <= 0)
		{
			break;
		}
	}

	return 0;
}

BOOL ConnectServer(NET_CONNECTION *pNetConnection)
{
	WORD wVersionRequested;
	WSADATA wsaData;
    CWinThread *pThread;
	int err;
	int len = sizeof(int);
	timeval tm;
	fd_set set;
	unsigned long ul = 1;
	bool ret = false;
	wVersionRequested = MAKEWORD(1,1);
	err = WSAStartup(wVersionRequested,&wsaData);
	if (err != 0)
		return FALSE;
	
	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion != 1))
	{
		WSACleanup();
		return FALSE;
	}
	// 创建套接字
	sockClient = socket(AF_INET,SOCK_STREAM,0);
	
	if (sockClient == INVALID_SOCKET)
	{
		return FALSE;
	}
	SOCKADDR_IN addrSrv;
	addrSrv.sin_addr.S_un.S_addr = htonl(pNetConnection->dwServerIP);
	addrSrv.sin_family = AF_INET;
	addrSrv.sin_port = htons(pNetConnection->unServerPort);
	ioctlsocket(sockClient,FIONBIO,&ul);
	// 向服务器发出连接请求
	int iConnect = connect(sockClient,(SOCKADDR*)&addrSrv,sizeof(SOCKADDR));
	if(iConnect == -1)
	{   
		tm.tv_sec = TIME_OUT_SECOND;
		tm.tv_usec = 0;
		FD_ZERO(&set);
		FD_SET(sockClient,&set);
		if (select(sockClient+1,NULL,&set,NULL,&tm) > 0)
		{
			getsockopt(sockClient,SOL_SOCKET,SOCKET_ERROR,(char*)err,&len);
			if (err == 0)
			{
				ret = true;
			} 
			else
			{
				ret = false;
			}
		}
		else
			ret = false;		
	}
	else
		ret = false;
	ul = 0;
	ioctlsocket(sockClient,FIONBIO,&ul);
	if (!ret)
	{
		return false;
	}
	g_aNetConnection[g_nConnectNum].sckClient = sockClient;
	pNetConnection->nID = g_nConnectNum;
	pNetConnection->bConnected = TRUE;
	g_aNetConnection[g_nConnectNum].netConnection = *pNetConnection;
	g_nConnectNum++;	 
	pThread = ::AfxBeginThread(RecvThreadProc,&g_aNetConnection[pNetConnection->nID].netConnection.nID);
	Sleep(500);
	//发送硬件及版本号
	CTime time=CTime::GetCurrentTime();
	char Hour= time.GetHour();
	char Miu= time.GetMinute();
	char Sec= time.GetSecond();
	unsigned char pn[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x04,0x01};
	ZhiLing(Version_zhiling,pn,0);
	AfxGetMainWnd()->PostMessage(WM_SENDCMD, Version_zhilingSize, (LPARAM)Version_zhiling);
	//发送获取系统参数
	Sleep(500);
	unsigned char pn1[14]={0x00,0x1E,0x00,0x00,0x00,Hour,Miu,Sec,0x01,0x01,0x00,0x07,0x05,0x01};
	ZhiLing(BasicState_zhiling,pn1,0);
	BasicState_zhiling[25] = 0x01;   //代表生产版上位机发送
	AfxGetMainWnd()->PostMessage(WM_SENDCMD,BasicState_zhilingSize, (LPARAM)BasicState_zhiling);
	//获取激光器网络参数
	Sleep(500);
	unsigned char pn2[14] = { 0x00, 0x1E, 0x00, 0x00, 0x00, Hour, Miu, Sec, 0x01, 0x01, 0x00, 0x07, 0x05, 0x02 };
	ZhiLing(IpPort_zhiling_S, pn2, 0);
	AfxGetMainWnd()->PostMessage(WM_SENDCMD, IpPort_zhilingSize_S, (LPARAM)IpPort_zhiling_S);
	//读取点数比例
	CTime time1 = CTime::GetCurrentTime();
	char Hour1 = time1.GetHour();
	char Miu1 = time1.GetMinute();
	char Sec1 = time1.GetSecond();
	unsigned char pnl[14] = { 0x00, 0x1E, 0x00, 0x00, 0x00, Hour1, Miu1, Sec1, 0x01, 0x01, 0x00, 0x07, 0x05, 0x06 };
	ZhiLing(pid_zhiling_S, pnl, 0);
	AfxGetMainWnd()->PostMessage(WM_SENDCMD, pid_zhilingSize_S, (LPARAM)pid_zhiling_S);

	if (pThread == NULL)
	 	{
			return FALSE;
		}
//	SetTimer(NULL,3,5000,TimeProc);//设置定时器
	return TRUE;
}

BOOL UDP_Connect(NET_CONNECTION *pNetConnection)
{
	SOCKET UDP_Socket;
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	CWinThread *pThread;
	
	wVersionRequested = MAKEWORD(1,1);
	err = WSAStartup(wVersionRequested,&wsaData);
	if (err != 0)
		return FALSE;
	
	if (LOBYTE(wsaData.wVersion) != 1 ||
		HIBYTE(wsaData.wVersion != 1))
	{
		WSACleanup();
		return FALSE;
	}
	// 创建套接字
	UDP_Socket = socket(AF_INET,SOCK_DGRAM,0);
	SOCKADDR_IN addrSock;
	addrSock.sin_addr.S_un.S_addr = htonl(pNetConnection->dwServerIP);
	addrSock.sin_family = AF_INET;
	addrSock.sin_port = htons(pNetConnection->unServerPort);
	// 绑定套接字
	int nBind = bind(UDP_Socket,(SOCKADDR*)&addrSock,sizeof(SOCKADDR));
	if(SOCKET_ERROR != nBind)
	{
		g_aNetConnection[g_nConnectNum].sckClient = UDP_Socket;
		pNetConnection->nID = g_nConnectNum;
		pNetConnection->bConnected = TRUE;
		g_aNetConnection[g_nConnectNum].netConnection = *pNetConnection;
		g_nConnectNum++;	
		
		pThread = ::AfxBeginThread(UDP_RecvThreadProc,&g_aNetConnection[pNetConnection->nID].netConnection.nID);
		if (pThread == NULL)
		{
			return FALSE;
		}			
	}
	else
	{
		closesocket(UDP_Socket);
		WSACleanup();	
		return FALSE;
	}
	return TRUE;
}


BOOL DisconnectServer(NET_CONNECTION *pNetConnection)
{
	int nID;

	nID = pNetConnection->nID;
	if (g_aNetConnection[nID].netConnection.nID == nID &&
		g_aNetConnection[nID].netConnection.bConnected)
	{
		g_aNetConnection[nID].netConnection.bConnected =FALSE;
			Sleep(100);
		closesocket(g_aNetConnection[nID].sckClient);
		WSACleanup();	
		//g_aNetConnection[nID].Init();
		g_nConnectNum--;
	}	
	pNetConnection->bConnected = false;
	return TRUE;
}

BOOL SendData(NET_CONNECTION *pNetConnection)
{
	int nID;
	CWinThread *pThread;
	nID = pNetConnection->nID;

	if (g_aNetConnection[nID].netConnection.bConnected)
	{
		if (*(pNetConnection->pcSendDataBuf) == (char)0xff)
		{
			Xor(pNetConnection->pcSendDataBuf,pNetConnection->nSendDataBufSize);
			g_aNetConnection[nID].netConnection.nSendDataBufSize = pNetConnection->nSendDataBufSize;
		}
		else if(*(pNetConnection->pcSendDataBuf) == (char)0x02 )
		{
			if (*(pNetConnection->pcSendDataBuf+1) == (char)0x02)
			{
				Xor(pNetConnection->pcSendDataBuf,pNetConnection->nSendDataBufSize);
				g_aNetConnection[nID].netConnection.nSendDataBufSize = pNetConnection->nSendDataBufSize;
			} 
			else
			{
				g_aNetConnection[nID].netConnection.nSendDataBufSize = pNetConnection->nSendDataBufSize;
			}
			
		}
		else
		{
			;
		}
		
		g_aNetConnection[nID].netConnection.pcSendDataBuf = pNetConnection->pcSendDataBuf;
	//	g_aNetConnection[nID].netConnection.nSendDataBufSize = pNetConnection->nSendDataBufSize;
		pThread = ::AfxBeginThread(SendThreadProc,&g_aNetConnection[nID].netConnection.nID);
		if (pThread == NULL)
		{
			return FALSE;
		}			
	}
	return TRUE;
}

BOOL UDP_SendData(NET_CONNECTION *pNetConnection, SOCKADDR_IN *pAddrSock)
{
	int nID;
	nID = pNetConnection->nID;
	
	if (g_aNetConnection[nID].netConnection.bConnected)
	{
		Xor(pNetConnection->pcSendDataBuf,pNetConnection->nSendDataBufSize);
		g_aNetConnection[nID].netConnection.pcSendDataBuf = pNetConnection->pcSendDataBuf;
		g_aNetConnection[nID].netConnection.nSendDataBufSize = pNetConnection->nSendDataBufSize;
		
		sendto(g_aNetConnection[nID].sckClient, g_aNetConnection[nID].netConnection.pcSendDataBuf,
		g_aNetConnection[nID].netConnection.nSendDataBufSize,0,(sockaddr *)pAddrSock,sizeof(SOCKADDR));		
	}
	return 0;
}


void Xor(char* sendbuf,int sendlen)
{
	int i =0;
	char check=0;
	char* p = sendbuf;
	int len ;
	if (*p == (char)0x02)
	{
		p=p+8;
	    len	= sendlen;
		for (i=8;i<len;i++)
		{
			check ^= *p++;
		}
	}
	else if (*p == (char)0xff)
	{
		*p++;
		*p++;
		for(i =2;i<sendlen-4;i++)
		{
			check ^= *p++;
		}
		 *p++=0x00;
		*p++ = check;
	    *p++=0xEE;
	    *p++=0xEE;
	}
	else
	{
		;
	}
	
}

BOOL checkXor(char* recvbuf,int recvlen)
{
	int i =0;
	char check = 0;
	char * p = recvbuf;
	int len ;//末尾两位无需校验
	if (*p == (char)0xff)
	{
		len = recvlen-2;
		p++;
		p++;
		for (i = 0 ; i<len;i++)
		{
			check ^= *p;
			p++;
		}
		p++;
	} 
	else if (*p == (char)0x02)
	{
		p = p+8;
		len = recvlen-9;
		for (i = 0; i<len;i++)
		{
			check ^= *p++;
		}
	}
	else
	{
		return FALSE;
	}

	if (check == *p)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
 char Xor_download( char x, char* sendbuf,int sendlen)
{
	int i =0;
	 char check=0;
	 char* p = sendbuf;
	int len ;
	check ^= x;
	for(i =0;i<sendlen;i++)
	{
		check ^= *p++;
	}
	
	return check;
}
 short  XorMaiKuan_download( short x, short * sendbuf,int sendlen)
 {
	 short check=0;
	 int i =0;
	  short* p = sendbuf;
	 check ^= x;
	for(i =0;i<sendlen;i++)
	{
		check ^= *p++;
	}
	return check;
 }