#include "Game.h"
#include "GameAccount.h"
#include "GameProc.h"
#include "../Encrypt/Des.h"
#include "../Common/Json.h"
#include "../Common/Explode.h"
#include "../Common/mystring.h"
#include "../Common/OpenTextFile.h"
#include "../Common/Preg.h"
#include "../stdafx.h"
#include "../ErZhanFengYunDlg.h"
#include <string.h>

#define SHENGWANG_MAX  10000

#define GAME_DES_KEY   "2644894d08a9c3b784993006ea980cfd"

#define GAME_HOST      L"sso.wistone.com"
#define GAME_LOGINPATH L"/wistoneSSO/mlogin"
#define GAME_REGISTNPATH L"/wistoneSSO/service/registerAccount?source_area=China&version=1.0.3&"
#define GAME_APPID     L"4244c1323c61df864413ee47654cbaec"
#define GAME_APPSECRET L"a4ab2ae6030541212ecb6773b505471d833b8c75e5756de0"
#define GAME_LANG      L"zh_CN"
#define GAME_PALTFORM  L""
#define GAME_REGIST_ADD  L"add"
#define GAME_DEVICE    L"oppo a33"
#define GAME_SERVICE   L"http://sso.wistone.com/wistoneSSO/mlogin"

#define GAME_SER_IP    "106.75.2.182"
#define GAME_SER_PORT  8083
#define GAME_SER_IP2   "106.75.17.197"
#define GAME_SER_PORT2 8082

Game* Game::self = nullptr;

Game::Game(GameAccount* pGameAccount)
{
	m_pGameAccount = pGameAccount;
}

Game::~Game()
{
	if (m_pGameProc) {
		//delete m_pGameProc;
		//m_pGameProc = nullptr;
	}
	printf("~Game！！！\n");
}

/* 初始化数据 */
void Game::InitData()
{
	m_qwLoginTime = 0;

	m_pGameProc = new GameProc(this);
	m_pGameProc->ReadLiuCheng();

	start = false;
	self = nullptr;
	memset(m_ServerHost, 0, sizeof(m_ServerHost));
	memset(m_ServerId, 0, sizeof(m_ServerId));
	m_ServerPort = 0;
	memset(m_SendStr, 0, sizeof(m_SendStr));
	m_SendLength = 0;
	m_SendTogeth = false;

	memset(&m_ZiYuan, 0, sizeof(m_ZiYuan));
	memset(m_jsJianZhu, 0, sizeof(m_jsJianZhu));
	memset(m_zyJianZhu, 0, sizeof(m_zyJianZhu));
	memset(m_KeJi, 0, sizeof(m_KeJi));
}

/* 回收数据 */
void Game::ReleaseData()
{
	Game::~Game();
}

// 1222221fff1@qq.com 123456
// 6c86ddf217be055983d1391343dd2b85ad7d23483146340e 717b2aaa6d933836

int Game::GetConfAccount()
{
	return 0;
}

int Game::GetAccoutStatus()
{
	return m_pAccount->Status;
}

void Game::Start()
{
	printf("开始！！！\n");

	while (true) {
		time_t start_time = time(NULL);

		//printf("m_pAccount:%016x\n", m_pAccount);
		m_pAccount = m_pGameAccount->GetPlayAccount();
		//printf("m_pAccount:%016x\n", m_pAccount);

		if (!m_pAccount) {
			//printf("没有符合要求帐号！！！\n");
			Sleep(2500);
			continue;
		}

		InitData();
		m_pAccount->Status = STATUS_ING;

		// 重绘账号列表控件
		m_pGameAccount->ListReDraw();
		// 更新文字控件信息
		//m_pGameAccount->SetText(IDC_STATIC_ACCOUT_NUM, m_iAccoutCurrentNum + 1, m_iAccountNum);

		CString account = m_pAccount->Mail;
		CString text;
		text.Format(L"(%d)正在处理帐号：%ws", m_pAccount->index + 1, account);
		m_pGameAccount->InsertLog(text.GetBuffer());
		m_pGameAccount->UpdateStatusText(m_pAccount, "正在登录...");
		
		// 登录
		if (!Login(m_pAccount->Mail, m_pAccount->Password)) {
			m_pGameAccount->InsertLog(L"登录失败，请检查帐号或密码是否正确！");
			m_pGameAccount->UpdateStatusText(m_pAccount, "登录失败.");
			m_pAccount->Status = STATUS_LOGIN_FAILD;
			m_iFailNum++;
			m_pGameAccount->SetText(IDC_STATIC_FAILED, m_iFailNum, -1);
			goto end;
		}
		m_pGameAccount->UpdateStatusText(m_pAccount, "连接服务器...");
		m_bIsInit = true;
		m_qwLoginTime = time(nullptr);
		// 连接游戏服务器
		Connect();
	end:
		m_bIsInit = false;
		ReleaseData();

		printf("帐号:%s 处理完成！！！\n", m_pAccount->Mail);
		// 解锁帐号使用
		m_pGameAccount->UnUsed(m_pAccount);
		// 重绘账号列表控件
		m_pGameAccount->ListReDraw();

		CString csHTime;
		csHTime.Format(L"(%d)处理完成帐号：%ws，用时：%d秒", m_pAccount->index + 1, account, time(NULL) - start_time);
		m_pGameAccount->InsertLog(csHTime.GetBuffer());

		if (m_stConfig.AccoutInterval > 0) {
			Sleep(m_stConfig.AccoutInterval);
		}
		//break;

		Sleep(2500000);
	}
}

