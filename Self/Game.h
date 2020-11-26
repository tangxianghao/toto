#pragma once

#include "../Common/HttpClient.h"
#include "../Socket/Client.h"
#include <mysql.h>

#define STATUS_NO_START    0x00        
#define STATUS_LOGIN_FAILD 0x01
#define STATUS_SUCCESS     0x02
#define STATUS_TIME_OUT    0x03
#define STATUS_ING         0x04

#define DATA_UKNOW       0x00000000 // 未知
#define OP_UNKNOW        0x00000000 // 未知指令
#define OP_VIEWRES       0x000007d3 // 查看现有资源
#define OP_YIMIN         0x000007e1 // 移民
#define OP_VIEWKEJI      0x00000fa1 // 查看科技
#define OP_YANJIU        0x00000fa2 // 研究科技
#define OP_LIANTIAO      0x00001773 // 聊天
#define OP_LIANTIAO_XT   0x00001774 // 聊天系统
#define OP_USEGOODS      0x00001f41 // 使用物品
#define OP_VIEWGOODS     0x00001f45 // 查看物品
#define OP_VIEWTASK      0x00002712 // 查询任务是否可完成
#define OP_CPTTASK       0x00002713 // 完成任务
#define OP_VIEWCURTASK   0x00002718 // 查询下一个主线任务
#define OP_ZHAOMULIST    0x00002af9 // 可招募军官列表 
#define OP_ZHAOMU        0x00002afc // 招募
#define OP_LOGIN         0x000003e9 // 登录操作码
#define OP_COOR          0x000007d1 // 坐标数据操作码
#define OP_CISHAN        0x000007e4 // 慈善操作码
#define OP_SHENGWANG     0x000003f9 // 声望数据操作码
#define OP_XUNLIAN       0x00000bbc // 训练操作码
#define OP_JIESAN        0x00000bba // 解散操作码
#define OP_JIANZHU_JS    0x00004269 // 建筑数据-军事区
#define OP_JIANZHU_ZY    0x0000426a // 建筑数据-资源田
#define OP_JIANZAO       0x0000426b // 建筑建造
#define OP_SHENGJI       0x0000426e // 建筑升级
#define OP_SHENGJINEED   0x0000426f // 建筑升级所需条件
#define OP_CHAI_JZ		 0x0000426d // 拆建筑
#define OP_SHENGJI_JIASU 0x00004272 // 建筑升级加速
#define OP_FREE          0x00004275 // 免费队列
#define OP_VIEWQIANDAO   0x000055fa // 查看签到
#define OP_QIANDAO       0x000055fb // 签到
#define RW_LIST			 0x00002711 // 任务列表
#define RW_LIST_1        0x01 // 任务列表序号
#define RW_LIST_2        0x02 // 任务列表序号
#define OP_CITY          0x000007eb // 城市信息



#define JZ_MAX        64         // 最大建筑数量
#define KJ_MAX        32         // 最大科技数量

#define FREE_SECOND   300        // 游戏建筑免费时间(秒)

// 建筑区域
enum JZ_AREA
{
	JZ_AREA_JS = 0x01, // 军事区
	JZ_AREA_ZY = 0x02, // 自由区
};

// 建筑类型
enum JZ_TYPE
{
	JZ_SHIZHENG  = 0x01,  // 市政厅
	JZ_MINJU     = 0x02,  // 民居
	JZ_NONGTIAN  = 0x03,  // 农田
	JZ_LIANGANG  = 0x04,  // 炼钢厂
	JZ_SHIYOU    = 0x05,  // 石油基地
	JZ_XIKUANG   = 0x06,  // 稀矿石
	JZ_WEIQIANG  = 0x07,  // 围墙
	JZ_KEYAN     = 0x08,  // 科研中心
	JZ_JUNXIAO   = 0x09,  // 军校
	JZ_CANMOUBU  = 0x0a,  // 参谋部
	JZ_UNKONOW2  = 0x0b,  // --
	JZ_CANGKU    = 0x0c,  // 仓库
	JZ_SILINGBU  = 0x0d,  // 司令部
	JZ_JUNGONG   = 0x0e,  // 军工厂
	JZ_LIANLUO   = 0x0f,  // 联络中心
	JZ_QINGCHANG = 0x10,  // 轻工厂
};

// 科技类型
enum KJ_TYPE
{
	KJ_LIANGSHI = 0x01,  // 粮食生产
	KJ_GANGTIE  = 0x02,  // 钢铁生产
	KJ_SHIYOU   = 0x03,  // 石油生产
	KJ_XIKUANG  = 0x04,  // 稀矿生产
};

// 礼包类型
enum LB_TYPE
{
	LB_XINSHOU_1 = 0x4e,  // 新手礼包1
	LB_XINSHOU_2 = 0x92,  // 新手礼包2
	LB_XINSHOU_3 = 0x93,  // 新手礼包3
	LB_XINSHOU_4 = 0x94,  // 新手礼包4
	LB_XINSHOU_5 = 0x95,  // 新手礼包5
	LB_XINSHOU_6 = 0x96,  // 新手礼包6
	LB_XINSHOU_7 = 0x97,  // 新手礼包7
};

