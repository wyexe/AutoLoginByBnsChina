#include "GameLauncher.h"
#include <MyTools/CLProcess.h>
#include <MyTools/Character.h>
#include <MyTools/CLPublic.h>
#include <MyTools/CLFile.h>
#include "TextConfig.h"

#define _SELF L"GameLauncher.cpp"
BOOL CGameLauncher::RunLauncher(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw()
{
	// Change Config!
	if (!SetLoginAccountNameForLauncher(pAccGame))
		return FALSE;

	DeleteVerCodeFile();

	if (!CreateGameLauncher())
		return FALSE;

	if (!WaitToShow(pAccGame))
		return FALSE;

	if (!LoginStep(pAccGame))
		return FALSE;

	WCHAR wszDLLPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryW(MAX_PATH, wszDLLPath);
	lstrcatW(wszDLLPath, L"\\DLL\\BnsDLL.dll");
	return CLProcess::LoadRemoteDLL(CLProcess::GetPid_For_ProcName(L"Client.exe"), wszDLLPath);
}

BOOL CGameLauncher::CreateGameLauncher() CONST throw()
{
	Log(LOG_LEVEL_NORMAL, L"CreateGameLauncher");
	if (CLProcess::Is_Exist_Process_For_ProcName(L"Launcher.exe"))
	{
		Log(LOG_LEVEL_NORMAL, L"Exist Launcher.exe");
		return TRUE;
	}

	std::wstring wsGameLauncherPath;
	if (!CTextConfig::GetInstance().GetConfigValue_By_KeyName(L"GameLauncherPath", wsGameLauncherPath))
		return FALSE;

	CConsoleVariable::GetInstance().PrintToConsole(CTextConfig::GetInstance().GetText_By_Code(0x26).c_str());
	CLProcess::CreateProcess_InjectorRemoteDLL(wsGameLauncherPath.c_str(), nullptr, nullptr);
	::Sleep(10 * 1000);
	Log(LOG_LEVEL_NORMAL, L"Is_Exist_Process_For_ProcName=%d", CLProcess::Is_Exist_Process_For_ProcName(L"Launcher.exe"));
	return CLProcess::Is_Exist_Process_For_ProcName(L"Launcher.exe");
}

BOOL CGameLauncher::WaitToShow(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw()
{
	Log(LOG_LEVEL_NORMAL, L"WaitToShow");

	DWORD dwMaxTime = NULL;
	if (!CTextConfig::GetInstance().GetConfigHexValue_By_KeyName(L"ShowLuncherTimeOut", dwMaxTime))
		return FALSE;

	auto ulTick = ::GetTickCount64();
	while (true)
	{
		if (static_cast<DWORD>(::GetTickCount64() - ulTick) / 1000 >= dwMaxTime)
		{
			Kill();
			return FALSE;
		}

		if(!::EnumWindows((WNDENUMPROC)EnumGameLauncher, (LPARAM)pAccGame))
			break;

		::Sleep(1000);
	}

	return WaitToLauncherInitialize(pAccGame->hLauncher);
}

BOOL CGameLauncher::Kill() CONST throw()
{
	while (CLProcess::Is_Exist_Process_For_ProcName(L"Launcher.exe"))
		CLProcess::TerminateProc_For_ProcName(L"Launcher.exe");
	return TRUE;
}

BOOL CALLBACK CGameLauncher::EnumGameLauncher(_In_ HWND hLauncher, LPARAM lpParm)
{
	if (IsWindow(hLauncher) && IsWindowVisible(hLauncher) && (GetWindowLong(hLauncher, GWL_EXSTYLE)&WS_EX_TOOLWINDOW) != WS_EX_TOOLWINDOW && (GetWindowLong(hLauncher, GWL_HWNDPARENT) == 0))
	{
		WCHAR wszWinText[64] = { 0 };
		WCHAR wszWinClass[64] = { 0 };

		if (GetClassNameW(hLauncher, wszWinClass, _countof(wszWinClass) / sizeof(WCHAR)) > 0 && GetWindowTextW(hLauncher, wszWinText, _countof(wszWinText)) > 0)
		{
			if(CCharacter::wstrcmp_my(wszWinText,L"剑灵登录程序"))
			{
				reinterpret_cast<ACCOUNT_INFO_GAME*>(lpParm)->hLauncher = hLauncher;
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CGameLauncher::SetLoginAccountNameForLauncher(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw()
{
	Log(LOG_LEVEL_NORMAL, L"SetLoginAccountNameForLauncher");
	std::wstring wsGameLauncherPath;
	if (!CTextConfig::GetInstance().GetConfigValue_By_KeyName(L"GameLauncherPath", wsGameLauncherPath))
		return FALSE;

	if (wsGameLauncherPath.find(L"Launcher.exe") == -1)
		return FALSE;

	CCharacter::GetRemoveLeft(wsGameLauncherPath, L"Launcher.exe", wsGameLauncherPath);
	wsGameLauncherPath += L"config\\LoginQ.dat";

	return ::WritePrivateProfileStringW(L"Public", L"LastQQUin", pAccGame->GetAccName(), wsGameLauncherPath.c_str());
}

BOOL CGameLauncher::WaitToLauncherInitialize(_In_ HWND hLauncher) CONST throw()
{
	// wait to load complete
	Log(LOG_LEVEL_NORMAL, L"WaitToLauncherInitialize");
	DWORD dwTime = NULL;
	if (!CTextConfig::GetInstance().GetConfigHexValue_By_KeyName(L"TimeForShowLauncher", dwTime))
		return FALSE;

	auto ulTick = ::GetTickCount64();
	for (; static_cast<DWORD>(::GetTickCount64() - ulTick) / 1000 <= dwTime;)
	{
		// Wait to Safe Check Finish.
		if (PicFinder(L"SafeCheck.bmp"))
		{
			Log(LOG_LEVEL_NORMAL, L"Exist SafeCheck.bmp");
			return TRUE;
		}

		::SetWindowPos(hLauncher, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		::SwitchToThisWindow(hLauncher, TRUE);
		::Sleep(1000);
	}
	Log(LOG_LEVEL_NORMAL, L"UnExist SafeCheck.bmp, SpendTime:%d < %d", static_cast<DWORD>(::GetTickCount64() - ulTick), dwTime);
	return FALSE;
}

BOOL CGameLauncher::InputPass(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw()
{
	// Set Window Topest
	::SetWindowPos(pAccGame->hLauncher, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	::SwitchToThisWindow(pAccGame->hLauncher, TRUE);
	::Sleep(1000);

	RECT LauncherRect = { 0 };
	::GetWindowRect(pAccGame->hLauncher, &LauncherRect);

	CTextConfig::ConfigRect ConfigRect_ = { 0 };
	if (!CTextConfig::GetInstance().GetConfigRect_By_KeyName(L"LauncherPassRect", ConfigRect_))
		return FALSE;

	CLPublic::SimulationMouse(static_cast<int>(LauncherRect.left) + ConfigRect_.nLeft, static_cast<int>(LauncherRect.top) + ConfigRect_.nTop);
	::Sleep(1000);
	CLPublic::SendKeys(cwstring(pAccGame->MyAcount_Info.szPassWord).c_str());
	::Sleep(1000);
	return CLPublic::SendKey(VK_RETURN);
}

BOOL CGameLauncher::HandleShowVerCode(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw()
{
	typedef int (FAR WINAPI * D2file)(const char* pszSoftWareld, const char* pszUserName, const char * pszPass, const char* pFilePath, unsigned short usTimeOut, unsigned long ulVCodeTypeID, char * pszCodeText);

	std::wstring wsGameLauncherPath;
	if (!CTextConfig::GetInstance().GetConfigValue_By_KeyName(L"GameLauncherPath", wsGameLauncherPath))
		return FALSE;

	if (wsGameLauncherPath.find(L"Launcher.exe") == -1)
		return FALSE;

	std::wstring wsVerCodePath1;
	CCharacter::GetRemoveLeft(wsGameLauncherPath, L"Launcher.exe", wsVerCodePath1);
	wsVerCodePath1 += L"TenProtect\\ui\\res\\";

	std::wstring wsVerCodePath2 = wsVerCodePath1 + L"pic.png";
	wsVerCodePath1 += L"pic1.png";

	WCHAR wszDLLPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryW(MAX_PATH, wszDLLPath);
	lstrcatW(wszDLLPath, L"\\DLL\\CrackCaptchaAPI.dll");

	HMODULE hmDLL = ::LoadLibraryW(wszDLLPath);
	D2file pD2file = (D2file)::GetProcAddress(hmDLL, "D2File");

	if (CLPublic::FileExit(wsVerCodePath1.c_str()))
	{
		Log(LOG_LEVEL_NORMAL, L"Exist VerCode1");
		auto wsAnswer = Asker(wsVerCodePath1);
		::DeleteFileW(wsVerCodePath1.c_str());
		AnswerVerCode(pAccGame, wsAnswer);
		return TRUE;
	}

	if (CLPublic::FileExit(wsVerCodePath2.c_str()))
	{
		Log(LOG_LEVEL_NORMAL, L"Exist VerCode2");
		auto wsAnswer = Asker(wsVerCodePath2);
		::DeleteFileW(wsVerCodePath2.c_str());
		AnswerVerCode(pAccGame, wsAnswer);
		return TRUE;
	}
	return TRUE;
}

BOOL CGameLauncher::DeleteVerCodeFile() CONST throw()
{
	std::wstring wsGameLauncherPath;
	if (!CTextConfig::GetInstance().GetConfigValue_By_KeyName(L"GameLauncherPath", wsGameLauncherPath))
		return FALSE;

	if (wsGameLauncherPath.find(L"Launcher.exe") == -1)
		return FALSE;

	std::wstring wsVerCodePath1;
	CCharacter::GetRemoveLeft(wsGameLauncherPath, L"Launcher.exe", wsVerCodePath1);
	wsVerCodePath1 += L"TenProtect\\ui\\res\\";

	std::wstring wsVerCodePath2 = wsVerCodePath1 + L"pic.png";
	wsVerCodePath1 += L"pic1.png";

	if (CLPublic::FileExit(wsVerCodePath1.c_str()))
		::DeleteFileW(wsVerCodePath1.c_str());

	if (CLPublic::FileExit(wsVerCodePath2.c_str()))
		::DeleteFileW(wsVerCodePath2.c_str());
	return TRUE;
}

VOID CGameLauncher::SetAsker(_In_ std::function<std::wstring(CONST std::wstring&)> fnAsker) throw()
{
	Asker = fnAsker;
}

VOID CGameLauncher::AnswerVerCode(_In_ ACCOUNT_INFO_GAME* pAccGame, _In_ CONST std::wstring& wsVerCode) CONST throw()
{
	RECT LauncherRect = { 0 };
	::GetWindowRect(pAccGame->hLauncher, &LauncherRect);

	CTextConfig::ConfigRect ConfigRect_ = { 0 };
	if (!CTextConfig::GetInstance().GetConfigRect_By_KeyName(L"LauncherVerCode", ConfigRect_))
		return;

	CLPublic::SimulationMouse(static_cast<int>(LauncherRect.left) + ConfigRect_.nLeft, static_cast<int>(LauncherRect.top) + ConfigRect_.nTop);
	::Sleep(1000);

	CLPublic::SendKeys(wsVerCode.c_str());
	::Sleep(1000);

	CLPublic::SendKey(VK_RETURN);
}

VOID CGameLauncher::ClickStartGame(_In_ HWND hLauncher) CONST throw()
{
	RECT LauncherRect = { 0 };
	::GetWindowRect(hLauncher, &LauncherRect);

	CTextConfig::ConfigRect ConfigRect_ = { 0 };
	if (!CTextConfig::GetInstance().GetConfigRect_By_KeyName(L"LauncherStartGame", ConfigRect_))
		return;

	CLPublic::SimulationMouse(static_cast<int>(LauncherRect.left) + ConfigRect_.nLeft, static_cast<int>(LauncherRect.top) + ConfigRect_.nTop);
}

BOOL CGameLauncher::SetAutoAnswerVerCode() throw()
{
	// CrackCaptchaAPI.dll
	WCHAR wszPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryW(MAX_PATH, wszPath);

	std::wstring wsPath = wszPath;
	wsPath += L"\\DLL\\CrackCaptchaAPI.dll";

	HMODULE hmDLL = ::LoadLibraryW((wsPath).c_str());
	if (hmDLL == NULL)
	{
		CConsoleVariable::GetInstance().PrintErrLog(CTextConfig::GetInstance().GetText_By_Code(0x21).c_str(), wsPath.c_str());
		return FALSE;
	}

	typedef int (FAR WINAPI * D2file)(const char* pszSoftWareld, const char* pszUserName, const char * pszPass, const char* pFilePath, unsigned short usTimeOut, unsigned long ulVCodeTypeID, char * pszCodeText);
	static D2file pD2file = nullptr;
	pD2file = (D2file)::GetProcAddress(hmDLL, "D2File");
	if (pD2file == nullptr)
	{
		CConsoleVariable::GetInstance().PrintErrLog(CTextConfig::GetInstance().GetText_By_Code(0x22).c_str());
		return FALSE;
	}

	static std::wstring wsDamaAccount;
	static std::wstring wsDamaPass;

	if (!CTextConfig::GetInstance().GetConfigValue_By_KeyName(L"DamaAccount", wsDamaAccount))
		return FALSE;
	if (!CTextConfig::GetInstance().GetConfigValue_By_KeyName(L"DamaPass", wsDamaPass))
		return FALSE;

	SetAsker([](CONST std::wstring& wsVerCodePath)
	{
		CHAR szAnswerText[32] = { 0 };
		int nRetCode = pD2file("f2e0d5bfad94c9e325e1a7b707f3fcc9", CCharacter::UnicodeToASCII(wsDamaAccount).c_str(), CCharacter::UnicodeToASCII(wsDamaPass).c_str(), \
													CCharacter::UnicodeToASCII(wsVerCodePath).c_str(), 30, 101, szAnswerText);

		if (nRetCode == -101)
		{
			CConsoleVariable::GetInstance().PrintErrLog(CTextConfig::GetInstance().GetText_By_Code(0x23).c_str());
			return wstring(L"ABCD");
		}
		else if (nRetCode < 0)
		{
			CConsoleVariable::GetInstance().PrintErrLog(CTextConfig::GetInstance().GetText_By_Code(0x24).c_str(), nRetCode);
			return wstring(L"ABCD");
		}

		return CCharacter::ASCIIToUnicode(szAnswerText);
	});

	return TRUE;
}

BOOL CGameLauncher::PicFinder(_In_ cwstring& wsPicName) CONST throw()
{
	std::wstring wsMatchPath = CConsoleVariable::GetInstance().GetShareInfo()->szConsolePath;
	wsMatchPath += L"\\Res\\" + wsPicName;

	if (!CLPublic::FileExist(wsMatchPath.c_str()))
	{
		CConsoleVariable::GetInstance().PrintErrLog(CTextConfig::GetInstance().GetText_By_Code(0x5).c_str(), wsMatchPath.c_str());
		return FALSE;
	}

	SaveDeskWindow();

	std::wstring wsPath = CConsoleVariable::GetInstance().GetShareInfo()->szConsolePath;
	HMODULE hm = ::LoadLibraryW((wsPath + L"\\DLL\\MatchTemplateDLL.dll").c_str());
	if (hm == NULL)
	{
		CConsoleVariable::GetInstance().PrintErrLog(CTextConfig::GetInstance().GetText_By_Code(0x29).c_str(), (wsPath + L"\\DLL\\MatchTemplateDLL.dll").c_str());
		return FALSE;
	}

	typedef float(WINAPIV *pfnMatchPic_SQDIFF)(_In_ LPCSTR pszSourcePath, _In_ LPCSTR pszMatchPath);
	pfnMatchPic_SQDIFF MatchPic_SQDIFF = (pfnMatchPic_SQDIFF)::GetProcAddress(hm, "MatchPic_SQDIFF");
	if (MatchPic_SQDIFF == nullptr)
	{
		CConsoleVariable::GetInstance().PrintErrLog(CTextConfig::GetInstance().GetText_By_Code(0x2A).c_str(), L"MatchTemplateDLL", L"MatchPic_SQDIFF");
		return FALSE;
	}

	std::wstring wsSourcePath = CConsoleVariable::GetInstance().GetShareInfo()->szConsolePath;
	wsSourcePath += L"\\Res\\DeskPic.bmp";

	float fValue = MatchPic_SQDIFF(CCharacter::UnicodeToASCII(wsSourcePath).c_str(), CCharacter::UnicodeToASCII(wsMatchPath).c_str());
	Log(LOG_LEVEL_NORMAL, L"fValue=%.2f, Name=%s", fValue, wsPicName.c_str());
	return fValue >= 0.7;
}

BOOL CGameLauncher::SaveDeskWindow() CONST throw()
{
	HWND hWnd = ::GetDesktopWindow();

	RECT rect;
	::GetWindowRect(hWnd, &rect);

	int nWidht = rect.right - rect.left;  
	int nHeight = rect.bottom - rect.top;  

	HDC hSrcDC = ::GetWindowDC(hWnd);
	HDC hMemDC = ::CreateCompatibleDC(hSrcDC);
	HBITMAP hBitmap = ::CreateCompatibleBitmap(hSrcDC, nWidht, nHeight);
	HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hMemDC, hBitmap);
	::BitBlt(hMemDC, 0, 0, nWidht, nHeight, hSrcDC, 0, 0, SRCCOPY);

	BITMAP bitmap = { 0 };
	::GetObject(hBitmap, sizeof(BITMAP), &bitmap);
	BITMAPINFOHEADER bi = { 0 };
	BITMAPFILEHEADER bf = { 0 };

	CONST int nBitCount = 24;
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = bitmap.bmWidth;
	bi.biHeight = bitmap.bmHeight;
	bi.biPlanes = 1;
	bi.biBitCount = nBitCount;
	bi.biCompression = BI_RGB;
	DWORD dwSize = ((bitmap.bmWidth * nBitCount + 31) / 32) * 4 * bitmap.bmHeight;

	HANDLE hDib = GlobalAlloc(GHND, dwSize + sizeof(BITMAPINFOHEADER));
	LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
	*lpbi = bi;

	::GetDIBits(hMemDC, hBitmap, 0, bitmap.bmHeight, (BYTE*)lpbi + sizeof(BITMAPINFOHEADER), (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

	try
	{
		bf.bfType = 0x4d42;
		dwSize += sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		bf.bfSize = dwSize;
		bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

		WCHAR wszPath[MAX_PATH] = { 0 };
		::GetCurrentDirectoryW(MAX_PATH, wszPath);
		lstrcatW(wszPath, L"\\Res\\DeskPic.bmp");

		CLFile::WriteFile(cwstring(wszPath), reinterpret_cast<BYTE*>(&bf), sizeof(BITMAPFILEHEADER));
		CLFile::AppendFile(cwstring(wszPath), reinterpret_cast<BYTE*>(lpbi), dwSize);
	}
	catch (...)
	{
		
	}

	GlobalUnlock(hDib);
	GlobalFree(hDib);

	::SelectObject(hMemDC, hOldBitmap);
	::DeleteObject(hBitmap);
	::DeleteDC(hMemDC);
	::ReleaseDC(hWnd, hSrcDC);
	return TRUE;
}

BOOL CGameLauncher::LoginStep(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw()
{
	DWORD dwShowLuncherTimeOut = 0;
	if (!CTextConfig::GetInstance().GetConfigHexValue_By_KeyName(L"ShowLuncherTimeOut", dwShowLuncherTimeOut))
		return FALSE;

	for (auto ulTick = ::GetTickCount64(); static_cast<DWORD>(::GetTickCount64() - ulTick) / 1000 < dwShowLuncherTimeOut;)
	{
		// Check Game
		if (CLProcess::Is_Exist_Process_For_ProcName(L"Client.exe"))
		{
			pAccGame->dwPid = CLProcess::GetPid_For_ProcName(L"Client.exe");
			return WaitForClient(pAccGame);
		}

		// Check AD
		if (PicFinder(L"LoginAD.bmp"))
		{
			Log(LOG_LEVEL_NORMAL, L"Exist LoginAD.bmp");
			if (!CloseAD(pAccGame))
				return FALSE;

			continue;
		}

		// Password invalid
		if (PicFinder(L"InvaildPassword.bmp"))
		{
			pAccGame->AccountStatus.bPassInvalid = TRUE;
			Log(LOG_LEVEL_NORMAL, L"Exist InvaildPassword.bmp");
			return FALSE;
		}

		// Start Game
		if (PicFinder(L"ChoiceServer.bmp"))
		{
			Log(LOG_LEVEL_NORMAL, L"Exist ChoiceServer.bmp");
			ClickStartGame(pAccGame->hLauncher);
			continue;
		}

		// Safe Check
		if (PicFinder(L"SafeCheck.bmp"))
		{
			Log(LOG_LEVEL_NORMAL, L"Exist SafeCheck.bmp");

			// Delete File
			DeleteVerCodeFile();

			// 
			InputPass(pAccGame);
			::Sleep(5 * 1000);

			// Ver Code
			if (!HandleShowVerCode(pAccGame))
				return FALSE;

			continue;
		}

		::Sleep(1000);
	}
	return FALSE;
}

BOOL CGameLauncher::CloseAD(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw()
{
	// Set Window Topest
	::SetWindowPos(pAccGame->hLauncher, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	::SwitchToThisWindow(pAccGame->hLauncher, TRUE);
	::Sleep(1000);

	RECT LauncherRect = { 0 };
	::GetWindowRect(pAccGame->hLauncher, &LauncherRect);

	CTextConfig::ConfigRect ConfigRect_ = { 0 };
	if (!CTextConfig::GetInstance().GetConfigRect_By_KeyName(L"LauncherAD", ConfigRect_))
		return FALSE;

	CLPublic::SimulationMouse(static_cast<int>(LauncherRect.left) + ConfigRect_.nLeft, static_cast<int>(LauncherRect.top) + ConfigRect_.nTop);
	::Sleep(3000);
	return TRUE;
}

BOOL CALLBACK EnumGameClient(_In_ HWND hLauncher, LPARAM lpParm)
{
	if (IsWindow(hLauncher) && IsWindowVisible(hLauncher) && (GetWindowLong(hLauncher, GWL_EXSTYLE)&WS_EX_TOOLWINDOW) != WS_EX_TOOLWINDOW && (GetWindowLong(hLauncher, GWL_HWNDPARENT) == 0))
	{
		WCHAR wszWinText[64] = { 0 };
		WCHAR wszWinClass[64] = { 0 };

		if (GetClassNameW(hLauncher, wszWinClass, _countof(wszWinClass) / sizeof(WCHAR)) > 0 && GetWindowTextW(hLauncher, wszWinText, _countof(wszWinText)) > 0)
		{
			if (CCharacter::wstrcmp_my(wszWinClass, L"LaunchUnrealUWindowsClient"))//如果能遍历到这个,并且是
			{
				reinterpret_cast<ACCOUNT_INFO_GAME*>(lpParm)->hGameWnd = hLauncher;
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CGameLauncher::WaitForClient(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw()
{
	// Show Game Windows
	auto ulTick = ::GetTickCount64();
	while (true)
	{
		if (static_cast<DWORD>(::GetTickCount64() - ulTick) >= 3 * 60 * 1000)
		{
			Kill();
			return FALSE;
		}

		if (!CLProcess::Is_Exist_Process_For_ProcId(pAccGame->dwPid))
		{
			Kill();
			return FALSE;
		}

		if(!::EnumWindows((WNDENUMPROC)EnumGameClient, (LPARAM)pAccGame))
			break;

		::Sleep(1000);
	}
	return TRUE;
}

cwstring CGameLauncher::TestAsker(_In_ cwstring& wsPicPath) throw()
{
	return Asker(wsPicPath);
}

