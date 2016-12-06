#ifndef __AUTOLOGINBYBNSCHINA_BNSDLL_GAME_BNSLOGIN_H__
#define __AUTOLOGINBYBNSCHINA_BNSDLL_GAME_BNSLOGIN_H__

#include "GameVariable.h"

class CBnsLogin : public CClassInstance<CBnsLogin>
{
public:
	CBnsLogin() = default;
	~CBnsLogin() = default;

	// Init Share Memory
	BOOL InitializeShareMemory() CONST throw();

	// Wait To Choice Player
	BOOL WaitToChoicePlayer() CONST throw();

	// wait to Into Game
	BOOL WaitToMainGameForm() CONST throw();

	// Is Game Loading
	BOOL IsGameLoading() CONST throw();

	// Set Volumn for Share Memory
	BOOL SetVolumn() CONST throw();

	// Set Login Done!
	BOOL Finish() CONST throw();
private:
	// Set Player info to Share Memory
	BOOL RecordLoginPlayers() CONST throw();
};



#endif // !__AUTOLOGINBYBNSCHINA_BNSDLL_GAME_BNSLOGIN_H__