/*
注册[http]
big_area 哪个大区
user_num 生成账号数量
default_account 初始化账号
str_wid_value wid值
*/
bool Game::Regist(int big_area, int user_num, char* default_account, char* str_wid_value)
{
	if (!big_area) {
		MessageBox(NULL, L"大区不能为空", L"注册", MB_OK);
		return 0;
	}
	if (!user_num) {
		MessageBox(NULL, L"账号数量不能为空", L"注册", MB_OK);
		return 0;
	}
	if (!default_account[0]) {
		MessageBox(NULL, L"账号不能为空", L"注册", MB_OK);
		return 0;
	}
	
	if (!str_wid_value[0]) {
		MessageBox(NULL, L"wid不能为空", L"注册", MB_OK);
		return 0;
	}

	char desMail[128], desPassword[128], des_secret_key[128];
	char password[] = "135246";

	char str_len = sizeof(default_account);
	char start_index = 2;
	const char end_index = 7;

	char temp_str[end_index];
	char temp_str_index;
	long long wid_increasing = _atoi64(str_wid_value);
	Json json;

	char sql[256];//注册sql语句
	
	mysql_init(&m_mysqlConnect);//分配对象 p4302
	if (!(mysql_real_connect(&m_mysqlConnect, "localhost", "root", "root", "erzhan", 3306, NULL, 0))) {
		printf("Failed to access to the database...Error: %s\n", mysql_error(&m_mysqlConnect));
		return 0;
	}
	if (!mysql_set_character_set(&m_mysqlConnect, "utf-8")) {
		printf("New client character set: %s\n", mysql_character_set_name(&m_mysqlConnect));
		return 0;
	}
	printf("MySQL连接成功.\n");

	for (int account_num = 0; account_num < user_num; account_num++) {

		temp_str_index = 0;
		wid_increasing++;
		sprintf(str_wid_value, "%lld", wid_increasing);
		for (char i = 0; i < str_len; i++) {
			if (i >= start_index && i <= end_index) {
				temp_str[temp_str_index] = default_account[i];
				temp_str_index++;
			}
		}
		temp_str[temp_str_index] = 0;
		temp_str_index = 0;

		long long int_temp_str = _atoi64(temp_str);
		int_temp_str++;
		sprintf_s(temp_str, "%lld", int_temp_str);
		for (char j = start_index; j <= end_index; j++) {
			default_account[j] = temp_str[j - start_index];
		}


		DesEncrypt(desMail, GAME_DES_KEY, default_account, strlen(default_account));
		DesEncrypt(desPassword, GAME_DES_KEY, password, strlen(password));
		DesEncrypt(des_secret_key, GAME_DES_KEY, str_wid_value, strlen(str_wid_value));

		//printf("账号：%s\n", default_account);
		printf("wid值：%s\n", str_wid_value);

		CString result;
		CString loginPath = GAME_REGISTNPATH;

		__int64 now_time = time(nullptr) * 1000;
		char time_str[16];
		sprintf_s(time_str, "%lld", now_time);
		HttpClient http;

		http.AddParam(L"WID", des_secret_key);
		http.AddParam(L"email", desMail);
		http.AddParam(L"password", desPassword);
		http.AddParam(L"re_password", desPassword);
		http.AddParam(L"username", default_account);
		http.AddParam(L"appid", GAME_APPID);
		http.AddParam(L"app_secret", GAME_APPSECRET);
		http.AddParam(L"lang", GAME_LANG);
		http.AddParam(L"paltform", GAME_REGIST_ADD);
		http.AddParam(L"device_mode", GAME_PALTFORM);
		http.AddParam(L"cachetime", time_str);
		http.Request(L"sso.wistone.com", loginPath.GetBuffer(), result);
		json.parseString((char*)result.GetBuffer());

		if (json.valueToInt("resultType") == 1) {
			sprintf(sql, "insert into `cmf_account`(`user_login`,`user_pass`,`create_time`,`wid`,`region`) VALUES ('%s','%s','%d','%s','%d')",
			 default_account, password, time(nullptr), str_wid_value, big_area);
			int res = mysql_query(&m_mysqlConnect, sql);
			if (res == 0) {
				printf("插入成功%d.\n", account_num);
			}
			else {
				printf("插入失败：%s\n", sql);
			}
		}
		else {
			printf("注册账号shibai返回值：%s\n", Utf82GB2312((const char*)result.GetBuffer()));
		}
		Sleep(500);
	}
	mysql_close(&m_mysqlConnect);
	//20110610009562754
	return 0;
}

/*
	MYSQL mysqlConnect;  //数据源指针
	mysql_init(&mysqlConnect);//分配对象 p4302
	if (!(mysql_real_connect(&mysqlConnect, "localhost", "root", "root", "erzhan", 3306, NULL, 0))) {
		printf("Failed to access to the database...Error: %s\n", mysql_error(&mysqlConnect));
	}
	if (!mysql_set_character_set(&mysqlConnect, "gbk")) {
		printf("New client character set: %s\n", mysql_character_set_name(&mysqlConnect));
	}
	printf("MySQL连接成功.\n");

	char sql[] = "insert into `cmf_test`(`name`) VALUES ('阿豪2')";
	int res = mysql_query(&mysqlConnect, sql);
	if (res == 0) {
		printf("插入成功.\n");
	}
	char sql2[] = "SELECT * FROM `cmf_test`";
	res = mysql_query(&mysqlConnect, sql2);
	//char sql2[] = "SELECT LAST_INSERT_ID()";
	//res = mysql_query(&mysqlConnect, sql2);
	char sq3[100];
	if (res == 0) {
		MYSQL_RES* result = mysql_store_result(&mysqlConnect);
		if (result) {
			int num_fields = mysql_num_fields(result);
			MYSQL_ROW row;
			while (row = mysql_fetch_row(result)) {
				for (int i = 0; i < num_fields; i++) {
					printf("%s ", row[i]);
				}
				sprintf(sq3, "UPDATE cmf_test SET name='阿灿%s' WHERE id=%s", row[0], row[0]);
				mysql_query(&mysqlConnect, sq3);
				printf("\n");
			}
		}
	}

	return false;
	*/

/*
解密wid 
str_wid_value 需要解密字符串
jm_wid_value  解密后字符串
*/
bool Game::DecryptWid(char* str_wid_value, char* jm_wid_value)
{
	DesDecrypt(jm_wid_value, GAME_DES_KEY, str_wid_value, strlen(str_wid_value), true);
	printf("%s\n", jm_wid_value);
	return false;
}

// 登录[http]
bool Game::Login(char* mail, char* password)
{
	char desMail[128], desPassword[128];
	DesEncrypt(desMail, GAME_DES_KEY, mail, strlen(mail));
	DesEncrypt(desPassword, GAME_DES_KEY, password, strlen(password));
	//printf("mail:%s\n", desMail);
	//printf("password:%s\n", desPassword);

	CString result;
	CString loginPath = GAME_LOGINPATH;
	loginPath += L"?source_area=China&version=1.0.3";

	HttpClient http;
	http.AddParam(L"appid", GAME_APPID);
	http.AddParam(L"app_secret", GAME_APPSECRET);
	http.AddParam(L"login_type", L"0");
	http.AddParam(L"lang", GAME_LANG);
	http.AddParam(L"paltform", GAME_PALTFORM);
	http.AddParam(L"device_mode", GAME_DEVICE);
	http.AddParam(L"android_id", L"1ccb1456441fe328");
	http.AddParam(L"imei", L"191316920188881");
	http.AddParam(L"sim_serial_number", L"39183219036381574113");
	http.AddParam(L"cachetime", L"0");
	http.AddParam(L"service", GAME_SERVICE);
	http.AddParam(L"id_type", L"0");
	http.Request(L"sso.wistone.com", loginPath.GetBuffer(), result);

	Json json;
	json.parseString((char*)result.GetBuffer());
	//printf("antiAddiction:%s\nloginTicket:%s\nsessionId:%s\n",
	//	json.value("antiAddiction"), json.value("loginTicket"), json.value("sessionId"));
	//return false;
	loginPath = GAME_LOGINPATH;
	loginPath += L";jsession=";
	loginPath += json.value("sessionId");
	loginPath += "?source_area=China&version=1.0.3";

	http.ResetParam();
	http.SetCookie(json.value("sessionId"));
	http.AddParam(L"WID", L"b01fbfd1df71fbecd7548c8d6e6187a78a201d395328bfa1");
	http.AddParam(L"email", desMail);
	http.AddParam(L"password", desPassword);
	http.AddParam(L"sp_userid", L"");
	http.AddParam(L"sp_type", L"");
	http.AddParam(L"l_t", json.value("loginTicket"));
	http.AddParam(L"execution", json.value("flowExecutionKey"));
	http.AddParam(L"_eventId", L"loginSubmit");
	http.AddParam(L"appid", GAME_APPID);
	http.AddParam(L"app_secret", GAME_APPSECRET);
	http.AddParam(L"login_type", L"1");
	http.AddParam(L"lang", GAME_LANG);
	http.AddParam(L"paltform", GAME_PALTFORM);
	http.AddParam(L"device_mode", GAME_DEVICE);
	http.AddParam(L"cachetime", L"0");
	http.AddParam(L"service", GAME_SERVICE);
	http.AddParam(L"id_type", L"0");
	http.Request(L"sso.wistone.com", loginPath.GetBuffer(), result);

	json.clear();

	json.parseString((char*)result.GetBuffer());
	char* wst = json.value("WST");
	if (!wst) {
		return false;
	}
	//printf("WST:%s\n", json.value("WST"));
	int wst_length = strlen(wst); // strlen(json.value("WST"));
	memcpy(m_Wst, wst, wst_length + 1);
	SetWistoneId(json.value("wistoneId"));
	SetUid();
	//printf("uid:%s (%d)\n", m_Uid, strlen(m_Uid)); // 19011310008890077
	json.clear();


	return wst_length > 5;
}

