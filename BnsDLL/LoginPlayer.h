#ifndef __AUTOLOGINBYBNSCHINA_BNSDLL_GAME_OBJECT_LOGINPLAYER_H__
#define __AUTOLOGINBYBNSCHINA_BNSDLL_GAME_OBJECT_LOGINPLAYER_H__

#include "GameVariable.h"

class CLoginPlayer
{
public:
	explicit CLoginPlayer();
	explicit CLoginPlayer(_In_ DWORD dwBase_, _In_ UINT uIndex_);
	~CLoginPlayer() = default;

public:
	// Array Base
	DWORD	GetBase() CONST throw();

	// PlayerName
	cwstring GetName() CONST throw();

	// Level
	DWORD	GetLevel() CONST throw();

	// ServerId
	DWORD	GetServerId() CONST throw();

	// Classes
	em_Player_Classes GetLoginPlayerClasses() CONST throw();

	// Index
	UINT	GetIndex() CONST throw();
private:
	static cwstring GetServerText_By_Id(_In_ DWORD dwServerId) throw();

	static cwstring GetClassesText_By_Classes(_In_ em_Player_Classes emPlayerClasses) throw();
private:
	DWORD	dwBase;
	UINT	uIndex;
};



#endif // !__AUTOLOGINBYBNSCHINA_BNSDLL_GAME_OBJECT_LOGINPLAYER_H__
