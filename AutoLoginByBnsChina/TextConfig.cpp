#include "TextConfig.h"
#include <MyTools/CLFile.h>
#include <MyTools/CLPublic.h>
#include <MyTools/Character.h>

#define _SELF L"TextConfig.cpp"

BOOL CTextConfig::GetAccountSchedule(_Out_ std::vector<TextAccountSchedule>& vlst) CONST throw()
{
	vector<AccountConfig> AccountConfigVec;
	if (!ReadAccountConfig_By_File(AccountConfigVec))
		return FALSE;

	std::vector<TextAccountSchedule> TextAccountScheduleVec;
	if (!ReadAccountSchedule_By_File(TextAccountScheduleVec))
		return FALSE;

	for (CONST auto& itm : AccountConfigVec)
	{
		auto p = CLPublic::Vec_find_if(TextAccountScheduleVec, [&itm](CONST TextAccountSchedule AccountSchedule_) {
			return AccountSchedule_.AccountContent.wsAccountName == itm.wsAccountName;
		});

		if (p != nullptr)
		{
			p->AccountContent = itm;
			vlst.push_back(*p);
		}
		else
		{
			TextAccountSchedule AccountSchedule_;
			AccountSchedule_.Clear();

			AccountSchedule_.AccountContent = itm;
			vlst.push_back(AccountSchedule_);
		}
	}

	return TRUE;
}

BOOL CTextConfig::GetConfigValue_By_KeyName(_In_ CONST std::wstring& wsKeyName, _Out_ std::wstring& wsConfigValue) CONST throw()
{
	WCHAR wszPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryW(MAX_PATH, wszPath);
	lstrcatW(wszPath, L"\\Config.ini");
	if (!CLPublic::FileExit(wszPath))
	{
		CConsoleVariable::GetInstance().PrintErrLog(GetText_By_Code(0x2).c_str());
		return FALSE;
	}

	WCHAR wszValue[MAX_PATH] = { 0 };
	if (!::GetPrivateProfileStringW(L"ConsoleConfig", wsKeyName.c_str(), L"Empty", wszValue, _countof(wszValue), wszPath))
	{
		CConsoleVariable::GetInstance().PrintErrLog(GetText_By_Code(0x3).c_str(), wszPath, ::GetLastError());
		return FALSE;
	}

	if (CCharacter::wstrcmp_my(wszValue,L"Empty"))
	{
		CConsoleVariable::GetInstance().PrintErrLog(GetText_By_Code(0x4).c_str(), wsKeyName.c_str());
		return FALSE;
	}
	wsConfigValue = wszValue;
	return TRUE;
}

BOOL CTextConfig::GetConfigHexValue_By_KeyName(_In_ CONST std::wstring& wsKeyName, _Out_ DWORD& dwValue) CONST throw()
{
	std::wstring wsValue;
	if (!GetConfigValue_By_KeyName(wsKeyName, wsValue))
		return FALSE;

	dwValue = static_cast<DWORD>(_wtoi(wsValue.c_str()));
	return TRUE;
}

BOOL CTextConfig::GetConfigRect_By_KeyName(_In_ CONST std::wstring& wsKeyName, _Out_ ConfigRect& ConfigRect_) CONST throw()
{
	std::wstring wsValue;
	if (!GetConfigValue_By_KeyName(wsKeyName, wsValue))
		return FALSE;

	vector<std::wstring> ParmVec;
	CCharacter::Split(wsValue, L",", ParmVec, Split_Option_KeepOnly | Split_Option_RemoveEmptyEntries);
	if (ParmVec.size() != 2)
	{
		CConsoleVariable::GetInstance().PrintErrLog(GetText_By_Code(0x27).c_str(), wsKeyName.c_str(), wsValue.c_str());
		return FALSE;
	}

	ConfigRect_.nLeft = _wtoi(ParmVec.at(0).c_str());
	ConfigRect_.nTop = _wtoi(ParmVec.at(1).c_str());
	return TRUE;
}

BOOL CTextConfig::ResetAccountSchedule(_In_ std::function<BOOL(CONST TextAccountSchedule&)> Finder, _In_ std::function<VOID(TextAccountSchedule&)> Seter) CONST throw()
{
	vector<TextAccountSchedule> vlst;
	if (!GetAccountSchedule(vlst))
		return FALSE;

	// Clear All of Account in File
	ClearAccountScheduleFile();

	for (auto& itm : vlst)
	{
		if (Finder(itm))
			Seter(itm);
		AppendAccountScedule(itm);
	}
	return TRUE;
}

