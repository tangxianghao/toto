#include <iostream>
#include "GameProc.h"
#include "GameAccount.h"
#include "Game.h"
#include "../Common/mystring.h"
#include "../Common/OpenTextFile.h"
#include "../Common/Explode.h";
#include "../Common/Preg.h"

#define IS_GET_JS    0x01  // �Ѿ���ȡ�˾���������
#define IS_GET_ZY    0x02  // �Ѿ���ȡ������������
#define IS_GET_KJ    0x04  // �Ѿ���ȡ�����о��Ƽ�
#define IS_GET_GOODS 0x08  // �Ѿ���ȡ����Ʒ
#define IS_GET_RES   0x10  // �Ƿ��ȡ����Դ����

#define GETALLVAL() (IS_GET_JS|IS_GET_ZY|IS_GET_KJ|IS_GET_GOODS|IS_GET_RES)
#define SETISGET(v) (m_iIsGetValue|=(v)); // �����Ѿ���ȡ��ʲô��Ϣ
#define ISGETALL() (m_iIsGetValue==(IS_GET_JS|IS_GET_ZY|IS_GET_KJ|IS_GET_GOODS|IS_GET_RES)) // �Ƿ��ѻ�ȡ������

// ����Gameָ��
GameProc::GameProc(Game * game)
{
	m_pGame = game;
	m_iDuiLieMax = 2;
	m_cDuiLie = new char[256];
}

GameProc::~GameProc()
{
	if (m_cDuiLie) {
		delete[] m_cDuiLie;
		m_cDuiLie = nullptr;
	}
	m_DuiLie.Release(1);
	m_Link.Release(1);
	printf("~GameProc������\n");
}

// ������Ӧ����
void GameProc::SetData(const char* data, int length, char flag, bool success)
{
	m_cData = data;
	m_iDataLength = length;
	m_chFlag = flag;
	m_bSuccess = success;
}

// ����һЩ����
void GameProc::SendTheData()
{
#if 0
	m_pGame->MakeTcpOP(OP_VIEWRES);
	m_pGame->MakeTcpOP(OP_JIANZHU_JS, 0, true);
	m_pGame->MakeTcpOP(OP_JIANZHU_ZY, 0, true);
	m_pGame->MakeTcpOP(OP_VIEWKEJI, 0, true);
	m_pGame->MakeTcpOP(OP_VIEWGOODS, 0, true);
	//m_pGame->MakeTcpOP(OP_VIEWCURTASK, 0, true);
	//m_pGame->MakeTcpOP(OP_ZHAOMULIST, 0, true);
	m_pGame->Send();
#else
	/*m_pGame->MakeTcpOP(OP_VIEWRES);
	m_pGame->Send();
	Sleep(250);
	m_pGame->MakeTcpOP(OP_JIANZHU_JS);
	m_pGame->Send();
	Sleep(250);
	m_pGame->MakeTcpOP(OP_JIANZHU_ZY);
	m_pGame->Send();
	Sleep(250);
	m_pGame->MakeTcpOP(OP_VIEWKEJI);
	m_pGame->Send();
	Sleep(250);
	m_pGame->MakeTcpOP(OP_VIEWGOODS);
	m_pGame->Send();
	Sleep(250);
	m_pGame->MakeTcpOP(RW_LIST, RW_LIST_2);
	m_pGame->Send();
	Sleep(250);
	m_pGame->MakeTcpOP(RW_LIST, RW_LIST_1);
	m_pGame->Send();*/
	//for (char i = 1; i <= 7; i++) {
	//	printf("-----------------ǩ�� һ:%d----------------\n", i);
	//	//m_pGame->UpdateAccountStatusText("ǩ��(��2��)");
	//	m_pGame->MakeTcpOP(OP_QIANDAO, i);
	//	m_pGame->Send();
	//	Sleep(2500);
	//}
	//m_pGame->MakeTcpOP(OP_VIEWCURTASK, 0, true);
	//m_pGame->MakeTcpOP(OP_ZHAOMULIST, 0, true);
#endif
	
}

// �����¼
void GameProc::Login()
{
	EmptyDuiLieText();
	if (m_chFlag == 0x01 || m_chFlag == 0x02) {
#if 0
		m_pGame->UpdateAccountStatusText("��֤�ɹ�.");
		//m_pGame->InsertLog(L"��ȡ����...");
		m_pGame->UpdateAccountStatusText("��ȡ����...");
		m_pGame->MakeTcpOP(OP_COOR);
		m_pGame->Send();


		m_pGame->UpdateAccountStatusText("��ȡ����������...");
		//m_pGame->InsertLogFunc(L"��ȡ����������...");
#endif
		m_pGame->SetLoginOkTime(); // ���õ�¼�ɹ�ʱ��
		if (m_pGame->m_stConfig.Close || m_pGame->m_stConfig.Open
			|| m_pGame->m_stConfig.JiangSheng) {
			m_pGame->UpdateAccountStatusText("��ȡ������...");
			m_pGame->MakeTcpOP(OP_JIANZHU_JS);
			m_pGame->Send();
			Sleep(250);
			m_pGame->MakeTcpOP(OP_SHENGWANG);
			m_pGame->Send();
			return;
		}
		else {
			printf("ִ������");
			return;
			//��ѯ�û���Ϣ
			char user_info_sql[128];
			sprintf(user_info_sql, "SELECT * FROM `cmf_account` WHERE `region`='%s' AND `user_login`='%s' LIMIT 1",
				m_pGame->m_ServerId, m_pGame->m_pAccount->Mail);
			int res = mysql_query(&m_pGame->m_mysqlConnect, user_info_sql);
			if (res == 0) {
				MYSQL_RES* result = mysql_store_result(&m_pGame->m_mysqlConnect);
				if (result) {
					int num_fields = mysql_num_fields(result);
					MYSQL_ROW row;
					row = mysql_fetch_row(result);
					//ǩ����24��ʾǩ��������38�賿ǩ��ʱ�䣩
					if (getMorningTime() > atoi(row[38]) && atoi(row[24]) < 8) {
						printf("����ǩ��");
						m_pGame->MakeTcpOP(OP_QIANDAO, atoi(row[24]));
						m_pGame->Send();
						//���³�������
						if (atoi(row[24]) == 1) {
							m_pGame->MakeTcpOP(OP_COOR);
							m_pGame->Send();
						}
					}
					m_pGame->MakeTcpOP(OP_VIEWRES);
					m_pGame->Send();
					m_pGame->MakeTcpOP(OP_CITY);
					m_pGame->Send();
					SendTheData();

					//m_pGame->MakeTcpOP(OP_VIEWQIANDAO);
					//m_pGame->Send();
					//Sleep(1000);
					//ExecLiuCheng();
				}
			}
		}
#if 0
		if (m_pGame->m_stConfig.Close) {
			m_pGame->InsertLogFunc(L"��ȡ����ֵ...");
			m_pGame->UpdateAccountStatusText("��ȡ����ֵ...");
			m_pGame->MakeTcpOP(OP_SHENGWANG);
			m_pGame->Send();
		}
#endif
	}
	else {
		m_pGame->m_pAccount->CptTime = time(nullptr) + 300;

		int open_hour = 0;
		for (int i = 9; i < m_iDataLength; i++) {
			if (m_cData[i] == ':') {
				if (m_cData[i - 2] >= '0' && m_cData[i - 2] <= '9') {
					open_hour = 10 + m_cData[i - 2] - '0';
				}
				if (m_cData[i - 1] >= '0' && m_cData[i - 1] <= '9') {
					open_hour += m_cData[i - 1] - '0';
				}
			}
		}
		if (open_hour > 0 && open_hour < 20) {
			printf("Ԥ��%d�㿪��������\n", open_hour);

			int hour;
			getday(&hour);
			hour = open_hour - hour;
			if (hour > 0) {
				m_pGame->m_pAccount->CptTime = time(nullptr) + (hour * 3600);
			}
		}
		m_pGame->SetAccountFailed();
	}
}

