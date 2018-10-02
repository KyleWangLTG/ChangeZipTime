// ChangeZipFileTimes.cpp : �������̨Ӧ�ó������ڵ㡣
//
#include "stdafx.h"
#include "zip/ZipDefine.h"
#include "zip/unzip.h"
#include "zip/zip.h"

wchar_t* CharToWChar(const char *str)
{
	wchar_t* buffer = NULL;
	if (str)
	{
		size_t nu = strlen(str);
		size_t n = (size_t)MultiByteToWideChar(ZIP_UNICODE, 0, (const char *)str, int(nu), NULL, 0);
		buffer = 0;
		buffer = new wchar_t[n + 1];
		MultiByteToWideChar(ZIP_UNICODE, 0, (const char *)str, int(nu), buffer, int(n));
		buffer[n] = 0;
	}
	return buffer;
}


int main()
{
	LPCTSTR SrcPath = _T("hello1.jar");

	if (_waccess(SrcPath, 0))
	{

		printf("ԴĿ�ļ�:, �����ڣ���ѹʧ��\n");
		return ZR_NOFILE;
	}

	HZIP hz = OpenZip(SrcPath, 0);														//zip����ʼ


	ZIPENTRY ze;

	GetZipItem(hz, -1, &ze);																//��ȡβ�ڵ���Ϣ

	ChangeFileTime(hz);

	CloseZip(hz);																		//�������

    return 0;
}