void Game::Connect()
{
	memset(m_ServerHost, 0, sizeof(m_ServerHost));
	memset(m_ServerId, 0, sizeof(m_ServerId));
	m_ServerPort = 0;
	//return;
	UpdateAccountStatusText("获取游戏区服...");
	m_pClient = new Client(this);
	m_pClient->m_iTimeOut = m_stConfig.AccountTimeOut;
	m_pClient->onconnect_error = OnConnectError;
	m_pClient->onconect = OnConnect;
	m_pClient->onread = OnRead;
	m_pClient->onclose = OnClose;
	m_pClient->onerror = OnError;
	m_pClient->Connect(GAME_SER_IP2, GAME_SER_PORT2);
	delete m_pClient;
}

int Game::GetAccoutId(const char * data)
{
	return m_iAccountId = m_pClient->Intel2Length(&data[9]);
}

/*
设置帐号成功
*/
void Game::SetAccountSuceess()
{
	m_pAccount->Status = STATUS_SUCCESS;
	UpdateAccountStatusText("请求成功.");
	m_pGameAccount->ListReDraw();
	m_iSuccessNum++;
	//SetTextFunc(IDC_STATIC_SUC, m_iSuccessNum, -1);
	m_pClient->Close(true);
}
/*
设置帐号失败
*/
void Game::SetAccountFailed()
{
	m_pAccount->Status = STATUS_LOGIN_FAILD;
	UpdateAccountStatusText("请求失败.");
	m_pGameAccount->ListReDraw();
	m_iFailNum++;
	//SetTextFunc(IDC_STATIC_FAILED, m_iFailNum, -1);
	m_pClient->Close(true);
}

bool Game::GetTcpServerInfo(const char * data)
{
	if (m_pClient->Intel2Length(data) != 0x01) {
		return false;
	}

	int xLength = m_pClient->Intel2Length(&data[17]); // 长度

	memcpy(m_ServerId, &data[21], 3);

	bool isPort = false;
	int hostLength = m_pClient->Intel2Length(&data[21 + xLength]); // 区服数据长度
	printf("xLength:%08x hostLength:%08x\n", xLength, hostLength);
	const char* host = &data[21 + xLength + 4];
	for (int i = 0; i < hostLength; i++) {
		if (host[i] == ':') {
			isPort = true;
			continue;
		}
		if (isPort) {
			m_ServerPort = m_ServerPort * 10 + host[i] - '0';
		}
		else {
			m_ServerHost[i] = host[i];
		}
	}
	printf("服务器地址:%s:%d(%s)\n", m_ServerHost, m_ServerPort, m_ServerId);
	return true;
}

void Game::MakeTcpLogin()
{
	char data[] = {
		0x57, 0x49, 0x53, 0x54,
		0x5c, 0x03, 0x32, 0x09,
		0x00, 0x00, 0x00, 0x5b,
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x11, 
		0x31, 0x39, 0x30, 0x31, 0x30, 0x37, 0x31, 0x30, 0x30, 0x30, 0x38, 0x38, 0x38, 0x30, 0x38, 0x39, 0x32, 
		0x00, 0x00, 0x00, 0x03, 
		0x61, 0x64, 0x64, 0x00, 0x00, 0x00, 0x0e, 0x77, 0x73, 0x74, 0x5f, 0x61, 0x64, 0x64, 0x5f, 0x7a, 0x68,
		0x5f, 0x30, 0x30, 0x31, 0x00, 0x00, 0x00, 0x02, 0x7a, 0x68, 0x01,
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x0c, 
		0x4e, 0x65, 0x74, 0x65, 0x61, 0x73, 0x65, 0x20, 0x4d, 0x75, 0x4d, 0x75, 
		0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x06, 0x32, 0x2e, 0x31, 0x32, 0x2e, 0x30
	};
	memcpy(m_SendStr, data, 19); // 复制前27个字符

	int uidLength = strlen(m_Uid);
	m_SendStr[19] = uidLength; // 第27个字符串为UID长度
	memcpy(&m_SendStr[20], m_Uid, uidLength); // UID字符串
	memcpy(&m_SendStr[20 + uidLength], &data[20 + 17], sizeof(data) - (20 + 17));
	m_SendLength = sizeof(data) + uidLength - 17;
}

