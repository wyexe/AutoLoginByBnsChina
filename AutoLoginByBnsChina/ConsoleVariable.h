#ifndef __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_VARIABLE_CONSOLEVARIABLE_H__
#define __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_VARIABLE_CONSOLEVARIABLE_H__

#include "../BnsDLL/GameStruct.h"

class CConsoleVariable : public CClassInstance<CConsoleVariable>
{
private:
	struct PlayerClassText
	{
		em_Player_Classes	emClasses;
		std::wstring		wsClassText;
	};
public:
	CConsoleVariable();
	~CConsoleVariable() = default;

	// Get Ref Value By VarId
	DWORD& GetVariableReferenceByID(_In_ em_Console_Variable dwVarID) CONST throw();

	// get Value by VarId
	DWORD GetVariableByID(_In_ em_Console_Variable VarID) CONST throw();

	// set Value by VarId
	DWORD SetVariableAndGetOldValueByID(_In_ em_Console_Variable VarID, _In_ DWORD NewValue) CONST throw();

	// SharePtr
	PSHARED_INFO& GetShareInfo() CONST throw();

	// Action when Value = dwValue by VarId
	BOOL DoEqualVariableByID(_In_ em_Console_Variable VarID, _In_ DWORD dwValue, _In_ std::function<VOID(VOID)> Worker) CONST throw();

	// Get PlayerClassesText By PlayerClasses
	BOOL GetPlayerClassesText(_In_ em_Player_Classes emPlayerClasses, _Out_ std::wstring& wsText) CONST throw();

	BOOL GetPlayerClassByText(_In_ CONST std::wstring& wsText, _Out_ em_Player_Classes& emPlayerClass) CONST throw();

	// Log........
	VOID SetLogerByConfig() throw();

	VOID PrintErrLog(_In_ LPCWSTR pwszFormat,...) CONST throw();

	// Text
	CONST std::wstring ConvertToText(_In_ LPCWSTR pwszFormat, ...) CONST throw();

	// Language
	CONST std::wstring& GetLanguage() CONST throw();

	VOID PrintToConsole(_In_ LPCWSTR pwszFormat, ...) CONST throw();

	DWORD GetMaxLoginTime() CONST throw();
private:
	CONST vector<PlayerClassText>& GetPlayerClassTextVec() CONST throw();
private:
	std::function<VOID(LPCWSTR)> Loger;
};



#endif