// 优先类型
enum YX_TYPE
{
	YX_ALL = 0x00, // 所有
	YX_MAX = 0x01, // 最大
	YX_MIN = 0x02, // 最小
};

// 任务指针下标
enum RW_INFO
{
	TASK_NUMBER_INDEX = 6,//任务数量开始下标
	TASK_INFO_INDEX = 10,//任务信息开始下标
};

typedef struct struct_accout {
	char Mail[128];     // 邮箱
	char Password[128]; // 密码
	int  JieSanNum;     // 解散数量
	int  Status;        // 状态
} Accout, *PAccout;

// 资源
typedef struct struct_ziyuan {
	__int64 liangshi; // 粮食
	__int64 gangtie;  // 钢铁
	__int64 shiyou;   // 石油
	__int64 xikuang;  // 稀矿
	__int64 huangjin; // 黄金
	__int64 renkou;   // 人口
} ZiYuan;

// 城市信息
typedef struct struct_city_info {
	__int32 minxin; // 民心
	__int32 minyuan; // 民怨
	__int32 all_hj; // 黄金总量
	__int32 shuilv; // 税率
	__int32 hj_cl; // 黄金产量
	__int32 dq_people; // 当前人口
	__int32 all_people; // 总人口
	__int32 kx_people; // 空闲人口
} CityInfo;

// 建筑建造或升级所需条件
typedef struct struct_jianzhu_need {
	__int32 gangtie;  // 钢铁
	__int32 xikuang;  // 稀矿
	__int32 renkou;   // 人口
	__int32 liangshi; // 粮食
	__int32 shiyou;   // 石油
	struct {
		JZ_TYPE type; // 建筑类型
		__int32 level;// 建筑等级
	} jz[8];
	__int32 jz_count; // 要求建筑数量
} JianZhu_Need;

// 建筑结构体
typedef struct struct_jianzhu {
	JianZhu_Need need;      // 建筑建造或升级所需条件
	__int64      code;      // 建筑代码
	__int32      type;      // 建筑类型
	__int32      level;     // 建筑等级
	__int32      no;        // 建筑所在位置编号
	__int64      timeleft;  // 剩余建造时间
	__int64      mdtime;    // 操作时间
	__int64      cpttime;   // 完成时间[正在建造或升级的建筑]
	__int32      sendneed;  // 是否已发送升级需求
	__int32      ready;     // 是否准备好可以操作
	__int32      exec;      // 正在执行
	__int32      free;      // 是否已经点击免费
	__int32      ignoreck;  // 是否忽略检查  
	__int32      status;    // 0-不存在 1-存在
} JianZhu;

// 科技结构体
typedef struct struct_keji {
	KJ_TYPE type;  // 科技类型
	int     level; // 等级
} KeJi;

// 回调执行的类型
enum BACK_TYPE
{
	BACK_WALL = 1, // 围墙
};

class GameAccount;
class GameProc;
struct struct_account;