void Game::MakeTcpFirstStr()
{
	// 0-3固定"WIST", 4-7 WIST的标记
	// 11-后面字符长度 19-后面字符的长度
	// 20-36 "19010710008880892"
	printf("用户ID:%08X\n", m_iAccountId);
	/*
	byte data[] = {
		0x57, 0x49, 0x53, 0x54, 
		0x5c, 0x03, 0x32, 0x09, 
		0x00, 0x00, 0x00, 0xc3, 
		0x00, 0x00, 0x03, 0xe9, 
		0x00, 0x00, 0x00, 0x00, // 前20

		0x00, 0x00, 0x00, 0x00, // 用户ID
		//0x00, 0x9e, 0x3b, 0x1c,
		//0x00, 0x9e, 0x4a, 0xbc,
		//0x00, 0x9a, 0xaf, 0x28,
		0x00, 0x00, 0x00, 0x11, // UID长度[24-27]
		0x31, 0x39, 0x30, 0x31, 0x30, 0x37, 0x31, 0x30, 0x30, 0x30, 0x38, 0x38, 0x38, 0x30, 0x38, 0x39, 0x32, 
		// 19010710008880892↑ 19011310008890077 18090410008592005 [28-44]
		0x00, 0x00, 0x00, 0x02,
		0x00, 0x00, 0x00, 0x03, 
		0x61, 0x64, 0x64, // "add"
		0x00, 0x00, 0x00, 0x0e, 
		0x77, 0x73, 0x74, 0x5f, 0x61, 0x64, 0x64, 0x5f, 0x7a, 0x68, 0x5f, 0x30, 0x30, 0x31, // "wst_add_zh_001"
		0x00, 0x00, 0x00, 0x02, 
		0x7a, 0x68, // "zh"
		0x00, 0x00, 0x00, 0x20,
		0x63, 0x66, 0x33, 0x38, 0x62, 0x31, 0x38, 0x32, 0x61, 0x66, 0x39, 0x61, 0x34, 0x65, 0x30, 0x30,
		0x38, 0x39, 0x61, 0x36, 0x61, 0x37, 0x64, 0x38, 0x66, 0x33, 0x32, 0x65, 0x62, 0x64, 0x35, 0x66,
		// cf38b182af9a4e0089a6a7d8f32ebd5f↑
		0x00, 0x00, 0x00, 0x33,
		0x53, 0x54, 0x2d, 0x48, 0x77, 0x79, 0x32, 0x43, 0x31, 0x64, 0x63, 0x37, 0x46, 0x45, 0x6b, 0x67, 
		0x6d, 0x73, 0x69, 0x58, 0x57, 0x73, 0x62, 0x2d, 0x33, 0x35, 0x35, 0x36, 0x35, 0x30, 0x2d, 0x70, 
		0x61, 0x73, 0x73, 0x70, 0x6f, 0x72, 0x74, 0x2e ,0x77, 0x69, 0x73, 0x74, 0x6f, 0x6e, 0x65, 0x2e, 
		0x63, 0x6f, 0x6d,
		// ST-Hwy2C1dc7FEkgmsiXWsb-355650-passport.wistone.com↑
		0x00, 0x00, 0x00, 0x20, 
		0x39, 0x63, 0x31, 0x36, 0x38, 0x61, 0x62, 0x32, 0x36, 0x31, 0x30, 0x62, 0x61, 0x31, 0x36, 0x39, 
		0x64, 0x36, 0x35, 0x64, 0x38, 0x35, 0x64, 0x34, 0x31, 0x62, 0x63, 0x33, 0x63, 0x64, 0x35, 0x64
		// 9c168ab2610ba169d65d85d41bc3cd5d↑
	};
	*/
	byte data_f20[] = {
		0x57, 0x49, 0x53, 0x54,
		0x5c, 0x03, 0x32, 0x09,
		0x00, 0x00, 0x00, 0xc3, // 整个数据长度
		0x00, 0x00, 0x03, 0xe9,
		0x00, 0x00, 0x00, 0x00
	}; // 前20
    // 0x00, 0x00, 0x00, 0x00, // 用户ID
	// 0x00, 0x00, 0x00, 0x11, // UID长度
	/*
	0x31, 0x39, 0x30, 0x31, 0x30, 0x37, 0x31, 0x30, 0x30, 0x30, 0x38, 0x38, 0x38, 0x30, 0x38, 0x39, 0x32,
	19010710008880892↑ 19011310008890077 18090410008592005 
	*/
	byte data_add[] = {
		0x00, 0x00, 0x00, 0x02,
		0x00, 0x00, 0x00, 0x03,
		0x61, 0x64, 0x64 // "add"
	};
	byte data_wst_add_zh[] = {
		0x00, 0x00, 0x00, 0x0e,
		0x77, 0x73, 0x74, 0x5f, 0x61, 0x64, 0x64, 0x5f, 0x7a, 0x68, 0x5f, 0x30, 0x30, 0x31, // "wst_add_zh_001"
		0x00, 0x00, 0x00, 0x02,
		0x7a, 0x68, // "zh"
	};
	byte data_unknow_1[] = {
		0x00, 0x00, 0x00, 0x20,
		0x63, 0x66, 0x33, 0x38, 0x62, 0x31, 0x38, 0x32, 0x61, 0x66, 0x39, 0x61, 0x34, 0x65, 0x30, 0x30,
		0x38, 0x39, 0x61, 0x36, 0x61, 0x37, 0x64, 0x38, 0x66, 0x33, 0x32, 0x65, 0x62, 0x64, 0x35, 0x66,
		// cf38b182af9a4e0089a6a7d8f32ebd5f↑
	};
	/*
	0x00, 0x00, 0x00, 0x33,
	0x53, 0x54, 0x2d, 0x48, 0x77, 0x79, 0x32, 0x43, 0x31, 0x64, 0x63, 0x37, 0x46, 0x45, 0x6b, 0x67,
	0x6d, 0x73, 0x69, 0x58, 0x57, 0x73, 0x62, 0x2d, 0x33, 0x35, 0x35, 0x36, 0x35, 0x30, 0x2d, 0x70,
	0x61, 0x73, 0x73, 0x70, 0x6f, 0x72, 0x74, 0x2e ,0x77, 0x69, 0x73, 0x74, 0x6f, 0x6e, 0x65, 0x2e,
	0x63, 0x6f, 0x6d,
	// ST-Hwy2C1dc7FEkgmsiXWsb-355650-passport.wistone.com↑
	*/
	byte data_unknow_2[] = {
		0x00, 0x00, 0x00, 0x20,
		0x39, 0x63, 0x31, 0x36, 0x38, 0x61, 0x62, 0x32, 0x36, 0x31, 0x30, 0x62, 0x61, 0x31, 0x36, 0x39,
		0x64, 0x36, 0x35, 0x64, 0x38, 0x35, 0x64, 0x34, 0x31, 0x62, 0x63, 0x33, 0x63, 0x64, 0x35, 0x64
		// 9c168ab2610ba169d65d85d41bc3cd5d↑
	};
	// 36 + 51 = 87 207 - 87 = 120
	///printf("%c%c%c\n", data[120], data[121], data[122]);
	int index = 0;
	memcpy(m_SendStr, data_f20, 20); // 复制前20个字符
	copy_i32_net(&m_SendStr[20], m_iAccountId); // 复制用户ID [20-23]

	int uidLength = strlen(m_Uid);
	copy_i32_net(&m_SendStr[24], uidLength); // 第24-27个字符串为UID长度
	memcpy(&m_SendStr[28], m_Uid, uidLength); // UID字符串
	index = 28 + uidLength;

	memcpy(&m_SendStr[index], data_add, sizeof(data_add)); // 复制add
	index += sizeof(data_add);

	memcpy(&m_SendStr[index], data_wst_add_zh, sizeof(data_wst_add_zh)); // 复制wst_add_zh_001
	index += sizeof(data_wst_add_zh);

	memcpy(&m_SendStr[index], data_unknow_1, sizeof(data_unknow_1)); // 复制未知数值
	index += sizeof(data_unknow_1);

	copy_i32_net(&m_SendStr[index], strlen(m_Wst));      // 复制wst长度
	memcpy(&m_SendStr[index + 4], m_Wst, strlen(m_Wst)); // 复制wst
	index += 4 + strlen(m_Wst);

	memcpy(&m_SendStr[index], data_unknow_2, sizeof(data_unknow_2)); // 复制未知数值2
	index += sizeof(data_unknow_2);

	copy_i32_net(&m_SendStr[8], index - 12); // 整个数据长度
	m_SendLength = index;
}

