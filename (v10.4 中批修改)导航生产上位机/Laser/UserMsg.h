#define WM_NETCMD_CLEARCHARTSI  (WM_USER + 101)
#define WM_NETCMD_DRAWCHARTSI  (WM_USER + 102)
#define WM_NETCMD_DRAWCHARTSC  (WM_USER + 103)
#define WM_NETCMD_CLEARCHARTSC (WM_USER + 104)
#define WM_NETCMD_CHARTCLOSE (WM_USER + 105)
#define WM_NETCMD_DRAW3D (WM_USER + 106)
#define WM_NETCMD_UARTDATA (WM_USER + 107)
#define WM_CONNECT7070		(WM_USER + 108)

#define WM_SENDCMD		WM_USER+2000
#define WM_SENDCMD_TIMER		WM_USER+2001

//mainfrm to GraphForm2dStat的消息
#define WM_SETAPP_BASICPIB_S    (WM_USER + 111)
#define WM_VERSION_READ     (WM_USER + 112)
#define WM_MAIN2GR2DST_READ     (WM_USER + 113)
#define WM_SETAPP_BASICPIB_U    (WM_USER + 114)
#define WM_SETAPP_RESETNUM_U    (WM_USER + 115)
#define WM_NETHEART_BEAT    (WM_USER + 116)
#define WM_HEATBACK_U    (WM_USER + 117)
#define WM_RESETLASER_U    (WM_USER + 118)
#define WM_MOTOSPEED_TEST    (WM_USER + 119)

#define WM_SETPROD_APDPIB_U		(WM_USER + 120)
#define WM_SETPROD_APDTEST_U1	(WM_USER + 121)
#define WM_SETPROD_BASICPIB_U1	(WM_USER + 122)
#define WM_SETPROD_LASERTEST_U1 (WM_USER + 123)
#define WM_SETPROD_NetWork_Read		(WM_USER + 124)
#define WM_CHECK_MAIKUAN		(WM_USER + 125)

#define WM_SETRD_BASICSTATE_READ (WM_USER + 130)
#define WM_SETRD_MOTOPID_READ (WM_USER + 131)
#define WM_SETRD_MOTOPID_SET (WM_USER + 133)
#define WM_SETRD_MOTOPID_SPEEDREAD (WM_USER + 132)
#define WM_SETRD_APDTEST_BACK (WM_USER + 134)
#define WM_SETRD_LaserGL_BACK (WM_USER + 135)
#define WM_SETRD_MOTOSPEED_TEST (WM_USER + 136)
#define WM_WRITELOG  (WM_USER + 137)

#define WM_STATEDLG_STOPTIMER   (WM_USER + 140)
#define WM_STATEDLG_STARTTIMER  (WM_USER + 141)

#define WM_DIAPP_DIAGNOSIS_U    (WM_USER + 150)


#define WM_BTNEN (WM_USER + 160)

#define WM_DI_UART_U    (WM_USER + 170)
#define WM_TEMPERRECORD_U    (WM_USER + 171)

#define WM_DUST_U    (WM_USER + 180)


#define WM_MES (WM_USER + 180)
#define WM_RECEIVE (WM_USER+181)

#define WM_ISSUELOG (WM_USER+182)

#define WM_SICK_POSEREAD (WM_USER+183)
//码盘偏心
#define WM_LMInfoView (WM_USER+190)
#define WM_LMSettingInfo (WM_USER+191)
#define WM_LMAngleDeal (WM_USER+192)
#define WM_COM_PARSE (WM_USER+193)
#define WM_MOTOSTATE (WM_USER+194)
#define WM_GetLMAngle (WM_USER+195)

//模拟AGV
#define WM_MNWRITELOG (WM_USER+290)
#define WM_SICK_MNPOSEREAD (WM_USER+293)
#define WM_MNLMInfoView (WM_USER+294)
#define WM_MNLMSettingInfo (WM_USER+295)
//网络消息
#define   GET_HEART_BEAT_RQ    ((uint16)0x5E73)     
#define   GET_HEART_BEAT_RS    ((unsigned char)0xAE)      

#define APP_QUERY_STATEPIB_READ_RQ ((uint16)0x5173)
#define APP_QUERY_STATEPIB_READ_RS ((unsigned char)0xA1)	

#define APP_QUERY_RESETNUM_READ_RQ ((uint16)0x5B73)
#define APP_QUERY_RESETNUM_READ_RS ((unsigned char)0xAB)	

