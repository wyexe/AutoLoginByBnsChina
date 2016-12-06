#include "stdafx.h"
#include "BnsLogin.h"
#include "LoginPlayer.h"
#include "BnsFunction.h"
#include <MyTools/CLSearchBase.h>
#include <MyTools/CLPublic.h>

#define _SELF L"BnsLogin.cpp"

BOOL CBnsLogin::InitializeShareMemory() CONST throw()
{
	HANDLE hFileMap = ::OpenFileMappingW(FILE_MAP_READ | FILE_MAP_WRITE, FALSE, SZFILE_NAME_SHAREDINFO);
	if (hFileMap == NULL)
	{
		Log(LOG_LEVEL_NORMAL, L"hFileMap = NULL;");
		return FALSE;
	}

	auto& pShareInfo = CGameVariable::GetInstance().GetShareInfo();
	pShareInfo = reinterpret_cast<PSHARED_INFO>(::MapViewOfFile(hFileMap, FILE_MAP_WRITE | FILE_MAP_READ, NULL, NULL, sizeof(SHARED_INFO)));
	if (pShareInfo == nullptr)
	{
		Log(LOG_LEVEL_NORMAL, L"pShareInfo = nullptr;");
		::CloseHandle(hFileMap);
		hFileMap = NULL;
		return FALSE;
	}

	auto& pAccShareInfo = CGameVariable::GetInstance().GetAccShareInfo();
	pAccShareInfo = pShareInfo->GetCurrentAccountGame(::GetCurrentProcessId());
	if (pAccShareInfo == nullptr)
	{
		Log(LOG_LEVEL_NORMAL, L"pAccShareInfo = nullptr;");
		::UnmapViewOfFile(pShareInfo);
		::CloseHandle(hFileMap);
		hFileMap = NULL;
		return FALSE;
	}

	return TRUE;
}

BOOL CBnsLogin::WaitToChoicePlayer() CONST throw()
{
	DWORD dwLoginBase = CLSearchBase::FindBase("8B??83C4043B??0F84????????F605", 0x8E58722 - 0x8E58764, 0x2, 0x0, L"bsengine_Shipping.dll", 0xFFFFFFFF);
	Log(LOG_LEVEL_NORMAL, L"dwLoginBase=%X", dwLoginBase);
	if (dwLoginBase == NULL)
	{
		LogMsgBox(LOG_LEVEL_EXCEPTION, L"auto Search Membase Faild!");
		return FALSE;
	}

	Log(LOG_LEVEL_NORMAL, L"Waiting to Choice Player……");
	while (ReadDWORD(dwLoginBase) != 0x4)
		::Sleep(1000);

	Log(LOG_LEVEL_NORMAL, L"Done");
	return TRUE;
}

BOOL CBnsLogin::WaitToMainGameForm() CONST throw()
{
	// Record Player Info
	Log(LOG_LEVEL_NORMAL, L"RecordLoginPlayers");
	RecordLoginPlayers();

	Log(LOG_LEVEL_NORMAL, L"Wait to GameLoading");
	while (!IsGameLoading())
	{
		CLPublic::SimulationKey(VK_RETURN, CGameVariable::GetInstance().GetAccShareInfo()->hGameWnd);
		::Sleep(3000);
	}

	while (IsGameLoading())
	{
		::Sleep(3000);
	}
	Log(LOG_LEVEL_NORMAL, L"Done!");
	return TRUE;
}

BOOL CBnsLogin::IsGameLoading() CONST throw()
{
	static DWORD dwBase = NULL;
	static DWORD dwOffset = NULL;
	if (dwBase == NULL)
	{
		dwBase = CLSearchBase::FindBase_ByCALL("8B44240450E8????????8BC8E8????????B001", 0x092B6EF0 - 0x092B6EF5, (DWORD)::GetModuleHandleW(L"bsengine_Shipping"), 1, 0, L"bsengine_Shipping", 0x21 + 0x1);
		if (dwBase == NULL)
		{
			LogMsgBox(LOG_LEVEL_EXCEPTION, L"读图的自动更新地址失效拉!");
			return 0;
		}

		dwOffset = CLSearchBase::FindBase_ByCALL("8B44240450E8????????8BC8E8????????B001", 0x092B6EF0 - 0x092B6EFC, (DWORD)::GetModuleHandleW(L"bsengine_Shipping"), 1, 0, L"bsengine_Shipping", 0xF + 0x2, 0xFF);
		if (dwOffset == NULL)
		{
			LogMsgBox(LOG_LEVEL_EXCEPTION, L"读图的自动更新地址偏移失效拉!");
			return 0;
		}
		//Log(LOG_LEVEL_NORMAL, L"GameLoad:[[%X]+%X]", dwBase, dwOffset);
	}

	if (ReadDWORD(ReadDWORD(ReadDWORD(ReadDWORD(CGameVariable::GetInstance().GetRefValue_By_Id(em_Base::em_Base_Person)) + CGameVariable::GetInstance().GetRefValue_By_Id(em_Base::em_Base_PersonOffset3)) + 0xC) + 0x4) == 0x5)
		return TRUE;

	DWORD dwValue1 = ReadDWORD(ReadDWORD(dwBase) + 0x28);
	DWORD dwValue2 = ReadDWORD(ReadDWORD(dwBase) + dwOffset);
	if (ReadDWORD(ReadDWORD(dwBase) + dwOffset) == 0x1)
		return dwValue1 == 0x13 ? FALSE : TRUE;
	return FALSE;
}

BOOL CBnsLogin::SetVolumn() CONST throw()
{
	auto& pAccShare = CGameVariable::GetInstance().GetAccShareInfo();
	pAccShare->MyAcount_Info.uVolumn = CBnsFunction::GetInstance().GetVolume();
	return Finish();
}

BOOL CBnsLogin::Finish() CONST throw()
{
	auto& pAccShare = CGameVariable::GetInstance().GetAccShareInfo();
	pAccShare->AccountStatus.bDone = TRUE;
	return TRUE;
}

BOOL CBnsLogin::RecordLoginPlayers() CONST throw()
{
	std::vector<CLoginPlayer> vlst;
	CBnsFunction::GetInstance().GetLoginPlayerList(vlst);

	auto& pAccShare = CGameVariable::GetInstance().GetAccShareInfo();
	for (UINT i = 0;i < vlst.size(); ++i)
	{
		auto& itm = vlst.at(i);
		pAccShare->AccountPlayerArr[i].emPlayerClasses = itm.GetLoginPlayerClasses();
		pAccShare->AccountPlayerArr[i].uIndex = itm.GetIndex();
		pAccShare->AccountPlayerArr[i].uLevel = itm.GetLevel();
		CCharacter::wstrcpy_my(pAccShare->AccountPlayerArr[i].wszPlayerName, itm.GetName().c_str());
	}
	pAccShare->uAccountPlayerCount = vlst.size();
	return TRUE;
}
