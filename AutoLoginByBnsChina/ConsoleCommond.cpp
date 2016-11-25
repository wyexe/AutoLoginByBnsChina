#include "ConsoleCommond.h"
#include <MyTools/Character.h>
#include "TextConfig.h"
#include "AccountServices.h"

DWORD WINAPI CConsoleCommond::ResetAccount(vector<GrammarContext>& vlst, LPWSTR pwszRetMsg)
{
	// -all || -account -123456
	BOOL bRetCode = FALSE;
	if (vlst.size() < 2)
	{
		swprintf_s(pwszRetMsg, 1024 - 1, CTextConfig::GetInstance().GetText_By_Code(0xF).c_str(), 2);
		return FALSE;
	}
	else if (CCharacter::wstrcmp_my(vlst.at(1).szCmd, L"all"))
	{
		bRetCode = CTextConfig::GetInstance().ResetAccountSchedule([](CONST TextAccountSchedule& AccountSchedule_) {
			return TRUE;
		},
			[](TextAccountSchedule& AccountSchedule_) {
			AccountSchedule_.bFinish = FALSE;
		});
	}
	else if (CCharacter::wstrcmp_my(vlst.at(1).szCmd, L"account"))
	{
		if (vlst.size() < 3)
		{
			swprintf_s(pwszRetMsg, 1024 - 1, CTextConfig::GetInstance().GetText_By_Code(0xF).c_str(), 3);
			return FALSE;
		}

		cwstring wsAccountName = vlst.at(2).szCmd;
		bRetCode = CTextConfig::GetInstance().ResetAccountSchedule([&wsAccountName](CONST TextAccountSchedule& AccountSchedule_) {
			return AccountSchedule_.AccountContent.wsAccountName == wsAccountName;
		},
			[](TextAccountSchedule& AccountSchedule_) {
			AccountSchedule_.bFinish = FALSE;
		});
	}
	else
	{
		swprintf_s(pwszRetMsg, 1024 - 1, CTextConfig::GetInstance().GetText_By_Code(0x10).c_str());
		return 0;
	}
	swprintf_s(pwszRetMsg, 1024 - 1, L"ResetAccount %s!", bRetCode ? L"Succ" : L"Faild");
	return 0;
}

DWORD WINAPI CConsoleCommond::Run(vector<GrammarContext>& vlst, LPWSTR pwszRetMsg)
{
	std::vector<TextAccountSchedule> AccountScheduleVec;
	if (!CTextConfig::GetInstance().GetAccountSchedule(AccountScheduleVec))
	{
		swprintf_s(pwszRetMsg, 1024 - 1, CTextConfig::GetInstance().GetText_By_Code(0x18).c_str());
		return 0;
	}

	if (!CAccountServices::GetInstance().InitializeAccountShare())
	{
		swprintf_s(pwszRetMsg, 1024 - 1, L"InitializeAccountShare = FALSE!");
		return 0;
	}

	std::wstring wsLoginFaildCount;
	if (!CTextConfig::GetInstance().GetConfigValue_By_KeyName(L"MaxLoginFaildCount", wsLoginFaildCount))
	{
		swprintf_s(pwszRetMsg, 1024 - 1, L"Run Faild!");
		return 0;
	}

	UINT uLoginFaildCount = static_cast<UINT>(_wtoi(wsLoginFaildCount.c_str()));
	auto pShare = CConsoleVariable::GetInstance().GetShareInfo();

	ACCOUNT_INFO_GAME* pAccGame = nullptr;
	while ((pAccGame = pShare->GetUnLoginArrGameInfo()) != nullptr)
	{
		CConsoleVariable::GetInstance().PrintToConsole(CTextConfig::GetInstance().GetText_By_Code(0x19).c_str(), pAccGame->GetAccName());
		CAccountServices::GetInstance().RunGame(pAccGame);
		if (pAccGame->AccountStatus.bPassInvalid)
		{
			CConsoleVariable::GetInstance().PrintErrLog(CTextConfig::GetInstance().GetText_By_Code(0x2B).c_str(), pAccGame->GetAccName(), pAccGame->GetAccName());
			pAccGame->AccountStatus.bDone = TRUE;
		}
		else if (pAccGame->uLoginFaildCount >= uLoginFaildCount)
		{
			CConsoleVariable::GetInstance().PrintErrLog(CTextConfig::GetInstance().GetText_By_Code(0x1D).c_str(), pAccGame->GetAccName(), uLoginFaildCount);
			pAccGame->AccountStatus.bDone = TRUE;
		}
	}

	swprintf_s(pwszRetMsg, 1024 - 1, L"Run Done!");
	return 0;
}

DWORD WINAPI CConsoleCommond::Stop(vector<GrammarContext>& vlst, LPWSTR pwszRetMsg)
{
	return TRUE;
}

DWORD WINAPI CConsoleCommond::Total(vector<GrammarContext>& vlst, LPWSTR pwszRetMsg)
{
	CAccountServices::GetInstance().PrintTotal();
	return 0;
}
