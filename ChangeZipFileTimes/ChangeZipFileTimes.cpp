// ChangeZipFileTimes.cpp : 定义控制台应用程序的入口点。
//
#include "stdafx.h"
#include <iostream>
#include "zip/ZipDefine.h"
#include "zip/unzip.h"
#include "zip/zip.h"

static int g_countALL = 0, g_countSucess = 0;

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

int ChangeFileTime(const wchar_t *filePath, SYSTEMTIME *pst)
{
	g_countALL++;
	if (_waccess(filePath, 06))
	{
		std::wcout<< L"Can not Access File: "<< filePath<<std::endl;
		return ZR_NOTFOUND;
	}

	HZIP hz = OpenZip(filePath, 0);

	int err = ChangeFileTime(hz, pst);

	if (err == ZR_NOFILE)
	{
		std::wcout << L"File Format Error: " << filePath << std::endl;
	}
	else if (err == ZR_FLATE)
	{
		std::wcout << L"Modify file inner time error: " << filePath << std::endl;
	}
	else if(err == ZR_OK)
	{
		std::wcout << L"Change \""<<filePath <<L"\" inner time successfully." << std::endl;
		g_countSucess++;
	}

	CloseZip(hz);//清除缓存

	return err;
}

int ChangePathTime(const wchar_t *filePath, SYSTEMTIME *pst)
{
	int err = ZR_OK;

	WIN32_FIND_DATA FindFileData;
	HANDLE hListFile;
	std::wstring strFilePath = filePath;
	if (filePath[lstrlen(filePath) - 1] != L'\\' && filePath[lstrlen(filePath) - 1] != L'/')
	{
		strFilePath += L'\\';
	}
	std::wstring strTemp;
	strTemp = strFilePath + L"*";

	hListFile = FindFirstFile(strTemp.c_str(), &FindFileData);

	if (hListFile == INVALID_HANDLE_VALUE)
	{
		printf("Path not Found：%ws, %d\n", filePath, GetLastError());
		return ZR_NOTFOUND;
	}
	else
	{
		do
		{
			if(lstrcmp(FindFileData.cFileName, TEXT(".")) == 0 ||
			lstrcmp(FindFileData.cFileName, TEXT("..")) == 0)
			{
				continue;
			}

			if (FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			{
				strTemp = strFilePath + FindFileData.cFileName;
				ChangePathTime(strTemp.c_str(), pst);
			}
			else
			{
				strTemp = FindFileData.cFileName;
				int n = strTemp.length();
				if (n > 4 && strTemp[n -4] == L'.' && strTemp[n - 3] == L'j' && strTemp[n - 2] == L'a' && strTemp[n - 1] == L'r')
				{
					strTemp = strFilePath + FindFileData.cFileName;
					ChangeFileTime(strTemp.c_str(), pst);
				}
			}
		} while (FindNextFile(hListFile, &FindFileData));
	}
	return err;
}

bool StringTOSystemTime(LPCTSTR szTime, SYSTEMTIME *pst)
{
	if (nullptr == pst || szTime == nullptr)
	{
		return false;
	}

	using namespace std;
	int year = -1, month = -1, date = -1, hour = -1, minute = -1, second = -1;
	swscanf(szTime, L"%d-%d-%d %d:%d:%d", &year, &month, &date, &hour, &minute, &second);

	if (year < 1980 || year > 2999)
	{
		cout << "Datetime Format Error: year = "<<year<<endl;
		return false;
	}

	if (month < 1 || month > 12)
	{
		cout << "Datetime Format Error: month = " << month << endl;
		return false;
	}

	if (date < 1 || date > 31)
	{
		cout << "Datetime Format Error: date = " << date << endl;
		return false;
	}
	if (hour < 0 || hour > 23)
	{
		cout << "Datetime Format Error: hour = " << hour << endl;
		return false;
	}

	if (minute < 0 || minute > 59)
	{
		cout << "Datetime Format Error: minute = " << minute << endl;
		return false;
	}

	if (second < 0 || second > 59)
	{
		cout << "Datetime Format Error: second = " << second << endl;
		return false;
	}

	memset(pst, 0, sizeof(SYSTEMTIME));
	pst->wYear = year;
	pst->wMonth = month;
	pst->wDay = date;
	pst->wHour = hour;
	pst->wMinute = minute;
	pst->wSecond = second;
	return true;
}

int wmain(int argc, wchar_t *args[])
{
	using namespace std;
	if (argc == 1)
	{
		wstring exeName = args[0];
		wstring::size_type found = exeName.find_last_of(L"/\\");
		exeName = exeName.substr(found + 1);
		wstring hint = L"Modify jar file internal time: \n";
		hint = hint + L"    " + exeName + L" File/Path [Date, yyyy-mm-dd] [Time, hh:mm:ss]\n";
		hint = hint + L"        " + L"Date Default is 2018-01-01, Time Default is 01:00:00\n";
		hint = hint + L"    Example: " + exeName + L" \"D:\\Jar\\\" 2018-01-01 01:00:00 \r\n";
		wcout << hint.c_str();
		return 1;
	}

	//LPCTSTR SrcPath = L"zip/assad.d";
	wstring strPath = args[1];
	int len = strPath.length();

	for (int i = 0; i < len; ++i)
	{
		if (strPath[i] == L'/')
		{
			strPath[i] = L'\\';
		}
	}

	if (_waccess(strPath.c_str(), 0))
	{
		std::wcout << L"File not Found: " << strPath.c_str() << std::endl;
		return ZR_NOTFOUND;
	}

	if (len > 1 && strPath[len-1] == L'\\')
	{
		strPath = strPath.erase(len - 1, 1);
	}
	WIN32_FIND_DATA wfd;
	bool isDir = false;
	HANDLE hFind = FindFirstFile(strPath.c_str(), &wfd);
	if ((hFind != INVALID_HANDLE_VALUE) && (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
	{
		isDir = true;
	}
	FindClose(hFind);

	SYSTEMTIME st = { 2018, 1, 0, 1, 1, 0, 0 };

	if (argc >= 3)
	{
		wstring szTime = args[2];
		if (argc >= 4)
		{
			szTime = szTime + L" " + args[3];
		}
		else
		{
			szTime = szTime + L" 01:00:00";
		}
		if (!StringTOSystemTime(szTime.c_str(), &st))
		{
			return -1;
		}
	}

	if (isDir)
	{
		ChangePathTime(strPath.c_str(), &st);
	}
	else
	{
		ChangeFileTime(strPath.c_str(), &st);
	}

	cout << "Found " << g_countALL << " *.jar file(s), " << g_countSucess << " file(s) change time successfully." << endl;
	return 0;
}