#define APP_CONFIG_BASICPIB_UP_RQ ((uint16)0x5573)
#define APP_CONFIG_BASICPIB_UP_RS ((unsigned char)0xA5)	

#define APP_CONFIG_BASICPIB_DOWN_RQ ((uint16)0x5673)
#define APP_CONFIG_BASICPIB_DOWN_RS ((unsigned char)0xA6)

#define PROD_QUERY_APDPIB_READ_RQ ((uint16)0x6873)
#define PROD_QUERY_APDPIB_READ_RS	 ((unsigned char)0xB8)

#define PROD_CONFIG_APDTEST_UP_RQ ((uint16)0x7377)	 
#define PROD_CONFIG_APDTEST_UP_RS	 ((unsigned char)0xC7)

#define PROD_CONFIG_BASICPIB_UP_RQ ((uint16)0x6473)
#define PROD_CONFIG_BASICPIB_UP_RS	 ((unsigned char)0xB4)

#define PROD_QUERY_BASICPIB_READ_RQ ((uint16)0x6273)
#define PROD_QUERY_BASICPIB_READ_RS	 ((unsigned char)0xB2)	

#define PROD_CONFIG_PWTEST_DOWN_RQ ((uint16)0x7273)	//ÎÂ¶ÈÅäÖÃ²ÎÊýÉÏ´«//ÐÞ¸Ä³É75
#define PROD_CONFIG_PWTEST_DOWN_RS	 ((unsigned char)0xC2)

#define PROD_CONFIG_PWTEST_UP_RQ ((uint16)0x7376)	 
#define PROD_CONFIG_PWTEST_UP_RS	 ((unsigned char)0xC6)

#define RD_QUERY_BASICPIB_READ_RQ ((uint16)0x7973)	
#define RD_QUERY_BASICPIB_READ_RS	 ((unsigned char)0xc9)

#define RD_CONFIG_MOTORPID_DOWN_RQ ((uint16)0x8373)	  //APD????2?êyé?'?
#define RD_CONFIG_MOTORPID_DOWN_RS	 ((unsigned char)0xd3)

#define PROD_CONFIG_BASICPIB_DOWN_RQ ((uint16)0x6573)	//²ÎÊýÅäÖÃ
#define PROD_CONFIG_BASICPIB_DOWN_RS	 ((unsigned char)0xB5)

#define PROD_CONFIG_APDTEST_DN_RQ ((uint16)0x7873)	//ÎÂ¶ÈÅäÖÃ²ÎÊýÉÏ´«
#define PROD_CONFIG_APDTEST_DN_RS	 ((unsigned char)0xC8)


#define RD_CONFIG_BASICPIB_DOWN_RQ ((uint16)0x8073)	  //APDÅäÖÃ²ÎÊýÉÏ´«
#define RD_CONFIG_BASICPIB_DOWN_RS	 ((unsigned char)0xd0)

#define PROD_CONFIG_APDPIB_DOWN_RQ ((uint16)0x6773)	//ÎÂ¶ÈÅäÖÃ²ÎÊýÉÏ´«
#define PROD_CONFIG_APDPIB_DOWN_RS	 ((unsigned char)0xB7)

#define RD_CONFIG_MOTORPID_READ_RQ ((uint16)0x8473)	  //APD????2?êyé?′?
#define RD_CONFIG_MOTORPID_READ_RS	 ((unsigned char)0xd4)	
#define RD_CONFIG_MOTORSPEED_READ_RQ  ((uint16)0x8573)	 
#define RD_CONFIG_MOTORSPEED_READ_RS	 ((unsigned char)0xd5)	

#define DI_CHECK_UART_RQ  ((uint16)0xB173)	 
#define DI_CHECK_UART_RS	 ((unsigned char)0xE5)	

#define TEMPRECORD_RQ ((uint16)0xB273)	
#define TEMPRECORD_RS ((unsigned char)0xE6)

#define TEMPRECORDCLEAR_RQ ((uint16)0xB373)	
#define TEMPRECORDCLEAR_RS ((unsigned char)0xE7)

#define     DI_CHECK_RQ       ((uint16)0xB073)                    //Ð£Ñé´íÎóÖ¸Áî
#define     DI_CHECK_RS      ((unsigned char)0xE4)  

#define DUST_RQ    ((uint16)0xC073)	
#define DUST_RS    ((unsigned char)0xF0)	

#define DUSTINIT_RQ    ((uint16)0xC273)	
#define DUSTINIT_RS    ((unsigned char)0xF2)	