#include "stdafx.h"
#include "WriteLog.h"


_ConnectionPtr m_pConnection; // 连接对象指针
_RecordsetPtr m_pRecordset; // 记录集对象指针
//_CommandPtr m_pCommand; //指令执行指针
// 数据库连接对象
HRESULT hr;

byte g_szDbOldPassword[16];
CString g_strDbOldPassword;
byte g_cszDbPasswordKey[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
byte g_szKeyHead[] = {0x11, 0x22, 0x33, 0x44};
byte g_szKeyTail[] = {0x55, 0x66, 0x77, 0x88};
CString g_strKeyHead = CString("11223344");
CString g_strKeyTail = CString("55667788");
KeyDBStruct KeyDB;
//打开数据库
int  OpenDb(CString& strDbName, CString& strPassword)
{
	::CoInitialize(NULL); 

	// ADO对象的实例化必须在COM环境初始化之后
	try
	{
		hr = m_pConnection.CreateInstance(__uuidof(Connection)); // 实例化连接对象
		if(SUCCEEDED(hr))
		{
			CString temp;
			temp.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Jet OLEDB:Database Password=%s; Mode = Share Deny Read|Share Deny Write"),
				strDbName, strPassword);
			hr = m_pConnection->Open((_bstr_t)temp, "", "", adModeUnknown); // 连接数据库
		}
	}
	catch (_com_error e)
	{
		try
		{
			hr = m_pConnection.CreateInstance(__uuidof(Connection)); // 实例化连接对象
			if(SUCCEEDED(hr))
			{
				CString temp;
				temp.Format(_T("Provider=Microsoft.ACE.OLEDB.15.0;Data Source=%s;Database Password=%s; Mode = Share Deny Read|Share Deny Write"),
					strDbName, strPassword);
				hr = m_pConnection->Open((_bstr_t)temp, "", "", adModeUnknown); // 连接数据库
			}
		}
		catch (_com_error e)
		{
			CString errormessage;
			errormessage.Format(_T("%s"),e.ErrorMessage());
			return ERR_OPEN_DB;
		}
	}
	m_pRecordset.CreateInstance(__uuidof(Recordset));//实例化记录对象集

	return SUCCESS;
}

//更改数据库密码
int  ChangeDbPassword(CString& strNewPassword, CString& strOldPassword)
{
	CString strSQL;
	strSQL.Format(_T("ALTER DATABASE PASSWORD [%s] [%s]"), strNewPassword, strOldPassword);
	BSTR bsSQL = strSQL.AllocSysString();
	hr = m_pConnection->Execute(bsSQL, NULL, adCmdText);
	if (FAILED(hr))
	{
		return ERR_CHANGE_DB_PASSWORD;
	}
	return SUCCESS;
}

//写操作日志
int  WriteALog(CString& strOpera)
{
	CString strSQL;
	BSTR bsSQL;

	CString strTime;

	//当前时间
	CTime m_time;
	m_time = CTime::GetCurrentTime();
	strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), m_time.GetYear(), m_time.GetMonth(), m_time.GetDay(), \
		m_time.GetHour(), m_time.GetMinute(), m_time.GetSecond());

	strSQL.Format(_T("INSERT INTO 用户操作日志(用户操作, 操作时间) VALUES('%s', '%s')"), 
		strOpera, strTime);
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
	if (FAILED(hr))
	{
		return ERR_WRITE_DB;
	}

	return SUCCESS;
}

int WriteKeyLog(CString& Id, CString& ProductCode, CString& Key)
{
	CString strSQL;
	BSTR bsSQL;

	CString strTime;
	CTime m_time;
	m_time = CTime::GetCurrentTime();
	strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), m_time.GetYear(), m_time.GetMonth(), m_time.GetDay(), \
		m_time.GetHour(), m_time.GetMinute(), m_time.GetSecond());

	strSQL.Format(_T("INSERT INTO 密钥管理(日期,芯片ID,产品代码,密钥) VALUES('%s', '%s', '%s', '%s')"), 
		strTime,Id,ProductCode,Key);
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
	if (FAILED(hr))
	{
		return ERR_WRITE_DB;
	}

	return SUCCESS;
}