BOOL CTextConfig::AppendAccountScedule(CONST TextAccountSchedule& AccountSchedule_) CONST throw()
{
	// Text Format:
	// AccountName, bFinish, uVolume \r\n
	// <!--\r\n
	// Index, PlayerName, PlayerClass, Level \r\n
	// -->\r\n

	std::wstring wsText;

	wsText += AccountSchedule_.AccountContent.wsAccountName;
	wsText += L",";
	wsText += AccountSchedule_.AccountContent.wsAccountPass;
	wsText += L",";
	wsText += CConsoleVariable::GetInstance().ConvertToText(L"%d", AccountSchedule_.bFinish);
	wsText += L",";
	wsText += CConsoleVariable::GetInstance().ConvertToText(L"%d", AccountSchedule_.uVolume);


	wsText += L"\r\n<!-- \r\n";
	for (CONST auto& itm : AccountSchedule_.AccountPlayerConfigVec)
	{
		wsText += CConsoleVariable::GetInstance().ConvertToText(L"%d", itm.uIndex);
		wsText += L",";
		wsText += itm.wsPlayerName;
		wsText += L",";
		
		std::wstring wsClassText;
		if (!CConsoleVariable::GetInstance().GetPlayerClassesText(itm.emPlayerClass, wsClassText))
			return FALSE;

		wsText += wsClassText;
		wsText += L",";
		wsText += CConsoleVariable::GetInstance().ConvertToText(L"%d", itm.uLevel);
		wsText += L"\r\n";
	}
	wsText += L"-->\r\n";


	WCHAR wszPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryW(MAX_PATH, wszPath);
	lstrcatW(wszPath, L"\\AccountSchedule.txt");
	if (!CLPublic::FileExit(wszPath))
	{
		CConsoleVariable::GetInstance().PrintErrLog(GetText_By_Code(0x1).c_str());
		return FALSE;
	}

	return CLFile::WriteASCIIFile(wszPath, wsText);
}

CONST std::wstring CTextConfig::GetText_By_Code(_In_ DWORD dwCode) CONST throw()
{
	WCHAR wszPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryW(MAX_PATH, wszPath);
	lstrcatW(wszPath, L"\\Language\\Text.ini");
	if (!CLPublic::FileExit(wszPath))
	{
		CConsoleVariable::GetInstance().PrintErrLog(L"where's the '%s'?", wszPath);
		ExitProcess(0);
	}

	WCHAR wszValue[MAX_PATH] = { 0 };
	if (!::GetPrivateProfileStringW(CConsoleVariable::GetInstance().GetLanguage().c_str(), CConsoleVariable::GetInstance().ConvertToText(L"%X", dwCode).c_str(), L"Empty", wszValue, _countof(wszValue), wszPath))
	{
		CConsoleVariable::GetInstance().PrintErrLog(L"Read '%s' Error, Code=%X", wszPath,::GetLastError());
		ExitProcess(0);
	}

	if (CCharacter::wstrcmp_my(wszValue, L"Empty"))
	{
		CConsoleVariable::GetInstance().PrintErrLog(L"KeyName:%X, UnExist Value!", dwCode);
		ExitProcess(0);
	}
	
	return cwstring(wszValue);
}

BOOL CTextConfig::ReadAccountConfig_By_File(_Out_ std::vector<AccountConfig>& vlst) CONST throw()
{
	std::wstring wsText;

	WCHAR wszPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryW(MAX_PATH, wszPath);
	lstrcatW(wszPath, L"\\Account.txt");
	if (!CLPublic::FileExit(wszPath))
	{
		CConsoleVariable::GetInstance().PrintErrLog(GetText_By_Code(0x1).c_str());
		return FALSE;
	}

	if (!ReadASCIIFile_By_Path(wszPath, wsText))
		return FALSE;

	std::vector<std::wstring> TextAccountVec;
	CCharacter::Split(wsText, L"\r\n", TextAccountVec, Split_Option_RemoveEmptyEntries | Split_Option_KeepOnly);
	for (CONST auto& itm : TextAccountVec)
	{
		std::vector<std::wstring> SingleAccountVec;
		if (CCharacter::Split(itm, L",", SingleAccountVec, Split_Option_RemoveEmptyEntries | Split_Option_KeepOnly) != 2)
		{
			CConsoleVariable::GetInstance().PrintErrLog(GetText_By_Code(0x6).c_str(), itm.c_str());
			return FALSE;
		}

		vlst.push_back(AccountConfig{ SingleAccountVec.at(0),SingleAccountVec.at(1) });
	}

	if (vlst.size() == 0)
	{
		CConsoleVariable::GetInstance().PrintErrLog(GetText_By_Code(0x7).c_str());
		return FALSE;
	}

	return TRUE;
}

