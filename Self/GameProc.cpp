#include <iostream>
#include "GameProc.h"
#include "GameAccount.h"
#include "Game.h"
#include "../Common/mystring.h"
#include "../Common/OpenTextFile.h"
#include "../Common/Explode.h";
#include "../Common/Preg.h"

#define IS_GET_JS    0x01  // 已经获取了军事区建筑
#define IS_GET_ZY    0x02  // 已经获取了自由区建筑
#define IS_GET_KJ    0x04  // 已经获取了已研究科技
#define IS_GET_GOODS 0x08  // 已经获取了物品
#define IS_GET_RES   0x10  // 是否获取了资源数据

#define GETALLVAL() (IS_GET_JS|IS_GET_ZY|IS_GET_KJ|IS_GET_GOODS|IS_GET_RES)
#define SETISGET(v) (m_iIsGetValue|=(v)); // 设置已经获取了什么信息
#define ISGETALL() (m_iIsGetValue==(IS_GET_JS|IS_GET_ZY|IS_GET_KJ|IS_GET_GOODS|IS_GET_RES)) // 是否已获取了所有

// 设置Game指针
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
	printf("~GameProc！！！\n");
}

// 设置响应数据
void GameProc::SetData(const char* data, int length, char flag, bool success)
{
	m_cData = data;
	m_iDataLength = length;
	m_chFlag = flag;
	m_bSuccess = success;
}

// 发送一些数据
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
	//	printf("-----------------签到 一:%d----------------\n", i);
	//	//m_pGame->UpdateAccountStatusText("签到(第2天)");
	//	m_pGame->MakeTcpOP(OP_QIANDAO, i);
	//	m_pGame->Send();
	//	Sleep(2500);
	//}
	//m_pGame->MakeTcpOP(OP_VIEWCURTASK, 0, true);
	//m_pGame->MakeTcpOP(OP_ZHAOMULIST, 0, true);
#endif
	
}

// 处理登录
void GameProc::Login()
{
	EmptyDuiLieText();
	if (m_chFlag == 0x01 || m_chFlag == 0x02) {
#if 0
		m_pGame->UpdateAccountStatusText("验证成功.");
		//m_pGame->InsertLog(L"获取坐标...");
		m_pGame->UpdateAccountStatusText("获取坐标...");
		m_pGame->MakeTcpOP(OP_COOR);
		m_pGame->Send();


		m_pGame->UpdateAccountStatusText("获取军事区建筑...");
		//m_pGame->InsertLogFunc(L"获取军事区建筑...");
#endif
		m_pGame->SetLoginOkTime(); // 设置登录成功时间
		if (m_pGame->m_stConfig.Close || m_pGame->m_stConfig.Open
			|| m_pGame->m_stConfig.JiangSheng) {
			m_pGame->UpdateAccountStatusText("获取军工厂...");
			m_pGame->MakeTcpOP(OP_JIANZHU_JS);
			m_pGame->Send();
			Sleep(250);
			m_pGame->MakeTcpOP(OP_SHENGWANG);
			m_pGame->Send();
			return;
		}
		else {
			printf("执行流程");
			return;
			//查询用户信息
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
					//签到（24表示签到天数，38凌晨签到时间）
					if (getMorningTime() > atoi(row[38]) && atoi(row[24]) < 8) {
						printf("进来签到");
						m_pGame->MakeTcpOP(OP_QIANDAO, atoi(row[24]));
						m_pGame->Send();
						//更新城市坐标
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
			m_pGame->InsertLogFunc(L"获取声望值...");
			m_pGame->UpdateAccountStatusText("获取声望值...");
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
			printf("预计%d点开服！！！\n", open_hour);

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

// 获取坐标
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
		printf("坐标更新成功%s.\n", m_pGame->m_pAccount->Mail);
	}
	else {
		printf("坐标更新失败：%s\n", sql);
	}
	/*char sStr[16];
	sprintf_s(sStr, "%d,%d\n", x, y);
	m_pGame->UpdateText(2, sStr);*/
}

// 声望处理
void GameProc::ShengWang()
{
	int shengwang = m_pGame->GetShengWang(m_cData);

	char sSW[16];
	sprintf_s(sSW, "%d", shengwang);
	m_pGame->UpdateText(3, sSW);
	printf("声望：%d\n", shengwang);

	//关门
	if (m_pGame->m_stConfig.Close) {
		printf("-----------guanmen---------\n");
		if (shengwang > 9999) {
			/*m_pGame->UpdateAccountStatusText("慈善...");
			m_pGame->MakeTcpOP(OP_CISHAN, 0x01);
			m_pGame->Send();*/

			int cha = shengwang - 9999;
			int san = cha >> 1;
			if (cha & 0x01)
				san += 1;

			CString csSan;
			csSan.Format(L"声望达到%d，解散数量:%d", shengwang, san);
			m_pGame->UpdateAccountStatusText("解散...");
			m_pGame->MakeTcpJieSan(san);
			//m_pGame->MakeTcpOP(OP_JIESAN, san);
			m_pGame->Send();
		}
		else {
			m_pGame->UpdateAccountStatusText("无须解散.");
			m_pGame->SetAccountSuceess();
		}
	}

	//开门
	if (m_pGame->m_stConfig.Open) {
		printf("-----------kaimen---------\n");
		if (shengwang < 10000) {
			JianZhu* jz = m_pGame->GetJianZhuByType(JZ_JUNGONG);
			if (jz) { // 军工厂建筑代码
				int cha = 10000 - shengwang;
				int san = cha >> 1;
				if (cha & 0x01)
					san += 1;

				CString csSan;
				csSan.Format(L"声望达到%d，训练步兵数量:%d", shengwang, san);
				m_pGame->XunLianMore(jz->code, san);
				m_pGame->Send();
			}
		}
		else {
			m_pGame->UpdateAccountStatusText("无须训练.");
			m_pGame->SetAccountSuceess();
		}
	}

	//降声望
	if (m_pGame->m_stConfig.JiangSheng) {
		printf("-----------降声望---------\n");
		if (shengwang > 9999) {
			Sleep(100);
			JianZhu* weiqiang = m_pGame->GetJianZhuByType(JZ_WEIQIANG);
			if (weiqiang) { // 围墙建筑代码
				printf("weiqiang->level：%d\n", weiqiang->level);
				if (weiqiang->level < 1) {
					//升级围墙
					printf("-----升级围墙------\n");
					m_pGame->MakeTcpOPInt64(OP_SHENGJI, weiqiang->code);
					m_pGame->Send();
					m_pGame->back_type = 1;
				}

				if (weiqiang->level == 1) {
					//拆围墙
					printf("-----拆围墙------\n");
					m_pGame->MakeTcpOPInt64(OP_CHAI_JZ, weiqiang->code);
					m_pGame->Send();
					m_pGame->back_type = 1;
				}

				if (weiqiang->level > 1) {
					//拆围墙
					printf("-----拆围墙------\n");
					m_pGame->MakeTcpOPInt64(OP_CHAI_JZ, weiqiang->code);
					m_pGame->Send();
				}
			}
		}
	}
}

// 资源
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
		printf("拥有资源更新成功%s.\n", m_pGame->m_pAccount->Mail);
	}
	else {
		printf("拥有资源更新失败：%s\n", sql);
	}

}