//读所有操作日志
int  ReadAllLog(CString &strLog)
{
	CString strSQL;
	BSTR bsSQL;
	_variant_t vDataVal;
	CString strTemp;

	strLog.Empty();

	strSQL.Format(_T("SELECT * FROM 用户操作日志"));
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, 
		m_pConnection.GetInterfacePtr(),
		adOpenDynamic, adLockOptimistic, adCmdText);

	if (FAILED(hr))
	{
		return ERR_READ_DB;
	}
	else
	{
		// 遍历记录集，查看是否和数据库中数据重复
		while (!m_pRecordset->EndOfFile)
		{
			vDataVal = m_pRecordset->GetCollect(_variant_t("用户操作"));
			strTemp = (char*)(_bstr_t)(vDataVal);
			strLog += strTemp;
			strLog += CString(" ");

			// 			vDataVal = m_pRecordset->GetCollect(_variant_t("操作状态"));
			// 			strTemp = (char*)(_bstr_t)(vDataVal);
			// 			strLog += strTemp;
			// 			strLog += CString(" ");

			vDataVal = m_pRecordset->GetCollect(_variant_t("操作时间"));
			strTemp = (char*)(_bstr_t)(vDataVal);
			strLog += strTemp;
			strLog += CString(" ");

			// 			vDataVal = m_pRecordset->GetCollect(_variant_t("操作用户"));
			// 			strTemp = (char*)(_bstr_t)(vDataVal);
			// 			strLog += strTemp;
			// 			strLog += CString(" ");

			strLog += CString("\n");

			m_pRecordset->MoveNext();
		}
	}
	m_pRecordset->Close();//关闭记录集
	return SUCCESS;
}

int  ReadKeyDBLog(HWND hWnd, CString& TimeStart,CString& TimeEnd,CString& Id,CString& ProductCode)
{
	CString strSQL;
	BSTR bsSQL;
	_variant_t vDataVal;
	CString strTemp;

	strSQL.Format(_T("SELECT * FROM 密钥管理"));
	if(Id != "" && ProductCode != "")
	{
		strSQL += " where 日期 >= '";
		strSQL +=  TimeStart;
		strSQL += "'";
		strSQL += " and 日期 <= '";
		strSQL +=  TimeEnd;
		strSQL += "'";
		strSQL += " and 芯片ID = '";
		strSQL += Id;
		strSQL += "'";
		strSQL += " and 产品代码 = '";
		strSQL += ProductCode;
		strSQL += "'";
	}
	else if(Id != "" && ProductCode == "")
	{

		strSQL += " where 日期 >= '";
		strSQL +=  TimeStart;
		strSQL += "'";
		strSQL += " and 日期 <= '";
		strSQL +=  TimeEnd;
		strSQL += "'";
		strSQL += " and 芯片ID = '";
		strSQL += Id;
		strSQL += "'";
	}
	else if(ProductCode != "" && Id == "")
	{
		strSQL += " where 日期 >= '";
		strSQL +=  TimeStart;
		strSQL += "'";
		strSQL += " and 日期 <= '";
		strSQL +=  TimeEnd;
		strSQL += "'";
		strSQL += " and 产品代码 = '";
		strSQL += ProductCode;
		strSQL += "'";
	}
	else if(Id == "" && ProductCode == "")
	{
		strSQL += " where 日期 >= '";
		strSQL +=  TimeStart;
		strSQL += "'";
		strSQL += "and 日期 <= '";
		strSQL +=  TimeEnd;
		strSQL += "'";
	}
	strSQL += "order by 日期 DESC";
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, 
		m_pConnection.GetInterfacePtr(),
		adOpenDynamic, adLockOptimistic, adCmdText);

	if (FAILED(hr))
	{
		PostMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(),WM_SENDDBDATA,(WPARAM)(&KeyDB),-1);//发送消息
		return ERR_READ_DB;
	}
	else
	{
		// 遍历记录集，查看是否和数据库中数据重复
		int count = 0;
		while (!m_pRecordset->EndOfFile)
		{
			vDataVal = m_pRecordset->GetCollect(_variant_t("日期"));
			strTemp = (char*)(_bstr_t)(vDataVal);
			KeyDB.Date = strTemp;

			vDataVal = m_pRecordset->GetCollect(_variant_t("芯片ID"));
			strTemp = (char*)(_bstr_t)(vDataVal);
			KeyDB.ID = strTemp;

			vDataVal = m_pRecordset->GetCollect(_variant_t("产品代码"));
			strTemp = (char*)(_bstr_t)(vDataVal);
			KeyDB.ProductCode = strTemp;

			vDataVal = m_pRecordset->GetCollect(_variant_t("密钥"));
			strTemp = (char*)(_bstr_t)(vDataVal);
			KeyDB.Key = strTemp;

			SendMessage(hWnd,WM_SENDDBDATA,(WPARAM)(&KeyDB),0);//发送消息

			m_pRecordset->MoveNext();
			count++;
		}
		if(count == 0)
			AfxMessageBox("查询数据库为空！请重新确认查询条件！");
	}
	//m_pRecordset->Release();
	m_pRecordset->Close();//关闭记录集
	return SUCCESS;
}
int GetUserKey(CString &key)
{
	CString strSQL;
	BSTR bsSQL;
	_variant_t vDataVal;

	strSQL.Format(_T("SELECT * FROM 用户管理"));
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, 
		m_pConnection.GetInterfacePtr(),
		adOpenDynamic, adLockOptimistic, adCmdText);

	if (FAILED(hr))
	{
		return ERR_READ_DB;
	}
	else
	{
		// 遍历记录集，查看是否和数据库中数据重复
		while (!m_pRecordset->EndOfFile)
		{
			vDataVal = m_pRecordset->GetCollect(_variant_t("密码"));
			key = (char*)(_bstr_t)(vDataVal);
			m_pRecordset->MoveNext();
		}
	}
	m_pRecordset->Close();//关闭记录集
	if (key.IsEmpty())//读取的密码为空
	{
		return ERR_GET_USERKEY;
	}
	return SUCCESS;

}