void Game::MakeTcpConfirmLogin()
{
	char data[] = {
		0x57, 0x49, 0x53, 0x54,
		0x51, 0xec, 0x69, 0xc0,
		0x00, 0x00, 0x00, 0x04,
		0x00, 0x00, 0x0f, 0xa6,
		0x57, 0x49, 0x53, 0x54,
		0x51, 0xec, 0x69, 0xc0, 
		0x00, 0x00, 0x00, 0x37, 
		0x00, 0x00, 0x17, 0x71, 
		0x00, 0x00, 0x00, 0x0e, 
		0x77, 0x73, 0x74, 0x5f, 0x61, 0x64, 0x64, 0x5f, 0x7a, 0x68, 0x5f, 0x30, 0x30, 0x31, 
		0x00, 0x00, 0x00, 0x11, 
		0x31, 0x39, 0x30, 0x31, 0x31, 0x33, 0x31, 0x30, 0x30, 0x30, 0x38, 0x38, 0x39, 0x30, 0x30, 0x37, 0x37,
		0x00, 0x00, 0x00, 0x00, 0x00, 0x9e, 0x4a, 0xbc, 0x00, 0x00, 0x08, 0xab, 
		0x57, 0x49, 0x53, 0x54, 0x51, 0xec, 0x69, 0xc0, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0b, 0xbf, 
		0x57, 0x49, 0x53, 0x54, 0x51, 0xec, 0x69, 0xc0, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x1f, 0x54
	};

	memcpy(m_SendStr, data, 53); // 复制前27个字符

	int uidLength = strlen(m_Uid);
	m_SendStr[53] = uidLength; // 第27个字符串为UID长度
	memcpy(&m_SendStr[54], m_Uid, uidLength); // UID字符串
	memcpy(&m_SendStr[54 + uidLength], &data[54 + 17], sizeof(data) - (54 + 17));
	m_SendLength = sizeof(data) + uidLength - 17;
}

/*
制作获取数据
OP=操作码
*/
void Game::MakeTcpOP(int op, char num, bool togeth)
{
	MakeOP(op, num, 0, 1, false, togeth);
}

/*
制作获取数据
OP=操作码
*/
void Game::MakeTcpOPInt(int op, int num, bool togeth)
{
	MakeOP(op, num, 0, 4, false, togeth);
}

/* 制作获取数据 */
void Game::MakeTcpOPInt64(int op, __int64 num, bool togeth)
{
	MakeOP(op, num&0xffffffff, (num>>32)&0xffffffff, 4, true, togeth);
}

/*
制作获取数据
OP=操作码
*/
void Game::MakeTcpOPMid(int op, int num, int mid, bool togeth)
{
	MakeOP(op, num, mid, 4, true, togeth);
}

/*
制作获取数据
op=操作码
num=操作数据
mid=操作数据中间4字节
byte=num字节数
ismid=是否有中间数
togeth=是否多个合并
*/
void Game::MakeOP(int op, int num, int mid, int byte, bool ismid, bool togeth)
{
	char data[] = {
		0x57, 0x49, 0x53, 0x54,
		0x61, 0x2d, 0xd4, 0x96,
		0x00, 0x00, 0x00, 0x04,
		0x00, 0x00, 0x00, 0x00  // 指令数据
	};
	copy_i32_net(&data[12], op); // 复制指令

	int mid_len = ismid ? 4 : 0;
	int index = togeth ? m_SendLength : 0;
	memcpy(m_SendStr + index, data, sizeof(data));
	m_SendLength = sizeof(data) + index + mid_len;

	if (ismid) { // 有中间数据
		copy_i32_net(&m_SendStr[16 + index], mid);
	}

	//printf("mid:%d num:%d\n", mid, num);

	if (num > 0) {
		if (byte == 1) { // 1字节
			m_SendStr[16 + mid_len + index] = num;
		}
		else if (byte == 4) { // 4字节
			copy_i32_net(&m_SendStr[16 + mid_len + index], num);
		}

		copy_i32_net(&m_SendStr[8 + index], 4 + byte + mid_len); // 数据长度
		m_SendLength += byte;
	}
	
	if (op == OP_YANJIU) {
		printf("<<<<<<<<<<<<<<<<<--------------->>>>>>>>>>>>>>>>>>\n");
		for (int i = 0; i < m_SendLength; i++) {
			printf("%02x ", m_SendStr[i]&0xff);
		}
		printf("\n");
	}
	printf("send length:%d\n", m_SendLength);
}

/* 建造建筑 */
void Game::MakeHouse(int type, int pos)
{
	char data[] = {
		0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 
		0x00, 0x00, 0x00, 0x0c, 
		0x00, 0x00, 0x42, 0x6b, // 建筑指令
		0x00, 0x00, 0x00, 0x00, 
		0x00, 0x00, 0x00, 0x00
	};

	copy_i32_net(&data[16], type); // 建筑类型
	copy_i32_net(&data[20], pos);  // 建筑位置
	memcpy(m_SendStr, data, sizeof(data));
	m_SendLength = sizeof(data);
}

/* 建筑升级所需 */
void Game::MakeUpNeed(__int64 code, JZ_TYPE type, int no)
{
	char data[] = {
		0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96,
		0x00, 0x00, 0x00, 0x14,
		0x00, 0x00, 0x42, 0x6f, // 建筑升级所需条件指令
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 建筑代码
		0x00, 0x00, 0x00, 0x00, // 建筑类型
		0x00, 0x00, 0x00, 0x00, // 建筑位置
	};

	copy_i64_net(&data[16], code); // 建筑代码
	copy_i32_net(&data[24], type); // 建筑类型
	copy_i32_net(&data[28], no);   // 建筑位置
	memcpy(m_SendStr, data, sizeof(data));
	m_SendLength = sizeof(data);

	printf("<<<<<<<<<<<<<<<<<--------------->>>>>>>>>>>>>>>>>>\n");
	for (int i = 0; i < m_SendLength; i++) {
		printf("%02x ", m_SendStr[i] & 0xff);
	}
	printf("\n");
}

/* 使用加速 */
void Game::MakeJiaSu(__int64 code, int jiasu_type)
{
	char data[] = {
		0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96,
		0x00, 0x00, 0x00, 0x10,
		0x00, 0x00, 0x42, 0x72, // 建筑加速所需条件指令
		0x00, 0x00, 0x00, 0x00, // 加速类型
		0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // 建筑代码
	};

	copy_i32_net(&data[16], jiasu_type); // 建筑类型
	copy_i64_net(&data[20], code); // 建筑代码
	memcpy(m_SendStr, data, sizeof(data));
	m_SendLength = sizeof(data);
}

void Game::MakeTcpGetCoor()
{
	char data[] = {
		0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x07, 0xd1,  // 获取坐标
	};
	for (int i = 0; i < sizeof(data); i += 16) {
		memcpy(&data[i + 4], m_Wist, 4);
	}
	memcpy(m_SendStr, data, sizeof(data));
	m_SendLength = sizeof(data);
}

