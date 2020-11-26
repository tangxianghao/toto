#pragma once

#ifndef IDC_EDIT_LOGIN_INTERVAL
#define IDC_EDIT_LOGIN_INTERVAL         1003
#define IDC_EDIT_LOGIN_TIMEOUT          1004
#define IDC_EDIT_START_KEY              1005
#define IDC_STATIC_ACCOUT_NUM           1008
#define IDC_STATIC_SUC                  1009
#define IDC_STATIC_FAILED               1010
#endif

#define STATUS_NO_START    0x00        
#define STATUS_LOGIN_FAILD 0x01
#define STATUS_SUCCESS     0x02
#define STATUS_TIME_OUT    0x03
#define STATUS_ING         0x04

typedef struct struct_account {
	char    Mail[128];     // 邮箱
	char    Password[128]; // 密码
	__int32 Status;        // 状态
	__int64 UpTextTime;    // 更新在控件上时间
	__int64 CptTime;       // 队列最快完成时间
	__int32 index;         // 在列表中索引
	bool    Used;          // 是否已使用
} Account, *PAccount;

// 游戏账号
class GameAccount
{
public:
	// 读取配置文件里面的帐号
	int ReadConfAccount();
	// 获取可游戏帐号
	Account* GetPlayAccount();
	// 想好是否可用
	bool AccountCanUse(Account* p, __int64 now_time=0);
	// 下一个
	int      Next();
	// 设置未使用
	void UnUsed(Account* p);
	// 更新控件文字
	void UpdateText(Account* account, int col, char* text);
	// 更新状态栏文字
	void UpdateStatusText(Account* account, char* text);
	// 更新队列文字
	void UpdateDuiLieText(Account* account, char* text);
	// 重绘控件
	void ListReDraw();
	// 写入日记控件
	void InsertLog(wchar_t* text);
	// 写入文本控件内容
	void SetText(int id, int num, int num2 = -1);
public:
	// 锁定
	bool m_bLock = false;
	// 当前帐号索引
	int m_iCurrent = 0;
	// 帐号总数目
	int m_iCount = 0;
	// 帐号列表
	Account* m_pAccounts;
protected:
private:
};