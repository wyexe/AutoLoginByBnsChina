#include "ConsoleCommond.h"
#include <MyTools/Character.h>
#include "TextConfig.h"

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
	return 0;
}

DWORD WINAPI CConsoleCommond::Total(vector<GrammarContext>& vlst, LPWSTR pwszRetMsg)
{
	// Logining Account + Finish AccountCount + UnFinish AccountCount + Sum Volumn Count
	return 0;
}
