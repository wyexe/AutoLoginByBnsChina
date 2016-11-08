#include "ConsoleVariable.h"
#include <map>
#include <iostream>
#include <MyTools/CLLock.h>
#include <MyTools/CLPublic.h>
#include <MyTools/Character.h>
#include "TextConfig.h"

#define _SELF L"ConsoleVariable.cpp"

CConsoleVariable::CConsoleVariable()
{
	Loger = [](_In_ LPCWSTR LogBuffer) {LogMsgBox(LOG_LEVEL_EXCEPTION, L"%s", LogBuffer); };
}

DWORD& CConsoleVariable::GetVariableReferenceByID(_In_ em_Console_Variable dwVarID) CONST throw()
{
	static std::map<em_Console_Variable, DWORD> GlobalVariableMap;
	static CLLock Lock(L"CConsoleVariable::GetVariableReferenceByID");


	DWORD* pValue = nullptr;
	Lock.Access([&pValue, &dwVarID] {
		auto& itr = GlobalVariableMap.find(dwVarID);
		if (itr != GlobalVariableMap.end())
			pValue = &itr->second;
		else
			GlobalVariableMap.insert(map<em_Console_Variable, DWORD>::value_type(dwVarID, 0));
	});

	return pValue != nullptr ? *pValue : GetVariableReferenceByID(dwVarID);
}

DWORD CConsoleVariable::GetVariableByID(_In_ em_Console_Variable VarID) CONST throw()
{
	return GetVariableReferenceByID(VarID);
}

DWORD CConsoleVariable::SetVariableAndGetOldValueByID(_In_ em_Console_Variable VarID, _In_ DWORD NewValue) CONST throw()
{
	auto& dwValue = GetVariableReferenceByID(VarID);
	::swap(dwValue, NewValue);
	return NewValue;
}

PSHARED_INFO& CConsoleVariable::GetShareInfo() CONST throw()
{
	return CClassInstance::GetStaticVariable<PSHARED_INFO>();
}

BOOL CConsoleVariable::DoEqualVariableByID(_In_ em_Console_Variable VarID, _In_ DWORD dwValue, _In_ std::function<VOID(VOID)> Worker) CONST throw()
{
	if (GetVariableByID(VarID) == dwValue)
	{
		Worker();
		return TRUE;
	}
	return FALSE;
}


BOOL CConsoleVariable::GetPlayerClassesText(_In_ em_Player_Classes emPlayerClasses, _Out_ std::wstring& wsText) CONST throw()
{
	auto& Vec = GetPlayerClassTextVec();

	auto p = CLPublic::Vec_find_if_Const(Vec, [&emPlayerClasses](CONST PlayerClassText& itm) {return itm.emClasses == emPlayerClasses; });
	if (p == nullptr)
	{
		PrintErrLog(CTextConfig::GetInstance().GetText_By_Code(0xB).c_str(), emPlayerClasses);
		return FALSE;
	}
	wsText = p->wsClassText;
	return TRUE;
}

BOOL CConsoleVariable::GetPlayerClassByText(_In_ CONST std::wstring& wsText, _Out_ em_Player_Classes& emPlayerClass) CONST throw()
{
	auto& Vec = GetPlayerClassTextVec();

	auto p = CLPublic::Vec_find_if_Const(Vec, [&wsText](CONST PlayerClassText& itm) {return itm.wsClassText == wsText; });
	if (p == nullptr)
	{
		PrintErrLog(CTextConfig::GetInstance().GetText_By_Code(0xC).c_str(), wsText.c_str());
		return FALSE;
	}
	emPlayerClass = p->emClasses;
	return TRUE;
}

