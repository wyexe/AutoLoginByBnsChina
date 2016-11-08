// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include "BnsLogin.h"

#define _SELF L"dllmain.cpp"
unsigned WINAPI _WorkThread(LPVOID lpParm)
{
	if (!CBnsLogin::GetInstance().InitializeShareMemory())
	{
		Log(LOG_LEVEL_NORMAL, L"InitializeShareMemory Faild!");
		ExitProcess(0);
		return 0;
	}
	Log(LOG_LEVEL_NORMAL, L"Seting Value");
	CGameVariable::GetInstance().SetValueAndGetOldValue_By_Id(em_Base::em_Base_Person, 0x12A79DC);
	CGameVariable::GetInstance().SetValueAndGetOldValue_By_Id(em_Base::em_Base_PersonOffset1, 0x50);
	CGameVariable::GetInstance().SetValueAndGetOldValue_By_Id(em_Base::em_Base_PersonOffset2, 0x80);
	CGameVariable::GetInstance().SetValueAndGetOldValue_By_Id(em_Base::em_Base_PersonOffset3, 0x48);
	CGameVariable::GetInstance().SetValueAndGetOldValue_By_Id(em_Base::em_Base_PlayerTraverseOffset, 0x19C);
	Log(LOG_LEVEL_NORMAL, L"Set Value Done!");

	CBnsLogin::GetInstance().WaitToChoicePlayer();

	CBnsLogin::GetInstance().WaitToMainGameForm();

	CBnsLogin::GetInstance().SetVolumn();

	return 0;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	static HANDLE hThread = NULL;
	if (hThread == NULL)
	{
		Log(LOG_LEVEL_NORMAL, L"Create Thread!");
		hThread = cbBEGINTHREADEX(NULL, NULL, _WorkThread, NULL, NULL, NULL);
	}
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}