int SetUserKey(CString &Key)
{
	CString strSQL;
	BSTR bsSQL;

	strSQL.Format(_T("UPDATE 用户管理 SET 密码 = '%s'"), Key);
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
	if (FAILED(hr))
	{
		return ERR_SET_USERKEY;
	}

	return SUCCESS;
}

int  CloseDb()
{
	m_pRecordset->Close();
	m_pConnection->Close();
	// 	m_pRecordset->Release();
	// 	m_pConnection->Release();
	return SUCCESS;
}


int  DelKeyDBLog(HWND hWnd, CString& Time,CString& Id,CString& ProductCode)
{
	CString strSQL;
	BSTR bsSQL;
	_variant_t vDataVal;
	CString strTemp;

	//strSQL.Format(_T("SELECT * FROM 密钥管理"));
	strSQL.Format(_T("DELETE FROM 密钥管理"));
	strSQL += " where 日期 = '";
	strSQL += Time;
	strSQL += "' and 芯片ID = '";
	strSQL += Id;
	strSQL += "' and 产品代码 = '";
	strSQL += ProductCode;
	strSQL += "'";
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, 
		m_pConnection.GetInterfacePtr(),
		adOpenDynamic, adLockOptimistic, adCmdText);

	//if (FAILED(hr))
	//{
	//	return ERR_READ_DB;
	//}
	//else
	//{
	//	// 遍历记录集，查看是否和数据库中数据重复
	//	while (!m_pRecordset->EndOfFile)
	//	{
	//		m_pRecordset->MoveNext();
	//	}
	//}
	//m_pRecordset->Close();
	return SUCCESS;
}
int SaveParamDB(CString str[])
{
	CString strSQL;
	BSTR bsSQL;


	strSQL.Format(_T("SELECT * FROM 生产设置参数 where 设备号 = '"));
	strSQL += str[0];
	strSQL += "'";
	strSQL += "order by 日期 DESC";
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, 
		m_pConnection.GetInterfacePtr(),
		adOpenDynamic, adLockOptimistic, adCmdText);

	if (FAILED(hr))
	{
		//PostMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(),WM_SENDDBDATA,(WPARAM)(&KeyDB),-1);//发送消息
		return ERR_READ_DB;
	}
	else
	{
		if(!m_pRecordset->EndOfFile)
		{
			m_pRecordset->Close();//关闭记录集
			CString strTime;
			//当前时间
			CTime m_time;
			m_time = CTime::GetCurrentTime();
			strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), m_time.GetYear(), m_time.GetMonth(), m_time.GetDay(), \
			m_time.GetHour(), m_time.GetMinute(), m_time.GetSecond());
			strSQL.Format(_T("UPDATE 生产设置参数 SET 日期 = '%s',设备号 = '%s',零点偏移 = '%s',高整体偏移 = '%s',低整体偏移 = '%s',激光高压 = '%s',扫描模式 = '%s',通道选择 = '%s',击穿电压 = '%s',击穿电压温度 = '%s',电压衰减系数 = '%s' where 设备号 = '%s'"), 
						strTime,str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7], str[8], str[9],str[0]);
			bsSQL = strSQL.AllocSysString();
			hr = m_pRecordset->Open(bsSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
			if (FAILED(hr))
			{
				return ERR_SET_USERKEY;
			}
			return SUCCESS;
			
			return -1;
		}
		
	}
	m_pRecordset->Close();//关闭记录集