// 获取声望数据
void Game::MakeTcpGetShengWang()
{
	char data[] = {
		0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x03, 0xf9,  // 获取坐标
	};
	for (int i = 0; i < sizeof(data); i += 16) {
		memcpy(&data[i + 4], m_Wist, 4);
	}
	memcpy(m_SendStr, data, sizeof(data));
	m_SendLength = sizeof(data);
}

// 获取建筑ID请求数据
void Game::MakeTcpGetZY()
{
	char data[] = {
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x07, 0xea, 0xfe,
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x05, 0x00, 0x00, 0x27, 0x16, 0x04,
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0f, 0xa1,
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x07, 0xd1,
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x07, 0xea,
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x07, 0xe2,
		0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x42, 0x69, // 获得建筑ID
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x42, 0x6a,
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0b, 0xbd,
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x55, 0xf6,
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x32, 0xca,
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x03, 0xed,
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x03, 0xf7,
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x03, 0xf9, // 获得声望
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x03, 0xfc,
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x03, 0xed,
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x0b, 0xbd,
		//0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x13, 0x89,
	};
	for (int i = 0; i < sizeof(data); i += 16) {
		memcpy(&data[i + 4], m_Wist, 4);
	}
	memcpy(m_SendStr, data, sizeof(data));
	m_SendLength = sizeof(data);
}

// 慈善数据
void Game::MakeTcpCiShan()
{
	char data[] = {
		0x57, 0x49, 0x53, 0x54, 
		0x61, 0x2d, 0xd4, 0x96, 
		0x00, 0x00, 0x00, 0x05, 
		0x00, 0x00, 0x07, 0xe4,
		0x01
	};
	memcpy(m_SendStr, data, sizeof(data));
	m_SendLength = sizeof(data);
}

void Game::MakeTcpJieSan(int num)
{
	char data[] = {
		0x57, 0x49, 0x53, 0x54,
		0x5c, 0x03, 0x32, 0x09,
		0x00, 0x00, 0x00, 0x0c,
		0x00, 0x00, 0x0b, 0xba,
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, (num>>8)&0xff, num&0xff
	};
	memcpy(m_SendStr, data, sizeof(data));
	m_SendLength = sizeof(data);
}

void Game::MakeXunLian(__int64 id)
{
	char data[] = {
		0x57, 0x49, 0x53, 0x54,
		0x5c, 0x03, 0x32, 0x09,
		0x00, 0x00, 0x00, 0x14,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x01
	};
	//printf("OP:%08x ID:%08x\n", op, id);
	copy_i32_net(&data[12], OP_XUNLIAN);
	copy_i64_net(&data[16], id);
	memcpy(m_SendStr, data, sizeof(data));
	m_SendLength = sizeof(data);
}

void Game::XunLianMore(__int64 id, __int32 num)
{
	char data[] = {
		0x57, 0x49, 0x53, 0x54,
		0x5c, 0x03, 0x32, 0x09,
		0x00, 0x00, 0x00, 0x14,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x01,
		0x00, 0x00, 0x00, 0x01
	};
	//printf("OP:%08x ID:%08x\n", id, num);
	copy_i32_net(&data[12], OP_XUNLIAN);
	copy_i64_net(&data[16], id);
	copy_i32_net(&data[28], num);
	memcpy(m_SendStr, data, sizeof(data));
	m_SendLength = sizeof(data);
}

/* 研究科技 */
void Game::MakeYanJiu(KJ_TYPE type)
{
	char data[] = {
		0x57, 0x49, 0x53, 0x54,
		0x5c, 0x03, 0x32, 0x09,
		0x00, 0x00, 0x00, 0x0c,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00
	};
	//printf("OP:%08x ID:%08x\n", op, id);
	copy_i32_net(&data[12], OP_YANJIU);
	copy_i32_net(&data[16], type);
	memcpy(m_SendStr, data, sizeof(data));
	m_SendLength = sizeof(data);
}

/*
获取操作码
data=游戏服务器返回的数据
*/
int Game::GetOpCode(const char* data)
{
	return m_pClient->Intel2Length(data);
}

/*
获取坐标
data = 游戏服务器返回的数据
x = x坐标变量
y = y坐标标量
*/
void Game::GetCoor(const char* data, int& x, int& y)
{
	x = m_pClient->Intel2Length(&data[24]);
	y = m_pClient->Intel2Length(&data[28]);
}

/*
获取声望值
data=游戏服务器返回的数据
*/
int Game::GetShengWang(const char* data)
{
	return m_pClient->Intel2Length(&data[55]);
}
/*
获得训练营ID
data=游戏服务器返回的数据
length=data长度
*/
int Game::GetXLYId(const char* data, int length)
{
	int id = 0;
	for (int i = 0; i < length; i += 0x31) {
		if ((i + 0x08) > length)
			break;

		int no = m_pClient->Intel2Length(&data[i + 4]);
		if (no == 0x0e) {
			id = m_pClient->Intel2Length(&data[i]);
		}
	}
	printf("\n");
	return id;
}

/* 获得建筑列表 */
JianZhu* Game::GetJianZhuList(JZ_TYPE type)
{
	return (type >= JZ_NONGTIAN && type <= JZ_XIKUANG) ? m_zyJianZhu : m_jsJianZhu;
}

/* 
根据类型获得建筑信息 
flag[0获取第一个建筑, 1获取等级最大建筑, -1获取等级最小建筑]
*/
JianZhu* Game::GetJianZhuByType(JZ_TYPE type,  int flag)
{
	JianZhu* value = nullptr;
	JianZhu* list = GetJianZhuList(type);
	for (int i = 0; i < JZ_MAX; i++) {
		if (list[i].status == 0)
			continue;

		if (list[i].type == type) {
			if (value == nullptr) {
				value = &list[i];
				if (flag == 0) // 取第一个
					break;
			}
			else {
				if (flag == 1) {  // 取最大
					if (list[i].level > value->level)
						value = &list[i];
				}
				else if (flag == -1) { // 取最小
					if (list[i].level < value->level)
						value = &list[i];
				}
			}
		}
	}

	return value;
}

/* 根据类型获得正在升级建筑信息 */
JianZhu* Game::GetJianZhuExecByType(JZ_TYPE type)
{
	JianZhu* value = nullptr;
	JianZhu* list = GetJianZhuList(type);
	for (int i = 0; i < JZ_MAX; i++) {
		if (list[i].type == type && list[i].exec) {
			//printf("type:%d exec:%d\n", list[i].type, list[i].exec);
			value = &list[i];
			break;
		}
	}
	return value;
}

/* 检查建筑等级 */
bool Game::CheckJianZhuLevel(JZ_TYPE type, int level, bool all)
{
	JianZhu* list = GetJianZhuList(type);
	for (int i = 0; i < JZ_MAX; i++) {
		if (list[i].type == type) {
			if (list[i].level >= level) {
				if (!all)
					return true;
			}
			else {
				if (all)
					return false;
			}
		}
	}
	return false;
}