// 建筑-军事区
void GameProc::JianZhu_JS()
{
	int num = 0;
	for (int i = 6; i < m_iDataLength; i += 49) {
		if ((i + 0x16) > m_iDataLength)
			break;

		int no = ctoi32(&m_cData[i + 16]); // 所在位置编号
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
			SetZYNum(jz->level); // 设置自由区可建造数量
			//m_pGame->MakeUpNeed(jz->code, (JZ_TYPE)jz->type, jz->no);
			//m_pGame->Send();
		}
#endif

		num++;
	}
	printf("军事区建筑：%d\n", num);
	SETISGET(IS_GET_JS);

	//ExecLiuCheng();
	/*
	int id = m_pGame->GetXLYId(&m_cData[10], m_iDataLength - 10);
	if (id) {
		m_pGame->InsertLogFunc(L"成功获取军工厂ID！");
	}
	else {
		m_pGame->UpdateAccountStatusText("获取军工厂id失败.");
		m_pGame->InsertLogFunc(L"无法获取军工厂ID！");
	}
	m_pGame->UpdateAccountStatusText("训练...");
	m_pGame->InsertLogFunc(L"训练...");
	// 0x00000bb9 为训练指令
	m_pGame->MakeTcpStr(OP_XUNLIAN, id);
	//m_pGame->MakeTcpJieSan(0x02);
	m_pGame->Send();
	*/
}

// 建筑-自由区
void GameProc::JianZhu_ZY()
{
	int num = 0;
	for (int i = 10; i < m_iDataLength; i += 49) {
		if ((i + 0x16) > m_iDataLength)
			break;

		int no = ctoi32(&m_cData[i + 16]); // 所在位置编号
		if (no >= JZ_MAX)
			continue;

		JianZhu* jz = &m_pGame->m_zyJianZhu[no];
		SaveJianZhu(&m_cData[i], jz, no);

		num++;
	}
	printf("自由区建筑：%d\n", num);
	SETISGET(IS_GET_ZY);
}

// 建筑保存
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

	printf("代码：%016x", jz->code);
	printf("类型：%02x 等级：%02x 位置：%02x", jz->type & 0xff, jz->level & 0xff, jz->no & 0xff);
	
	if (m_bFirstCalc) {
		printf(" 第一次");
	}
	if (jz->exec) {
		printf(" 设置为正在执行...");
	}
	if (jz->ignoreck) {
		printf(" 忽略检查");
	}
	printf(" 剩余时间：%lld", jz->timeleft);

	printf("\n");
}

// 设置自由区域可建造数量
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

// 建筑建造
void GameProc::JianZhu_JianZao()
{
	if (!m_bSuccess)
		return;

	int no = ctoi32(&m_cData[21]);  // 编号 
	if (no >= JZ_MAX)
		return;

	int type = ctoi32(&m_cData[13]); // 类型
	JianZhu* list = m_pGame->GetJianZhuList((JZ_TYPE)type);
	list[no].code = ctoi64(&m_cData[5]);
	list[no].type = type;
	list[no].no = no;
	list[no].level = ctoi32(&m_cData[17]);
	list[no].timeleft = ctoi64(&m_cData[m_iDataLength - 8]) / 1000;
	list[no].mdtime = time(nullptr);
	list[no].cpttime = list[no].mdtime + list[no].timeleft + 1;
	list[no].status = 1;

	printf("建造->代码：%016x ", list[no].code);
	printf("类型：%02x 等级：%02x 位置：%02x 剩余时间：%lld\n", list[no].type & 0xff,
		list[no].level & 0xff, list[no].no & 0xff, list[no].timeleft);

	if (list[no].timeleft < FREE_SECOND) { // 可以免费
		m_pGame->MakeTcpOPInt64(OP_FREE, list[no].code);
		if (m_pGame->Send()) {
			//list[no].level++;
		}
	}
}