class Game
{
public:
	Game(GameAccount* pGameAccount);
	~Game();
	/* 初始化数据 */
	void InitData();
	/* 回收数据 */
	void ReleaseData();
	/* 
		获得配置文件中的帐号 
		返回帐号数量
	*/
	int GetConfAccount();
	/*
		获得帐号状态
		index=索引
	*/
	int GetAccoutStatus();
	/* 开始执行 */
	void Start();
	/* 登录 */
	bool Login(char* mail, char* password);
	/* 注册 */
	bool Regist(int big_area, int user_num, char* default_account, char* str_wid_value);
	/* 解密wid */
	bool DecryptWid(char* str_wid_value, char* jm_wid_value);
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
	/* ... */
	void MakeTcpConfirmLogin();
	/* 制作获取数据 */
	void MakeTcpOP(int op, char num=0x00, bool togeth=false);
	/* 制作获取数据 */
	void MakeTcpOPInt(int op, int num = 0x00, bool togeth = false);
	/* 制作获取数据 */
	void MakeTcpOPInt64(int op, __int64 num = 0x00, bool togeth = false);
	/* 制作获取数据 */
	void MakeTcpOPMid(int op, int num, int mid, bool togeth=false);
	/* 制作获取数据 */
	void MakeOP(int op, int num, int mid, int byte, bool ismid, bool togeth);
	/* 建造建筑 */
	void MakeHouse(int type, int pos);
	/* 建筑升级所需 */
	void MakeUpNeed(__int64 code, JZ_TYPE type, int no);
	/* 使用加速 */
	void MakeJiaSu(__int64 code, int jiasu_type);
	/* 获取坐标数据 */
	void MakeTcpGetCoor();
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
	void MakeXunLian(__int64 id);
	void XunLianMore(__int64 id, __int32 num);
	/* 研究科技 */
	void MakeYanJiu(KJ_TYPE type);
	/*
		获取操作码
		data=游戏服务器返回的数据
	*/
	int GetOpCode(const char* data);
	/*
	获取坐标
	data=游戏服务器返回的数据
	x=x坐标变量
	y=y坐标标量
	*/
	void GetCoor(const char* data, int& x, int& y);
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
	/* 获得建筑列表 */
	JianZhu* GetJianZhuList(JZ_TYPE type);
	/* 根据类型获得建筑信息 */
	JianZhu* GetJianZhuByType(JZ_TYPE type, int flag=0);
	/* 根据类型获得正在升级建筑信息 */
	JianZhu* GetJianZhuExecByType(JZ_TYPE type);
	/* 检查建筑等级 */
	bool CheckJianZhuLevel(JZ_TYPE type, int level, bool all);
	/* 获得正在执行的队列建筑数量 */
	int GetJianZhuExecNum(int& no_wait);
	/* 操作正在执行建筑 */
	int HandleExecJianZhu(JianZhu* jz);
	/* 获取建筑空闲位置 */
	int GetJianZhuFreeNo(JZ_AREA area);
	/* 根据类型获取建筑空闲位置 */
	int GetJianZhuFreeNoByType(JZ_TYPE type);
	/* 根据类型获取建筑数量 */
	int GetJianZhuNumByType(JZ_TYPE type, int level=0);
	/* 检查建筑等级是否符合 */
	bool CheckJianZhuLevel(JZ_TYPE type, int level);
	/* 检查资源是否OK */
	bool CheckRes(JianZhu_Need& need);
	/* 检查科技等级 */
	bool CheckKeJiLevel(KJ_TYPE type, int level);
	/* 队列是否可免费 */
	bool CheckCanFree(__int64 timeleft);
	/* 获得可以免费倒计时 */
	int  GetFreeLeft(__int64 timeleft);
	/* 发送数据到游戏服务器 */
	int  Send();
	/* 发送空信息数据到游戏服务器 */
	int  SendEmptyMsg();
	/* 设置WistoneId */
	void SetWistoneId(char* wistoneId);
	/* 设置解密后的WistoneId */
	void SetUid();
	void SetSocket(SOCKET);
	/* 设置登录成功时间 */
	void SetLoginOkTime(time_t t=0);
	/* 在线时长超过设定时间 */
	bool OnLineIsTimeOut();
	Client& GetClient();
	char Hex2Char(char ch, char ch2);
	char Char2Num(char ch);
	// 更新控件文字
	void UpdateText(int col, char* text);
	void UpdateAccountStatusText(char* text, int row=-1);
	void UpdateAccountDuiLieText(char* text);
	static void OnConnectError(Game* game, const char*, USHORT);
	static void OnConnect(Game* game, const char*, USHORT, SOCKET);
	static void OnRead(Game* game, const char*, USHORT, const char*, int, const char*);
	static void OnClose(Game* game, const char*, USHORT);
	static void OnError(Game* game, const char*, USHORT);
	static void OnTimeOut(Game* game, const char*, USHORT, int);
	static Game* GetInstance();
public:
	/* 是否开始 */
	bool start;
	int    m_UpNum = 0;
	//int
	
	/* 一些配置 */
	struct {
		int  AccoutInterval;   // 每个帐号登录间隔
		int  AccountTimeOut;   // 每个帐号最大超时
		int  msLCInterval;     // 每个流程执行间隔
		int  StartNum;         // 帐号列表开始执行序号
		int  WaitSecond;       // 队列等待最大时间
		int  OnlineTimeOut;    // 每个帐号最大在线时长
		int  Close;            // 关门操作
		int  Open;             // 开门操作
		int  JiangSheng;       // 降声望操作
	} m_stConfig;
public:
	MYSQL m_mysqlConnect;
	// 是否初始化
	bool m_bIsInit = false;
	// 登录时间
	time_t m_qwLoginTime = 0;
	// 登录成功时间
	time_t m_qwLoginOkTime = 0;
	// 帐号类
	GameAccount* m_pGameAccount = nullptr;
	// 处理过程类
	GameProc*    m_pGameProc = nullptr;
	/* 帐号列表 */
	struct_account* m_pAccount = nullptr;
	/* 成功数量 */
	int m_iSuccessNum;
	//回调返回类型
	int back_type = 0;
	//回调返回等级
	int back_level = 0;
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
	char  m_SendStr[1024];
	/* 发送长度 */
	int    m_SendLength;
	/* 指令集中在一起 */
	bool   m_SendTogeth = false;
	/* 拥有资源 */
	ZiYuan  m_ZiYuan;
	/* 城市信息 */
	CityInfo m_CityInfo;
	/* 军事区建筑列表 */
	JianZhu m_jsJianZhu[JZ_MAX];
	/* 自由区建筑列表 */
	JianZhu m_zyJianZhu[JZ_MAX];
	/* 自由区建筑可拥有数量 */
	int m_iZYNum = 0x04;
	/* 科技列表 */
	KeJi  m_KeJi[KJ_MAX];
	/* 新手礼包代码 */
	u_int m_iXinShouCode = 0;
	/* 最后一次获得信息时间 */
	time_t m_i64LastGetMsgTime = 0;
	/* 多少秒后自动登录 */
	int    m_iAutoLoginSec = 0;
private:
	static Game* self;
};