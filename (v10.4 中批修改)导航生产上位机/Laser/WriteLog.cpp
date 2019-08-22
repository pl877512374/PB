#include "stdafx.h"
#include "WriteLog.h"


_ConnectionPtr m_pConnection; // ���Ӷ���ָ��
_RecordsetPtr m_pRecordset; // ��¼������ָ��
//_CommandPtr m_pCommand; //ָ��ִ��ָ��
// ���ݿ����Ӷ���
HRESULT hr;

byte g_szDbOldPassword[16];
CString g_strDbOldPassword;
byte g_cszDbPasswordKey[] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88};
byte g_szKeyHead[] = {0x11, 0x22, 0x33, 0x44};
byte g_szKeyTail[] = {0x55, 0x66, 0x77, 0x88};
CString g_strKeyHead = CString("11223344");
CString g_strKeyTail = CString("55667788");
KeyDBStruct KeyDB;
//�����ݿ�
int  OpenDb(CString& strDbName, CString& strPassword)
{
	::CoInitialize(NULL); 

	// ADO�����ʵ����������COM������ʼ��֮��
	try
	{
		hr = m_pConnection.CreateInstance(__uuidof(Connection)); // ʵ�������Ӷ���
		if(SUCCEEDED(hr))
		{
			CString temp;
			temp.Format(_T("Provider=Microsoft.Jet.OLEDB.4.0;Data Source=%s;Jet OLEDB:Database Password=%s; Mode = Share Deny Read|Share Deny Write"),
				strDbName, strPassword);
			hr = m_pConnection->Open((_bstr_t)temp, "", "", adModeUnknown); // �������ݿ�
		}
	}
	catch (_com_error e)
	{
		try
		{
			hr = m_pConnection.CreateInstance(__uuidof(Connection)); // ʵ�������Ӷ���
			if(SUCCEEDED(hr))
			{
				CString temp;
				temp.Format(_T("Provider=Microsoft.ACE.OLEDB.15.0;Data Source=%s;Database Password=%s; Mode = Share Deny Read|Share Deny Write"),
					strDbName, strPassword);
				hr = m_pConnection->Open((_bstr_t)temp, "", "", adModeUnknown); // �������ݿ�
			}
		}
		catch (_com_error e)
		{
			CString errormessage;
			errormessage.Format(_T("%s"),e.ErrorMessage());
			return ERR_OPEN_DB;
		}
	}
	m_pRecordset.CreateInstance(__uuidof(Recordset));//ʵ������¼����

	return SUCCESS;
}

//�������ݿ�����
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

//д������־
int  WriteALog(CString& strOpera)
{
	CString strSQL;
	BSTR bsSQL;

	CString strTime;

	//��ǰʱ��
	CTime m_time;
	m_time = CTime::GetCurrentTime();
	strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), m_time.GetYear(), m_time.GetMonth(), m_time.GetDay(), \
		m_time.GetHour(), m_time.GetMinute(), m_time.GetSecond());

	strSQL.Format(_T("INSERT INTO �û�������־(�û�����, ����ʱ��) VALUES('%s', '%s')"), 
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

	strSQL.Format(_T("INSERT INTO ��Կ����(����,оƬID,��Ʒ����,��Կ) VALUES('%s', '%s', '%s', '%s')"), 
		strTime,Id,ProductCode,Key);
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
	if (FAILED(hr))
	{
		return ERR_WRITE_DB;
	}

	return SUCCESS;
}



//�����в�����־
int  ReadAllLog(CString &strLog)
{
	CString strSQL;
	BSTR bsSQL;
	_variant_t vDataVal;
	CString strTemp;

	strLog.Empty();

	strSQL.Format(_T("SELECT * FROM �û�������־"));
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
		// ������¼�����鿴�Ƿ�����ݿ��������ظ�
		while (!m_pRecordset->EndOfFile)
		{
			vDataVal = m_pRecordset->GetCollect(_variant_t("�û�����"));
			strTemp = (char*)(_bstr_t)(vDataVal);
			strLog += strTemp;
			strLog += CString(" ");

			// 			vDataVal = m_pRecordset->GetCollect(_variant_t("����״̬"));
			// 			strTemp = (char*)(_bstr_t)(vDataVal);
			// 			strLog += strTemp;
			// 			strLog += CString(" ");

			vDataVal = m_pRecordset->GetCollect(_variant_t("����ʱ��"));
			strTemp = (char*)(_bstr_t)(vDataVal);
			strLog += strTemp;
			strLog += CString(" ");

			// 			vDataVal = m_pRecordset->GetCollect(_variant_t("�����û�"));
			// 			strTemp = (char*)(_bstr_t)(vDataVal);
			// 			strLog += strTemp;
			// 			strLog += CString(" ");

			strLog += CString("\n");

			m_pRecordset->MoveNext();
		}
	}
	m_pRecordset->Close();//�رռ�¼��
	return SUCCESS;
}

