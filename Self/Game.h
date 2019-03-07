#pragma once

#include "../Common/HttpClient.h"
#include "../Socket/Client.h"

#define STATUS_NO_START    0x00        
#define STATUS_LOGIN_FAILD 0x01
#define STATUS_SUCCESS     0x02
#define STATUS_TIME_OUT    0x03
#define STATUS_ING         0x04

// 登录操作码
#define OP_LOGIN     0x000003e9
// 声望数据操作码
#define OP_SHENGWANG 0x000003f9
// 建筑数据操作码
#define OP_JIANZHU   0x00004269
// 训练操作码
#define OP_XUNLIAN   0x00000bb9
// 解散操作码
#define OP_JIESAN    0x00000bba
// 慈善操作码
#define OP_CISHAN    0x000007e4

typedef unsigned char byte;
typedef void(*InsertAccount)(int, int, char*);
typedef void (*InsertLog)(wchar_t*);
typedef void(*SetText)(int, int, int);
typedef void(*UpdateText)(int, int, char*);
typedef void(*ListReDraw)();

typedef struct struct_accout {
	char Mail[128];     // 邮箱
	char Password[128]; // 密码
	int  JieSanNum;     // 解散数量
	int  Status;        // 状态
} Accout, *PAccout;

class Game
{
public:
	~Game();
	/* 
		获得配置文件中的帐号 
		返回帐号数量
	*/
	int GetConfAccount();
	/*
		获得帐号状态
		index=索引
	*/
	int GetAccoutStatus(int index);
	/* 开始执行 */
	void Start();
	/* 登录 */
	bool Login(char* mail, char* password);
	/* 连接游戏服务器 */
	void Connect();
	/*
		获取用户ID
		data=请求106.75.17.197获取到的数据
	*/
	int  GetAccoutId(const char* data);
	/*
	设置帐号成功
	*/
	void SetAccountSuceess();
	/*
	设置帐号失败
	*/
	void SetAccountFailed();
	/*
		获取游戏服务器IP地址和端口
		data=请求106.75.17.197获取到的数据
	*/
	bool GetTcpServerInfo(const char* data);
	/* 从106.75.17.197获得登录信息的数据 */
	void MakeTcpLogin();
	/* 连接正式服务器的登录数据 */
	void MakeTcpFirstStr();
	/* 暂无 */
	void MakeTcpConfirmLogin();
	/* 获取声望数据 */
	void MakeTcpGetShengWang();
	/* 获取资源数据 */
	void MakeTcpGetZY();
	/* 慈善 */
	void MakeTcpCiShan();
	/*
		解散兵力的数据
		num=解散数量
	*/
	void MakeTcpJieSan(int num);
	/*
		制作数据
		op=操作指令
		id=资源ID
	*/
	void MakeTcpStr(int op, int id);
	/*
		获取操作码
		data=游戏服务器返回的数据
	*/
	int GetOpCode(const char* data);
	/*
		获取声望值
		data=游戏服务器返回的数据
	*/
	int GetShengWang(const char* data);
	/*
		获得训练营ID
		data=游戏服务器返回的数据
		length=data长度
	*/
	int  GetXLYId(const char* data, int length);
	/* 发送数据到游戏服务器 */
	int  Send();
	/* 设置WistoneId */
	void SetWistoneId(char* wistoneId);
	/* 设置解密后的WistoneId */
	void SetUid();
	void SetSocket(SOCKET);
	Client& GetClient();
	char Hex2Char(char ch, char ch2);
	char Char2Num(char ch);
	void UpdateAccountStatusText(char* text, int row=-1);
	static void OnConnectError(const char*, USHORT);
	static void OnConnect(const char*, USHORT, SOCKET);
	static void OnRead(const char*, USHORT, const char*, int, const char*);
	static void OnClose(const char*, USHORT);
	static void OnError(const char*, USHORT);
	static void OnTimeOut(const char*, USHORT, int);
	static Game* GetInstance();
public:
	/* 是否开始 */
	bool start;
	int    m_UpNum = 0;
	/* 写入List控件帐号列表函数 */
	InsertAccount InsertAccountFunc;
	/* 更新List控件文字函数 */
	UpdateText UpdateTextFunc;
	/* 写入List控件日记函数 */
	InsertLog InsertLogFunc;
	/* 写入Static控件函数 */
	SetText SetTextFunc;
	/* 帐号列表控件重绘 */
	ListReDraw ListReDrawFunc;
	/* 一些配置 */
	struct {
		int  AccoutInterval; // 每个帐号登录间隔
		int  AccountTimeOut; // 每个帐号最大超时
		int  StartNum;       // 帐号列表开始执行序号
		int Close;           // 是否是关门操作
	} m_stConfig;
protected:
	/* 当前执行帐号序号 */
	int    m_iAccoutCurrentNum;
	/* 帐号总数量 */
	int    m_iAccountNum;
	/* 帐号列表 */
	Accout* m_stAccoutList;
	/* 成功数量 */
	int m_iSuccessNum;
	/* 失败数量 */
	int m_iFailNum;
	/* */
	Client* m_pClient;
	/* */
	SOCKET m_Socket;
	/* 游戏服务器地址 */
	char   m_ServerHost[32];
	/* 游戏服务器端口 */
	int    m_ServerPort;
	/* 游戏的区服 */
	char   m_ServerId[16];
	/* WIST */
	char   m_Wist[4];
	/* WST */
	char   m_Wst[64];
	/* WistoneId */
	char   m_WistoneId[64];
	/* 解密后的WistoneId */
	char   m_Uid[32];
	/* 用户帐号ID */
	int    m_iAccountId;
	/* 发送数据[会new申请空间] */
	byte*  m_SendStr;
	/* 发送长度 */
	int    m_SendLength;
private:
	Game();
	static Game* self;
};