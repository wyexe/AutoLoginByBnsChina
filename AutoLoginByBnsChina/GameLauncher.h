#ifndef __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_GAMELAUNCHER_GAMELAUNCHER_H__
#define __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_GAMELAUNCHER_GAMELAUNCHER_H__

#include "ConsoleVariable.h"

class CGameLauncher : public CClassInstance<CGameLauncher>
{
public:
	CGameLauncher() = default;
	~CGameLauncher() = default;

	BOOL RunLauncher(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw();
private:

};



#endif // !__AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_GAMELAUNCHER_GAMELAUNCHER_H__
