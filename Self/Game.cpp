#include "Game.h"
#include "../Encrypt/Des.h"
#include "../Common/Json.h"
#include "../Common/Explode.h"
#include "../Common/mystring.h"
#include "../Common/OpenTextFile.h"
#include <string.h>

#ifndef IDC_EDIT_LOGIN_INTERVAL
#define IDC_EDIT_LOGIN_INTERVAL         1003
#define IDC_EDIT_LOGIN_TIMEOUT          1004
#define IDC_EDIT_START_KEY              1005
#define IDC_STATIC_ACCOUT_NUM           1008
#define IDC_STATIC_SUC                  1009
#define IDC_STATIC_FAILED               1010
#endif

#define SHENGWANG_MAX  10000

#define GAME_DES_KEY   "2644894d08a9c3b784993006ea980cfd"

#define GAME_HOST      L"sso.wistone.com"
#define GAME_LOGINPATH L"/wistoneSSO/mlogin"
#define GAME_APPID     L"4244c1323c61df864413ee47654cbaec"
#define GAME_APPSECRET L"a4ab2ae6030541212ecb6773b505471d833b8c75e5756de0"
#define GAME_LANG      L"zh_CN"
#define GAME_PALTFORM  L""
#define GAME_DEVICE    L""
#define GAME_SERVICE   L"http://sso.wistone.com/wistoneSSO/mlogin"

#define GAME_SER_IP    "106.75.2.182"
#define GAME_SER_PORT  8083
#define GAME_SER_IP2   "106.75.17.197"
#define GAME_SER_PORT2 8082

Game* Game::self = nullptr;

Game::Game()
{
	start = false;
	self = nullptr;
	memset(m_ServerHost, 0, sizeof(m_ServerHost));
	memset(m_ServerId, 0, sizeof(m_ServerId));
	m_ServerPort = 0;
	m_SendStr = new byte[4096];
}

Game::~Game()
{
	printf("释放Game\n");
	delete[] m_SendStr;
	if (Game::self)
		delete Game::self;
}

// 1222221fff1@qq.com 123456
// 6c86ddf217be055983d1391343dd2b85ad7d23483146340e 717b2aaa6d933836

int Game::GetConfAccount()
{
	if (m_iAccoutCurrentNum) {
		return m_iAccountNum;
	}

	m_iAccoutCurrentNum = 0;
	m_iAccountNum = 0;
	m_iSuccessNum = 0;
	m_iFailNum = 0;
	InsertLogFunc(L"===准备读取配置文件(account.ini)===");
	CString log;
	OpenTextFile file;
	if (file.Open("account.ini")) {
		int line = file.GetCount();
		if (line > 0) {
			m_stAccoutList = new Accout[line];
		}
		printf("文件行数:%d\n", line);
		int row = 0, length;
		char data[256];
		while ((length = file.GetLine(data, 256)) > -1) {
			row++;
			if (length == 0) {
				continue;
			}
			if (m_iAccountNum >= line) {
				break;
			}

			Explode explode("::", data);
			if (explode.GetCount() != 2) {
				log.Format(L"第(%d)行配置不正确，忽略。", row + 1);
				InsertLogFunc(log.GetBuffer());
				continue;
			}

			/* 保存帐号信息 */
			strcpy(m_stAccoutList[m_iAccountNum].Mail, explode[0]);
			strcpy(m_stAccoutList[m_iAccountNum].Password, explode[1]);
			m_stAccoutList[m_iAccountNum].JieSanNum = explode.GetValue2Int(2, 1);
			m_stAccoutList[m_iAccountNum].Status = STATUS_NO_START;
			/* 显示到List控件 */
			InsertAccountFunc(m_iAccountNum, 1, explode[0]);
			InsertAccountFunc(m_iAccountNum, 2, "--");
			InsertAccountFunc(m_iAccountNum, 3, "--");
			InsertAccountFunc(m_iAccountNum, 4, "未开始");

			m_iAccountNum++;
			ListReDrawFunc();
			//printf("data:%s\n", trim(data));
		}
		file.Close();
		log.Format(L"读取配置文件完成，共有帐号：%d", m_iAccountNum);
		SetTextFunc(IDC_STATIC_ACCOUT_NUM, 0, m_iAccountNum);
	}
	else {
		log = L"找不到配置文件(account.ini)";
	}
	InsertLogFunc(log.GetBuffer());

	return m_iAccountNum;
}

int Game::GetAccoutStatus(int index)
{
	if (index >= m_iAccountNum)
		return STATUS_NO_START;

	return m_stAccoutList[index].Status;
}

