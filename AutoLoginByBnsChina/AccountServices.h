#ifndef __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_ACCOUNT_ACCOUNTSERVICES_H__
#define __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_ACCOUNT_ACCOUNTSERVICES_H__

#include "ConsoleVariable.h"

class CAccountServices : public CClassInstance<CAccountServices>
{
public:
	CAccountServices() = default;
	~CAccountServices() = default;

	LPCWSTR GetLoginingAccountName() CONST throw();

	VOID PrintTotal() CONST throw();

	BOOL InitializeAccountShare() CONST throw();

	BOOL RunGame(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw();

	BOOL KillGame(_In_ ACCOUNT_INFO_GAME* pAccGame) CONST throw();
private:

};



#endif // !__AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_ACCOUNT_ACCOUNTSERVICES_H__
