#ifndef __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_CONFIG_TEXTCONFIG_H__
#define __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_CONFIG_TEXTCONFIG_H__

#include "ConsoleVariable.h"


class CTextConfig : public CClassInstance<CTextConfig>
{
public:
	CTextConfig() = default;
	~CTextConfig() = default;
	
	// get all of account
	BOOL GetAccountSchedule(_Out_ std::vector<TextAccountSchedule>& vlst) CONST throw();

	// get config value
	BOOL GetConfigValue_By_KeyName(_In_ CONST std::wstring& wsKeyName, _Out_ std::wstring& wsConfigValue) CONST throw();

	// reset account schedule
	BOOL ResetAccountSchedule(_In_ std::function<BOOL(CONST TextAccountSchedule&)> Finder, _In_ std::function<VOID(TextAccountSchedule&)> Seter) CONST throw();

	// append account schedule
	BOOL AppendAccountScedule(CONST TextAccountSchedule& AccountSchedule_) CONST throw();

	// Get Text for Text.txt
	CONST std::wstring GetText_By_Code(_In_ DWORD dwCode) CONST throw();
private:
	// Read All of Account infomation in File
	BOOL ReadAccountConfig_By_File(_Out_ std::vector<AccountConfig>& vlst) CONST throw();

	// Read All of Account Schedule in File
	BOOL ReadAccountSchedule_By_File(_In_ _Out_ std::vector<TextAccountSchedule>& vlst) CONST throw();

	// Get File Content
	BOOL ReadASCIIFile_By_Path(_In_ CONST wstring& wsPath, _Out_ std::wstring& wsContent) CONST throw();

	// Clear Account Schedule File
	BOOL ClearAccountScheduleFile() CONST throw();
private:
};


#endif