BOOL CTextConfig::ReadAccountSchedule_By_File(_In_ _Out_ std::vector<TextAccountSchedule>& vlst) CONST throw()
{
	// Text Format:
	// AccountName, bFinish, uVolume \r\n
	// <!-- \r\n
	// Index, PlayerName, PlayerClass, Level \r\n
	// --> \r\n

	TextAccountSchedule AccountSchedule_;
	std::wstring		wsText;
	BOOL				bStartGroup			= FALSE;

	WCHAR wszPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryW(MAX_PATH, wszPath);
	lstrcatW(wszPath, L"\\AccountSchedule.txt");
	if (!CLPublic::FileExit(wszPath))
	{
		CConsoleVariable::GetInstance().PrintErrLog(GetText_By_Code(0x25).c_str());
		return FALSE;
	}

	if (!ReadASCIIFile_By_Path(wszPath, wsText))
		return FALSE;

	

	std::vector<std::wstring> TextVec;
	CCharacter::Split(wsText, L"\r\n", TextVec, Split_Option_RemoveEmptyEntries | Split_Option_KeepOnly);
	for (auto& itm : TextVec)
	{
		// Get Account information
		std::vector<std::wstring> AccountInfoVec;
		if (CCharacter::Split(itm, L",", AccountInfoVec, Split_Option_RemoveEmptyEntries | Split_Option_KeepOnly) == 3)
		{
			AccountSchedule_.AccountContent.wsAccountName = AccountInfoVec.at(0);
			AccountSchedule_.bFinish = _wtoi(AccountInfoVec.at(1).c_str()) == 0 ? FALSE : TRUE;
			AccountSchedule_.uVolume = static_cast<UINT>(_wtoi(AccountInfoVec.at(2).c_str()));
			continue;
		}

		// Start by <!--
		if (CCharacter::Trim_W(itm) == L"<!--")
		{
			bStartGroup = TRUE;
			continue;
		}

		// loop
		if (CCharacter::Split(itm, L",", AccountInfoVec, Split_Option_RemoveEmptyEntries | Split_Option_KeepOnly) == 4)
		{
			AccountPlayerConfig PlayerConfig_;
			PlayerConfig_.uIndex = static_cast<UINT>(_wtoi(AccountInfoVec.at(0).c_str()));
			PlayerConfig_.wsPlayerName = CCharacter::Trim_W(AccountInfoVec.at(1));

			em_Player_Classes emPlayerClass;
			if (!CConsoleVariable::GetInstance().GetPlayerClassByText(CCharacter::Trim_W(AccountInfoVec.at(2)), emPlayerClass))
			{
				CConsoleVariable::GetInstance().PrintErrLog(GetText_By_Code(0x8).c_str(), itm.c_str());
				return FALSE;
			}

			PlayerConfig_.emPlayerClass = emPlayerClass;
			PlayerConfig_.uLevel = static_cast<UINT>(_wtoi(AccountInfoVec.at(3).c_str()));

			AccountSchedule_.AccountPlayerConfigVec.push_back(PlayerConfig_);
			continue;
		}

		// End by -->
		if (CCharacter::Trim_W(itm) == L"-->")
		{
			bStartGroup = FALSE;
			vlst.push_back(AccountSchedule_);
			continue;
		}
	}
	return TRUE;
}

BOOL CTextConfig::ReadASCIIFile_By_Path(_In_ CONST wstring& cwsPath, _Out_ std::wstring& wsContent) CONST throw()
{
	ULONG uFileLen = 0;
	if (!CLFile::ReadAsciiFileLen(cwsPath, uFileLen))
	{
		CConsoleVariable::GetInstance().PrintErrLog(GetText_By_Code(0x9).c_str());
		return FALSE;
	}

	std::shared_ptr<CHAR> pszFileContent(new CHAR[uFileLen + 1], [](CHAR* p) {delete[] p; });
	if (pszFileContent == nullptr)
	{
		CConsoleVariable::GetInstance().PrintErrLog(L"pszFileContent = nullptr!");
		return FALSE;
	}

	if (!CLFile::ReadAsciiFileContent(cwsPath, uFileLen, pszFileContent))
	{
		CConsoleVariable::GetInstance().PrintErrLog(GetText_By_Code(0xA).c_str());
		return FALSE;
	}

	wsContent = CCharacter::ASCIIToUnicode(std::string(pszFileContent.get()));
	return TRUE;
}

BOOL CTextConfig::ClearAccountScheduleFile() CONST throw()
{
	WCHAR wszPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryW(MAX_PATH, wszPath);
	lstrcatW(wszPath, L"\\AccountSchedule.txt");

	return CLPublic::FileExit(wszPath) ? CLFile::ClearFileContent(wszPath) : FALSE;
}
