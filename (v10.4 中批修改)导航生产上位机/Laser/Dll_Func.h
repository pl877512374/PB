#pragma once

static HINSTANCE hDLL_watch;//存储动态加载wdcrwv.dll时返回句柄
//DES
typedef int (_stdcall *pSingleDES)(int DESType, byte SingleDESKey[] , int SourDataLen, byte SourData[], byte DestData[]);
static pSingleDES SingleDES;

//TripleDES
typedef int (_stdcall *pTripleDES)(int DESType, byte TripleDESKey[], int SourDataLen, byte SourData[], byte DestData[]);
static pTripleDES TripleDES;

//连接卡机
typedef int (_stdcall *pCT_open)(byte name[], int param1, byte param2[]);
static pCT_open CT_open;

//断开连接
typedef int (_stdcall *pCT_close)(int ihandle);
static pCT_close CT_close;

// 
typedef int (_stdcall *pICC_present)(int ihandle);
static pICC_present ICC_present;

typedef int (_stdcall *pICC_set_NAD)(int ihandle, byte nad);
static pICC_set_NAD ICC_set_NAD;

typedef int (_stdcall *pICC_resetVB)(int ihandle, int len, byte ret[]);
static pICC_resetVB ICC_resetVB;

//通用命令集
typedef int (_stdcall *pICC_tsi_apiVB)(int ihandle, int len, byte command[], int *rlen, byte resp[]);
static pICC_tsi_apiVB ICC_tsi_apiVB;





