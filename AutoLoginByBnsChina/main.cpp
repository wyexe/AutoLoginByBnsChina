#include <iostream>
#include <string>
#include "ConsoleCommond.h"

VOID WeclomeStep()
{
	wcout << L"---------------------------------------" << endl;
	wcout << L"Author: ÎÞÓÇÎÞÇó" << endl;
}

int main()
{
	// Set Log Type
	CConsoleVariable::GetInstance().SetLogerByConfig();

	// important!
	WeclomeStep();

	CLGrammar Grammar;
	Grammar.AddTranList(L"ResetAccount", CConsoleCommond::ResetAccount);

	std::wstring wsCommond;
	while (true)
	{
		std::getline(std::wcin, wsCommond);
		if (wsCommond == L"exit")
			break;

		
	}

	return 0;
}