void Game::Start()
{
	time_t start_time = time(NULL);
	m_iAccoutCurrentNum = m_stConfig.StartNum;
	while (m_iAccoutCurrentNum < m_iAccountNum) {
		Accout* p = &m_stAccoutList[m_iAccoutCurrentNum];
		p->Status = STATUS_ING;

		if (!start) {
			Sleep(50);
			continue;
		}

		// 重绘账号列表控件
		ListReDrawFunc();
		// 更新文字控件信息
		SetTextFunc(IDC_STATIC_ACCOUT_NUM, m_iAccoutCurrentNum + 1, m_iAccountNum);

		CString account = p->Mail;
		CString text;
		text.Format(L"%d、正在登录帐号：%ws", m_iAccoutCurrentNum + 1, account);
		InsertLogFunc(text.GetBuffer());
		UpdateAccountStatusText("正在登录...");
		// 登录
		if (!Login(p->Mail, p->Password)) {
			InsertLogFunc(L"登录失败，请检查帐号或密码是否正确！");
			UpdateAccountStatusText("登录失败.");
			p->Status = STATUS_LOGIN_FAILD;
			m_iFailNum++;
			SetTextFunc(IDC_STATIC_FAILED, m_iFailNum, -1);
			goto end;
		}
		UpdateAccountStatusText("连接服务器...");
		// 连接游戏服务器
		Connect();
	end:
		m_iAccoutCurrentNum++;
		// 重绘账号列表控件
		ListReDrawFunc();

		if (m_stConfig.AccoutInterval > 0) {
			Sleep(m_stConfig.AccoutInterval);
		}
		if (m_iAccoutCurrentNum == 20)
			break;
	}
	time_t end_time = time(NULL);
	CString csHTime;
	csHTime.Format(L"===已全部完成，用时：%d秒===", end_time - start_time);
	InsertLogFunc(csHTime.GetBuffer());
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
	http.AddParam(L"WID", L"");
	http.AddParam(L"email", desMail);
	http.AddParam(L"password", desPassword);
	http.AddParam(L"sp_userid", L"0");
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
	Game::GetInstance()->InsertLogFunc(L"连接服务器[1]！");
	m_pClient = new Client;
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
	m_stAccoutList[m_iAccoutCurrentNum].Status = STATUS_SUCCESS;
	UpdateAccountStatusText("请求成功.");
	ListReDrawFunc();
	m_iSuccessNum++;
	SetTextFunc(IDC_STATIC_SUC, m_iSuccessNum, -1);
	m_pClient->Close(true);
}
/*
设置帐号失败
*/
void Game::SetAccountFailed()
{
	m_stAccoutList[m_iAccoutCurrentNum].Status = STATUS_LOGIN_FAILD;
	UpdateAccountStatusText("请求失败.");
	ListReDrawFunc();
	m_iFailNum++;
	SetTextFunc(IDC_STATIC_FAILED, m_iFailNum, -1);
	m_pClient->Close(true);
}

