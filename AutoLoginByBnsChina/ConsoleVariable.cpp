#include "ConsoleVariable.h"
#include <map>
#include <MyTools/CLLock.h>

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

BOOL CConsoleVariable::DoEqualVariableByID(_In_ em_Console_Variable VarID, _In_ DWORD Value, _In_ std::function<VOID(VOID)> Worker) CONST throw()
{
	if (GetVariableByID(VarID) == Value)
	{
		Worker();
		return TRUE;
	}
	return FALSE;
}
