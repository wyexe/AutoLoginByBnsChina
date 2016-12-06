#ifndef __AUTOLOGINBYBNSCHINA_BNSDLL_GAME_BNSFUNCTION_H__
#define __AUTOLOGINBYBNSCHINA_BNSDLL_GAME_BNSFUNCTION_H__

#include "GameVariable.h"

class CLoginPlayer;
class CBnsFunction : public CClassInstance<CBnsFunction>
{
public:
	CBnsFunction() = default;
	~CBnsFunction() = default;

	// Get All of Login Player
	UINT GetLoginPlayerList(_Out_ std::vector<CLoginPlayer>& vlst) CONST throw();

	// Get Count of Login Player
	UINT GetLoginPlayerCount() CONST throw();

	// Get Volumn Balance
	UINT GetVolume() CONST throw();
private:

};



#endif // !__AUTOLOGINBYBNSCHINA_BNSDLL_GAME_BNSFUNCTION_H__