// ��ȡ����
void GameProc::Coor()
{
	int x, y;
	m_pGame->GetCoor(m_cData, x, y);

	char sql[256];
	sprintf(sql, "update `cmf_account` set"
		"`x_axis`='%d',`y_axis`='%d'"
		"where `region`='%s' and `user_login`='%s'",
		x,y,m_pGame->m_ServerId, m_pGame->m_pAccount->Mail);
	int res = mysql_query(&m_pGame->m_mysqlConnect, sql);
	if (res == 0) {
		printf("������³ɹ�%s.\n", m_pGame->m_pAccount->Mail);
	}
	else {
		printf("�������ʧ�ܣ�%s\n", sql);
	}
	/*char sStr[16];
	sprintf_s(sStr, "%d,%d\n", x, y);
	m_pGame->UpdateText(2, sStr);*/
}

// ��������
void GameProc::ShengWang()
{
	int shengwang = m_pGame->GetShengWang(m_cData);

	char sSW[16];
	sprintf_s(sSW, "%d", shengwang);
	m_pGame->UpdateText(3, sSW);
	printf("������%d\n", shengwang);

	//����
	if (m_pGame->m_stConfig.Close) {
		printf("-----------guanmen---------\n");
		if (shengwang > 9999) {
			/*m_pGame->UpdateAccountStatusText("����...");
			m_pGame->MakeTcpOP(OP_CISHAN, 0x01);
			m_pGame->Send();*/

			int cha = shengwang - 9999;
			int san = cha >> 1;
			if (cha & 0x01)
				san += 1;

			CString csSan;
			csSan.Format(L"�����ﵽ%d����ɢ����:%d", shengwang, san);
			m_pGame->UpdateAccountStatusText("��ɢ...");
			m_pGame->MakeTcpJieSan(san);
			//m_pGame->MakeTcpOP(OP_JIESAN, san);
			m_pGame->Send();
		}
		else {
			m_pGame->UpdateAccountStatusText("�����ɢ.");
			m_pGame->SetAccountSuceess();
		}
	}

	//����
	if (m_pGame->m_stConfig.Open) {
		printf("-----------kaimen---------\n");
		if (shengwang < 10000) {
			JianZhu* jz = m_pGame->GetJianZhuByType(JZ_JUNGONG);
			if (jz) { // ��������������
				int cha = 10000 - shengwang;
				int san = cha >> 1;
				if (cha & 0x01)
					san += 1;

				CString csSan;
				csSan.Format(L"�����ﵽ%d��ѵ����������:%d", shengwang, san);
				m_pGame->XunLianMore(jz->code, san);
				m_pGame->Send();
			}
		}
		else {
			m_pGame->UpdateAccountStatusText("����ѵ��.");
			m_pGame->SetAccountSuceess();
		}
	}

	//������
	if (m_pGame->m_stConfig.JiangSheng) {
		printf("-----------������---------\n");
		if (shengwang > 9999) {
			Sleep(100);
			JianZhu* weiqiang = m_pGame->GetJianZhuByType(JZ_WEIQIANG);
			if (weiqiang) { // Χǽ��������
				printf("weiqiang->level��%d\n", weiqiang->level);
				if (weiqiang->level < 1) {
					//����Χǽ
					printf("-----����Χǽ------\n");
					m_pGame->MakeTcpOPInt64(OP_SHENGJI, weiqiang->code);
					m_pGame->Send();
					m_pGame->back_type = 1;
				}

				if (weiqiang->level == 1) {
					//��Χǽ
					printf("-----��Χǽ------\n");
					m_pGame->MakeTcpOPInt64(OP_CHAI_JZ, weiqiang->code);
					m_pGame->Send();
					m_pGame->back_type = 1;
				}

				if (weiqiang->level > 1) {
					//��Χǽ
					printf("-----��Χǽ------\n");
					m_pGame->MakeTcpOPInt64(OP_CHAI_JZ, weiqiang->code);
					m_pGame->Send();
				}
			}
		}
	}
}

// ��Դ
void GameProc::ViewRes()
{
	if (!m_bSuccess)
		return;

	ZiYuan* p = &m_pGame->m_ZiYuan;
	p->liangshi = ctoi64(&m_cData[5]);
	p->gangtie = ctoi64(&m_cData[33]);
	p->shiyou = ctoi64(&m_cData[73]);
	p->xikuang = ctoi64(&m_cData[53]);

	int i = ctoi32(&m_cData[93]);
	int index = 97 + (i * 8);

	p->huangjin = ctoi32(&m_cData[index]) & 0xffffffff;
	p->renkou = ctoi32(&m_cData[index + 20]) & 0xffffffff;

	SETISGET(IS_GET_RES);
	
	char sql[256];
	sprintf(sql, "update `cmf_account` set"
		"`food`='%d',`steel`='%d',`petroleum`='%d',`tinore`='%d',`last_login_time`='%d'"
		"where `region`='%s' and `user_login`='%s'",
		p->liangshi, p->gangtie, p->shiyou, p->xikuang, (int)time(nullptr),
		m_pGame->m_ServerId,m_pGame->m_pAccount->Mail);
	int res = mysql_query(&m_pGame->m_mysqlConnect, sql);
	if (res == 0) {
		printf("ӵ����Դ���³ɹ�%s.\n", m_pGame->m_pAccount->Mail);
	}
	else {
		printf("ӵ����Դ����ʧ�ܣ�%s\n", sql);
	}

}

// ����-������
void GameProc::JianZhu_JS()
{
	int num = 0;
	for (int i = 6; i < m_iDataLength; i += 49) {
		if ((i + 0x16) > m_iDataLength)
			break;

		int no = ctoi32(&m_cData[i + 16]); // ����λ�ñ��
		if (no >= JZ_MAX) {
			printf("area: %08x\n", no);
			continue;
		}

		JianZhu* jz  = &m_pGame->m_jsJianZhu[no];
		SaveJianZhu(&m_cData[i], jz, no);
		//else {
		//	jz->free = m_bFirstCalc ? 1 : 0;
		//}

#if 1
		if (jz->type == JZ_SHIZHENG) {
			SetZYNum(jz->level); // �����������ɽ�������
			//m_pGame->MakeUpNeed(jz->code, (JZ_TYPE)jz->type, jz->no);
			//m_pGame->Send();
		}
#endif

		num++;
	}
	printf("������������%d\n", num);
	SETISGET(IS_GET_JS);

	//ExecLiuCheng();
	/*
	int id = m_pGame->GetXLYId(&m_cData[10], m_iDataLength - 10);
	if (id) {
		m_pGame->InsertLogFunc(L"�ɹ���ȡ������ID��");
	}
	else {
		m_pGame->UpdateAccountStatusText("��ȡ������idʧ��.");
		m_pGame->InsertLogFunc(L"�޷���ȡ������ID��");
	}
	m_pGame->UpdateAccountStatusText("ѵ��...");
	m_pGame->InsertLogFunc(L"ѵ��...");
	// 0x00000bb9 Ϊѵ��ָ��
	m_pGame->MakeTcpStr(OP_XUNLIAN, id);
	//m_pGame->MakeTcpJieSan(0x02);
	m_pGame->Send();
	*/
}