bool Game::GetTcpServerInfo(const char * data)
{
	if (m_pClient->Intel2Length(data) != 0x01) {
		return false;
	}

	memcpy(m_ServerId, &data[21], 3);

	bool isPort = false;
	int hostLength = data[43];
	const char* host = &data[44];
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
	byte data[] = {
		0x57, 0x49, 0x53, 0x54, 
		0x5c, 0x03, 0x32, 0x09, 
		0x00, 0x00, 0x00, 0xc3, 
		0x00, 0x00, 0x03, 0xe9, 
		0x00, 0x00, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00, // 用户ID
		//0x00, 0x9e, 0x3b, 0x1c,
		//0x00, 0x9e, 0x4a, 0xbc,
		//0x00, 0x9a, 0xaf, 0x28,
		0x00, 0x00, 0x00, 0x11, 
		0x31, 0x39, 0x30, 0x31, 0x30, 0x37, 0x31, 0x30, 0x30, 0x30, 0x38, 0x38, 0x38, 0x30, 0x38, 0x39, 0x32, 
		// 19010710008880892↑ 19011310008890077 18090410008592005
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
		0x53, 0x54, 0x2d, 0x48, 0x77, 0x79, 0x32, 0x43, 0x31, 0x64, 0x63, 0x37, 0x46, 0x45, 0x6b, 0x67, 0x6d,
		0x73, 0x69, 0x58, 0x57, 0x73, 0x62, 0x2d, 0x33, 0x35, 0x35, 0x36, 0x35, 0x30, 0x2d, 0x70, 0x61, 0x73,
		0x73, 0x70, 0x6f, 0x72, 0x74, 0x2e ,0x77, 0x69, 0x73, 0x74, 0x6f, 0x6e, 0x65, 0x2e, 0x63, 0x6f, 0x6d,
		// ST-Hwy2C1dc7FEkgmsiXWsb-355650-passport.wistone.com↑
		0x00, 0x00, 0x00, 0x20, 
		0x39, 0x63, 0x31, 0x36, 0x38, 0x61, 0x62, 0x32, 0x36, 0x31, 0x30, 0x62, 0x61, 0x31, 0x36, 0x39, 
		0x64, 0x36, 0x35, 0x64, 0x38, 0x35, 0x64, 0x34, 0x31, 0x62, 0x63, 0x33, 0x63, 0x64, 0x35, 0x64
		// 9c168ab2610ba169d65d85d41bc3cd5d↑
	};
	// 36 + 51 = 87 207 - 87 = 120
	///printf("%c%c%c\n", data[120], data[121], data[122]);
	memcpy(m_SendStr, data, 27); // 复制前27个字符

	for (int i = 0; i < 4; i++) { // 复制用户ID
		int right = (3 - i) * 8;
		m_SendStr[20 + i] = m_iAccountId >> right;
	}

	int uidLength = strlen(m_Uid);
	m_SendStr[27] = uidLength; // 第27个字符串为UID长度
	memcpy(&m_SendStr[28], m_Uid, uidLength); // UID字符串

	for (int i = 0; i <= 51; i++) {
		if (m_Wst[i] == 0) {
			break;
		}
		data[120 + i] = m_Wst[i];
	}

	memcpy(&m_SendStr[28 + uidLength], &data[28 + 17], sizeof(data) - (28 + 17));
	m_SendLength = sizeof(data) + uidLength - 17;
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

// 获取声望数据
void Game::MakeTcpGetShengWang()
{
	char data[] = {
		0x57, 0x49, 0x53, 0x54, 0x61, 0x2d, 0xd4, 0x96, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x03, 0xf9
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
		0x00, 0x00, 0x00, num
	};
	memcpy(m_SendStr, data, sizeof(data));
	m_SendLength = sizeof(data);
}

void Game::MakeTcpStr(int op, int id)
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
	printf("OP:%08x ID:%08x\n", op, id);
	int i;
	for (i = 0; i < 4; i++) {
		data[12 + i] = (op >> ((3 - i) * 8)) & 0xff;
	}
	for (i = 0; i < 4; i++) {
		data[20 + i] = (id >> ((3 - i) * 8)) & 0xff;
	}
	memcpy(m_SendStr, data, sizeof(data));
	m_SendLength = sizeof(data);
}

