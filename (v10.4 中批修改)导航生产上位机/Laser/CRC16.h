#ifndef _CRC16_H_
#define _CRC16_H_

#include "stdafx.h"
UINT16 CRC16_Cal( UINT8 * pBuf,UINT16 Buflen);
UINT16 CRC16_CalAdd( UINT8 * pBuf,UINT16 Buflen);
UINT16 CRC712_Add( UINT8 * pBuf,UINT16 Buflen);
BOOL CRC16_Check( UINT8 * pBuf,UINT16 Buflen);
#endif