// ����-������
void GameProc::JianZhu_ZY()
{
	int num = 0;
	for (int i = 10; i < m_iDataLength; i += 49) {
		if ((i + 0x16) > m_iDataLength)
			break;

		int no = ctoi32(&m_cData[i + 16]); // ����λ�ñ��
		if (no >= JZ_MAX)
			continue;

		JianZhu* jz = &m_pGame->m_zyJianZhu[no];
		SaveJianZhu(&m_cData[i], jz, no);

		num++;
	}
	printf("������������%d\n", num);
	SETISGET(IS_GET_ZY);
}

// ��������
void GameProc::SaveJianZhu(const char* data, struct_jianzhu* jz, int no)
{
	jz->code = ctoi64(&data[0]);
	jz->type = ctoi32(&data[8]);
	jz->level = ctoi32(&data[12]);
	jz->no = no;
	jz->timeleft = ctoi64(&data[24]) / 1000;
	jz->mdtime = time(nullptr);
	jz->cpttime = jz->mdtime + jz->timeleft + 1;
	jz->status = 1;
	jz->exec = jz->timeleft > 0 ? 1 : 0;
	jz->free = 0;
	if (jz->timeleft > 0 && jz->timeleft < 5) {
		jz->free = jz->free ? 1 : 0;
	}
	if (jz->ignoreck) {
		if (jz->timeleft == 0) {
			jz->ignoreck = 0;
		}
	}

	printf("���룺%016x", jz->code);
	printf("���ͣ�%02x �ȼ���%02x λ�ã�%02x", jz->type & 0xff, jz->level & 0xff, jz->no & 0xff);
	
	if (m_bFirstCalc) {
		printf(" ��һ��");
	}
	if (jz->exec) {
		printf(" ����Ϊ����ִ��...");
	}
	if (jz->ignoreck) {
		printf(" ���Լ��");
	}
	printf(" ʣ��ʱ�䣺%lld", jz->timeleft);

	printf("\n");
}

// ������������ɽ�������
int GameProc::SetZYNum(int szt_level)
{
	switch (szt_level)
	{
	case 1:
		m_pGame->m_iZYNum = 4;
		break;
	case 2:
		m_pGame->m_iZYNum = 9;
		break;
	case 3:
		m_pGame->m_iZYNum = 12;
		break;
	case 4:
		m_pGame->m_iZYNum = 16;
		break;
	case 5:
		m_pGame->m_iZYNum = 20;
		break;
	case 6:
		m_pGame->m_iZYNum = 24;
		break;
	case 7:
		m_pGame->m_iZYNum = 28;
		break;
	case 8:
		m_pGame->m_iZYNum = 32;
		break;
	case 9:
		m_pGame->m_iZYNum = 36;
		break;
	default:
		m_pGame->m_iZYNum = 36;
		break;
	}
	return m_pGame->m_iZYNum;
}

// ��������
void GameProc::JianZhu_JianZao()
{
	if (!m_bSuccess)
		return;

	int no = ctoi32(&m_cData[21]);  // ��� 
	if (no >= JZ_MAX)
		return;

	int type = ctoi32(&m_cData[13]); // ����
	JianZhu* list = m_pGame->GetJianZhuList((JZ_TYPE)type);
	list[no].code = ctoi64(&m_cData[5]);
	list[no].type = type;
	list[no].no = no;
	list[no].level = ctoi32(&m_cData[17]);
	list[no].timeleft = ctoi64(&m_cData[m_iDataLength - 8]) / 1000;
	list[no].mdtime = time(nullptr);
	list[no].cpttime = list[no].mdtime + list[no].timeleft + 1;
	list[no].status = 1;

	printf("����->���룺%016x ", list[no].code);
	printf("���ͣ�%02x �ȼ���%02x λ�ã�%02x ʣ��ʱ�䣺%lld\n", list[no].type & 0xff,
		list[no].level & 0xff, list[no].no & 0xff, list[no].timeleft);

	if (list[no].timeleft < FREE_SECOND) { // �������
		m_pGame->MakeTcpOPInt64(OP_FREE, list[no].code);
		if (m_pGame->Send()) {
			//list[no].level++;
		}
	}
}

// ��������
void GameProc::JianZhu_ShengJi()
{
	if (!m_bSuccess) {
		printf("����ʧ�ܣ�����\n");
		return;
	}
	
	printf("�����У�����\n");
	//printf("�����б�ţ�%d\n", ctoi32(&m_cData[21]));
	int no = ctoi32(&m_cData[21]);  // ��� 
	if (no >= JZ_MAX)
		return;

	int type = ctoi32(&m_cData[13]); // ����
	JianZhu* list = m_pGame->GetJianZhuList((JZ_TYPE)type);
	list[no].code = ctoi64(&m_cData[5]);
	list[no].type = type;
	list[no].no = no;
	list[no].level = ctoi32(&m_cData[17]);
	list[no].timeleft = ctoi64(&m_cData[m_iDataLength - 8]) / 1000;
	list[no].mdtime = time(nullptr);
	list[no].cpttime = list[no].mdtime + list[no].timeleft + 1;
	list[no].ready = 0;
	list[no].exec = 1;
	list[no].status = 1;

	printf("list[no].level:%d\n", list[no].level);

	printf("����->���룺%016x ", list[no].code);
	printf("���ͣ�%02x �ȼ���%02x λ�ã�%02x ʣ��ʱ�䣺%lld\n", list[no].type & 0xff,
		list[no].level & 0xff, list[no].no & 0xff, list[no].timeleft);

	if (m_pGame->CheckCanFree(list[no].timeleft)) { // �������
		printf("����ʹ����Ѽ���\n");
		m_pGame->MakeTcpOPInt64(OP_FREE, list[no].code);
		m_pGame->Send();
		Sleep(1000);
	}

	if (m_pGame->back_type == BACK_WALL) {
		printf("m_pGame->back_type��%d\n", m_pGame->back_type);
		printf("BACK_WALL��%d\n", BACK_WALL);
		Sleep(500);
		m_pGame->MakeTcpOP(OP_SHENGWANG);
		m_pGame->Send();
		m_pGame->back_type = 0;
		list[no].level = list[no].level + 1;
	}
}

void GameProc::Chai_JianZhu()
{
	if (!m_bSuccess) {
		printf("����ʧ�ܣ�����\n");
		return;
	}
	int no = ctoi32(&m_cData[21]);  // ��� 
	if (no >= JZ_MAX)
		return;

	int type = ctoi32(&m_cData[13]); // ����
	JianZhu* list = m_pGame->GetJianZhuList((JZ_TYPE)type);
	list[no].code = ctoi64(&m_cData[5]);
	list[no].type = type;
	list[no].no = no;
	list[no].level = ctoi32(&m_cData[17]);
	list[no].timeleft = ctoi64(&m_cData[m_iDataLength - 8]) / 1000;
	list[no].mdtime = time(nullptr);
	list[no].cpttime = list[no].mdtime + list[no].timeleft + 1;
	list[no].ready = 0;
	list[no].exec = 1;
	list[no].status = 1;

	printf("list[no].level:%d\n", list[no].level);
	printf("����->���룺%016x ", list[no].code);
	printf("���ͣ�%02x �ȼ���%02x λ�ã�%02x ʣ��ʱ�䣺%lld\n", list[no].type & 0xff,
		list[no].level & 0xff, list[no].no & 0xff, list[no].timeleft);

	if (m_pGame->CheckCanFree(list[no].timeleft)) { // �������
		printf("���ʹ������Ѽ���\n");
		m_pGame->MakeTcpOPInt64(OP_FREE, list[no].code);
		m_pGame->Send();
		Sleep(1000);
	}

	if (m_pGame->back_type == BACK_WALL) {
		printf("��m_pGame->back_type��%d\n", m_pGame->back_type);
		printf("��BACK_WALL��%d\n", BACK_WALL);
		Sleep(500);
		m_pGame->MakeTcpOP(OP_SHENGWANG);
		m_pGame->Send();
		m_pGame->back_type = 0;
		list[no].level = list[no].level - 1;
	}
}