/* 获得正在执行的队列建筑数量 */
int Game::GetJianZhuExecNum(int& no_wait)
{
	no_wait = 0;
	int num = 0;
	LiuCheng lc;
	lc.op = OP_UNKNOW;
	lc.noconf = true;

	JianZhu* list = m_jsJianZhu;
	for (int i = 0; i <= 0x21; i++) {
		if (!list[i].ignoreck && list[i].exec && list[i].cpttime > 0) {
			if (num > 0) {
				m_pGameProc->CopyToDuiLieText(" | ");
			}

			int ret = HandleExecJianZhu(&list[i]);
			if (ret == 1)
				no_wait++;
			if (ret != -1)
				num++;
		}
	}
	list = m_zyJianZhu;
	for (int i = 0; i < m_iZYNum; i++) {
		if (!list[i].ignoreck && list[i].exec && list[i].cpttime > 0) {
			if (num > 0) {
				m_pGameProc->CopyToDuiLieText(" | ");
			}

			int ret = HandleExecJianZhu(&list[i]);
			if (ret == 1)
				no_wait++;
			if (ret != -1)
				num++;
		}
	}
	return num;
}

/* 操作正在执行建筑 */
int Game::HandleExecJianZhu(JianZhu* jz)
{
	if (m_pGameProc->CanIgnoreCk(jz)) {
		printf("````````已在队列，忽略此建筑检查：%016x````````\n", jz->code);
		jz->ignoreck = 1;
		return -1;
	}

	LiuCheng lc;
	lc.op = OP_UNKNOW;
	lc.noconf = true;

	if (jz->level == 0) {
		lc.op = OP_JIANZAO;
		m_pGameProc->CopyToDuiLieText("建造 ");
	}
	else {
		lc.op = OP_SHENGJI;
		m_pGameProc->CopyToDuiLieText("升级 ");
	}

	char name[16];
	m_pGameProc->TransToTextJZ((JZ_TYPE)jz->type, name);
	m_pGameProc->CopyToDuiLieText(name);

	char str[16];
	sprintf_s(str, "到%d", jz->level + 1);
	m_pGameProc->CopyToDuiLieText(str);

	lc.jz = jz;
	if (!m_pGameProc->CheckWaitDuiLie(&lc)) {
		return 1;
	}
	return 0;
}

/* 获取建筑空闲位置 */
int Game::GetJianZhuFreeNo(JZ_AREA area)
{
	int count;
	JianZhu* list;
	if (area == JZ_AREA_JS) { // 军事区
		list = m_jsJianZhu;
		count = 0x21;
	}
	else {
		list = m_zyJianZhu;
		count = m_iZYNum;
	}

	for (int i = 0; i < count; i++) {
		if (list[i].status == 0)
			return i;
	}
	return -1;
}

/* 根据类型获取建筑空闲位置 */
int Game::GetJianZhuFreeNoByType(JZ_TYPE type)
{
	JZ_AREA area = (type >= JZ_NONGTIAN && type <= JZ_XIKUANG)
		? JZ_AREA_ZY : JZ_AREA_JS; // 获取建筑所在区域
	return GetJianZhuFreeNo(area);
}

/* 根据类型获取建筑数量 */
int Game::GetJianZhuNumByType(JZ_TYPE type, int level)
{
	int num = 0;
	JianZhu* list = GetJianZhuList(type);
	for (int i = 0; i < JZ_MAX; i++) {
		if (list[i].type == type && list[i].level >= level) {
			num++;
		}
	}
	return num;
}

/* 检查建筑等级是否符合 */
bool Game::CheckJianZhuLevel(JZ_TYPE type, int level)
{
	JianZhu* jz = GetJianZhuByType(type, 1);
	return jz && jz->level >= level;
}

/* 检查资源是否OK */
bool Game::CheckRes(JianZhu_Need& need)
{
	for (int i = 0; i < need.jz_count; i++) { // 检查建筑等级是否符合
		if (!CheckJianZhuLevel(need.jz[i].type, need.jz[i].level)) {
			return false;
		}
	}
	return m_ZiYuan.liangshi >= need.liangshi  // 粮食
		&& m_ZiYuan.gangtie >= need.gangtie    // 钢铁
		&& m_ZiYuan.shiyou >= need.shiyou      // 石油
		&& m_ZiYuan.xikuang >= need.xikuang    // 稀矿
		&& m_ZiYuan.renkou >= need.renkou;     // 人口
}

/* 检查科技等级 */
bool Game::CheckKeJiLevel(KJ_TYPE type, int level)
{
	for (int i = 0; i < KJ_MAX; i++) {
		if (m_KeJi[i].type == type)
			return m_KeJi[i].level >= level;
	}
	return false;
}

/* 队列是否可免费 */
bool Game::CheckCanFree(__int64 timeleft)
{
	return timeleft < FREE_SECOND;
}

/* 获得可以免费倒计时 */
int Game::GetFreeLeft(__int64 timeleft)
{
	return timeleft - FREE_SECOND + 1;
}

int Game::Send()
{
	return send(m_Socket, (char*)m_SendStr, m_SendLength, 0);
}

/* 发送空信息数据到游戏服务器 */
int Game::SendEmptyMsg()
{
	int ret = send(m_Socket, (char*)m_SendStr, 0, 0);
	printf("发送空消息(%d)\n", ret);
	return ret;
}

void Game::SetWistoneId(char* wistoneId)
{
	if (!wistoneId)
		return;

	memcpy(m_WistoneId, wistoneId, strlen(wistoneId) + 1);
}

void Game::SetUid()
{
	DesDecrypt(m_Uid, GAME_DES_KEY, m_WistoneId, strlen(m_WistoneId), true);
}

void Game::SetSocket(SOCKET socket)
{
	m_Socket = socket;
}

/* 设置登录成功时间 */
void Game::SetLoginOkTime(time_t t)
{
	m_qwLoginOkTime = t > 0 ? t : time(nullptr);
}

/* 在线时长超过设定时间 */
bool Game::OnLineIsTimeOut()
{
	if (!m_qwLoginTime)
		return false;

	return time(nullptr) >= (m_qwLoginTime + m_stConfig.OnlineTimeOut);
}

Client& Game::GetClient()
{
	return *m_pClient;
}

char Game::Hex2Char(char ch, char ch2)
{
	return Char2Num(ch) * 16 + Char2Num(ch2);
}

char Game::Char2Num(char ch)
{
	if (ch >= '0' && ch <= '9') {
		ch = ch - '0';
	}
	else if (ch >= 'A' && ch <= 'F') {
		ch = ch - 'A' + 10;
	}
	else if (ch >= 'a' && ch <= 'f') {
		ch = ch - 'a' + 10;
	}
	else {
		ch = 0;
	}
	return ch;
}

// 更新控件文字
void Game::UpdateText(int col, char* text)
{
	m_pGameAccount->UpdateText(m_pAccount, col, text);
}

// 更新状态文字
void Game::UpdateAccountStatusText(char * text, int row)
{
	m_pGameAccount->UpdateStatusText(m_pAccount, text);
}

// 更新队列文字
void Game::UpdateAccountDuiLieText(char* text)
{
	m_pGameAccount->UpdateDuiLieText(m_pAccount, text);
}

