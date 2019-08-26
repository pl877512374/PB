#ifndef __NETPARAM_H
#define __NETPARAM_H
#include "stdafx.h"
#include <WinBase.h>
#ifdef	__NETPARAM_C
#define	NETPARAM_EXT	
#else
#define NETPARAM_EXT extern
#endif
typedef byte u8;
typedef WORD u16;
typedef DWORD u32;

#define ASSERT_RANGE(a,min,max) (a>=min && a<=max)?(1):(-1)
typedef struct TagNetParam
{
	DWORD m_dwServerPort;
	DWORD m_dwServerIP;
}NetParam;

typedef struct TagNetDataStruct
{
	//char *m_pcData;
	char m_pcData[20000];
	int  m_n32Len;
	int m_n32BufCnt;
	int m_n32Channel;
}NetDataStruct;
typedef struct TagRecordInfo
{
	CString m_strTime;
	CString m_strAPDBV;
	CString m_strEquipNo;
	CString m_strInter;
	CString m_strName;
	CString m_strAPDHV;
	CString m_strAPDTemper;
	CString m_strAPDRatio;
}StrRecordInfo;
#define MAXLOGINFO 500
struct STRUCT_LOG
{
	int in;							//日志中写入的位置
	int out;						//日志中读出的位置
	CString strLogInfo[MAXLOGINFO];	//日志中的字符串
	unsigned char au8buf[MAXLOGINFO][1050];	//日志中的数组
	unsigned short au16buflen[MAXLOGINFO];	//日志中的数组的长度
	unsigned char au8IsShowTime[MAXLOGINFO];//日志中是否显示时间及换行
};
extern bool f_CorrectionNetState;//码盘偏心修正中用到的网络连接状态判断标志
extern CWinThread *pThread1;
const int NET_BUF_NUM = 1000;
extern CString LaserMAC;//激光器MAC地址
extern CString fileLaserMAC;//文件名称 目前是和Mac地址一样，后期也许会改，所以留次变量
extern int g_nNetRecvInd;
extern char g_cNetRecvBuf[NET_BUF_NUM][10000];
extern int g_nNetRecvSize[NET_BUF_NUM];
extern u8 BCC_CHECK(u8 *Buf,u16 u16_Check_Length);
extern u8 CHECK_BCC(u8 *Buf,u16 u16_Check_Length);
extern  unsigned char * ZhiLing(unsigned char *All, unsigned char *pn,int YuLiu);
extern NetDataStruct g_sNetData;
extern u8 g_u8NetStateCnt;
extern CRITICAL_SECTION g_cs_UserMsg;
extern bool f_nav;//导航模式设置提示信息标志位
extern bool rec_ang;

extern int g_n32App_QueryStatePibReadLen;
extern char g_acApp_QueryStatePibReadFrameBuf[24];

//应用配置参数读取
extern int g_n32App_ConfigBasicPibULen;
extern char g_acApp_ConfigBasicPibUFrameBuf[24];
//应用配置参数下载
extern int g_n32App_ConfigBasicPibDLen;
extern char g_acApp_ConfigBasicPibDFrameBuf[1024];

extern int g_n32Prod_QueryAPDPibReadLen;
extern char g_acProd_QueryAPDPibReadFrameBuf[24];

extern int g_n32Prod_ConfigAPDPibDLen;
extern char g_acProd_ConfigAPDPibDFrameBuf[1024];


extern int g_n32Prod_ConfigAPDTestFlagLen;
extern char g_acProd_ConfigAPDTestFlagBuf[1024];

extern int g_n32Prod_ConfigAPDTestULen;
extern char g_acProd_ConfigAPDTestUFrameBuf[24];

extern int g_n32Prod_ConfigAPDTestDLen;
extern char g_acProd_ConfigAPDTestDFrameBuf[1024];

extern int g_n32Prod_ConfigBasicPibULen;
extern char g_acProd_ConfigBasicPibUFrameBuf[24]; 

extern int g_n32Prod_QueryBasicPibReadLen;
extern char g_acProd_QueryBasicPibReadFrameBuf[24]; 