int  ReadKeyDBLog(HWND hWnd, CString& TimeStart,CString& TimeEnd,CString& Id,CString& ProductCode)
{
	CString strSQL;
	BSTR bsSQL;
	_variant_t vDataVal;
	CString strTemp;

	strSQL.Format(_T("SELECT * FROM ��Կ����"));
	if(Id != "" && ProductCode != "")
	{
		strSQL += " where ���� >= '";
		strSQL +=  TimeStart;
		strSQL += "'";
		strSQL += " and ���� <= '";
		strSQL +=  TimeEnd;
		strSQL += "'";
		strSQL += " and оƬID = '";
		strSQL += Id;
		strSQL += "'";
		strSQL += " and ��Ʒ���� = '";
		strSQL += ProductCode;
		strSQL += "'";
	}
	else if(Id != "" && ProductCode == "")
	{

		strSQL += " where ���� >= '";
		strSQL +=  TimeStart;
		strSQL += "'";
		strSQL += " and ���� <= '";
		strSQL +=  TimeEnd;
		strSQL += "'";
		strSQL += " and оƬID = '";
		strSQL += Id;
		strSQL += "'";
	}
	else if(ProductCode != "" && Id == "")
	{
		strSQL += " where ���� >= '";
		strSQL +=  TimeStart;
		strSQL += "'";
		strSQL += " and ���� <= '";
		strSQL +=  TimeEnd;
		strSQL += "'";
		strSQL += " and ��Ʒ���� = '";
		strSQL += ProductCode;
		strSQL += "'";
	}
	else if(Id == "" && ProductCode == "")
	{
		strSQL += " where ���� >= '";
		strSQL +=  TimeStart;
		strSQL += "'";
		strSQL += "and ���� <= '";
		strSQL +=  TimeEnd;
		strSQL += "'";
	}
	strSQL += "order by ���� DESC";
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, 
		m_pConnection.GetInterfacePtr(),
		adOpenDynamic, adLockOptimistic, adCmdText);

	if (FAILED(hr))
	{
		PostMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(),WM_SENDDBDATA,(WPARAM)(&KeyDB),-1);//������Ϣ
		return ERR_READ_DB;
	}
	else
	{
		// ������¼�����鿴�Ƿ�����ݿ��������ظ�
		int count = 0;
		while (!m_pRecordset->EndOfFile)
		{
			vDataVal = m_pRecordset->GetCollect(_variant_t("����"));
			strTemp = (char*)(_bstr_t)(vDataVal);
			KeyDB.Date = strTemp;

			vDataVal = m_pRecordset->GetCollect(_variant_t("оƬID"));
			strTemp = (char*)(_bstr_t)(vDataVal);
			KeyDB.ID = strTemp;

			vDataVal = m_pRecordset->GetCollect(_variant_t("��Ʒ����"));
			strTemp = (char*)(_bstr_t)(vDataVal);
			KeyDB.ProductCode = strTemp;

			vDataVal = m_pRecordset->GetCollect(_variant_t("��Կ"));
			strTemp = (char*)(_bstr_t)(vDataVal);
			KeyDB.Key = strTemp;

			SendMessage(hWnd,WM_SENDDBDATA,(WPARAM)(&KeyDB),0);//������Ϣ

			m_pRecordset->MoveNext();
			count++;
		}
		if(count == 0)
			AfxMessageBox("��ѯ���ݿ�Ϊ�գ�������ȷ�ϲ�ѯ������");
	}
	//m_pRecordset->Release();
	m_pRecordset->Close();//�رռ�¼��
	return SUCCESS;
}
int GetUserKey(CString &key)
{
	CString strSQL;
	BSTR bsSQL;
	_variant_t vDataVal;

	strSQL.Format(_T("SELECT * FROM �û�����"));
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
		// ������¼�����鿴�Ƿ�����ݿ��������ظ�
		while (!m_pRecordset->EndOfFile)
		{
			vDataVal = m_pRecordset->GetCollect(_variant_t("����"));
			key = (char*)(_bstr_t)(vDataVal);
			m_pRecordset->MoveNext();
		}
	}
	m_pRecordset->Close();//�رռ�¼��
	if (key.IsEmpty())//��ȡ������Ϊ��
	{
		return ERR_GET_USERKEY;
	}
	return SUCCESS;

}

