/******************************************************************
* �ļ����ƣ�FileUtils.h
* �������ߣ�zyy
* ����ʱ�䣺2011-8-17 11:25:38
* �����������ļ������֧࣬��ini�ļ����������ļ����ı��ļ��Ķ�д
* �޸ļ�¼��
******************************************************************/

#if !defined(AFX_FILEUTILS_H__91C8D3E0_39CD_449F_878B_BC7DEBF8D74B__INCLUDED_)
#define AFX_FILEUTILS_H__91C8D3E0_39CD_449F_878B_BC7DEBF8D74B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CFileUtils  
{
public:
	CFileUtils();
	virtual ~CFileUtils();
	//common
	//�ַ����ָ�
	void Split(CString str, CStringList & strList, const CString & divideCode, const CString & notIncCode); 
	// �ж�Ŀ¼�Ƿ����
	BOOL FolderExists(LPCSTR lpPathName);
	BOOL FileExists(LPCSTR lpFileName,BOOL bCurrentDir = TRUE);
	//����Ŀ¼
	BOOL CreateDir(LPCSTR lpPathName);
	//config
	BOOL WriteConfigfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, INT lpInt, LPCSTR lpFileName);
	UINT GetConfigfileInt(LPCSTR lpAppName, LPCSTR lpKeyName, INT nDefault, LPCSTR lpFileName);
	BOOL WriteConfigfileString(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpString, LPCSTR lpFileName);
	DWORD GetConfigfileString(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPSTR lpReturnedString, DWORD nSize, LPCSTR lpFileName);
	
	//data
	BOOL WriteDatafile(LPCSTR lpFileName, BOOL bCurrentDir=TRUE, BOOL bAppend=FALSE);
	BOOL ReadDatafile(LPCSTR lpFileName, BOOL bCurrentDir = TRUE);
	DWORD GetDatafileLength();
	BOOL WriteDatafileData(LPVOID lpData, UINT dataSize);
	BOOL ReadDatafileData(LPVOID lpData,UINT dataSize);
	void CloseDatafile();

	//text
	BOOL WriteTxtfile(LPCSTR lpFileName, BOOL bCurrentDir=TRUE, BOOL bAppend=FALSE);
	BOOL WriteTxtfileTxt(LPCSTR lpTxt,UINT txtSize);
	void CloseTxtfile();
	
	LONG SeekDatafile(LONG lOff, UINT nFrom);
	
	// ��ȡ��ǰĿ¼
	CString GetCurDir();
	// ��ȡ��ǰ��ִ���ļ�������·��
	CString GetExeFilePath();
	// ͨ��·������ȡ�ļ���
	static CString GetFileName(LPCSTR lpPathName);
	// ͨ���ļ�����ȡ�ļ�����
	static CString GetFileType(LPCSTR lpFileName);
	// ɾ���ļ�
	BOOL DelFile(LPCSTR lpFileName, BOOL bCurrentDir = TRUE);	
	// �����ļ�
	BOOL CpyFile(LPCSTR lpExistingFileName, LPCTSTR lpNewFileName, BOOL bFailIfExists, BOOL bCurrentDir = TRUE);

	BOOL RenameFile(LPCTSTR lpOldFileName, LPCTSTR lpNewFileName);
	/**************************************************
	* �������ƣ�CString OpenFileDlg
	* ���������5������ 
	*           ����1:BOOL bOpen=TRUE
	*           ����2:LPCTSTR lpFilter=NULL
	*           ����3:LPCTSTR lpTitle=NULL
	*           ����4:LPCTSTR lpDftPath=NULL
	*           ����5:LPCTSTR lpDftName=NULL
	* ���������static 
	* �������ܣ�������/����Ի��򣬲������û�ѡ�������·��
	* �������ߣ�zyy
	* ����ʱ�䣺2011-03-24 14:11:43
	**************************************************/
	static CString OpenFileDlg(BOOL bOpen=TRUE,	LPCTSTR lpFilter=NULL, LPCTSTR lpTitle=NULL, 
							   LPCTSTR lpDftPath=NULL, LPCTSTR lpDftName=NULL);

	static void OpenMultFileDlg(CStringList *pFileNameList, BOOL bOpen=TRUE,	LPCTSTR lpFilter=NULL, LPCTSTR lpTitle=NULL, 
								LPCTSTR lpDftPath=NULL, LPCTSTR lpDftName=NULL);

	static CString OpenDirDlg(HWND hWnd, LPCTSTR lpTitle=NULL, LPCTSTR lpDftPath=NULL);


	/**************************************************
	* �������ƣ�CString GetFileVersion
	* ���������1������ 
	*           ����1:char*  FileName	�ļ���
	* ���������CString �ļ��汾 
	* �������ܣ���ȡ�ļ��汾��Ϣ
	* �������ߣ�zyy
	* ����ʱ�䣺2011-8-10 14:38:27
	**************************************************/
	static CString GetFileVersion(char*  FileName);
private:
	//common
	CString m_strCurDir;
	CString m_ExeFilePath;
		
	//config
	
	//data
	CFile * m_DATA_file;
	CArchive * m_DATA_archive;
	
};

#endif // !defined(AFX_FILEUTILS_H__91C8D3E0_39CD_449F_878B_BC7DEBF8D74B__INCLUDED_)