extern int g_n32Prod_ConfigBasicPibDLen;
extern char g_acProd_ConfigBasicPibDFrameBuf[1024];

extern int g_n32Prod_ConfigPwTestULen;
extern char g_acProd_ConfigPwTestUFrameBuf[24];

extern int g_n32Prod_ConfigPwTestDLen;
extern char g_acProd_ConfigPwTestDFrameBuf[1024];

extern int g_n32RD_ConfigReadLen;
extern char g_acRD_ConfigReadFrameBuf[1024];

extern int g_n32RD_ConfigDnLen;
extern char g_acRD_ConfigDnFrameBuf[1024]; 


extern int g_n32RD_MotorPIDDnLen;
extern char g_acRD_MotorPIDDnFrameBuf[1024];

extern int g_n32RD_MotorPIDReadLen;
extern char g_acRD_MotorPIDReadFrameBuf[1024];

//单帧获取数据帧指令
extern char acGetSickOneFrameBuf[34];
extern int nGetSickOneFrameBufSize;

//连续获取数据帧指令
extern char acGetSickContinueFrameBuf[34];	
extern int nGetSickContinueFrameBufSize;

//停止连续获取数据帧指令
extern char acStopGetSickContinueFrameBuf[34];
extern int nStopGetSickContinueFrameBufSize;
extern char g_cPauseBuf[25];

extern char acClearResetNumBuf[24];
extern int nClearResetNumBufLength;

//复位次数查询
extern char acInqResetNumBuf[24];
extern int nInqResetNumBufLength;

extern int g_n32DI_StartLen;
extern char g_acDI_StartFrameBuf[25];//开启故障检测

extern int g_n32DI_StopLen;
extern char g_acDI_StopFrameBuf[25];//关闭故障检测


extern int g_n32UART_StartLen ;
extern char g_acUART_StartFrameBuf[25];

extern int g_n32UART_StopLen;
extern char g_acUART_StopFrameBuf[25];

extern char acBroadcastFrameBuf[31];
extern int nBroadcastFrameLength;

extern char acBroadcastIPRstFrameBuf[37];
extern int nBroadcastIPRstFrameLength;

extern int acBeatFrameLength;
extern char acBeatFrameBuf[24];

extern int g_n32RD_MotorSpeedReadLen;
extern char g_acRD_MotorSpeedReadFrameBuf[1024];

extern int g_n32TemperRecordReadLen;
extern char g_acRD_TemperRecordReadFrameBuf[1024];//激光功率调节

extern int g_n32TemperRecordClearLen;
extern char g_acRD_TemperRecordClearFrameBuf[1024];//激光功率调节

extern int g_n32Dust_ReadLen;
extern char g_acDust_ReadBuf[1024];
extern int g_n32Dust_InitLen;
extern char g_acDust_InitBuf[1024];
extern int g_n32Dust_Test;
extern char g_n32Dust_TestBuf[1024];
//修正表查询指令
extern char acXiuzhengbiaoQueryBuf[24];
extern int nXiuzhengbiaoQueryBufSize;


extern char acSystemFlashResetNumBuf[24];
extern int nSystemFlashRestLength;

extern char acFactorFlashResetNumBuf[24];
extern int nFactorFlashLength;

//修正表查询指令
extern char acXiuzhengbiaoQueryBuf1[24];
extern int nXiuzhengbiaoQueryBufSize1;

