#ifndef __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_GAMELAUNCHER_GAMELAUNCHER_H__
#define __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_GAMELAUNCHER_GAMELAUNCHER_H__

#include "ConsoleVariable.h"

class CGameLauncher : public CClassInstance<CGameLauncher>
{
public:
	CGameLauncher() = default;
	~CGameLauncher() = default;

	BOOL RunLauncher(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw();

	// Set Answer Ver Code
	BOOL SetAutoAnswerVerCode() throw();

	// Find Pic
	BOOL PicFinder(_In_ cwstring& wsPicName) CONST throw();

	cwstring TestAsker(_In_ cwstring& wsPicPath) throw();
private:
	// Create Launcher Process
	BOOL CreateGameLauncher() CONST throw();

	// Wait to show Launcher
	BOOL WaitToShow(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw();

	// Kill Launcher
	BOOL Kill() CONST throw();

	// EnumWindow
	static BOOL CALLBACK EnumGameLauncher(_In_ HWND hLauncher, LPARAM lpParm);

	// set AccountName for Launcher!
	BOOL SetLoginAccountNameForLauncher(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw();

	// wait to load complete for Launcher
	BOOL WaitToLauncherInitialize(_In_ HWND hLauncher) CONST throw();

	// input pass
	BOOL InputPass(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw();

	// Is show verCode
	BOOL HandleShowVerCode(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw();

	// Delete VerCode File
	BOOL DeleteVerCodeFile() CONST throw();

	// Answer Ver Code
	VOID AnswerVerCode(_In_ ACCOUNT_INFO_GAME* pAccGame, _In_ CONST std::wstring& wsVerCode) CONST throw();

	// Click StartGame
	VOID ClickStartGame(_In_ HWND hLauncher) CONST throw();

	// Set asker
	VOID SetAsker(_In_ std::function<std::wstring(CONST std::wstring&)> fnAsker) throw();

	// Save *.Bmp for deskWindow
	BOOL SaveDeskWindow() CONST throw();

	// Check AD
	BOOL LoginStep(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw();

	// Close AD
	BOOL CloseAD(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw();

	// wait to Show Client
	BOOL WaitForClient(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw();
private:
	std::function<std::wstring(CONST std::wstring&)> Asker;
};



#endif // !__AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_GAMELAUNCHER_GAMELAUNCHER_H__