// ����������������
void GameProc::JianZhu_ShengJiNeed()
{
	if (!m_bSuccess)
		return;

#if 0
	LiuCheng* plc = GetCurrentLiuCheng();
	if (!plc || plc->op != OP_SHENGJI) // ��ǰִ�����̲�һ�£�����
		return;
#endif
	int no = ctoi32(&m_cData[29]);  // ��� 
	if (no >= JZ_MAX)
		return;

	int index = ctoi32(&m_cData[33]) + 37 + 1;
	int type = ctoi32(&m_cData[5]); // ����
	JianZhu* list = m_pGame->GetJianZhuList((JZ_TYPE)type);
	list[no].need.gangtie = ctoi32(&m_cData[index]);
	list[no].need.xikuang = ctoi32(&m_cData[index + 4]);
	list[no].need.renkou = ctoi32(&m_cData[index + 8]);
	list[no].need.liangshi = ctoi32(&m_cData[index + 12]);
	list[no].need.shiyou = ctoi32(&m_cData[index + 16]);
	list[no].sendneed = false;
	list[no].ready = true;

	printf("%02x ��������->����:%d, ϡ��:%d, �˿�:%d, ��ʳ:%d, ʯ��:%d\n", no & 0xff, list[no].need.gangtie,
		list[no].need.xikuang, list[no].need.renkou, list[no].need.liangshi, list[no].need.shiyou);

	index += 24;
	int jz_count = m_cData[index++];
	printf("�������轨������[%08x]��", jz_count);
	for (int i = 0; i < jz_count && i < 8; i++) {
		list[no].need.jz[i].type = (JZ_TYPE)ctoi32(&m_cData[index]);
		list[no].need.jz[i].level = ctoi32(&m_cData[index + 4]);
		index += 12;

		printf("[%08x-%08x]��", list[no].need.jz[i].type, list[no].need.jz[i].level);
	}
	printf("\n");
#if 0
	if (plc->flag == list[no].code) { // ִ��������˽�����ַ
		list[no].ready = 1;
		ExecLiuCheng(); // ִ������
	}
#endif
}

// ��Ѷ���
void GameProc::Free()
{
	if (!m_bSuccess)
		return;

	Sleep(500);
	// ��ȡ������Ϣ
	m_pGame->MakeTcpOP(OP_JIANZHU_JS);
	m_pGame->MakeTcpOP(OP_JIANZHU_ZY, 0, true);
	m_pGame->Send();

	printf("~~~~~~~~~~~~~~~��Ѷ��гɹ�������~~~~~~~~~~~~~~~~~~~~\n");
}

// �Ƽ��б�
void GameProc::ViewKeJi()
{
	if (!m_bSuccess)
		return;

	printf("%08x\n", ctoi32(&m_cData[17]));
	int num = 0;
	for (int i = 17; i < m_iDataLength; ) {
		if (num >= KJ_MAX)
			break;
		if ((i + 0x16) > m_iDataLength)
			break;

		m_pGame->m_KeJi[num].type = (KJ_TYPE)ctoi32(&m_cData[i]);
		m_pGame->m_KeJi[num].level = ctoi32(&m_cData[i + 4]);
		//printf("%d ���ͣ�%02x �ȼ���%02x\n", i, m_pGame->m_KeJi[num].type&0xff, m_pGame->m_KeJi[num].level & 0xff);

		//printf("%d %08x\n", i, ctoi32(&m_cData[i]));
		i += 8;
		i += ctoi32(&m_cData[i]); // ����
		i += 4;
		i += 28; // ������Դ����
		i += 12; // ����������ĵȼ�
	
		
		int zeroNum = 0;
		while (true) {
			if ((i + 0x20) > m_iDataLength) {
				i += 0x20;
				break;
			}
				
			int v = m_cData[i];
			if (v == (m_pGame->m_KeJi[num].type + 1)) {
				int nameLength = ctoi32(&m_cData[i + 5]);
				if (nameLength > 0x04) {
					//printf("%d name length:%02x\n", i, nameLength);
					if ((i + 9 + nameLength) > m_iDataLength) {
						i++;
						continue;
					}

					int zeroNum = 0;
					for (int j = 0; j < nameLength; j++) {
						if (m_cData[i + 9 + j] == 0) {
							zeroNum++;
						}
					}
					if (zeroNum < 2) {
						i -= 3;
						break;
					}
				}
			}

			i++;
		}
		num++;
	}
	printf("�Ƽ��б�������%d\n", num);
	SETISGET(IS_GET_KJ);
}

// ����ļ�б�
void GameProc::ZhaoMuList()
{
	if (!m_bSuccess)
		return;

	int id = ctoi32(&m_cData[157]);
	printf("����ID:%08x\n", id);
}

// �鿴��Ʒ 
void GameProc::ViewGoods()
{
	if (!m_bSuccess)
		return;

	printf("�����Ʒ�ɹ�������\n");
	for (int i = 15; (i + 16) < m_iDataLength;) {
		int id = ctoi32(&m_cData[i]);
		int type = m_cData[i + 4];
		int num = ctoi32(&m_cData[i + 5]);
		printf("��Ʒid: %08x type:%02x, num:%08x\n", id, type&0xff, num);
		i += 9;

		if (type == 0x02) { // �������ID
			if (id == 0xe2) {
				printf("ʹ��ǩ��С��Դ��...\n");
				m_pGame->UpdateAccountStatusText("ʹ��ǩ��С��Դ��.");
				m_pGame->MakeTcpOPMid(OP_USEGOODS, 1, id);
				m_pGame->Send();
			}
			else {
				m_pGame->m_iXinShouCode = id;
				break;
			}
		}
		//printf("i:%d\n", i);
		while (i < m_iDataLength) {
			//printf("i2:%d\n", i);
			if (m_cData[i + 4] == 0x04) {
				break;
			}

			int length = ctoi32(&m_cData[i]);
			//printf("%d length:%08x\n", i, ctoi32(&m_cData[i]));

			int oi = i;
			//printf("i3:%08X\n", i);
			i += ctoi32(&m_cData[i]);
			//printf("i4:%08X\n", i);
			i += 4;
			if (i <= oi) {
				//printf("break\n");
				goto end;
			}
				
		}

		i += 12;
	}
end:
	printf("SETISGET:%08X\n", m_iIsGetValue);
	SETISGET(IS_GET_GOODS);
}

// ʹ����Ʒ
void GameProc::UseGoods()
{
	if (!m_bSuccess)
		return;

	printf("ʹ����Ʒ�ɹ�������\n");
	// �鿴��Ʒ
	m_pGame->m_iXinShouCode = 0;
	m_pGame->MakeTcpOP(OP_VIEWGOODS);
	m_pGame->Send();
}

// �鿴��ǰ��������
void GameProc::CurrentTask()
{
	if (m_bSuccess) {
		int task_id = ctoi32(&m_cData[5]);
		printf("����id:%08x\n", task_id);
		m_pGame->MakeTcpOPInt(OP_VIEWTASK, task_id);
		m_pGame->Send();
	}
}

// �鿴�����Ƿ�����
void GameProc::ViewTask()
{
	if (!m_bSuccess)
		return;

	int task_id = ctoi32(&m_cData[5]);
	for (int i = 9, n = 1; i < m_iDataLength; n++) {
		if (n == 6) {
			if (m_cData[i] == 0x01) {
				printf("�����\n");
			}
			printf("����id:%08x [%02x]\n", task_id, m_cData[i] & 0xff);
			break;
		}
		else {
			int length = ctoi32(&m_cData[i]);
			if (length > 1)
				i += length;

			printf("����:%08x n:%d\n", length, n);
			i += 4;
		}
	}
}