// 建筑升级
void GameProc::JianZhu_ShengJi()
{
	if (!m_bSuccess) {
		printf("升级失败！！！\n");
		return;
	}
	
	printf("升级中！！！\n");
	//printf("升级中编号：%d\n", ctoi32(&m_cData[21]));
	int no = ctoi32(&m_cData[21]);  // 编号 
	if (no >= JZ_MAX)
		return;

	int type = ctoi32(&m_cData[13]); // 类型
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

	printf("升级->代码：%016x ", list[no].code);
	printf("类型：%02x 等级：%02x 位置：%02x 剩余时间：%lld\n", list[no].type & 0xff,
		list[no].level & 0xff, list[no].no & 0xff, list[no].timeleft);

	if (m_pGame->CheckCanFree(list[no].timeleft)) { // 可以免费
		printf("升级使用免费加速\n");
		m_pGame->MakeTcpOPInt64(OP_FREE, list[no].code);
		m_pGame->Send();
		Sleep(1000);
	}

	if (m_pGame->back_type == BACK_WALL) {
		printf("m_pGame->back_type：%d\n", m_pGame->back_type);
		printf("BACK_WALL：%d\n", BACK_WALL);
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
		printf("拆建筑失败！！！\n");
		return;
	}
	int no = ctoi32(&m_cData[21]);  // 编号 
	if (no >= JZ_MAX)
		return;

	int type = ctoi32(&m_cData[13]); // 类型
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
	printf("拆建筑->代码：%016x ", list[no].code);
	printf("类型：%02x 等级：%02x 位置：%02x 剩余时间：%lld\n", list[no].type & 0xff,
		list[no].level & 0xff, list[no].no & 0xff, list[no].timeleft);

	if (m_pGame->CheckCanFree(list[no].timeleft)) { // 可以免费
		printf("拆除使用了免费加速\n");
		m_pGame->MakeTcpOPInt64(OP_FREE, list[no].code);
		m_pGame->Send();
		Sleep(1000);
	}

	if (m_pGame->back_type == BACK_WALL) {
		printf("拆：m_pGame->back_type：%d\n", m_pGame->back_type);
		printf("拆：BACK_WALL：%d\n", BACK_WALL);
		Sleep(500);
		m_pGame->MakeTcpOP(OP_SHENGWANG);
		m_pGame->Send();
		m_pGame->back_type = 0;
		list[no].level = list[no].level - 1;
	}
}

// 建筑升级所需条件
void GameProc::JianZhu_ShengJiNeed()
{
	if (!m_bSuccess)
		return;

#if 0
	LiuCheng* plc = GetCurrentLiuCheng();
	if (!plc || plc->op != OP_SHENGJI) // 当前执行流程不一致，不管
		return;
#endif
	int no = ctoi32(&m_cData[29]);  // 编号 
	if (no >= JZ_MAX)
		return;

	int index = ctoi32(&m_cData[33]) + 37 + 1;
	int type = ctoi32(&m_cData[5]); // 类型
	JianZhu* list = m_pGame->GetJianZhuList((JZ_TYPE)type);
	list[no].need.gangtie = ctoi32(&m_cData[index]);
	list[no].need.xikuang = ctoi32(&m_cData[index + 4]);
	list[no].need.renkou = ctoi32(&m_cData[index + 8]);
	list[no].need.liangshi = ctoi32(&m_cData[index + 12]);
	list[no].need.shiyou = ctoi32(&m_cData[index + 16]);
	list[no].sendneed = false;
	list[no].ready = true;

	printf("%02x 升级所需->钢铁:%d, 稀矿:%d, 人口:%d, 粮食:%d, 石油:%d\n", no & 0xff, list[no].need.gangtie,
		list[no].need.xikuang, list[no].need.renkou, list[no].need.liangshi, list[no].need.shiyou);

	index += 24;
	int jz_count = m_cData[index++];
	printf("升级所需建筑条件[%08x]：", jz_count);
	for (int i = 0; i < jz_count && i < 8; i++) {
		list[no].need.jz[i].type = (JZ_TYPE)ctoi32(&m_cData[index]);
		list[no].need.jz[i].level = ctoi32(&m_cData[index + 4]);
		index += 12;

		printf("[%08x-%08x]，", list[no].need.jz[i].type, list[no].need.jz[i].level);
	}
	printf("\n");
#if 0
	if (plc->flag == list[no].code) { // 执行流程与此建筑遗址
		list[no].ready = 1;
		ExecLiuCheng(); // 执行流程
	}
#endif
}

// 免费队列
void GameProc::Free()
{
	if (!m_bSuccess)
		return;

	Sleep(500);
	// 获取建筑信息
	m_pGame->MakeTcpOP(OP_JIANZHU_JS);
	m_pGame->MakeTcpOP(OP_JIANZHU_ZY, 0, true);
	m_pGame->Send();

	printf("~~~~~~~~~~~~~~~免费队列成功！！！~~~~~~~~~~~~~~~~~~~~\n");
}

