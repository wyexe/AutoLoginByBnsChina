#ifndef __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_CONSOLECOMMAND_CONSOLECOMMAND_H__
#define __AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_CONSOLECOMMAND_CONSOLECOMMAND_H__

#include "ConsoleVariable.h"
#include <MyTools/CLGrammar.h>

class CConsoleCommond
{
public:
	CConsoleCommond() = default;
	~CConsoleCommond() = default;

	static DWORD WINAPI ResetAccount(vector<GrammarContext>& vlst, LPWSTR pwszRetMsg);

	static DWORD WINAPI Run(vector<GrammarContext>& vlst, LPWSTR pwszRetMsg);

	static DWORD WINAPI Stop(vector<GrammarContext>& vlst, LPWSTR pwszRetMsg);

	// total how much Finish Account Count and UnFinish
	static DWORD WINAPI Total(vector<GrammarContext>& vlst, LPWSTR pwszRetMsg);
private:

};



#endif // !__AUTOLOGINBYBNSCHINA_AUTOLOGINBYBNSCHINA_CONSOLECOMMAND_CONSOLECOMMAND_H__