void Game::OnConnectError(Game* game, const char *, USHORT)
{
	game->UpdateAccountStatusText("连接服务器失败！");
}

void Game::OnConnect(Game* game, const char* host, USHORT port, SOCKET socket)
{
	game->SetSocket(socket);
	if (port == GAME_SER_PORT) {
		game->UpdateAccountStatusText("登录游戏帐号...");
		game->MakeTcpFirstStr();
	}
	else {
		game->UpdateAccountStatusText("获取游戏服务器信息...");
		game->MakeTcpLogin();
	}
	//game->MakeTcpFirstStr();
	int ret = game->Send();
	printf("发送长度:%d\n", ret);
}

void Game::OnRead(Game* game, const char* host, USHORT port, const char* data, int length, const char* wist)
{
	printf("读取数据(%d)！\n", length);
	int opcode = game->GetOpCode(data);
	//memcpy(game->m_Wist, wist, 4);
	//printf("%02x %02x %02x %02x\n", wist[0], wist[1], wist[2], wist[3]);
	int i;
#if 0
	if (opcode == OP_SHENGJI) {
		for (i = 0; i < length; i++) {
			printf("%02x ", data[i] & 0xff);
			if (((i + 1) % 0x10) == 0 && i > 0) {
				printf("\n");
			}
		}
		printf("\n-----------------------------\n");
	}
	
#endif
#if 0
	if (opcode == OP_LOGIN) {
		for (i = 0; i < length; i++) {
			printf("%c ", data[i] >= 38 && data[i] <= 126 ? data[i] : '.');
			if (((i + 1) % 0x10) == 0 && i > 0) {
				printf("\n");
			}
		}
		printf("\n");
	}
#endif
	if (port == GAME_SER_PORT2) {
		if (game->GetTcpServerInfo(data)) {
			CString text;
			text.Format(L"成功获取游戏服务器信息！");

			game->UpdateText(4, game->m_ServerId);
			game->GetAccoutId(data);
			game->GetClient().Close(true);

			printf("关闭获取服务器信息！\n");
		}
	}
	else {
		game->m_i64LastGetMsgTime = time(nullptr);

		GameProc* pGameProc = game->m_pGameProc;
		pGameProc->SetData(data, length, data[4], data[4] == 0x01?true:false);

		switch (opcode)
		{
		case OP_LOGIN: // 登录
			pGameProc->Login();
			break;
		case OP_COOR: // 坐标
			pGameProc->Coor();
			break;
		case OP_SHENGWANG: // 声望
			pGameProc->ShengWang();
			break;
		case OP_VIEWRES:  // 拥有资源
			pGameProc->ViewRes();
			break;
		case OP_JIANZHU_JS: // 建筑-军事区
			pGameProc->JianZhu_JS();
			break;
		case OP_JIANZHU_ZY: // 建筑-自由区
			pGameProc->JianZhu_ZY();
			break;
		case OP_JIANZAO:   // 建筑-建造
			pGameProc->JianZhu_JianZao();
			break;
		case OP_SHENGJI:  // 建筑-升级
			pGameProc->JianZhu_ShengJi();
			break;
		case OP_CHAI_JZ:  // 拆建筑
			pGameProc->Chai_JianZhu();
			break;
		case OP_SHENGJINEED: // 建筑-升级所需条件
			pGameProc->JianZhu_ShengJiNeed();
			break;
		case OP_FREE:       // 免费队列
			pGameProc->Free();
			break;
		case OP_VIEWKEJI:   // 科技列表
			pGameProc->ViewKeJi();
			break;
		case OP_ZHAOMULIST: // 可招募军官列表
			pGameProc->ZhaoMuList();
			break;
		case OP_VIEWGOODS: // 查看物品
			pGameProc->ViewGoods();
			break;
		case OP_USEGOODS:  // 使用物品
			pGameProc->UseGoods();
			break;
		case OP_VIEWCURTASK: // 查看当前主线任务
			pGameProc->CurrentTask();
			break;
		case OP_VIEWTASK:   // 查看任务是否可完成
			pGameProc->ViewTask();
			break;
		case OP_XUNLIAN:   // 训练士兵
			pGameProc->XunLian();
			break;
		case OP_JIESAN: // 解散
			pGameProc->JieSan();
			break;
		case OP_CISHAN: // 慈善
			pGameProc->CiShan();
			break;
		case OP_VIEWQIANDAO://查看签到
			pGameProc->ViewQianDao();
			break;
		case OP_QIANDAO://完成签到
			pGameProc->QianDao();
			break;
		case OP_LIANTIAO:
		case OP_LIANTIAO_XT:
			//game->SendEmptyMsg();
			break;
		case RW_LIST:
			//完成任务列表任务
			pGameProc->RwList();
			break;
		case OP_CITY:
			//城市信息
			pGameProc->City();
			break;
		default:
			break;
		}
#if 0
		// 计算那些流程可以完成
		while (pGameProc->IsLocked());
		pGameProc->Lock();
		if (pGameProc->CalcCompleteLiuCheng()) {
			if (!pGameProc->ExecDuiLie()) { // 没有队列可以执行
				CString account = game->m_pAccount->Mail;
				CString text;
				text.Format(L"(%d)帐号：%ws 已完成全部流程.", game->m_pAccount->index + 1, account);
				game->m_pGameAccount->InsertLog(text.GetBuffer());
				game->UpdateAccountStatusText("流程已全部完成！！！");
				game->m_pClient->Close(true);
			}
		}
		pGameProc->UnLock();
#endif
	}
}

void Game::OnClose(Game* game, const char* host, USHORT port)
{
	printf("OnClose\n");
	if (port == GAME_SER_PORT2) {
		game->GetClient().Close(false);
		game->UpdateAccountStatusText("准备连接游戏服务器...");
		game->GetClient().Connect(game->m_ServerHost, game->m_ServerPort);
	}
	else {
		if (game->m_pAccount->Status == STATUS_ING) {
			game->SetAccountFailed();
		}
		if (game->GetClient().m_Close) {
			// 多少秒后可自动登录
			int auto_login_sec = game->m_pAccount->CptTime - time(nullptr) - FREE_SECOND;
			if (auto_login_sec < 0)
				auto_login_sec = 0;

			char msg[32];
			sprintf_s(msg, "主动关闭 [%d秒后自动登录]", auto_login_sec);
			game->UpdateAccountStatusText(msg);

			printf("主动关闭游戏.\n");
		}
		else {
			game->UpdateAccountStatusText("游戏关闭 [被动关闭]");
			printf("被动关闭游戏.\n");
		}
	}
}

void Game::OnError(Game* game, const char *, USHORT)
{
	printf("连接错误.\n");
	game->UpdateAccountStatusText("连接错误.");
}

void Game::OnTimeOut(Game* game, const char* host, USHORT port, int sec)
{
	printf("连接超时.\n");
	game->UpdateAccountStatusText("连接超时.");
}

Game* Game::GetInstance()
{
	//if (Game::self == nullptr) {
	//	Game::self = new Game;
	//}
	return Game::self;
}
