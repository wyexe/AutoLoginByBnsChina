#ifndef __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_CONFIG_TEXTCONFIG_H__
#define __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_CONFIG_TEXTCONFIG_H__

#include "ConsoleVariable.h"


class CTextConfig : public CClassInstance<CTextConfig>
{
public:
	CTextConfig() = default;
	~CTextConfig() = default;
	
	BOOL GetUnFinishedTextAccount(_Out_ std::vector<AccountPlayerConfig>& vlst) CONST throw();
private:
	// Read All of Account infomation in File
	BOOL ReadAccountConfig_By_File(_Out_ std::vector<AccountConfig>& vlst) CONST throw();

	// Read All of Account Schedule in File
	BOOL ReadAccountSchedule_By_File(_In_ _Out_ std::vector<AccountPlayerConfig>& vlst) CONST throw();
private:
};


#endif