// ѵ��
void GameProc::XunLian()
{
	m_pGame->UpdateAccountStatusText("ѵ���ɹ���");
	m_pGame->SetAccountSuceess();
}

// ��ɢ����
void GameProc::JieSan()
{
	m_pGame->UpdateAccountStatusText("��ɢ�ɹ���");
	m_pGame->SetAccountSuceess();
}

// ����
void GameProc::CiShan()
{
	if (m_iDataLength < 10) {
		m_pGame->UpdateAccountStatusText("���Ƴɹ���");
	}
	else {
		m_pGame->UpdateAccountStatusText("����ʧ�ܣ�����δ��ȴ��");
	}
}

// �鿴ǩ��
void GameProc::ViewQianDao()
{
	if (!m_bSuccess)
		return;

	//m_pGame->MakeTcpOP(OP_QIANDAO, 2);
	//m_pGame->Send();
	//int oneDay = m_cData[357];
	//for (char i = 1; i <= 7; i++) {
		//printf("-----------------�鿴ǩ�� һ:%d----------------\n", i);
		//m_pGame->UpdateAccountStatusText("ǩ��(��2��)");
		/*m_pGame->MakeTcpOP(OP_QIANDAO, 2);
		m_pGame->Send();
		Sleep(250);*/
	//}
	/*if (!oneDay) {
		printf("ǩ��(��һ��)...\n");
		m_pGame->UpdateAccountStatusText("ǩ��(��һ��)");
		m_pGame->MakeTcpOP(OP_QIANDAO, 1);
		m_pGame->Send();
	}
	printf("�鿴ǩ�� һ:%d============\n", oneDay);*/
}

// ǩ��
void GameProc::QianDao()
{
	if (!m_bSuccess)
		return;
	char user_info_sql[256];
	sprintf(user_info_sql, "update `cmf_account` set `sign_in`=`sign_in`+1,`login_zero_time`='%lld'"
		" where `region`='%s' and `user_login`='%s'",
		getMorningTime(), m_pGame->m_ServerId, m_pGame->m_pAccount->Mail);
	int res = mysql_query(&m_pGame->m_mysqlConnect, user_info_sql);
	if (res != 0) {
		printf("����ǩ������ʧ��");
	}
	else {
		printf("����ǩ�������ɹ�");
	}
	printf("��ǩ������sql��%s\n", user_info_sql);
}

//��������б����ɵ�����
void GameProc::RwList()
{
	if (!m_bSuccess)
		return;
	//����������
	char task_number = ctoi32(&m_cData[TASK_NUMBER_INDEX]);
	//����id
	int task_id = 0;
	//���񳤶�
	char task_str_num = 0;
	//���������ֽ�
	int skip_num = 0;
	//��ǰ�±�
	int current_index = TASK_INFO_INDEX;
	//��ǰ�ڼ�������
	char current_task_num = 0;
	printf("length����%d\n", m_iDataLength);
	for (int i = TASK_INFO_INDEX; i < m_iDataLength; i += skip_num, skip_num = 0) {
		current_task_num++;
		printf("��%d������\n", current_task_num);
		printf("������%d\n", task_number);
		if (current_task_num > task_number) {
			break;
		}

		task_id = ctoi32(&m_cData[current_index]);
		printf("����id%08x\n", task_id);
		skip_num += 4;
		current_index += 4;

		task_str_num = ctoi32(&m_cData[current_index]);
		skip_num += 4;
		current_index += 4;

		//����״��
		skip_num += task_str_num;
		current_index += task_str_num;
		if (&m_cData[current_index]) {
			//�����������
			printf("�����������\n");
			m_pGame->MakeTcpOPInt(OP_CPTTASK, task_id);
			m_pGame->Send();
			Sleep(500);
		}
		else {
			printf("no\n");
		}

		//����δ֪�ַ�000000000000
		skip_num += 7;
		current_index += 7;
	}
	printf("\n-----------------------------\n");
	//printf("%s")

	//int task_id = ctoi32(&m_cData[5]);
}

//��ȡ���л�����Ϣ
void GameProc::City()
{
	if (!m_bSuccess)
		return;
	CityInfo* p = &m_pGame->m_CityInfo;
	p->minxin = ctoi32(&m_cData[5]);
	p->minyuan = ctoi32(&m_cData[9]);
	p->all_hj = ctoi32(&m_cData[17]);
	p->shuilv = ctoi32(&m_cData[25]);
	p->hj_cl = ctoi32(&m_cData[29]);
	p->dq_people = ctoi32(&m_cData[41]);
	p->all_people = ctoi32(&m_cData[45]);
	p->kx_people = ctoi32(&m_cData[53]);
	
	char sql[330];
	sprintf(sql, "update `cmf_account` set"
		"`minxin`='%d',`minyuan`='%d',`all_hj`='%d',`shuilv`='%d',`hj_cl`='%d',"
		"`dq_people`='%d',`all_people`='%d',`kx_people`='%d'"
		"where `region`='%s' and `user_login`='%s'",
		p->minxin, p->minyuan, p->all_hj, p->shuilv, p->hj_cl,
		p->dq_people, p->all_people, p->kx_people, m_pGame->m_ServerId, m_pGame->m_pAccount->Mail);
	int res = mysql_query(&m_pGame->m_mysqlConnect, sql);
	if (res == 0) {
		printf("���л�����Ϣ���³ɹ�%s.\n", m_pGame->m_pAccount->Mail);
	}
	else {
		printf("���л�����Ϣ����ʧ�ܣ�%s\n", sql);
	}
}

// ��ȡ��������
bool GameProc::ReadLiuCheng()
{
	OpenTextFile file;
	if (!file.Open("����.txt")) {
		printf("�Ҳ�������.txt�ļ�\n");
		return false;
	}

	int length = 0;
	char data[128];
	while ((length = file.GetLine(data, 128)) > -1) {
		//printf("length:%d\n", length);
		if (length == 0) {
			continue;
		}
		Explode explode("-", trim(data));
		LiuCheng* p = TransFormOP(explode);
		memcpy(p->cmd, data, length + 1); // ����ԭ�ַ�����
	}
	//printf("����������%d\n", m_Link.Count());
	return true;
}

// ִ�ж���
bool GameProc::ExecDuiLie()
{
	if (m_DuiLie.Count() == 0) {
		if (m_iLastNoWaitCount == 0) {
			EmptyDuiLieText(); // ��ն�������
			CopyToDuiLieText("��");
		}
		m_pGame->UpdateAccountDuiLieText(m_cDuiLie);
		return false;
	}

	NODE<DuiLie*>* p = m_DuiLie.Current();
	if (m_DuiLie.IsHead()) { // ������ͷ
		m_iNoWaitCount = 0;
	}

	m_pGame->UpdateAccountStatusText(p->value->lc->cmd);

	Explode explode("-", p->value->lc->cmd);
	if (explode.GetCount() >= 3) {
		if (strstr(explode[0], "[")) {
			Explode arr("[", explode[0]);
			CopyToDuiLieText(arr[0]);
		}
		else {
			CopyToDuiLieText(explode[0]);
		}
		
		CopyToDuiLieText(".");
		CopyToDuiLieText(explode[1]);
		CopyToDuiLieText(".");
		CopyToDuiLieText(explode[2]);
	}
	
	//printf("����ִ�����̣�%s ���ȴ�������%d\n", p->value->lc->cmd, m_iNoWaitCount);

	if (IsCompleteLiuCheng(p->value->lc)) { // �����Ѿ����
		CopyToDuiLieText(" �����");
		if (m_DuiLie.Remove(p->value, 1)) { // �Ƴ�������
			printf("����ɣ��Ƴ�����...(%d)\n", m_DuiLie.Count());
			p = m_DuiLie.Current();         // �Ƴ���ָ����Զ�������
		}
		else {
			printf("����ɣ��Ƴ�����ʧ��...\n");
			p = m_DuiLie.Next();
		}
	}
	else {
		bool exec = true;
		LiuCheng* lc = p->value->lc;   // ����
		if (lc->op == OP_CPTTASK) {    // ����Ϊ�������
			exec = p->prev == nullptr; // Ҫ��ǰ�����������
		}
		bool wait = false;
		if (exec) {
			wait = ExecLiuCheng(lc);
			if (!wait) {   // ִ�д˶���
				m_iNoWaitCount++;
			}
		}
		else {
			m_iNoWaitCount++;
		}

		printf("���ȴ�������%d exec:%d wait:%d %s\n", m_iNoWaitCount, exec, wait, p->value->lc->cmd);
		p = m_DuiLie.Next();
	}

	if (p) { // ����һ������
		CopyToDuiLieText(" | ");
	}
	else {   // ���һ������
		m_DuiLie.Reset();
		m_pGame->UpdateAccountDuiLieText(m_cDuiLie);
		EmptyDuiLieText(); // ��ն�������
	}

	return p != nullptr;
}

