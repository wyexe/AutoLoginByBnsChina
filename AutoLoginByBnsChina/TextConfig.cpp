#include "TextConfig.h"
#include <MyTools/CLFile.h>
#include <MyTools/CLPublic.h>
#include <MyTools/Character.h>

#define _SELF L"TextConfig.cpp"

BOOL CTextConfig::ReadAccountConfig_By_File(_Out_ std::vector<AccountConfig>& vlst) CONST throw()
{
	std::wstring wsText;
	ULONG uFileLen = NULL;

	WCHAR wszPath[MAX_PATH] = { 0 };
	::GetCurrentDirectoryW(MAX_PATH, wszPath);
	lstrcatW(wszPath, L"\\Account.txt");
	if (!CLPublic::FileExit(wszPath))
	{
		LogMsgBox(LOG_LEVEL_EXCEPTION, L"where's the 'Account.txt'?");
		return FALSE;
	}

	if (!CLFile::ReadAsciiFileLen(wszPath, uFileLen))
	{
		LogMsgBox(LOG_LEVEL_EXCEPTION, L"Get File Length in 'Account.txt' Faild!!! are u using now?");
		return FALSE;
	}

	std::shared_ptr<CHAR> pszFileContent(new CHAR[uFileLen], [](CHAR* p) {delete[] p; });
	if (pszFileContent == nullptr)
	{
		LogMsgBox(LOG_LEVEL_EXCEPTION, L"pszFileContent = nullptr!");
		return FALSE;
	}

	if (!CLFile::ReadAsciiFileContent(wszPath, uFileLen, pszFileContent))
	{
		LogMsgBox(LOG_LEVEL_EXCEPTION, L"Read File Content in 'Account.txt' Faild!!! are u using now?");
		return FALSE;
	}

	wsText = CCharacter::ASCIIToUnicode(std::string(pszFileContent.get()));

	std::vector<std::wstring> TextAccountVec;
	CCharacter::Split(wsText, L"\r\n", TextAccountVec, Split_Option_RemoveEmptyEntries | Split_Option_KeepOnly);
	for (CONST auto& itm : TextAccountVec)
	{
		std::vector<std::wstring> SingleAccountVec;
		if (CCharacter::Split(itm, L",", SingleAccountVec, Split_Option_RemoveEmptyEntries | Split_Option_KeepOnly) != 2)
		{
			LogMsgBox(LOG_LEVEL_EXCEPTION, L"Text:%s, Format Error! should be like that: 123456,123456", itm.c_str());
			return FALSE;
		}

		vlst.push_back(AccountConfig{ SingleAccountVec.at(0),SingleAccountVec.at(1) });
	}

	if (vlst.size() == 0)
	{
		LogMsgBox(LOG_LEVEL_EXCEPTION, L"at least One line, that u should be write it! ");
		return FALSE;
	}

	return TRUE;
}

BOOL CTextConfig::ReadAccountSchedule_By_File(_In_ _Out_ std::vector<AccountPlayerConfig>& vlst) CONST throw()
{

}
