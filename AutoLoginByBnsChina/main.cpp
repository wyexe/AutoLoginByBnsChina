#include <iostream>
#include <string>
#include <thread>
#include <MyTools/CLPublic.h>
#include "ConsoleCommond.h"
#include "GameLauncher.h"
#include "TextConfig.h"
VOID WeclomeStep()
{
	wcout << L"---------------------------------------" << endl;
	wcout << L"Author: ÎÞÓÇÎÞÇó" << endl;
}

BOOL InitializeShareMemory()
{
	// Create File
	wchar_t szFilePath[MAX_PATH] = { 0 };
	wsprintf(szFilePath, L"%s.SharedMem", SZFILE_NAME_SHAREDINFO);
	HANDLE hFileSharedInfo = CreateFile(szFilePath, GENERIC_READ | GENERIC_WRITE,          // open for reading
		FILE_SHARE_READ | FILE_SHARE_WRITE,       // share for reading
		NULL,                  // default security
		CREATE_ALWAYS,         // existing file only
		FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM, // normal file
		NULL);
	if (hFileSharedInfo == NULL)
	{
		CConsoleVariable::GetInstance().PrintErrLog(L"hFileSharedInfo = NULL");
		return FALSE;
	}

	// 
	HANDLE hFileMap = ::CreateFileMappingW(hFileSharedInfo, NULL, PAGE_READWRITE, NULL, sizeof(SHARED_INFO), SZFILE_NAME_SHAREDINFO);
	if (hFileMap == NULL)
	{
		CConsoleVariable::GetInstance().PrintErrLog(L"hFileMap = NULL");
		::CloseHandle(hFileSharedInfo);
		return FALSE;
	}

	::CloseHandle(hFileSharedInfo);

	// 
	auto& pShare = CConsoleVariable::GetInstance().GetShareInfo();
	pShare = (PSHARED_INFO)MapViewOfFile(hFileMap, FILE_MAP_READ | FILE_SHARE_WRITE, NULL, NULL, sizeof(SHARED_INFO));
	if (pShare == NULL)
	{
		CConsoleVariable::GetInstance().PrintErrLog(L"g_pSharedInfo = NULL");
		return FALSE;
	}

	// Init ShareInfo
	ZeroMemory(pShare, sizeof(SHARED_INFO));
	::GetCurrentDirectoryW(MAX_PATH, pShare->szConsolePath);
	return TRUE;
}

BOOL CheckFile()
{
	WCHAR wszPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryW(MAX_PATH, wszPath);
	std::wstring wsPath = wszPath;

	CONST static std::vector<std::wstring> FilePathVec = {
		L"\\Config.ini", L"\\DLL\\BnsDLL.dll", L"\\DLL\\CrackCaptchaAPI.dll",
		L"\\Language\\Text.ini"
	};

	for (CONST auto& itm : FilePathVec)
	{
		if (!CLPublic::FileExist(wsPath + itm))
		{
			CConsoleVariable::GetInstance().PrintErrLog(CTextConfig::GetInstance().GetText_By_Code(0x20).c_str(), (wsPath + itm).c_str());
			return FALSE;
		}
	}

	if (!CGameLauncher::GetInstance().SetAutoAnswerVerCode())
		return FALSE;
}

int main()
{
	// Set Log Type
	CConsoleVariable::GetInstance().SetLogerByConfig();

	// important!
	WeclomeStep();

	if (!CheckFile() || !InitializeShareMemory())
	{
		int n;
		wcin >> n;
		exit(0);
	}

	CLGrammar Grammar;
	Grammar.AddTranList(L"ResetAccount", CConsoleCommond::ResetAccount);

	std::wstring wsCommond;
	while (true)
	{
		std::getline(std::wcin, wsCommond);
		if (wsCommond == L"exit")
			break;

		
	}

	return 0;
}