// �������Ƿ�ȴ�
bool GameProc::CheckWaitDuiLie(LiuCheng* p)
{
	__int64 free_code = 0; // ��Ѷ��д���
	__int64 timeleft  = 0; // ������ɵ���ʱ
	__int64 freeleft  = 0; // ���������ѵ���ʱ
	__int64 now_time  = time(nullptr);
	__int32 is_free = 0;
	__int32 *p_free = nullptr;
	if (p->op == OP_JIANZAO || p->op == OP_SHENGJI) { // ���������
		if (p->jz == nullptr)
			return true;

		free_code = p->jz->code;
		timeleft  = p->jz->cpttime - now_time;
		freeleft  = m_pGame->GetFreeLeft(timeleft);
		is_free = p->jz->free;
		p_free = &(p->jz->free);

		if (!m_pGame->m_pAccount->CptTime) {
			m_pGame->m_pAccount->CptTime = p->jz->cpttime;
		}
		else {
			if (p->jz->cpttime < m_pGame->m_pAccount->CptTime) {
				m_pGame->m_pAccount->CptTime = p->jz->cpttime;
			}
		}
	}
	if (p->noconf) {
		//printf("noconf:%08x type:%d left:%d %d\n", p->op, p->jz->type, timeleft, p->jz->cpttime-now_time);
	}
	if (timeleft > 0) { // ����ʱ
		//printf("timeleft:%d\n", timeleft);
		if (1 || !p->noconf) {
			char tmp[32];
			if (timeleft <= 60) {
				sprintf_s(tmp, " [%02d]", timeleft);
			}
			else if (timeleft <= 3600) {
				sprintf_s(tmp, " [%02d:%02d]", (timeleft % 3600) / 60, timeleft % 60);
			}
			else {
				sprintf_s(tmp, " [%d:%02d:%02d]", timeleft / 3600, (timeleft % 3600) / 60, timeleft % 60);
			}
			CopyToDuiLieText(tmp);
		}
	}
	if (timeleft < 5) {
		return true;
	}
	if (freeleft < m_pGame->m_stConfig.WaitSecond) { // ���ȴ�ʱ��
		printf("isfree:%d\n", is_free);
		if (freeleft < 0 && !is_free) { // �������
			if (p_free) {
				*p_free = 1;
			}
			m_pGame->MakeTcpOPInt64(OP_FREE, free_code);
			if (m_pGame->Send()) {
				*p_free = 0;
			}

			if (1 || !p->noconf) {
				CopyToDuiLieText(" ���");
			}
			printf("~~~~~~~~~~~��Ѷ��У�����~~~~~~~~~~~~~~~\n");
		}
		return true;
	}
	return false;
}

// ִ������
bool GameProc::ExecLiuCheng(LiuCheng* p)
{
	if (p == nullptr || p->complete) { // ����ɻ򲻴���
		printf("��������ɣ�%s\n", p ? p->cmd : "��");
		return true;
	}

	if (p->jz && p->jz->exec) { // �Ѿ���ִ��
		char str[16];
		sprintf_s(str, "��%d", p->jz->level+1);
		CopyToDuiLieText(str);
		return CheckWaitDuiLie(p);
	}

	printf("׼��ִ�����̣�%s[%08x]\n", p->cmd, p->op);
	bool result = true;
	if (p->op == OP_JIANZAO) { // ����
		if (!p->jz) {
			int area = m_pGame->GetJianZhuFreeNoByType((JZ_TYPE)p->value);
			if (area > -1) {
				printf("����λ�ú�:%02x\n", area&0xff);
				m_pGame->MakeHouse(p->value, area);
				if (m_pGame->Send()) {
					m_pGame->MakeTcpOP(OP_VIEWRES);
					m_pGame->Send();
				}
			}
			else {
				CopyToDuiLieText(" û��λ��");
				printf("û�п���λ�ý���...\n");

				result = false;
				m_pGame->m_pAccount->CptTime = time(nullptr) + 600;
			}
		}
	}
	else if (p->op == OP_SHENGJI) {
		// ��÷���Ҫ����
		if (!p->jz) {
			p->jz = m_pGame->GetJianZhuByType((JZ_TYPE)p->value, p->extra[1] == YX_MAX ? 1 : -1);
		}
		
		if (p->jz) {
			if (p->jz->ready) {
				if (m_pGame->CheckRes(p->jz->need)) { // ��Դ�㹻������
					CopyToDuiLieText(" ��Դ�㹻");
					printf("��Դ�㹻������������\n");
					m_pGame->MakeTcpOPInt64(OP_SHENGJI, p->jz->code);
					if (m_pGame->Send()) {
						for (int i = 0; i < p->jiasunum; i++) {
							for (int j = 0; j < p->jiasu[i].num; j++) {
								if (p->jiasu[i].type) {
									Sleep(500);
									m_pGame->MakeJiaSu(p->jz->code, p->jiasu[i].type);
									m_pGame->Send();
									printf("ʹ�ü��٣�%d\n", p->jiasu[i].type);
								}
							}
						}
						SendTheData();
					}
				}
				else {
					CopyToDuiLieText(" ��Դ����");
					printf("��Դ����...\n");

					result = false;
					m_pGame->m_pAccount->CptTime = time(nullptr) + 600;
				}
			}
			else { // ��ѯ������������
				printf("�����������裬λ��:%d %08x %d\n", p->jz->no, p->jz->code, p->jz->exec);
				CopyToDuiLieText(" ��ѯ����");
				if (!p->jz->sendneed) {
					p->jz->sendneed = true;
					m_pGame->MakeUpNeed(p->jz->code, (JZ_TYPE)p->jz->type, p->jz->no);
					if (m_pGame->Send()) {
						m_pGame->MakeTcpOP(OP_VIEWRES);
						m_pGame->Send();
					}
				}
			}
		}
		else {
			CopyToDuiLieText(" ������Ҫ��");
			printf("�Ҳ�������Ҫ����...%d %d\n", p->value, p->extra[1]);
			result = false;
		}
	}
	else if (p->op == OP_YANJIU) { // �о��Ƽ�
		printf("�о��Ƽ�...\n");
		m_pGame->MakeYanJiu((KJ_TYPE)p->value);
		m_pGame->Send();
		p->complete = true;
	}
	else if (p->op == OP_XUNLIAN) {
		printf("ѵ��...\n");
		JianZhu* jz = m_pGame->GetJianZhuByType(JZ_JUNGONG);
		if (jz) { // ��������������
			m_pGame->MakeXunLian(jz->code);
			m_pGame->Send();
		}
		p->complete = true;
	}
	else if (p->op == OP_USEGOODS) { // ʹ����Ʒ
		printf("ʹ����Ʒ...\n");
		m_pGame->MakeTcpOPMid(OP_USEGOODS, p->extra[0], p->value);
		m_pGame->Send();
		p->complete = true;
	}
	else if (p->op == OP_CPTTASK) {
		printf("�������...\n");
		m_pGame->MakeTcpOPInt(OP_CPTTASK, p->value);
		m_pGame->Send();
		p->complete = true;
	}
	else if (p->op == OP_YIMIN) {
		printf("��������...\n");
		m_pGame->MakeTcpOPInt(OP_YIMIN, p->value);
		m_pGame->Send();
		p->complete = true;
	}

	return result;
}

