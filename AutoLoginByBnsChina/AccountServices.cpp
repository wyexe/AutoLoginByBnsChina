#include "AccountServices.h"
#include "TextConfig.h"
#include <MyTools/Character.h>
#include <MyTools/CLProcess.h>
#include "GameLauncher.h"

LPCWSTR CAccountServices::GetLoginingAccountName() CONST throw()
{
	auto pshare = CConsoleVariable::GetInstance().GetShareInfo();
	if (pshare == nullptr)
		return nullptr;

	auto pAccGame = std::find_if(std::begin(pshare->arrGameInfo), std::end(pshare->arrGameInfo), [](CONST ACCOUNT_INFO_GAME& AccGame) {
		return AccGame.AccountStatus.bLogining;
	});

	if (pAccGame == nullptr)
		return nullptr;

	return pAccGame->GetAccName();
}

VOID CAccountServices::PrintTotal() CONST throw()
{
	UINT uFinishCount	= 0;
	UINT uUnFinishCount = 0;
	UINT uCount			= 0;
	UINT uVolumnCount	= 0;

	// Logining Account + Finish AccountCount + UnFinish AccountCount + Sum Volumn Count
	auto pszLoginingAccountName = CAccountServices::GetInstance().GetLoginingAccountName();
	if (pszLoginingAccountName == nullptr)
		CConsoleVariable::GetInstance().PrintToConsole(CTextConfig::GetInstance().GetText_By_Code(0x11).c_str());
	else
		CConsoleVariable::GetInstance().PrintToConsole(CTextConfig::GetInstance().GetText_By_Code(0x12).c_str(), pszLoginingAccountName);

	CTextConfig::GetInstance().ResetAccountSchedule([](CONST TextAccountSchedule& AccountSchedule_) {
		return AccountSchedule_.bFinish;
	},
		[&uFinishCount](TextAccountSchedule&) {
		uFinishCount += 1;
	});

	CTextConfig::GetInstance().ResetAccountSchedule([](CONST TextAccountSchedule& AccountSchedule_) {
		return !AccountSchedule_.bFinish;
	},
		[&uUnFinishCount](TextAccountSchedule&) {
		uUnFinishCount += 1;
	});

	CTextConfig::GetInstance().ResetAccountSchedule([](CONST TextAccountSchedule&) {return TRUE; }, [&uCount](TextAccountSchedule&) {uCount += 1; });

	CTextConfig::GetInstance().ResetAccountSchedule([](CONST TextAccountSchedule&) {return TRUE; }, [&uVolumnCount](TextAccountSchedule& AccountSchedule_) { uVolumnCount += AccountSchedule_.uVolume; });

	CConsoleVariable::GetInstance().PrintToConsole(CTextConfig::GetInstance().GetText_By_Code(0x14).c_str(), uFinishCount);
	CConsoleVariable::GetInstance().PrintToConsole(CTextConfig::GetInstance().GetText_By_Code(0x15).c_str(), uUnFinishCount);
	CConsoleVariable::GetInstance().PrintToConsole(CTextConfig::GetInstance().GetText_By_Code(0x16).c_str(), uCount);
	CConsoleVariable::GetInstance().PrintToConsole(CTextConfig::GetInstance().GetText_By_Code(0x17).c_str(), uVolumnCount);
}

BOOL CAccountServices::InitializeAccountShare() CONST throw()
{
	auto& pShare = CConsoleVariable::GetInstance().GetShareInfo();

	std::vector<TextAccountSchedule> AccountScheduleVec;
	if (!CTextConfig::GetInstance().GetAccountSchedule(AccountScheduleVec))
		return FALSE;
	
	for (CONST auto& itm : AccountScheduleVec)
	{
		if (itm.bFinish)
			continue;

		auto pAccGame = pShare->ExistEmptyArrGameInfo();
		if (pAccGame == nullptr)
		{
			CConsoleVariable::GetInstance().PrintErrLog(L"ExistEmptyArrGameInfo = nullptr, AccountScheduleVec=%d, nAccountCount=%d", AccountScheduleVec.size(), pShare->nAccountCount);
			return FALSE;
		}

		pAccGame->Clear();
		CCharacter::wstrcpy_my(pAccGame->MyAcount_Info.szUserName, itm.AccountContent.wsAccountName.c_str());
		CCharacter::wstrcpy_my(pAccGame->MyAcount_Info.szPassWord, itm.AccountContent.wsAccountPass.c_str());
		pAccGame->AccountStatus.bExist = TRUE;
		pShare->nAccountCount += 1;
	}

	return TRUE;
}