int SetUserKey(CString &Key)
{
	CString strSQL;
	BSTR bsSQL;

	strSQL.Format(_T("UPDATE �û����� SET ���� = '%s'"), Key);
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

	//strSQL.Format(_T("SELECT * FROM ��Կ����"));
	strSQL.Format(_T("DELETE FROM ��Կ����"));
	strSQL += " where ���� = '";
	strSQL += Time;
	strSQL += "' and оƬID = '";
	strSQL += Id;
	strSQL += "' and ��Ʒ���� = '";
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
	//	// ������¼�����鿴�Ƿ�����ݿ��������ظ�
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


	strSQL.Format(_T("SELECT * FROM �������ò��� where �豸�� = '"));
	strSQL += str[0];
	strSQL += "'";
	strSQL += "order by ���� DESC";
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, 
		m_pConnection.GetInterfacePtr(),
		adOpenDynamic, adLockOptimistic, adCmdText);

	if (FAILED(hr))
	{
		//PostMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(),WM_SENDDBDATA,(WPARAM)(&KeyDB),-1);//������Ϣ
		return ERR_READ_DB;
	}
	else
	{
		if(!m_pRecordset->EndOfFile)
		{
			m_pRecordset->Close();//�رռ�¼��
			CString strTime;
			//��ǰʱ��
			CTime m_time;
			m_time = CTime::GetCurrentTime();
			strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), m_time.GetYear(), m_time.GetMonth(), m_time.GetDay(), \
			m_time.GetHour(), m_time.GetMinute(), m_time.GetSecond());
			strSQL.Format(_T("UPDATE �������ò��� SET ���� = '%s',�豸�� = '%s',���ƫ�� = '%s',������ƫ�� = '%s',������ƫ�� = '%s',�����ѹ = '%s',ɨ��ģʽ = '%s',ͨ��ѡ�� = '%s',������ѹ = '%s',������ѹ�¶� = '%s',��ѹ˥��ϵ�� = '%s' where �豸�� = '%s'"), 
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
	m_pRecordset->Close();//�رռ�¼��
//û��
	CString strTime;

	//��ǰʱ��
	CTime m_time;
	m_time = CTime::GetCurrentTime();
	strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), m_time.GetYear(), m_time.GetMonth(), m_time.GetDay(), \
		m_time.GetHour(), m_time.GetMinute(), m_time.GetSecond());

	strSQL.Format(_T("INSERT INTO �������ò���(����,�豸��,���ƫ��,������ƫ��,������ƫ��,�����ѹ,ɨ��ģʽ,ͨ��ѡ��,������ѹ,������ѹ�¶�,��ѹ˥��ϵ��) VALUES('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')"), 
		strTime,str[0], str[1], str[2], str[3], str[4], str[5], str[6], str[7], str[8], str[9]);
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
	if (FAILED(hr))
	{
		return ERR_WRITE_DB;
	}
	//m_pRecordset->Close();//�رռ�¼��
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

//����Կ�����أ�������ʧ�ܵ�idд�����ݿ�
int WriteFailedIdLog(CString &Id)
{
	CString strSQL;
	BSTR bsSQL;

	CString strTime;
	CTime m_time;
	m_time = CTime::GetCurrentTime();
	strTime.Format(_T("%04d-%02d-%02d %02d:%02d:%02d"), m_time.GetYear(), m_time.GetMonth(), m_time.GetDay(), \
		m_time.GetHour(), m_time.GetMinute(), m_time.GetSecond());
	strSQL.Format(_T("INSERT INTO ����ʧ�ܹ���(����,оƬID) VALUES('%s', '%s')"),strTime,Id);
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, m_pConnection.GetInterfacePtr(), adOpenDynamic, adLockOptimistic, adCmdText);
	if (FAILED(hr))
	{
		return ERR_WRITE_DB;
	}

	return SUCCESS;
	return 1;
}
//��WriteFailedIdLog��д���id���гɹ��󣬴����ݿ���ɾ��
int DeletFailedIdLog(CString &Id)
{
	CString strSQL;
	BSTR bsSQL;
	_variant_t vDataVal;
	CString strTemp;

	strSQL.Format(_T("DELETE FROM ����ʧ�ܹ���"));
	strSQL += " where оƬID = '";
	strSQL += Id;
	strSQL += "'";

	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, 
		m_pConnection.GetInterfacePtr(),
		adOpenDynamic, adLockOptimistic, adCmdText);
	return 1;
}
//��ѯWriteFailedIdLog���Ƿ�д����id
//�����򷵻�1�������򷵻�0
int ReadFailedIdLog(CString &Id)
{
	CString strSQL;
	BSTR bsSQL;
	_variant_t vDataVal;
	CString strTemp;
	strSQL.Format(_T("SELECT * FROM ����ʧ�ܹ��� where оƬID = '"));
	strSQL += Id;
	strSQL += "'";

	strSQL += "order by ���� DESC";
	bsSQL = strSQL.AllocSysString();
	hr = m_pRecordset->Open(bsSQL, 
		m_pConnection.GetInterfacePtr(),
		adOpenDynamic, adLockOptimistic, adCmdText);

	if (FAILED(hr))
	{
		//PostMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(),WM_SENDDBDATA,(WPARAM)(&KeyDB),-2);//������Ϣ
		return ERR_READ_DB;
	}
	else
	{
		while(!m_pRecordset->EndOfFile)
		{

			vDataVal = m_pRecordset->GetCollect(_variant_t("оƬID"));
			strTemp = (char*)(_bstr_t)(vDataVal);
			m_pRecordset->Close();//�رռ�¼��
			return 1;
		}
	}
	//PostMessage(AfxGetApp()->GetMainWnd()->GetSafeHwnd(),WM_SENDDBDATA,(WPARAM)(&KeyDB),-2);//������Ϣ
	m_pRecordset->Close();//�رռ�¼��
	return -1;
}