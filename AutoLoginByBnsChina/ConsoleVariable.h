#ifndef __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_VARIABLE_CONSOLEVARIABLE_H__
#define __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_VARIABLE_CONSOLEVARIABLE_H__

#include "../BnsDLL/GameStruct.h"

class CConsoleVariable : public CClassInstance<CConsoleVariable>
{
public:
	CConsoleVariable() = default;
	~CConsoleVariable() = default;

	// ref 
	DWORD& GetVariableReferenceByID(_In_ em_Console_Variable dwVarID) CONST throw();

	// get
	DWORD GetVariableByID(_In_ em_Console_Variable VarID) CONST throw();

	// set
	DWORD SetVariableAndGetOldValueByID(_In_ em_Console_Variable VarID, _In_ DWORD NewValue) CONST throw();

	// SharePtr
	PSHARED_INFO& GetShareInfo() CONST throw();

	// 
	BOOL DoEqualVariableByID(_In_ em_Console_Variable VarID, _In_ DWORD Value, _In_ std::function<VOID(VOID)> Worker) CONST throw();
private:

};



#endif