BOOL CAccountServices::RunGame(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw()
{
	pAccGame->Clear();
	pAccGame->AccountStatus.bLogining = TRUE;

	PrintTotal();

	DWORD dwMaxLoginTime = CConsoleVariable::GetInstance().GetMaxLoginTime();
	auto ulTick = ::GetTickCount64();
	
	if (!CGameLauncher::GetInstance().RunLauncher(pAccGame))
	{
		KillGame(pAccGame);
		pAccGame->AccountStatus.bLogining = FALSE;
		return FALSE;
	}

	while (true)
	{
		if (pAccGame->AccountStatus.bClose)
		{
			pAccGame->uLoginFaildCount += 1;
			KillGame(pAccGame);
			break;
		}

		if (pAccGame->AccountStatus.bDone)
		{
			KillGame(pAccGame);

			// print to console !
			CConsoleVariable::GetInstance().PrintToConsole(CTextConfig::GetInstance().GetText_By_Code(0x1C).c_str(), pAccGame->MyAcount_Info.szUserName);

			// Save Account infomation to File!
			CTextConfig::GetInstance().ResetAccountSchedule([&pAccGame](CONST TextAccountSchedule& AccountSchedule)
			{
				return AccountSchedule.AccountContent.wsAccountName == cwstring(pAccGame->GetAccName());
			}, [&pAccGame](TextAccountSchedule& AccountSchedule)
			{
				AccountSchedule.bFinish = TRUE;
				AccountSchedule.uVolume = pAccGame->MyAcount_Info.uVolumn;
				for (UINT i = 0;i < pAccGame->uAccountPlayerCount; ++i)
				{
					AccountPlayerConfig PlayerConfig_;
					PlayerConfig_.uIndex		= pAccGame->AccountPlayerArr[i].uIndex;
					PlayerConfig_.uLevel		= pAccGame->AccountPlayerArr[i].uLevel;
					PlayerConfig_.emPlayerClass = pAccGame->AccountPlayerArr[i].emPlayerClasses;
					PlayerConfig_.wsPlayerName	= pAccGame->AccountPlayerArr[i].wszPlayerName;
					AccountSchedule.AccountPlayerConfigVec.push_back(PlayerConfig_);
				}
			});
			break;
		}

		if (static_cast<DWORD>(::GetTickCount64() - ulTick) >= dwMaxLoginTime * 1000)
		{
			CConsoleVariable::GetInstance().PrintToConsole(CTextConfig::GetInstance().GetText_By_Code(0x1F).c_str(), pAccGame->GetAccName(), dwMaxLoginTime);
			pAccGame->uLoginFaildCount += 1;
			KillGame(pAccGame);
			break;
		}

		
		
	}

	pAccGame->AccountStatus.bLogining = FALSE;
	return TRUE;
}

BOOL CAccountServices::KillGame(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw()
{
	while (CLProcess::Is_Exist_Process_For_ProcName(L"Launcher.exe"))
	{
		CConsoleVariable::GetInstance().PrintToConsole(CTextConfig::GetInstance().GetText_By_Code(0x1A).c_str());
		CLProcess::TerminateProc_For_ProcName(L"Launcher.exe");
		::Sleep(1000);
	}

	while (CLProcess::Is_Exist_Process_For_ProcName(L"Client.exe"))
	{
		CConsoleVariable::GetInstance().PrintToConsole(CTextConfig::GetInstance().GetText_By_Code(0x1B).c_str());
		CLProcess::TerminateProc_For_ProcName(L"Client.exe");
		::Sleep(1000);
	}
	return TRUE;
}
