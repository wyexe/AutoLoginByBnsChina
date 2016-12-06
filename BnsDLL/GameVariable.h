#ifndef __AUTOLOGINBYBNACHINA_BNSDLL_GAMEVARIABLE_H__
#define __AUTOLOGINBYBNACHINA_BNSDLL_GAMEVARIABLE_H__

#include "GameStruct.h"

class CGameVariable : public CClassInstance<CGameVariable>, public CVariable<DWORD>
{
public:
	CGameVariable() = default;
	~CGameVariable() = default;

	PSHARED_INFO& GetShareInfo() CONST throw();

	PACCOUNT_INFO_GAME& GetAccShareInfo() CONST throw();
private:

};



#endif // !__AUTOLOGINBYBNACHINA_BNSDLL_GAMEVARIABLE_H__