//没有
	CString strTime;

	//当前时间
	CTime m_time;
	m_time = CTime::GetCurrentTime();
	strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), m_time.GetYear(), m_time.GetMonth(), m_time.GetDay(), \
		m_time.GetHour(), m_time.GetMinute(), m_time.GetSecond());

	strSQL.Format(_T("INSERT INTO 生产设置参数(日期,设备号,零点偏移,高整体偏移,低整体偏移,激光高压,扫描模式,通道选择,击穿电压,击穿电压温度,电压衰减系数) VALUES('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')"), 
		strTime,str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7], str[8], str[9]);
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
	if (FAILED(hr))
	{
		return ERR_WRITE_DB;
	}
	//m_pRecordset->Close();//关闭记录集
	return 1;
	return SUCCESS;
}
int myStrToByte(CString src,byte *dec,int len)
{
	CString l_str,l_str1;
	l_str = src;
	char a[20];
	memset(a,0x00,20);
	int i;
	for (i = 0; i < len; i ++)
	{
		l_str1 = l_str.Left(1);
		l_str = l_str.Right(len-(i+1));
		l_str1.MakeLower();
		a[i] = l_str1.GetAt(0);
		if ((a[i]<='9')&&(a[i]>='0'))
		{
			a[i] = a[i] - '0';
		}
		else if ((a[i] <= 'Z')&&(a[i] >= 'A'))
		{
			a[i]; 
		}
		else if ((a[i] <= 'z')&&(a[i] >= 'a'))
		{
			a[i] = a[i] - 'a' + 10;
		}
		else
			a[i] = 0;
		if (i%2 == 1)
		{
			dec[i/2] = ((a[i-1]<<4)&0xF0) + (a[i]&0x0F);
		}
	}
	return 1;
}

//将密钥已下载，但发行失败的id写入数据库
int WriteFailedIdLog(CString &Id)
{
	CString strSQL;
	BSTR bsSQL;

	CString strTime;
	CTime m_time;
	m_time = CTime::GetCurrentTime();
	strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), m_time.GetYear(), m_time.GetMonth(), m_time.GetDay(), \
		m_time.GetHour(), m_time.GetMinute(), m_time.GetSecond());
	strSQL.Format(_T("INSERT INTO 发行失败管理(日期,芯片ID) VALUES('%s', '%s')"),strTime,Id);
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
	if (FAILED(hr))
	{
		return ERR_WRITE_DB;
	}

	return SUCCESS;
	return 1;
}
//当WriteFailedIdLog中写入的id发行成功后，从数据库中删除
int DeletFailedIdLog(CString &Id)
{
	CString strSQL;
	BSTR bsSQL;
	_variant_t vDataVal;
	CString strTemp;

	strSQL.Format(_T("DELETE FROM 发行失败管理"));
	strSQL += " where 芯片ID = '";
	strSQL += Id;
	strSQL += "'";

	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, 
		m_pConnection.GetInterfacePtr(),
		adOpenDynamic, adLockOptimistic, adCmdText);
	return 1;
}
//查询WriteFailedIdLog中是否写入了id
//若是则返回1，若否则返回0
int ReadFailedIdLog(CString &Id)
{
	CString strSQL;
	BSTR bsSQL;
	_variant_t vDataVal;
	CString strTemp;
	strSQL.Format(_T("SELECT * FROM 发行失败管理 where 芯片ID = '"));
	strSQL += Id;
	strSQL += "'";

	strSQL += "order by 日期 DESC";
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, 
		m_pConnection.GetInterfacePtr(),
		adOpenDynamic, adLockOptimistic, adCmdText);

	if (FAILED(hr))
	{
		//PostMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(),WM_SENDDBDATA,(WPARAM)(&KeyDB),-2);//发送消息
		return ERR_READ_DB;
	}
	else
	{
		while(!m_pRecordset->EndOfFile)
		{

			vDataVal = m_pRecordset->GetCollect(_variant_t("芯片ID"));
			strTemp = (char*)(_bstr_t)(vDataVal);
			m_pRecordset->Close();//关闭记录集
			return 1;
		}
	}
	//PostMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(),WM_SENDDBDATA,(WPARAM)(&KeyDB),-2);//发送消息
	m_pRecordset->Close();//关闭记录集
	return -1;
}