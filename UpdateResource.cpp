#include "stdafx.h"
#include "PEResInfo.h"
#include <locale.h>

struct VS_VERSIONINFO
{
    WORD                wLength;
    WORD                wValueLength;
    WORD                wType;
    WCHAR               szKey[1];
    WORD                wPadding1[1];
    VS_FIXEDFILEINFO    Value;
    WORD                wPadding2[1];
    WORD                wChildren[1];
};

struct
{
    WORD wLanguage;
    WORD wCodePage;
} *lpTranslate;

struct UPDATE_INFO 
{
	TCHAR szCommand[MAX_PATH];  // show; update
	TCHAR szPath[MAX_PATH];
	TCHAR szType[MAX_PATH];		// exeres; string; 
	TCHAR szKey[MAX_PATH];
	TCHAR szValue[MAX_PATH];
};

void PrintUsage()
{
	printf("UpdateResource -c command -f file_path -t res_type -k res_key -v res_value\n");
	printf("options:\n");
	printf("  command: 显示或更新。show: 显示；update: 更新\n");
	printf("file_path: 安装器文件路径\n");
	printf(" res_type: 资源类型。exeres: EXE资源文件；string: 字符串资源\n");
	printf("  res_key: 资源key\n");
	printf("res_value: 资源value。当type位exeres时为发布包文件路径\n");

	exit(-1);
}

bool VerStringToInt(LPCTSTR lpVersion, DWORD VerNum[4])
{
	if (!lpVersion)
	{
		return false;
	}
	
	LPCTSTR lpToks = _tcstok((LPTSTR)lpVersion, _T("."));
	
	DWORD *pArr = VerNum;
	
	while (lpToks)
	{
		*pArr++ = wcstol(lpToks, NULL, 10);
		lpToks = _tcstok(NULL, _T("."));
	}

	return true;
}

bool ParseCmdLine(int _Argc, wchar_t ** _Argv, UPDATE_INFO &UpdateInfo)
{
	if (_Argc < 2 || !_Argv)
	{
		return false;
	}

	ZeroMemory(&UpdateInfo, sizeof(UPDATE_INFO));
	
	wchar_t **pArg = &_Argv[1];

	for (; *pArg; pArg++)
	{
		if (lstrcmpi(*pArg, _T("-c")) == 0)
		{
			StringCchCopy(UpdateInfo.szCommand, sizeof(UpdateInfo.szCommand), *++pArg);
		}
		else if (lstrcmpi(*pArg, _T("-f")) == 0)
		{
			StringCchCopy(UpdateInfo.szPath, sizeof(UpdateInfo.szPath), *++pArg);
		}
		else if (lstrcmpi(*pArg, _T("-t")) == 0)
		{
			StringCchCopy(UpdateInfo.szType, sizeof(UpdateInfo.szType), *++pArg);
		}
		else if (lstrcmpi(*pArg, _T("-k")) == 0)
		{
			StringCchCopy(UpdateInfo.szKey, sizeof(UpdateInfo.szKey), *++pArg);
		}
		else if (lstrcmpi(*pArg, _T("-v")) == 0)
		{
			StringCchCopy(UpdateInfo.szValue, sizeof(UpdateInfo.szValue), *++pArg);
		}
	}

	bool valid = lstrlen(UpdateInfo.szCommand) > 0 &&
		lstrlen(UpdateInfo.szPath) > 0 &&
		lstrlen(UpdateInfo.szType) > 0 &&
		lstrlen(UpdateInfo.szKey) > 0;

	if (lstrcmpi(UpdateInfo.szCommand, _T("show")) != 0) {
		valid = valid && lstrlen(UpdateInfo.szValue) > 0;
	}

	return valid;
}

int Update(const UPDATE_INFO &UpdateInfo) {
	CPEResInfo PEInfo;
	if (!PEInfo.Open(UpdateInfo.szPath)) {
		printf("Open File fail\n");
		return -1;
	}

	bool update_ret = false;
	if (lstrcmpi(UpdateInfo.szType, _T("exeres")) == 0) {
		update_ret = PEInfo.UpdateResRCData(UpdateInfo.szKey, L"EXERES", UpdateInfo.szValue);
	} else if (lstrcmpi(UpdateInfo.szType, _T("string")) == 0) {
		update_ret = PEInfo.UpdateResString(wcstol(UpdateInfo.szKey, NULL, 10), UpdateInfo.szValue, lstrlen(UpdateInfo.szValue));
	} else {
		printf("unknown type %s\n", UpdateInfo.szType);
	}

	auto end_ret = PEInfo.Close(TRUE);
	if (!update_ret || !end_ret) {
		printf("update fail\n");
		return -1;
	}
	return 0;
}

int Show(const UPDATE_INFO &UpdateInfo) {
	CPEResInfo PEInfo;
	if (!PEInfo.Open(UpdateInfo.szPath)) {
		printf("Open File fail\n");
		return -1;
	}

	bool update_ret = false;
	if (lstrcmpi(UpdateInfo.szType, _T("string")) == 0) {
		wstring val;
		update_ret = PEInfo.GetResString(wcstol(UpdateInfo.szKey, NULL, 10), val);
		if (update_ret) {
			wprintf(L"%ls\n", val.c_str());
		}
	} else {
		printf("unsupport type %s\n", UpdateInfo.szType);
	}

	auto end_ret = PEInfo.Close(TRUE);
	if (!update_ret || !end_ret) {
		printf("show fail\n");
		return -1;
	}	
	return 0;
}

int wmain(int _Argc, wchar_t ** _Argv)
{
	setlocale(LC_ALL, "chs");

	if (_Argc < 2)
		PrintUsage();
	
	UPDATE_INFO UpdateInfo = {};
	if (!ParseCmdLine(_Argc, _Argv, UpdateInfo))
	{
		printf("Parse CmdLine failure\n");
		exit(-1);
	}

	if (lstrcmpi(UpdateInfo.szCommand, _T("update")) == 0) {
		Update(UpdateInfo);
	} else if (lstrcmpi(UpdateInfo.szCommand, _T("show")) == 0) {
		Show(UpdateInfo);
	} else {
		wprintf(L"unsupport command %ls\n", UpdateInfo.szCommand);
		PrintUsage();
	}

	return 0;
}