extern int g_n32Prod_ConfigHeatTestFlagLen;
extern char g_acProd_ConfigHeatTestFlagBuf[1024];
extern u8 SystemLoader;
//靶标获取过程中上位机发送下位机指令
extern unsigned char LMGetting_zhiling[34];
extern int LMGetting_zhilingSize;
//靶标设置上位机发给下位机的第一包指令
extern unsigned char LMSettingFir_zhiling[36];
extern int LMSettingFir_zhilingSize;
//APD击穿指令
extern  unsigned char apdjc_zhiling_S[34];
extern int apdjc_zhilingSize_S;
extern  unsigned char apdjc_zhiling[36];
extern int apdjc_zhilingSize;
//PID指令
extern  unsigned char pid_zhiling_S[34];
extern int pid_zhilingSize_S;
extern  unsigned char pid_zhiling[40];
extern int pid_zhilingSize;
//激光功率测试指令指令
extern  unsigned char LaserTest_zhiling[34];
extern int LaserTest_zhilingSize;
extern  unsigned char APDTest_zhiling[34];
extern int APDTest_zhilingSize;
//系统基本参数
extern unsigned char BasicState_zhiling[34];
extern int BasicState_zhilingSize;
extern unsigned char BasicStateWR_zhiling[92];
extern int BasicStateWR_zhilingSize;
//开启加热
extern  unsigned char Heate_zhiling[34];
extern  int Heate_zhilingSize;
 //重启设备
extern  unsigned char Reset_zhiling[34];
extern  int Reset_zhilingSize;
 //Ip和端口
extern unsigned char IpPort_zhiling_S[34];
extern int IpPort_zhilingSize_S;
extern unsigned char IpPort_zhiling[51];
extern int IpPort_zhilingSize;
 //单点模式下获取单次数据
extern  unsigned char Single_zhiling[34];
extern int Single_zhilingSize;
//心跳
extern unsigned char LaserHeart_zhiling[34];
extern int LaserHeart_zhilingSize;
//激光运行状态
extern unsigned char LaserState_zhiling[34];
extern int LaserState_zhilingSize;
//硬件及版本号
extern unsigned char Version_zhiling[34];
extern int Version_zhilingSize;
//复位信息
extern unsigned char Restar_zhiling[34];
extern int Restar_zhilingSize;
//电机测试
extern unsigned char DianJiTest_zhiling[34];
extern int DianJiTest_zhilingSize;
//广播
extern  char Broad[34];
extern int nBroad;
////高阈值修正表查询指令
//extern  unsigned char HYuZhiSearch_zhiling[34];
//extern  int HYuZhiSearch_zhilingSize;
//  //低阈值修正表查询指令
//extern  unsigned char LYuZhiSearch_zhiling[34];
//extern  int LYuZhiSearch_zhilingSize;
//修正表查询指令
extern  unsigned char XiuZhengSearch_zhiling[34];
extern int XiuZhengSearch_zhilingSize;
//反射率表查询指令
extern unsigned char FanSheLvSearch_zhiling[34];
extern int FanSheLvSearch_zhilingSize;
//码盘偏心表查询指令
extern unsigned char EncoderSearch_zhiling[34];
extern int EncoderSearch_zhilingSize;
//脉宽修正烧写指令
extern unsigned char ShaoXie_zhiling[34];
extern int ShaoXie_zhilingSize;
 //灰尘检测查询
extern  unsigned char DustTest_zhiling[34];
extern   int DustTest_zhilingSize;
 //灰尘检测初始化指令
extern unsigned char DustReset_zhiling[34];
extern int DustReset_zhilingSize;

//模拟AGV速度设置指令
extern unsigned char SpeedSet_zhiling[50];
extern unsigned char GetTimeStamp_zhiling[50];
extern int SpeedSet_zhilingSize;
extern int GetTimeStamp_zhilingSize;
//模拟AGV获取位置指令02 73 4D 4E 20 6D 4E 50 4F 53 47 65 74 50 6F 73 65 20 31 03
extern unsigned char GetPose_zhiling[20];
extern int GetPose_zhilingSize;

//靶标探测模式
extern unsigned char LandmarkModeSet_zhiling[36];
extern int LandmarkModeSet_zhilingSize;
//Mapping模式
extern unsigned char MappingModeSet_zhiling[50];
extern int MappingModeSet_zhilingSize;
//导航模式
extern unsigned char NaviModeSet_zhiling[36];
extern int NaviModeSet_zhilingSize;
//
extern unsigned char PXGetLmIndo_zhiling[34];
extern int PXGetLmIndo_zhilingSize;
#endif    /*#ifndef __INITIALIZE_H*/
