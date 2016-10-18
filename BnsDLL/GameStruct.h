#ifndef __AUTOLOGINBYBNSCHINA_BNSDLL_GAMESTRUCT_H__
#define __AUTOLOGINBYBNSCHINA_BNSDLL_GAMESTRUCT_H__

#include <memory>
#include <algorithm>
#include <vector>
#include <MyTools/CLLog.h>
#include <MyTools/ClassInstance.h>


enum em_Player_Classes
{
	// 力士
	em_Player_Classes_Destroyer = 0x5,
	// 刺客
	em_Player_Classes_Assassin = 0x7,
	// 剑士
	em_Player_Classes_BladeMaster = 0x1,
	// 召唤师
	em_Player_Classes_Summoner = 0x6,
	// 气功
	em_Player_Classes_ForceMaster = 0x3,
	// 拳师
	em_Player_Classes_KongFuMaster = 0x2,
	// 灵剑士
	em_Player_Classes_Danncer = 0x8,
	// 咒术师
	em_Player_Classes_Warlock = 0x9,
	// 气宗
	em_Player_Classes_KongFuForceMaster = 0xA

};

struct AccountConfig
{
	std::wstring		wsAccountName;
	std::wstring		wsAccountPass;

	VOID Clear()
	{
		wsAccountName = wsAccountPass = L"";
	}
};

struct AccountPlayerConfig
{
	UINT				uIndex;
	UINT				uLevel;
	em_Player_Classes	emPlayerClass;
	std::wstring		wsPlayerName;

	VOID Clear()
	{
		uIndex = uLevel = NULL;
		wsPlayerName = L"";
	}
};

struct TextAccountSchedule
{
	AccountConfig				AccountContent;
	BOOL						bFinish;
	UINT						uVolume;
	vector<AccountPlayerConfig> AccountPlayerConfigVec;

	VOID Clear()
	{
		bFinish = FALSE;
		uVolume = NULL;
		AccountContent.Clear();
		AccountPlayerConfigVec.clear();
	}
};

enum em_Console_Variable
{

};

///////////////帐号///////////////////////////////////////////////////////////
typedef struct _Account_Info
{
	WCHAR szUserName[64];			//	帐号
	WCHAR szPassWord[32];			//	密码
	DWORD dwServerId;				//	服务器
	DWORD dwOrderIndex;				//	角色列表
	WCHAR szPlayerName[32];			//	玩家名称
	BOOL bAlive;					//	活动标识，定期检测一次此标识，如果没有设置为TRUE表示目标进程出问题了。
	BOOL bLogin;					//	登录完毕=FALSE
	WCHAR szCardNo[64];				//	卡号
	DWORD dwClasses;				// 职业
}ACCOUNT_INFO, *PACCOUNT_INFO;

typedef enum _em_Close_Result
{
	em_Close_Result_None,			// 正常状态
	em_Close_Result_ReStart,		// 重新上号
	em_Close_Result_LoseConnect,	// 掉线
	em_Close_Result_FixPoint,		// 5分钟
	em_Close_Result_CloseConsole,	// 关闭控制台
	em_Close_Result_UnExistTime,	// 点卡没了
}em_Close_Result;

typedef struct _AccountRunStatus
{
	BOOL bDone;												// 是否刷完了这个号
	BOOL bExist;											// 该帐号已经被占用了
	BOOL bLogining;											// 是否登录中
	BOOL bClose;											// 是否强制关闭该帐号
	em_Close_Result	emCloseResult;							// 关闭理由
}AccountRunStatus;

typedef struct _AccountLog
{
	BOOL bOccupy;											// 是否被占用
	WCHAR szMsg[64];										// 日志
}AccountLog;

typedef struct _Account_Info_GAME
{
	ACCOUNT_INFO MyAcount_Info;								// 帐号信息
	AccountLog   AccountLog_;								// 帐号日志
	DWORD dwPid;											// 进程ID
	HWND hGameWnd;											// 窗口句柄
	AccountRunStatus AccountStatus;							// 帐号状态

	LPCWSTR GetAccName()
	{
		return this->GetAccountInfo()->szUserName;
	}
	DWORD& GetOrderIndex()
	{
		return this->GetAccountInfo()->dwOrderIndex;
	}
	PACCOUNT_INFO GetAccountInfo()
	{
		return &this->MyAcount_Info;
	}
}ACCOUNT_INFO_GAME, *PACCOUNT_INFO_GAME;


#define MAX_GAME_COUNT				1000
typedef struct _Shared_Info
{
	WCHAR szConsolePath[MAX_PATH];							// 控制台路径
	WCHAR wszGamePath[MAX_PATH];							// 游戏路径
	int nAccountCount;										// Size
	ACCOUNT_INFO_GAME arrGameInfo[MAX_GAME_COUNT];			// 数组

	PACCOUNT_INFO_GAME ExistEmptyArrGameInfo()
	{
		auto itr = std::find_if(std::begin(arrGameInfo), std::end(arrGameInfo), [](ACCOUNT_INFO_GAME& AccountGame) {
			return !AccountGame.AccountStatus.bExist;
		});

		if (itr == std::end(arrGameInfo))
			return nullptr;
		return itr;
	}
	PACCOUNT_INFO_GAME GetCurrentAccountGame(_In_ DWORD dwPid)
	{
		auto itr = std::find_if(std::begin(arrGameInfo), std::end(arrGameInfo), [&dwPid](ACCOUNT_INFO_GAME& AccountGame) {
			return AccountGame.dwPid == dwPid;
		});

		if (itr == std::end(arrGameInfo))
			return nullptr;

		return itr;
	}
}SHARED_INFO, *PSHARED_INFO;

#endif