// �������������
int GameProc::CalcCompleteLiuCheng()
{
	if (!ISGETALL()) {
		printf("!ISGETALL %08x %08x\n", m_iIsGetValue, GETALLVAL());
		return 0;
	}

	m_iLastExecCount = m_pGame->GetJianZhuExecNum(m_iLastNoWaitCount);
	if (m_iLastExecCount > 0) {
		printf("����ִ�н���������%d ���ȴ�������%d\n", m_iLastExecCount, m_iLastNoWaitCount);
	}
	if (m_iLastExecCount >= m_iDuiLieMax) {
		m_pGame->UpdateAccountStatusText("ִ�ж���...");
		m_pGame->UpdateAccountDuiLieText(m_cDuiLie);
		EmptyDuiLieText();
		return 2;
	}
	int free_count = m_iDuiLieMax - m_iLastExecCount;
		
	if (m_DuiLie.Count() >= m_iDuiLieMax) {
		printf("��������:%d>=%d\n", m_DuiLie.Count(), free_count);
		return 1;
	}
	
	//printf("��ʼִ������\n");
	bool last_complete = false;
	int n = 1;
	LiuCheng* p;
	bool flag = true;
	for (p = GetCurrentLiuCheng(); p; p = GetNextLiuCheng(), n++) {
		if (m_DuiLie.Count() >= free_count) {
			printf("��������[for]:%d>=%d\n", m_DuiLie.Count(), free_count);
			break;
		}
			
		bool complete = flag ? IsCompleteLiuCheng(p) : false;
		printf("%d������:%d OP:%08x V:%08x | %s\n", n, complete, p->op, p->value, p->cmd);
		if (!complete) { // û����ɵ���ӵ�ִ�ж���
			DuiLie* dl = new DuiLie;
			dl->lc = p;
			m_DuiLie.Add(dl);
			printf("�������!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%s(%d)\n", p->cmd, m_DuiLie.Count());
			flag = false;
		}
	}
	if (m_bFirstCalc) {
		//printf("m_bFirstCalcm_bFirstCalcm_bFirstCalcm_bFirstCalcm_bFirstCalc\n");
	}
	int result = m_bFirstCalc ? 3 : 1;
	m_bFirstCalc = false;
	return p ? result : -1;
}

// ��ȡ��ǰ����
LiuCheng* GameProc::GetCurrentLiuCheng()
{
	NODE<LiuCheng*>* p = m_Link.Current();
	return p ? p->value : nullptr;
}

// ��ȡ��һ��ִ������
LiuCheng* GameProc::GetNextLiuCheng()
{
	NODE<LiuCheng*>* p = m_Link.Next();
	return p ? p->value : nullptr;
}

// ��ɵ�ǰ����
LiuCheng* GameProc::CompleteLiuCheng()
{
	LiuCheng* p = GetCurrentLiuCheng();
	if (p) {
		p->complete = true;
	}
	return GetNextLiuCheng();
}

// ����ָ��ת����Ϸָ��
LiuCheng* GameProc::TransFormOP(Explode& lcLine)
{
	LiuCheng* pLiuCheng = new LiuCheng;
	memset(pLiuCheng, 0, sizeof(LiuCheng));
	pLiuCheng->op = OP_UNKNOW;
	pLiuCheng->byte = 4;
	pLiuCheng->jiasunum = 0;

	char* op = lcLine.GetValue(0);
	char* v = lcLine.GetValue(1);

	if (strstr(op, "����")) {
		pLiuCheng->op = OP_JIANZAO;
		pLiuCheng->value = TransFormRes(v);
		pLiuCheng->extra[0] = hextoi32(lcLine.GetValue(2));    // ��������

		TransFormJiaSu(op, pLiuCheng);
	}
	else if (strstr(op, "����")) {
		pLiuCheng->op = OP_SHENGJI;
		pLiuCheng->value = TransFormRes(v);
		pLiuCheng->extra[0] = hextoi32(lcLine.GetValue(2));     // �ȼ�
		pLiuCheng->extra[1] = TransFormRes(lcLine.GetValue(3)); // ������������

		TransFormJiaSu(op, pLiuCheng);
	}
	else if (strstr(op, "ѵ��")) {
		pLiuCheng->op = OP_XUNLIAN;
	}
	else if (strstr(op, "�о�")) {
		pLiuCheng->op = OP_YANJIU;
		pLiuCheng->value = TransFormRes(v);
		pLiuCheng->extra[0] = hextoi32(lcLine.GetValue(2));
	}
	else if (strstr(op, "�������")) {
		pLiuCheng->op = OP_CPTTASK;
		pLiuCheng->value = hextoi32(v);
	}
	else if (strstr(op, "ʹ����Ʒ")) {
		pLiuCheng->op = OP_USEGOODS;
		pLiuCheng->value = TransFormRes(v);
		pLiuCheng->extra[0] = 1;
	}
	else if (strstr(op, "��������")) {
		pLiuCheng->op = OP_YIMIN;
		pLiuCheng->value = 0x83;
	}
	//printf("->%s ->%08x ->%08x ->%08x\n", lcLine.GetValue(0), pLiuCheng->op, pLiuCheng->value, pLiuCheng->extra[0]);

	if (pLiuCheng->op != OP_UNKNOW) {
		m_Link.Add(pLiuCheng);
	}

	return pLiuCheng;
}

// �����Ƿ�����
bool GameProc::IsCompleteLiuCheng(LiuCheng* p)
{
	if (p->complete)
		return true;

	bool complete = false;
	if (p->op == OP_JIANZAO) {
		// ��ô˽�������
		p->jz = m_pGame->GetJianZhuExecByType((JZ_TYPE)p->value);
		if (p->jz && p->jz->level == 0) {
			printf("�����ڽ��콨��...\n");
			complete = false;
		}
		else {
			p->jz = nullptr;
			int num = m_pGame->GetJianZhuNumByType((JZ_TYPE)p->value);
			complete = num >= p->extra[0];
		}
	}
	else if (p->op == OP_SHENGJI) {
		// �������ִ�н���
		p->jz = m_pGame->GetJianZhuExecByType((JZ_TYPE)p->value);
		if (p->jz && p->extra[1] != YX_MAX) {
			printf("�������������� ����:%d ����:%08x...\n", p->jz->type, p->jz->code);
			complete = p->jz->level >= p->extra[0];
			/*if (p->jz->level >= p->extra[0]) {
				complete = true;
				printf("�������...\n");
			}*/
		}
		else {
			// ��÷���Ҫ����
			p->jz = m_pGame->GetJianZhuByType((JZ_TYPE)p->value, p->extra[1] == YX_MAX ? 1 : -1);
			if (p->jz  && p->jz->level >= p->extra[0])
				complete = true;
		}
	}
	else if (p->op == OP_YANJIU) {
		if (m_pGame->CheckKeJiLevel((KJ_TYPE)p->value, p->extra[0])) {
			complete = true;
		}
	}
	else if (p->op == OP_USEGOODS) {
		complete = (m_pGame->m_iXinShouCode > 0) && (p->value != m_pGame->m_iXinShouCode);
		if (complete) {
			printf("���������%d != %d\n", m_pGame->m_iXinShouCode, p->value);
		}
	}
	else {
		complete = m_bFirstCalc;
	}
	p->complete = complete;

	return complete;
}