// 科技列表
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
		//printf("%d 类型：%02x 等级：%02x\n", i, m_pGame->m_KeJi[num].type&0xff, m_pGame->m_KeJi[num].level & 0xff);

		//printf("%d %08x\n", i, ctoi32(&m_cData[i]));
		i += 8;
		i += ctoi32(&m_cData[i]); // 描述
		i += 4;
		i += 28; // 所需资源数据
		i += 12; // 所需科研中心等级
	
		
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
	printf("科技列表数量：%d\n", num);
	SETISGET(IS_GET_KJ);
}

// 可招募列表
void GameProc::ZhaoMuList()
{
	if (!m_bSuccess)
		return;

	int id = ctoi32(&m_cData[157]);
	printf("军官ID:%08x\n", id);
}

// 查看物品 
void GameProc::ViewGoods()
{
	if (!m_bSuccess)
		return;

	printf("获得物品成功！！！\n");
	for (int i = 15; (i + 16) < m_iDataLength;) {
		int id = ctoi32(&m_cData[i]);
		int type = m_cData[i + 4];
		int num = ctoi32(&m_cData[i + 5]);
		printf("物品id: %08x type:%02x, num:%08x\n", id, type&0xff, num);
		i += 9;

		if (type == 0x02) { // 新手礼包ID
			if (id == 0xe2) {
				printf("使用签到小资源包...\n");
				m_pGame->UpdateAccountStatusText("使用签到小资源包.");
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

// 使用物品
void GameProc::UseGoods()
{
	if (!m_bSuccess)
		return;

	printf("使用物品成功！！！\n");
	// 查看物品
	m_pGame->m_iXinShouCode = 0;
	m_pGame->MakeTcpOP(OP_VIEWGOODS);
	m_pGame->Send();
}

// 查看当前主线任务
void GameProc::CurrentTask()
{
	if (m_bSuccess) {
		int task_id = ctoi32(&m_cData[5]);
		printf("任务id:%08x\n", task_id);
		m_pGame->MakeTcpOPInt(OP_VIEWTASK, task_id);
		m_pGame->Send();
	}
}

// 查看任务是否可完成
void GameProc::ViewTask()
{
	if (!m_bSuccess)
		return;

	int task_id = ctoi32(&m_cData[5]);
	for (int i = 9, n = 1; i < m_iDataLength; n++) {
		if (n == 6) {
			if (m_cData[i] == 0x01) {
				printf("可完成\n");
			}
			printf("任务id:%08x [%02x]\n", task_id, m_cData[i] & 0xff);
			break;
		}
		else {
			int length = ctoi32(&m_cData[i]);
			if (length > 1)
				i += length;

			printf("长度:%08x n:%d\n", length, n);
			i += 4;
		}
	}
}

// 训练
void GameProc::XunLian()
{
	m_pGame->UpdateAccountStatusText("训练成功！");
	m_pGame->SetAccountSuceess();
}

// 解散处理
void GameProc::JieSan()
{
	m_pGame->UpdateAccountStatusText("解散成功！");
	m_pGame->SetAccountSuceess();
}

// 慈善
void GameProc::CiShan()
{
	if (m_iDataLength < 10) {
		m_pGame->UpdateAccountStatusText("慈善成功！");
	}
	else {
		m_pGame->UpdateAccountStatusText("慈善失败，可能未冷却！");
	}
}

// 查看签到
void GameProc::ViewQianDao()
{
	if (!m_bSuccess)
		return;

	//m_pGame->MakeTcpOP(OP_QIANDAO, 2);
	//m_pGame->Send();
	//int oneDay = m_cData[357];
	//for (char i = 1; i <= 7; i++) {
		//printf("-----------------查看签到 一:%d----------------\n", i);
		//m_pGame->UpdateAccountStatusText("签到(第2天)");
		/*m_pGame->MakeTcpOP(OP_QIANDAO, 2);
		m_pGame->Send();
		Sleep(250);*/
	//}
	/*if (!oneDay) {
		printf("签到(第一天)...\n");
		m_pGame->UpdateAccountStatusText("签到(第一天)");
		m_pGame->MakeTcpOP(OP_QIANDAO, 1);
		m_pGame->Send();
	}
	printf("查看签到 一:%d============\n", oneDay);*/
}

// 签到
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
		printf("更新签到次数失败");
	}
	else {
		printf("更新签到次数成功");
	}
	printf("更签到次数sql：%s\n", user_info_sql);
}

//完成任务列表可完成的任务
void GameProc::RwList()
{
	if (!m_bSuccess)
		return;
	//任务总数量
	char task_number = ctoi32(&m_cData[TASK_NUMBER_INDEX]);
	//任务id
	int task_id = 0;
	//任务长度
	char task_str_num = 0;
	//跳过多少字节
	int skip_num = 0;
	//当前下标
	int current_index = TASK_INFO_INDEX;
	//当前第几个任务
	char current_task_num = 0;
	printf("length长：%d\n", m_iDataLength);
	for (int i = TASK_INFO_INDEX; i < m_iDataLength; i += skip_num, skip_num = 0) {
		current_task_num++;
		printf("第%d个任务\n", current_task_num);
		printf("总任务%d\n", task_number);
		if (current_task_num > task_number) {
			break;
		}

		task_id = ctoi32(&m_cData[current_index]);
		printf("任务id%08x\n", task_id);
		skip_num += 4;
		current_index += 4;

		task_str_num = ctoi32(&m_cData[current_index]);
		skip_num += 4;
		current_index += 4;

		//任务状体
		skip_num += task_str_num;
		current_index += task_str_num;
		if (&m_cData[current_index]) {
			//可以完成任务
			printf("可以完成任务\n");
			m_pGame->MakeTcpOPInt(OP_CPTTASK, task_id);
			m_pGame->Send();
			Sleep(500);
		}
		else {
			printf("no\n");
		}

		//跳过未知字符000000000000
		skip_num += 7;
		current_index += 7;
	}
	printf("\n-----------------------------\n");
	//printf("%s")

	//int task_id = ctoi32(&m_cData[5]);
}

//获取城市基本信息
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
		printf("城市基本信息更新成功%s.\n", m_pGame->m_pAccount->Mail);
	}
	else {
		printf("城市基本信息更新失败：%s\n", sql);
	}
}

