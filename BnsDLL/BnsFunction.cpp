#include "stdafx.h"
#include "BnsFunction.h"
#include <MyTools/CLSearchBase.h>
#include <MyTools/CLHook.h>
#include <MyTools/CLPublic.h>
#include "LoginPlayer.h"

#define _SELF L"BnsFunction.cpp"
UINT CBnsFunction::GetLoginPlayerList(_Out_ std::vector<CLoginPlayer>& vlst) CONST throw()
{
	DWORD dwPersonBase = CGameVariable::GetInstance().GetRefValue_By_Id(em_Base::em_Base_Person);
	DWORD dwPersonOffset = CGameVariable::GetInstance().GetRefValue_By_Id(em_Base::em_Base_PersonOffset1);
	DWORD dwPlayerTraverseOffset = CGameVariable::GetInstance().GetRefValue_By_Id(em_Base::em_Base_PlayerTraverseOffset);

	DWORD dwAddr = ReadDWORD(ReadDWORD(dwPersonBase) + dwPersonOffset + 0x8) + dwPlayerTraverseOffset;
	UINT  uPlayerCount = GetLoginPlayerCount();

	for (UINT i = 0; i < uPlayerCount; ++i)
		vlst.push_back(CLoginPlayer(ReadDWORD(dwAddr) + i * 0x30, i));
	return vlst.size();
}

UINT CBnsFunction::GetLoginPlayerCount() CONST throw()
{
	DWORD dwPersonBase = CGameVariable::GetInstance().GetRefValue_By_Id(em_Base::em_Base_Person);
	DWORD dwPersonOffset = CGameVariable::GetInstance().GetRefValue_By_Id(em_Base::em_Base_PersonOffset1);
	DWORD dwPlayerTraverseOffset = CGameVariable::GetInstance().GetRefValue_By_Id(em_Base::em_Base_PlayerTraverseOffset);

	DWORD dwAddr = ReadDWORD(ReadDWORD(dwPersonBase) + dwPersonOffset + 0x8) + dwPlayerTraverseOffset;
	return static_cast<UINT>(ReadDWORD(dwAddr + 4) - ReadDWORD(dwAddr)) / 0x30;
}

static DWORD dwHookVolumeAddr = 0;
__declspec(naked) VOID WINAPI HookVolume()
{
	static CHAR* pszText = nullptr;
	_asm
	{
		MOV ECX, ESI;
		CALL EAX;
		MOV pszText, EAX;
		PUSHAD;
	}
	if (pszText != NULL)
	{
		if (CGameVariable::GetInstance().GetRefValue_By_Id(em_Base::em_Base_Volume) == NULL)
			CGameVariable::GetInstance().GetRefValue_By_Id(em_Base::em_Base_Volume) = static_cast<DWORD>(atoi(pszText));
	}
	__asm POPAD;
	__asm TEST EAX, EAX;
	__asm PUSH dwHookVolumeAddr;
	__asm RETN;
}

UINT CBnsFunction::GetVolume() CONST throw()
{
	// 8bceffd085c074186a006a0050e8????????8b4f54
	dwHookVolumeAddr = CLSearchBase::FindAddr("8bceffd085c074186a006a0050e8????????8b4f54", 0x0, 0, L"StsMsgCli32.dll");
	if (dwHookVolumeAddr == NULL)
	{
		LogMsgBox(LOG_LEVEL_EXCEPTION, L"GetVolume MachineCode Faild!");
		return 0;
	}
	Log(LOG_LEVEL_NORMAL, L"dwHookVolumeAddr=%X", dwHookVolumeAddr);

	MYHOOK_CONTENT MC;
	MC.dwHookAddr = dwHookVolumeAddr;
	MC.dwFunAddr = (DWORD)HookVolume;
	MC.uNopCount = 0x1;

	CLHook::Hook_Fun_Jmp_MyAddr(&MC);

	dwHookVolumeAddr += 0x6;
	CLPublic::TimeOut_By_Condition(30 * 1000, [] 
	{
		//CLPublic::SimulationKey(VK_F10, CGameVariable::GetInstance().GetAccShareInfo()->hGameWnd);
		::SetWindowPos(CGameVariable::GetInstance().GetAccShareInfo()->hGameWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		::SwitchToThisWindow(CGameVariable::GetInstance().GetAccShareInfo()->hGameWnd, TRUE);
		CLPublic::SendKey(VK_F10);
		::Sleep(3 * 1000);
		return CGameVariable::GetInstance().GetRefValue_By_Id(em_Base::em_Base_Volume) != NULL; 
	});
	Log(LOG_LEVEL_NORMAL, L"Value=%d", CGameVariable::GetInstance().GetRefValue_By_Id(em_Base::em_Base_Volume));
	CLHook::UnHook_Fun_Jmp_MyAddr(&MC);
	return static_cast<DWORD>(CGameVariable::GetInstance().GetRefValue_By_Id(em_Base::em_Base_Volume));
}