// ��������ת����ԴID(�������Ƽ����ƴ���)
int GameProc::TransFormRes(const char* data)
{
	if (data == nullptr)
		return DATA_UKNOW;

	// ����Ϊ��������
	if (strcmp(data, "������") == 0)
		return JZ_SHIZHENG;
	if (strcmp(data, "���") == 0)
		return JZ_MINJU;
	if (strcmp(data, "ũ��") == 0)
		return JZ_NONGTIAN;
	if (strcmp(data, "���ֳ�") == 0)
		return JZ_LIANGANG;
	if (strcmp(data, "ʯ�ͻ���") == 0)
		return JZ_SHIYOU;
	if (strcmp(data, "ϡ��") == 0)
		return JZ_XIKUANG;
	if (strcmp(data, "Χǽ") == 0)
		return JZ_WEIQIANG;
	if (strcmp(data, "��������") == 0)
		return JZ_KEYAN;
	if (strcmp(data, "��У") == 0)
		return JZ_JUNXIAO;
	if (strcmp(data, "��ı��") == 0)
		return JZ_CANMOUBU;
	if (strcmp(data, "--") == 0)
		return JZ_UNKONOW2;
	if (strcmp(data, "�ֿ�") == 0)
		return JZ_CANGKU;
	if (strcmp(data, "˾�") == 0)
		return JZ_SILINGBU;
	if (strcmp(data, "������") == 0)
		return JZ_JUNGONG;
	if (strcmp(data, "��������") == 0)
		return JZ_LIANLUO;
	if (strcmp(data, "�Ṥ��") == 0)
		return JZ_QINGCHANG;

	// ����Ϊ�Ƽ�����
	if (strcmp(data, "��ʳ����") == 0)
		return KJ_LIANGSHI;
	if (strcmp(data, "��������") == 0)
		return KJ_GANGTIE;
	if (strcmp(data, "ʯ������") == 0)
		return KJ_SHIYOU;
	if (strcmp(data, "ϡ������") == 0)
		return KJ_XIKUANG;

	// ����Ϊ�������
	if (strcmp(data, "�������1") == 0)
		return LB_XINSHOU_1;
	if (strcmp(data, "�������2") == 0)
		return LB_XINSHOU_2;
	if (strcmp(data, "�������3") == 0)
		return LB_XINSHOU_3;
	if (strcmp(data, "�������4") == 0)
		return LB_XINSHOU_4;
	if (strcmp(data, "�������5") == 0)
		return LB_XINSHOU_5;
	if (strcmp(data, "�������6") == 0)
		return LB_XINSHOU_6;
	if (strcmp(data, "�������7") == 0)
		return LB_XINSHOU_7;

	// ����Ϊ��������
	if (strcmp(data, "ȫ��") == 0)
		return YX_ALL;
	if (strcmp(data, "���") == 0)
		return YX_MAX;
	if (strcmp(data, "��С") == 0)
		return YX_MIN;

	return DATA_UKNOW;
}

// ת�ɼ���
int GameProc::TransFormJiaSu(const char* data, LiuCheng* p)
{
	Preg preg(data);
	char* str = preg.Match("[", "]", false);
	if (!str)
		return 0;

	printf("data:%s str:%s\n", data, str);
	Explode arr("&", str);
	for (int i = 0; i < arr.GetCount(); i++) {
		p->jiasu[i].type = 0;

		Explode data(":", arr[i]);
		if (strcmp("15����", data[0]) == 0) {
			p->jiasu[i].type = 1;
		}
		else if (strcmp("1Сʱ", data[0]) == 0) {
			p->jiasu[i].type = 2;
		}
		else if (strcmp("5Сʱ", data[0]) == 0) {
			p->jiasu[i].type = 3;
		}
		p->jiasu[i].num = data.GetValue2Int(1); // ����

		printf("���̼���:%d[%d]\n", p->jiasu[i].type, p->jiasu[i].num);
	}
	p->jiasunum = arr.GetCount();

	return p->jiasunum;
}

// ��������ת������
void GameProc::TransToTextJZ(JZ_TYPE type, char text[])
{
	if (type == JZ_SHIZHENG) {
		strcpy(text, "������");
		return;
	}
	if (type == JZ_MINJU) {
		strcpy(text, "���");
		return;
	}
	if (type == JZ_NONGTIAN) {
		strcpy(text, "ũ��");
		return;
	}
	if (type == JZ_LIANGANG) {
		strcpy(text, "���ֳ�");
		return;
	}
	if (type == JZ_SHIYOU) {
		strcpy(text, "ʯ�ͻ���");
		return;
	}
	if (type == JZ_XIKUANG) {
		strcpy(text, "ϡ��ʯ");
		return;
	}
	if (type == JZ_WEIQIANG) {
		strcpy(text, "Χǽ");
		return;
	}
	if (type == JZ_KEYAN) {
		strcpy(text, "��������");
		return;
	}
	if (type == JZ_JUNXIAO) {
		strcpy(text, "��У");
		return;
	}
	if (type == JZ_CANMOUBU) {
		strcpy(text, "��ı��");
		return;
	}
	if (type == JZ_UNKONOW2) {
		strcpy(text, "δ֪");
		return;
	}
	if (type == JZ_CANGKU) {
		strcpy(text, "�ֿ�");
		return;
	}
	if (type == JZ_SILINGBU) {
		strcpy(text, "˾�");
		return;
	}
	if (type == JZ_JUNGONG) {
		strcpy(text, "������");
		return;
	}
	if (type == JZ_LIANLUO) {
		strcpy(text, "��������");
		return;
	}
	if (type == JZ_QINGCHANG) {
		strcpy(text, "�Ṥ��");
		return;
	}
}

// �����Ƿ�ɺ��Լ��
bool GameProc::CanIgnoreCk(struct_jianzhu* jz)
{
	m_DuiLie.Store();

	m_DuiLie.Reset();
	for (NODE<DuiLie*>* p = m_DuiLie.Current(); p; p = m_DuiLie.Next()) {
		if (p->value && p->value->lc->jz) { // �˽������ڶ���
			if (p->value->lc->jz->code == jz->code)
				return true;
		}
	}

	m_DuiLie.ReStore();
	return false;
}

// ���Ƶ�������������
char* GameProc::CopyToDuiLieText(char* text)
{
	if (!m_cDuiLie)
		return nullptr;

	if (text) {
		strcat(m_cDuiLie, text);
	}
	else {
		int len = strlen(m_cDuiLie);
		if (len > 0) {
			m_cDuiLie[len - 1] = 0;
		}
	}
	return m_cDuiLie;
}

// ��ն�����������
char* GameProc::EmptyDuiLieText()
{
	if (m_cDuiLie)
		m_cDuiLie[0] = 0;

	return m_cDuiLie;
}

// �Ƿ�����
bool GameProc::IsLocked()
{
	return m_bLock;
}

// ����
void GameProc::Lock()
{
	m_bLock = true;
}

// ����
void GameProc::UnLock()
{
	m_bLock = false;
}

// �Ƿ�ر���Ϸ
bool GameProc::IsCloseGame()
{
	return (m_iLastNoWaitCount + m_iNoWaitCount) >= m_iDuiLieMax;
}
