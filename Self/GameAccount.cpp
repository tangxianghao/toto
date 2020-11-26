#include "GameAccount.h"
#include "../Common/Explode.h"
#include "../Common/mystring.h"
#include "../Common/OpenTextFile.h"
#include "../stdafx.h"
#include "../ErZhanFengYunDlg.h"
#include <string.h>

// 读取配置文件里面的帐号
int GameAccount::ReadConfAccount()
{
	printf("GetConfAccount!!!\n");
	if (m_iCount > 0) {
		printf("m_iAccoutCurrentNum:%d!!!\n", m_iCount);
		return m_iCount;
	}

	g_dlg->InsertLog(L">>>>>>读取配置文件(帐号.ini)<<<<<<");
	CString log;
	OpenTextFile file;
	if (file.Open("帐号.ini")) {
		int line = file.GetCount();
		if (line > 0) {
			m_pAccounts = new Account[line];
		}
		memset(m_pAccounts, 0, sizeof(Account)*line);
		printf("文件行数:%d\n", line);

		int row = 0, length;
		char data[256];
		while ((length = file.GetLine(data, 256)) > -1) {
			row++;
			if (length == 0) {
				continue;
			}
			if (m_iCount >= line) {
				break;
			}
			
			Explode explode("::", data);
			if (explode.GetCount() != 2) {
				//log.Format(L"第(%d)行配置不正确，忽略。", row + 1);
				//InsertLogFunc(log.GetBuffer());
				continue;
			}

			/* 保存帐号信息 */
			strcpy(m_pAccounts[m_iCount].Mail, explode[0]);
			strcpy(m_pAccounts[m_iCount].Password, explode[1]);
			m_pAccounts[m_iCount].Status = STATUS_NO_START;
			m_pAccounts[m_iCount].Used = false;
			m_pAccounts[m_iCount].index = m_iCount;
			/* 显示到List控件 */
			g_dlg->InsertAccout(m_iCount, 1, explode[0]);
			g_dlg->InsertAccout(m_iCount, 2, "--");
			g_dlg->InsertAccout(m_iCount, 3, "--");
			g_dlg->InsertAccout(m_iCount, 4, "--");
			g_dlg->InsertAccout(m_iCount, 5, "未开始");
			g_dlg->InsertAccout(m_iCount, 4, "--");

			m_iCount++;
			g_dlg->ListReDraw();
			//printf("data:%s\n", trim(data));
		}
		file.Close();
		log.Format(L">>>>>>读取完成，共有帐号(%d)个<<<<<<", m_iCount);
		//SetTextFunc(IDC_STATIC_ACCOUT_NUM, 0, m_iAccountNum);
	}
	else {
		log = L">>>>>>找不到配置文件(帐号.ini)<<<<<<";
		printf("找不到文件(帐号.ini)！！！\n");
	}
	g_dlg->InsertLog(log.GetBuffer());

	return m_iCount;
}

// 获取可游戏帐号
Account* GameAccount::GetPlayAccount()
{
	while (m_bLock);
	m_bLock = true;
	time_t now_time = time(nullptr);
	Account* p = nullptr;
	int i;
	for (i = m_iCurrent; i < m_iCount; i++) {
		//printf("%d %d index:%d\n", m_iCount, i, m_pAccounts[i].index);
		if (AccountCanUse(&m_pAccounts[i], now_time)) {
			p = &m_pAccounts[i];
			Next();
			goto end;
		}
	}
	for (i = m_iCurrent - 1; i >= 0; i--) {
		if (AccountCanUse(&m_pAccounts[i], now_time)) {
			p = &m_pAccounts[i];
			Next();
			goto end;
		}
	}
end:
	if (p) { // 标记成正在使用
		p->Used = true;
		p->CptTime = 0;
		//printf("PPPPPPPPPPPPPPP333 index:%d！！！\n", p->index);
	}
	//printf("p:%016x\n", p);
	m_bLock = false;
	return p;
}

// 想好是否可用
bool GameAccount::AccountCanUse(Account * p, __int64 now_time)
{
	if (p->Used)
		return false;

	if (p->CptTime > 0) {
		if (now_time == 0)
			now_time = time(nullptr);

		time_t timeleft = p->CptTime - now_time;
		if (timeleft > 300) {
			if ((p->UpTextTime + 2) < now_time) {
				char str[32];
				sprintf_s(str, "%d秒後自动登录", timeleft - 300);
				UpdateStatusText(p, str);
				p->UpTextTime = now_time;
			}
			return false;
		}
	}

	return true;
}

// 下一个
int GameAccount::Next()
{
	if (++m_iCurrent >= m_iCount)
		m_iCurrent = 0;
	return m_iCurrent;
}

// 设置未使用
void GameAccount::UnUsed(Account* p)
{
	if (!p)
		return;

	while (m_bLock);
	m_bLock = true;
	p->Used = false;
	m_bLock = false;
}

// 更新控件文字
void GameAccount::UpdateText(Account * account, int col, char * text)
{
	if (account) {
		g_dlg->UpdateText_M(account->index, col, text);
	}
}

// 更新状态栏文字
void GameAccount::UpdateStatusText(Account* account, char * text)
{
	UpdateText(account, 5, text);
}

// 更新队列文字
void GameAccount::UpdateDuiLieText(Account * account, char * text)
{
	UpdateText(account, 6, text);
}

// 重绘控件
void GameAccount::ListReDraw()
{
	g_dlg->ListReDraw();
}

// 写入日记控件
void GameAccount::InsertLog(wchar_t* text)
{
	g_dlg->InsertLog(text);
}

// 写入文本控件内容
void GameAccount::SetText(int id, int num, int num2)
{
	g_dlg->SetText_M(id, num, num2);
}