VOID CConsoleVariable::SetLogerByConfig() throw()
{
	std::wstring wsValue;
	if (!CTextConfig::GetInstance().GetConfigValue_By_KeyName(L"LogType", wsValue))
	{
		::MessageBoxW(NULL, CTextConfig::GetInstance().GetText_By_Code(0xD).c_str(), L"Error", NULL);
		ExitProcess(0);
	}
	else
	{
		CCharacter::Trim_W(wsValue);
		if (wsValue == L"MessageBox")
		{
			Loger = [](_In_ LPCWSTR LogBuffer) 
			{ 
				LogMsgBox(LOG_LEVEL_EXCEPTION, L"%s", LogBuffer); 
				::MessageBoxW(NULL, CTextConfig::GetInstance().GetText_By_Code(0xD).c_str(), L"Error", NULL);
				ExitProcess(0);
			};
			
		}
		else if (wsValue == L"Console")
		{
			Loger = [](_In_ LPCWSTR LogBuffer) 
			{ 
				std::wcout << LogBuffer << std::endl; 
				std::wcout << CTextConfig::GetInstance().GetText_By_Code(0xD).c_str() << std::endl;
				system("pause");
				ExitProcess(0);
			};
		}
		else
		{
			::MessageBoxW(NULL, CTextConfig::GetInstance().GetText_By_Code(0xE).c_str(), L"Error", NULL);
		}
	}
}

VOID CConsoleVariable::PrintErrLog(_In_ LPCWSTR pwszFormat, ...) CONST throw()
{
	va_list		args;
	wchar_t		Buffer[1024] = { 0 };

	va_start(args, pwszFormat);
	_vsnwprintf_s(Buffer, _countof(Buffer) - 1, _TRUNCATE, pwszFormat, args);
	va_end(args);
	
	Loger(Buffer);
}

cwstring CConsoleVariable::ConvertToText(_In_ LPCWSTR pwszFormat, ...) CONST throw()
{
	va_list		args;
	wchar_t		Buffer[1024] = { 0 };

	va_start(args, pwszFormat);
	_vsnwprintf_s(Buffer, _countof(Buffer) - 1, _TRUNCATE, pwszFormat, args);
	va_end(args);

	return cwstring(Buffer);
}

CONST std::wstring& CConsoleVariable::GetLanguage() CONST throw()
{
	static std::wstring wsLanguage = L"Empty";
	if (wsLanguage == L"Empty")
	{
		if (!CTextConfig::GetInstance().GetConfigValue_By_KeyName(L"Language", wsLanguage))
		{
			PrintErrLog(L"where's Language=? in Config.ini");
			ExitProcess(0);
		}
	}
	return wsLanguage;
}

VOID CConsoleVariable::PrintToConsole(_In_ LPCWSTR pwszFormat, ...) CONST throw()
{
	va_list		args;
	wchar_t		Buffer[1024] = { 0 };

	va_start(args, pwszFormat);
	_vsnwprintf_s(Buffer, _countof(Buffer) - 1, _TRUNCATE, pwszFormat, args);
	va_end(args);

	static CLLock Lock(L"CConsoleVariable::PrintToConsole");
	Lock.Access([&Buffer] {
		wcout << Buffer << endl;
	});
}

DWORD CConsoleVariable::GetMaxLoginTime() CONST throw()
{
	static DWORD dwMaxLoginTime = NULL;
	if (dwMaxLoginTime == NULL)
	{
		std::wstring wsMaxLoginTime;
		if (!CTextConfig::GetInstance().GetConfigValue_By_KeyName(L"MaxLoginTime", wsMaxLoginTime))
		{
			PrintErrLog(CTextConfig::GetInstance().GetText_By_Code(0x1E).c_str());
			ExitProcess(0);
		}
		dwMaxLoginTime = static_cast<DWORD>(_wtoi(wsMaxLoginTime.c_str()));
	}
	return dwMaxLoginTime;
}

CONST vector<CConsoleVariable::PlayerClassText>& CConsoleVariable::GetPlayerClassTextVec() CONST throw()
{
	CONST static vector<PlayerClassText> Vec = {
		{ em_Player_Classes::em_Player_Classes_Assassin,			L"刺客" },
		{ em_Player_Classes::em_Player_Classes_Destroyer,			L"力士" },
		{ em_Player_Classes::em_Player_Classes_BladeMaster,			L"剑士" },
		{ em_Player_Classes::em_Player_Classes_Summoner,			L"召唤师" },
		{ em_Player_Classes::em_Player_Classes_ForceMaster,			L"气功" },
		{ em_Player_Classes::em_Player_Classes_KongFuMaster,		L"拳师" },
		{ em_Player_Classes::em_Player_Classes_Danncer,				L"灵剑士" },
		{ em_Player_Classes::em_Player_Classes_Warlock,				L"咒术师" },
		{ em_Player_Classes::em_Player_Classes_KongFuForceMaster,	L"气宗" },
	};
	return Vec;
}
