#include "stdafx.h"
#include "GameVariable.h"

PSHARED_INFO& CGameVariable::GetShareInfo() CONST throw()
{
	return CClassInstance<>::GetStaticVariable<PSHARED_INFO>();
}

PACCOUNT_INFO_GAME& CGameVariable::GetAccShareInfo() CONST throw()
{
	return CClassInstance<>::GetStaticVariable<PACCOUNT_INFO_GAME>();
}
