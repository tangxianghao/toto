#include "GameAccount.h"
#include "../Common/Explode.h"
#include "../Common/mystring.h"
#include "../Common/OpenTextFile.h"
#include "../stdafx.h"
#include "../ErZhanFengYunDlg.h"
#include <string.h>

// ��ȡ�����ļ�������ʺ�
int GameAccount::ReadConfAccount()
{
	printf("GetConfAccount!!!\n");
	if (m_iCount > 0) {
		printf("m_iAccoutCurrentNum:%d!!!\n", m_iCount);
		return m_iCount;
	}

	g_dlg->InsertLog(L">>>>>>��ȡ�����ļ�(�ʺ�.ini)<<<<<<");
	CString log;
	OpenTextFile file;
	if (file.Open("�ʺ�.ini")) {
		int line = file.GetCount();
		if (line > 0) {
			m_pAccounts = new Account[line];
		}
		memset(m_pAccounts, 0, sizeof(Account)*line);
		printf("�ļ�����:%d\n", line);

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
				//log.Format(L"��(%d)�����ò���ȷ�����ԡ�", row + 1);
				//InsertLogFunc(log.GetBuffer());
				continue;
			}

			/* �����ʺ���Ϣ */
			strcpy(m_pAccounts[m_iCount].Mail, explode[0]);
			strcpy(m_pAccounts[m_iCount].Password, explode[1]);
			m_pAccounts[m_iCount].Status = STATUS_NO_START;
			m_pAccounts[m_iCount].Used = false;
			m_pAccounts[m_iCount].index = m_iCount;
			/* ��ʾ��List�ؼ� */
			g_dlg->InsertAccout(m_iCount, 1, explode[0]);
			g_dlg->InsertAccout(m_iCount, 2, "--");
			g_dlg->InsertAccout(m_iCount, 3, "--");
			g_dlg->InsertAccout(m_iCount, 4, "--");
			g_dlg->InsertAccout(m_iCount, 5, "δ��ʼ");
			g_dlg->InsertAccout(m_iCount, 4, "--");

			m_iCount++;
			g_dlg->ListReDraw();
			//printf("data:%s\n", trim(data));
		}
		file.Close();
		log.Format(L">>>>>>��ȡ��ɣ������ʺ�(%d)��<<<<<<", m_iCount);
		//SetTextFunc(IDC_STATIC_ACCOUT_NUM, 0, m_iAccountNum);
	}
	else {
		log = L">>>>>>�Ҳ��������ļ�(�ʺ�.ini)<<<<<<";
		printf("�Ҳ����ļ�(�ʺ�.ini)������\n");
	}
	g_dlg->InsertLog(log.GetBuffer());

	return m_iCount;
}

// ��ȡ����Ϸ�ʺ�
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
	if (p) { // ��ǳ�����ʹ��
		p->Used = true;
		p->CptTime = 0;
		//printf("PPPPPPPPPPPPPPP333 index:%d������\n", p->index);
	}
	//printf("p:%016x\n", p);
	m_bLock = false;
	return p;
}

// ����Ƿ����
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
				sprintf_s(str, "%d�����Զ���¼", timeleft - 300);
				UpdateStatusText(p, str);
				p->UpTextTime = now_time;
			}
			return false;
		}
	}

	return true;
}

// ��һ��
int GameAccount::Next()
{
	if (++m_iCurrent >= m_iCount)
		m_iCurrent = 0;
	return m_iCurrent;
}

// ����δʹ��
void GameAccount::UnUsed(Account* p)
{
	if (!p)
		return;

	while (m_bLock);
	m_bLock = true;
	p->Used = false;
	m_bLock = false;
}

// ���¿ؼ�����
void GameAccount::UpdateText(Account * account, int col, char * text)
{
	if (account) {
		g_dlg->UpdateText_M(account->index, col, text);
	}
}

// ����״̬������
void GameAccount::UpdateStatusText(Account* account, char * text)
{
	UpdateText(account, 5, text);
}

// ���¶�������
void GameAccount::UpdateDuiLieText(Account * account, char * text)
{
	UpdateText(account, 6, text);
}

// �ػ�ؼ�
void GameAccount::ListReDraw()
{
	g_dlg->ListReDraw();
}

// д���ռǿؼ�
void GameAccount::InsertLog(wchar_t* text)
{
	g_dlg->InsertLog(text);
}

// д���ı��ؼ�����
void GameAccount::SetText(int id, int num, int num2)
{
	g_dlg->SetText_M(id, num, num2);
}
