#include "GameLauncher.h"
#include <MyTools/CLProcess.h>
#include <MyTools/Character.h>
#include <MyTools/CLPublic.h>
#include "TextConfig.h"

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

	// Input Account Password
	InputPass(pAccGame);

	if (!HandleShowVerCode(pAccGame))
		return FALSE;

	ClickStartGame();
	if (!WaitForClient(pAccGame))
		return FALSE;

	WCHAR wszDLLPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryW(MAX_PATH, wszDLLPath);
	lstrcatW(wszDLLPath, L"\\DLL\\BnsDLL.dll");
	return CLProcess::LoadRemoteDLL(CLProcess::GetPid_For_ProcName(L"Client.exe"), wszDLLPath);
}

BOOL CGameLauncher::CreateGameLauncher() CONST throw()
{
	if (CLProcess::Is_Exist_Process_For_ProcName(L"Launcher.exe"))
		return TRUE;

	std::wstring wsGameLauncherPath;
	if (!CTextConfig::GetInstance().GetConfigValue_By_KeyName(L"GameLauncherPath", wsGameLauncherPath))
		return FALSE;


	CLProcess::CreateProcess_InjectorRemoteDLL(wsGameLauncherPath.c_str(), nullptr, nullptr);
	::Sleep(10 * 1000);
	return CLProcess::Is_Exist_Process_For_ProcName(L"Launcher.exe");
}

BOOL CGameLauncher::WaitToShow(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw()
{
	DWORD dwMaxTime = NULL;
	if (!CTextConfig::GetInstance().GetConfigHexValue_By_KeyName(L"ShowLuncherTimeOut", dwMaxTime))
		return FALSE;

	auto ulTick = ::GetTickCount64();
	while (true)
	{
		if (static_cast<DWORD>(::GetTickCount64() - ulTick) >= dwMaxTime * 1000)
		{
			Kill();
			return FALSE;
		}

		::EnumWindows((WNDENUMPROC)EnumGameLauncher, (LPARAM)pAccGame);
		::Sleep(1000);
	}
	return WaitToLauncherInitialize();
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
			//if (CCharacter::wstrcmp_my(wszWinClass, L"LaunchUnrealUWindowsClient"))//如果能遍历到这个,并且是
			if(CCharacter::wstrcmp_my(wszWinText,L"剑灵登录程序"))
			{
				DWORD PID;
				::GetWindowThreadProcessId(hLauncher, &PID);
				if (PID == ::GetCurrentProcessId())
				{
					reinterpret_cast<ACCOUNT_INFO_GAME*>(lpParm)->hLauncher = hLauncher;
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

BOOL CALLBACK CGameLauncher::EnumGameClient(_In_ HWND hLauncher, LPARAM lpParm)
{
	if (IsWindow(hLauncher) && IsWindowVisible(hLauncher) && (GetWindowLong(hLauncher, GWL_EXSTYLE)&WS_EX_TOOLWINDOW) != WS_EX_TOOLWINDOW && (GetWindowLong(hLauncher, GWL_HWNDPARENT) == 0))
	{
		WCHAR wszWinText[64] = { 0 };
		WCHAR wszWinClass[64] = { 0 };

		if (GetClassNameW(hLauncher, wszWinClass, _countof(wszWinClass) / sizeof(WCHAR)) > 0 && GetWindowTextW(hLauncher, wszWinText, _countof(wszWinText)) > 0)
		{
			if (CCharacter::wstrcmp_my(wszWinClass, L"LaunchUnrealUWindowsClient"))//如果能遍历到这个,并且是
			{
				DWORD PID;
				::GetWindowThreadProcessId(hLauncher, &PID);
				if (PID == ::GetCurrentProcessId())
				{
					reinterpret_cast<ACCOUNT_INFO_GAME*>(lpParm)->hGameWnd = hLauncher;
					return FALSE;
				}
			}
		}
	}
	return TRUE;
}

BOOL CGameLauncher::SetLoginAccountNameForLauncher(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw()
{
	std::wstring wsGameLauncherPath;
	if (!CTextConfig::GetInstance().GetConfigValue_By_KeyName(L"GameLauncherPath", wsGameLauncherPath))
		return FALSE;

	if (wsGameLauncherPath.find(L"Launcher.exe") == -1)
		return FALSE;

	CCharacter::GetRemoveLeft(wsGameLauncherPath, L"Launcher.exe", wsGameLauncherPath);
	wsGameLauncherPath += L"config\\LoginQ.dat";

	return ::WritePrivateProfileStringW(L"Public", L"LastQQUin", pAccGame->GetAccName(), wsGameLauncherPath.c_str());
}

BOOL CGameLauncher::WaitToLauncherInitialize() CONST throw()
{
	// wait to load complete
	DWORD dwTime = NULL;
	if (!CTextConfig::GetInstance().GetConfigHexValue_By_KeyName(L"TimeForShowLauncher", dwTime))
		return FALSE;

	::Sleep(dwTime);
	return TRUE;
}

BOOL CGameLauncher::InputPass(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw()
{
	// Set Window Topest
	::SetWindowPos(pAccGame->hLauncher, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	::SwitchToThisWindow(pAccGame->hLauncher, TRUE);
	::Sleep(1000);

	CLPublic::SimulationMouse(650, 300);
	CLPublic::SendKeys(CCharacter::UnicodeToASCII(cwstring(pAccGame->MyAcount_Info.szPassWord)).c_str());
	return CLPublic::SimulationKey(VK_RETURN, pAccGame->hLauncher);
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

	for (int i = 0;i < 10; ++i)
	{
		if(CLPublic::FileExit(wsVerCodePath1.c_str()))
		{
			auto wsAnswer = Asker(wsVerCodePath1);
			::DeleteFileW(wsVerCodePath1.c_str());
			AnswerVerCode(pAccGame, wsAnswer);
			i = 0;
			continue;
		}

		if (CLPublic::FileExit(wsVerCodePath2.c_str()))
		{
			auto wsAnswer = Asker(wsVerCodePath2);
			::DeleteFileW(wsVerCodePath2.c_str());
			AnswerVerCode(pAccGame, wsAnswer);
			i = 0;
			continue;
		}
		::Sleep(1000);
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
	CLPublic::SimulationMouse(758, 407);
	CLPublic::SendKeys(CCharacter::UnicodeToASCII(wsVerCode).c_str());
	CLPublic::SimulationKey(VK_RETURN, pAccGame->hLauncher);
}

VOID CGameLauncher::ClickStartGame() CONST throw()
{
	CLPublic::SimulationMouse(400, 580);
}

BOOL CGameLauncher::WaitForClient(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw()
{
	// Show Client.exe in 10s
	if (!IsShowClient())
		return FALSE;

	// Show Game Windows
	auto ulTick = ::GetTickCount64();
	while (true)
	{
		if (static_cast<DWORD>(::GetTickCount64() - ulTick) >= 60 * 1000)
		{
			Kill();
			return FALSE;
		}

		::EnumWindows((WNDENUMPROC)EnumGameClient, (LPARAM)pAccGame);
		::Sleep(1000);
	}
	return TRUE;
}

BOOL CGameLauncher::IsShowClient() CONST throw()
{
	for (int i = 0;i < 10; ++i)
	{
		if (CLProcess::Is_Exist_Process_For_ProcName(L"Client.exe"))
			return TRUE;
		::Sleep(1000);
	}
	return FALSE;
}