// 读取升级流程
bool GameProc::ReadLiuCheng()
{
	OpenTextFile file;
	if (!file.Open("流程.txt")) {
		printf("找不到流程.txt文件\n");
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
		memcpy(p->cmd, data, length + 1); // 保存原字符命令
	}
	//printf("流程数量：%d\n", m_Link.Count());
	return true;
}

// 执行队列
bool GameProc::ExecDuiLie()
{
	if (m_DuiLie.Count() == 0) {
		if (m_iLastNoWaitCount == 0) {
			EmptyDuiLieText(); // 清空队列文字
			CopyToDuiLieText("无");
		}
		m_pGame->UpdateAccountDuiLieText(m_cDuiLie);
		return false;
	}

	NODE<DuiLie*>* p = m_DuiLie.Current();
	if (m_DuiLie.IsHead()) { // 是链表头
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
	
	//printf("正在执行流程：%s 不等待数量：%d\n", p->value->lc->cmd, m_iNoWaitCount);

	if (IsCompleteLiuCheng(p->value->lc)) { // 队列已经完成
		CopyToDuiLieText(" 已完成");
		if (m_DuiLie.Remove(p->value, 1)) { // 移除出队列
			printf("已完成，移除队列...(%d)\n", m_DuiLie.Count());
			p = m_DuiLie.Current();         // 移除后指针会自动往下移
		}
		else {
			printf("已完成，移除队列失败...\n");
			p = m_DuiLie.Next();
		}
	}
	else {
		bool exec = true;
		LiuCheng* lc = p->value->lc;   // 流程
		if (lc->op == OP_CPTTASK) {    // 流程为完成任务
			exec = p->prev == nullptr; // 要求前置流程已完成
		}
		bool wait = false;
		if (exec) {
			wait = ExecLiuCheng(lc);
			if (!wait) {   // 执行此队列
				m_iNoWaitCount++;
			}
		}
		else {
			m_iNoWaitCount++;
		}

		printf("不等待数量：%d exec:%d wait:%d %s\n", m_iNoWaitCount, exec, wait, p->value->lc->cmd);
		p = m_DuiLie.Next();
	}

	if (p) { // 有下一个队列
		CopyToDuiLieText(" | ");
	}
	else {   // 最后一个队列
		m_DuiLie.Reset();
		m_pGame->UpdateAccountDuiLieText(m_cDuiLie);
		EmptyDuiLieText(); // 清空队列文字
	}

	return p != nullptr;
}

// 检查队列是否等待
bool GameProc::CheckWaitDuiLie(LiuCheng* p)
{
	__int64 free_code = 0; // 免费队列代码
	__int64 timeleft  = 0; // 队列完成倒计时
	__int64 freeleft  = 0; // 队列完成免费倒计时
	__int64 now_time  = time(nullptr);
	__int32 is_free = 0;
	__int32 *p_free = nullptr;
	if (p->op == OP_JIANZAO || p->op == OP_SHENGJI) { // 建造或升级
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
	if (timeleft > 0) { // 倒计时
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
	if (freeleft < m_pGame->m_stConfig.WaitSecond) { // 最大等待时间
		printf("isfree:%d\n", is_free);
		if (freeleft < 0 && !is_free) { // 可以免费
			if (p_free) {
				*p_free = 1;
			}
			m_pGame->MakeTcpOPInt64(OP_FREE, free_code);
			if (m_pGame->Send()) {
				*p_free = 0;
			}

			if (1 || !p->noconf) {
				CopyToDuiLieText(" 免费");
			}
			printf("~~~~~~~~~~~免费队列！！！~~~~~~~~~~~~~~~\n");
		}
		return true;
	}
	return false;
}

// 执行流程
bool GameProc::ExecLiuCheng(LiuCheng* p)
{
	if (p == nullptr || p->complete) { // 已完成或不存在
		printf("流程已完成：%s\n", p ? p->cmd : "无");
		return true;
	}

	if (p->jz && p->jz->exec) { // 已经在执行
		char str[16];
		sprintf_s(str, "到%d", p->jz->level+1);
		CopyToDuiLieText(str);
		return CheckWaitDuiLie(p);
	}

	printf("准备执行流程：%s[%08x]\n", p->cmd, p->op);
	bool result = true;
	if (p->op == OP_JIANZAO) { // 建造
		if (!p->jz) {
			int area = m_pGame->GetJianZhuFreeNoByType((JZ_TYPE)p->value);
			if (area > -1) {
				printf("空闲位置号:%02x\n", area&0xff);
				m_pGame->MakeHouse(p->value, area);
				if (m_pGame->Send()) {
					m_pGame->MakeTcpOP(OP_VIEWRES);
					m_pGame->Send();
				}
			}
			else {
				CopyToDuiLieText(" 没有位置");
				printf("没有空闲位置建造...\n");

				result = false;
				m_pGame->m_pAccount->CptTime = time(nullptr) + 600;
			}
		}
	}
	else if (p->op == OP_SHENGJI) {
		// 获得符合要求建筑
		if (!p->jz) {
			p->jz = m_pGame->GetJianZhuByType((JZ_TYPE)p->value, p->extra[1] == YX_MAX ? 1 : -1);
		}
		
		if (p->jz) {
			if (p->jz->ready) {
				if (m_pGame->CheckRes(p->jz->need)) { // 资源足够，升级
					CopyToDuiLieText(" 资源足够");
					printf("资源足够，升级！！！\n");
					m_pGame->MakeTcpOPInt64(OP_SHENGJI, p->jz->code);
					if (m_pGame->Send()) {
						for (int i = 0; i < p->jiasunum; i++) {
							for (int j = 0; j < p->jiasu[i].num; j++) {
								if (p->jiasu[i].type) {
									Sleep(500);
									m_pGame->MakeJiaSu(p->jz->code, p->jiasu[i].type);
									m_pGame->Send();
									printf("使用加速：%d\n", p->jiasu[i].type);
								}
							}
						}
						SendTheData();
					}
				}
				else {
					CopyToDuiLieText(" 资源不足");
					printf("资源不足...\n");

					result = false;
					m_pGame->m_pAccount->CptTime = time(nullptr) + 600;
				}
			}
			else { // 查询升级所需条件
				printf("发送升级所需，位置:%d %08x %d\n", p->jz->no, p->jz->code, p->jz->exec);
				CopyToDuiLieText(" 查询所需");
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
			CopyToDuiLieText(" 不符合要求");
			printf("找不到符合要求建筑...%d %d\n", p->value, p->extra[1]);
			result = false;
		}
	}
	else if (p->op == OP_YANJIU) { // 研究科技
		printf("研究科技...\n");
		m_pGame->MakeYanJiu((KJ_TYPE)p->value);
		m_pGame->Send();
		p->complete = true;
	}
	else if (p->op == OP_XUNLIAN) {
		printf("训练...\n");
		JianZhu* jz = m_pGame->GetJianZhuByType(JZ_JUNGONG);
		if (jz) { // 军工厂建筑代码
			m_pGame->MakeXunLian(jz->code);
			m_pGame->Send();
		}
		p->complete = true;
	}
	else if (p->op == OP_USEGOODS) { // 使用物品
		printf("使用物品...\n");
		m_pGame->MakeTcpOPMid(OP_USEGOODS, p->extra[0], p->value);
		m_pGame->Send();
		p->complete = true;
	}
	else if (p->op == OP_CPTTASK) {
		printf("完成任务...\n");
		m_pGame->MakeTcpOPInt(OP_CPTTASK, p->value);
		m_pGame->Send();
		p->complete = true;
	}
	else if (p->op == OP_YIMIN) {
		printf("移民条例...\n");
		m_pGame->MakeTcpOPInt(OP_YIMIN, p->value);
		m_pGame->Send();
		p->complete = true;
	}

	return result;
}

// 计算已完成流程
int GameProc::CalcCompleteLiuCheng()
{
	if (!ISGETALL()) {
		printf("!ISGETALL %08x %08x\n", m_iIsGetValue, GETALLVAL());
		return 0;
	}

	m_iLastExecCount = m_pGame->GetJianZhuExecNum(m_iLastNoWaitCount);
	if (m_iLastExecCount > 0) {
		printf("正在执行建筑数量：%d 不等待数量：%d\n", m_iLastExecCount, m_iLastNoWaitCount);
	}
	if (m_iLastExecCount >= m_iDuiLieMax) {
		m_pGame->UpdateAccountStatusText("执行队列...");
		m_pGame->UpdateAccountDuiLieText(m_cDuiLie);
		EmptyDuiLieText();
		return 2;
	}
	int free_count = m_iDuiLieMax - m_iLastExecCount;
		
	if (m_DuiLie.Count() >= m_iDuiLieMax) {
		printf("队列已满:%d>=%d\n", m_DuiLie.Count(), free_count);
		return 1;
	}
	
	//printf("开始执行流程\n");
	bool last_complete = false;
	int n = 1;
	LiuCheng* p;
	bool flag = true;
	for (p = GetCurrentLiuCheng(); p; p = GetNextLiuCheng(), n++) {
		if (m_DuiLie.Count() >= free_count) {
			printf("队列已满[for]:%d>=%d\n", m_DuiLie.Count(), free_count);
			break;
		}
			
		bool complete = flag ? IsCompleteLiuCheng(p) : false;
		printf("%d、流程:%d OP:%08x V:%08x | %s\n", n, complete, p->op, p->value, p->cmd);
		if (!complete) { // 没有完成的添加到执行队列
			DuiLie* dl = new DuiLie;
			dl->lc = p;
			m_DuiLie.Add(dl);
			printf("加入队列!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%s(%d)\n", p->cmd, m_DuiLie.Count());
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

// 获取当前流程
LiuCheng* GameProc::GetCurrentLiuCheng()
{
	NODE<LiuCheng*>* p = m_Link.Current();
	return p ? p->value : nullptr;
}

// 获取下一个执行流程
LiuCheng* GameProc::GetNextLiuCheng()
{
	NODE<LiuCheng*>* p = m_Link.Next();
	return p ? p->value : nullptr;
}

// 完成当前流程
LiuCheng* GameProc::CompleteLiuCheng()
{
	LiuCheng* p = GetCurrentLiuCheng();
	if (p) {
		p->complete = true;
	}
	return GetNextLiuCheng();
}

// 流程指令转成游戏指令
LiuCheng* GameProc::TransFormOP(Explode& lcLine)
{
	LiuCheng* pLiuCheng = new LiuCheng;
	memset(pLiuCheng, 0, sizeof(LiuCheng));
	pLiuCheng->op = OP_UNKNOW;
	pLiuCheng->byte = 4;
	pLiuCheng->jiasunum = 0;

	char* op = lcLine.GetValue(0);
	char* v = lcLine.GetValue(1);

	if (strstr(op, "建造")) {
		pLiuCheng->op = OP_JIANZAO;
		pLiuCheng->value = TransFormRes(v);
		pLiuCheng->extra[0] = hextoi32(lcLine.GetValue(2));    // 建造数量

		TransFormJiaSu(op, pLiuCheng);
	}
	else if (strstr(op, "升级")) {
		pLiuCheng->op = OP_SHENGJI;
		pLiuCheng->value = TransFormRes(v);
		pLiuCheng->extra[0] = hextoi32(lcLine.GetValue(2));     // 等级
		pLiuCheng->extra[1] = TransFormRes(lcLine.GetValue(3)); // 优先升级类型

		TransFormJiaSu(op, pLiuCheng);
	}
	else if (strstr(op, "训练")) {
		pLiuCheng->op = OP_XUNLIAN;
	}
	else if (strstr(op, "研究")) {
		pLiuCheng->op = OP_YANJIU;
		pLiuCheng->value = TransFormRes(v);
		pLiuCheng->extra[0] = hextoi32(lcLine.GetValue(2));
	}
	else if (strstr(op, "完成任务")) {
		pLiuCheng->op = OP_CPTTASK;
		pLiuCheng->value = hextoi32(v);
	}
	else if (strstr(op, "使用物品")) {
		pLiuCheng->op = OP_USEGOODS;
		pLiuCheng->value = TransFormRes(v);
		pLiuCheng->extra[0] = 1;
	}
	else if (strstr(op, "移民条例")) {
		pLiuCheng->op = OP_YIMIN;
		pLiuCheng->value = 0x83;
	}
	//printf("->%s ->%08x ->%08x ->%08x\n", lcLine.GetValue(0), pLiuCheng->op, pLiuCheng->value, pLiuCheng->extra[0]);

	if (pLiuCheng->op != OP_UNKNOW) {
		m_Link.Add(pLiuCheng);
	}

	return pLiuCheng;
}

// 流程是否可完成
bool GameProc::IsCompleteLiuCheng(LiuCheng* p)
{
	if (p->complete)
		return true;

	bool complete = false;
	if (p->op == OP_JIANZAO) {
		// 获得此建筑数量
		p->jz = m_pGame->GetJianZhuExecByType((JZ_TYPE)p->value);
		if (p->jz && p->jz->level == 0) {
			printf("有正在建造建筑...\n");
			complete = false;
		}
		else {
			p->jz = nullptr;
			int num = m_pGame->GetJianZhuNumByType((JZ_TYPE)p->value);
			complete = num >= p->extra[0];
		}
	}
	else if (p->op == OP_SHENGJI) {
		// 获得正在执行建筑
		p->jz = m_pGame->GetJianZhuExecByType((JZ_TYPE)p->value);
		if (p->jz && p->extra[1] != YX_MAX) {
			printf("有正在升级建筑 类型:%d 代码:%08x...\n", p->jz->type, p->jz->code);
			complete = p->jz->level >= p->extra[0];
			/*if (p->jz->level >= p->extra[0]) {
				complete = true;
				printf("升级完成...\n");
			}*/
		}
		else {
			// 获得符合要求建筑
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
			printf("新手礼包：%d != %d\n", m_pGame->m_iXinShouCode, p->value);
		}
	}
	else {
		complete = m_bFirstCalc;
	}
	p->complete = complete;

	return complete;
}

// 流程文字转成资源ID(建筑，科技名称代码)
int GameProc::TransFormRes(const char* data)
{
	if (data == nullptr)
		return DATA_UKNOW;

	// 以下为建筑类型
	if (strcmp(data, "市政厅") == 0)
		return JZ_SHIZHENG;
	if (strcmp(data, "民居") == 0)
		return JZ_MINJU;
	if (strcmp(data, "农田") == 0)
		return JZ_NONGTIAN;
	if (strcmp(data, "炼钢厂") == 0)
		return JZ_LIANGANG;
	if (strcmp(data, "石油基地") == 0)
		return JZ_SHIYOU;
	if (strcmp(data, "稀矿厂") == 0)
		return JZ_XIKUANG;
	if (strcmp(data, "围墙") == 0)
		return JZ_WEIQIANG;
	if (strcmp(data, "科研中心") == 0)
		return JZ_KEYAN;
	if (strcmp(data, "军校") == 0)
		return JZ_JUNXIAO;
	if (strcmp(data, "参谋部") == 0)
		return JZ_CANMOUBU;
	if (strcmp(data, "--") == 0)
		return JZ_UNKONOW2;
	if (strcmp(data, "仓库") == 0)
		return JZ_CANGKU;
	if (strcmp(data, "司令部") == 0)
		return JZ_SILINGBU;
	if (strcmp(data, "军工厂") == 0)
		return JZ_JUNGONG;
	if (strcmp(data, "联络中心") == 0)
		return JZ_LIANLUO;
	if (strcmp(data, "轻工厂") == 0)
		return JZ_QINGCHANG;

	// 以下为科技类型
	if (strcmp(data, "粮食生产") == 0)
		return KJ_LIANGSHI;
	if (strcmp(data, "钢铁生产") == 0)
		return KJ_GANGTIE;
	if (strcmp(data, "石油生产") == 0)
		return KJ_SHIYOU;
	if (strcmp(data, "稀矿生产") == 0)
		return KJ_XIKUANG;

	// 以下为礼包类型
	if (strcmp(data, "新手礼包1") == 0)
		return LB_XINSHOU_1;
	if (strcmp(data, "新手礼包2") == 0)
		return LB_XINSHOU_2;
	if (strcmp(data, "新手礼包3") == 0)
		return LB_XINSHOU_3;
	if (strcmp(data, "新手礼包4") == 0)
		return LB_XINSHOU_4;
	if (strcmp(data, "新手礼包5") == 0)
		return LB_XINSHOU_5;
	if (strcmp(data, "新手礼包6") == 0)
		return LB_XINSHOU_6;
	if (strcmp(data, "新手礼包7") == 0)
		return LB_XINSHOU_7;

	// 以下为优先类型
	if (strcmp(data, "全部") == 0)
		return YX_ALL;
	if (strcmp(data, "最大") == 0)
		return YX_MAX;
	if (strcmp(data, "最小") == 0)
		return YX_MIN;

	return DATA_UKNOW;
}

// 转成加速
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
		if (strcmp("15分钟", data[0]) == 0) {
			p->jiasu[i].type = 1;
		}
		else if (strcmp("1小时", data[0]) == 0) {
			p->jiasu[i].type = 2;
		}
		else if (strcmp("5小时", data[0]) == 0) {
			p->jiasu[i].type = 3;
		}
		p->jiasu[i].num = data.GetValue2Int(1); // 数量

		printf("流程加速:%d[%d]\n", p->jiasu[i].type, p->jiasu[i].num);
	}
	p->jiasunum = arr.GetCount();

	return p->jiasunum;
}

// 建筑类型转成文字
void GameProc::TransToTextJZ(JZ_TYPE type, char text[])
{
	if (type == JZ_SHIZHENG) {
		strcpy(text, "市政厅");
		return;
	}
	if (type == JZ_MINJU) {
		strcpy(text, "民居");
		return;
	}
	if (type == JZ_NONGTIAN) {
		strcpy(text, "农田");
		return;
	}
	if (type == JZ_LIANGANG) {
		strcpy(text, "炼钢厂");
		return;
	}
	if (type == JZ_SHIYOU) {
		strcpy(text, "石油基地");
		return;
	}
	if (type == JZ_XIKUANG) {
		strcpy(text, "稀矿石");
		return;
	}
	if (type == JZ_WEIQIANG) {
		strcpy(text, "围墙");
		return;
	}
	if (type == JZ_KEYAN) {
		strcpy(text, "科研中心");
		return;
	}
	if (type == JZ_JUNXIAO) {
		strcpy(text, "军校");
		return;
	}
	if (type == JZ_CANMOUBU) {
		strcpy(text, "参谋部");
		return;
	}
	if (type == JZ_UNKONOW2) {
		strcpy(text, "未知");
		return;
	}
	if (type == JZ_CANGKU) {
		strcpy(text, "仓库");
		return;
	}
	if (type == JZ_SILINGBU) {
		strcpy(text, "司令部");
		return;
	}
	if (type == JZ_JUNGONG) {
		strcpy(text, "军工厂");
		return;
	}
	if (type == JZ_LIANLUO) {
		strcpy(text, "联络中心");
		return;
	}
	if (type == JZ_QINGCHANG) {
		strcpy(text, "轻工厂");
		return;
	}
}

// 建筑是否可忽略检查
bool GameProc::CanIgnoreCk(struct_jianzhu* jz)
{
	m_DuiLie.Store();

	m_DuiLie.Reset();
	for (NODE<DuiLie*>* p = m_DuiLie.Current(); p; p = m_DuiLie.Next()) {
		if (p->value && p->value->lc->jz) { // 此建筑已在队列
			if (p->value->lc->jz->code == jz->code)
				return true;
		}
	}

	m_DuiLie.ReStore();
	return false;
}

// 复制到队列文字描述
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

// 清空队列文字内容
char* GameProc::EmptyDuiLieText()
{
	if (m_cDuiLie)
		m_cDuiLie[0] = 0;

	return m_cDuiLie;
}

// 是否锁定
bool GameProc::IsLocked()
{
	return m_bLock;
}

// 锁定
void GameProc::Lock()
{
	m_bLock = true;
}

// 解锁
void GameProc::UnLock()
{
	m_bLock = false;
}

// 是否关闭游戏
bool GameProc::IsCloseGame()
{
	return (m_iLastNoWaitCount + m_iNoWaitCount) >= m_iDuiLieMax;
}