/*
获取声望值
data=游戏服务器返回的数据
*/
int Game::GetOpCode(const char* data)
{
	return m_pClient->Intel2Length(data);
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

int Game::Send()
{
	return send(m_Socket, (char*)m_SendStr, m_SendLength, 0);
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

void Game::UpdateAccountStatusText(char * text, int row)
{
	if (row == -1)
		row = m_iAccoutCurrentNum;
	UpdateTextFunc(row, 4, text);
}

void Game::OnConnectError(const char *, USHORT)
{
	Game::GetInstance()->InsertLogFunc(L"连接服务器失败！");
}

void Game::OnConnect(const char* host, USHORT port, SOCKET socket)
{
	Game* game = Game::GetInstance();
	game->SetSocket(socket);
	if (port == GAME_SER_PORT) {
		Game::GetInstance()->InsertLogFunc(L"连接成功，准备登录游戏帐号！");
		game->MakeTcpFirstStr();
	}
	else {
		Game::GetInstance()->InsertLogFunc(L"连接成功，准备获取游戏服务器信息！");
		game->MakeTcpLogin();
	}
	//game->MakeTcpFirstStr();
	int ret = game->Send();
	printf("发送长度:%d\n", ret);
}

void Game::OnRead(const char* host, USHORT port, const char* data, int length, const char* wist)
{
	printf("读取数据(%d)！\n", length);
	Game* game = Game::GetInstance();
	//memcpy(game->m_Wist, wist, 4);
	//printf("%02x %02x %02x %02x\n", wist[0], wist[1], wist[2], wist[3]);
	int i;
	for (i = 0; i < length; i++) {
		//printf("%02x ", data[i] & 0xff);
		if ((i % 0x10) == 0 && i > 0) {
			//printf("\n");
		}
	}
	printf("\n-----------------------------\n");
	for (i = 0; i < length; i++) {
		//printf("%c ", data[i] >= 38 && data[i] <= 126 ? data[i] : '.');
		if ((i % 0x10) == 0 && i > 0) {
			//printf("\n");
		}
	}
	printf("\n");
	if (port == GAME_SER_PORT2) {
		if (game->GetTcpServerInfo(data)) {
			CString text;
			text.Format(L"成功获取游戏服务器信息！");

			game->InsertLogFunc(text.GetBuffer());
			game->UpdateTextFunc(game->m_iAccoutCurrentNum, 3, game->m_ServerId);
			game->GetAccoutId(data);
			game->GetClient().Close(true);
		}
	}
	else {
		int opcode = game->GetOpCode(data);
		switch (opcode)
		{
		case OP_LOGIN:
		{
			if (length < 10 && data[4] == 0x01) {
				game->InsertLogFunc(L"验证成功，获取声望值...");
				game->UpdateAccountStatusText("获取声望值...");
				game->MakeTcpGetShengWang();
				int ret = game->Send();
				game->m_UpNum++;
			}
			else {
				game->SetAccountFailed();
			}
			break;
		}
		case OP_SHENGWANG:
		{
			int shengwang = game->GetShengWang(data);
			CString text;
			text.Format(L"声望值：%d", shengwang);
			game->InsertLogFunc(text.GetBuffer());

			char sSW[16];
			sprintf_s(sSW, "%d", shengwang);
			game->UpdateTextFunc(game->m_iAccoutCurrentNum, 2, sSW);

			if (!game->m_stConfig.Close) {
				if (shengwang < 10000) {
					game->UpdateAccountStatusText("获取军工厂...");
					game->InsertLogFunc(L"声望小于10000，获取军工厂ID.");
					game->MakeTcpGetZY();
					game->Send();
				}
				else {
					game->InsertLogFunc(L"声望达到10000，无须训练.");
					game->SetAccountSuceess();
				}
			}
			else {
				if (shengwang > 9999) {
					game->UpdateAccountStatusText("慈善...");
					game->InsertLogFunc(L"慈善.");
					game->MakeTcpCiShan();
					game->Send();

					int cha = shengwang - 9999;
					int san = cha >> 1;
					if (cha & 0x01)
						san += 1;

					CString csSan;
					csSan.Format(L"声望达到%d，解散数量:%d", shengwang, san);
					game->UpdateAccountStatusText("解散...");
					game->InsertLogFunc(csSan.GetBuffer());
					game->MakeTcpJieSan(san);
					game->Send();
				}
				else {
					game->InsertLogFunc(L"声望小于10000，无须解散.");
					game->SetAccountSuceess();
				}
			}
			break;
		}
		case OP_JIANZHU:
		{
			const char* start = &data[10];
			int id = game->GetXLYId(&data[10], length - 10);
			if (id) {
				game->InsertLogFunc(L"成功获取军工厂ID！");
			}
			else {
				game->UpdateAccountStatusText("获取军工厂id失败.");
				game->InsertLogFunc(L"无法获取军工厂ID！");
			}
			game->UpdateAccountStatusText("训练...");
			game->InsertLogFunc(L"请求训练！");
			// 0x00000bb9 为训练指令
			game->MakeTcpStr(OP_XUNLIAN, id);
			//game->MakeTcpJieSan(0x02);
			int ret = game->Send();
			//printf("发送长度升级:%d\n", ret)；
			game->m_UpNum++;
			break;
		}
		case OP_XUNLIAN:
			game->InsertLogFunc(L"训练成功！");
			game->SetAccountSuceess();
			break;
		case OP_JIESAN:
			game->InsertLogFunc(L"解散成功！");
			game->SetAccountSuceess();
			break;
		case OP_CISHAN:
			if (length < 10) {
				game->InsertLogFunc(L"慈善成功！");
			}
			else {
				game->InsertLogFunc(L"慈善失败，可能未冷却！");
			}
			break;
		default:
			break;
		}
	}
}

void Game::OnClose(const char* host, USHORT port)
{
	Game* game = Game::GetInstance();
	if (port == GAME_SER_PORT2) {
		game->GetClient().Close(false);
		Game::GetInstance()->InsertLogFunc(L"连接关闭[1]，准备连接游戏服务器！");
		game->GetClient().Connect(game->m_ServerHost, game->m_ServerPort);
	}
	else {
		if (game->m_stAccoutList[game->m_iAccoutCurrentNum].Status == STATUS_ING) {
			game->SetAccountFailed();
		}
		Game::GetInstance()->InsertLogFunc(L"连接关闭[2]！");
	}
}

void Game::OnError(const char *, USHORT)
{
	Game::GetInstance()->UpdateAccountStatusText("连接错误.");
	Game::GetInstance()->InsertLogFunc(L"连接发送错误！");
}

void Game::OnTimeOut(const char* host, USHORT port, int sec)
{
	Game::GetInstance()->UpdateAccountStatusText("连接超时.");
	Game::GetInstance()->InsertLogFunc(L"连接超时！");
}

Game* Game::GetInstance()
{
	if (Game::self == nullptr) {
		Game::self = new Game;
	}
	return Game::self;
}
