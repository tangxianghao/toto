#pragma once

#include "../Common/HttpClient.h"
#include "../Socket/Client.h"
#include <mysql.h>

#define STATUS_NO_START    0x00        
#define STATUS_LOGIN_FAILD 0x01
#define STATUS_SUCCESS     0x02
#define STATUS_TIME_OUT    0x03
#define STATUS_ING         0x04

#define DATA_UKNOW       0x00000000 // δ֪
#define OP_UNKNOW        0x00000000 // δָ֪��
#define OP_VIEWRES       0x000007d3 // �鿴������Դ
#define OP_YIMIN         0x000007e1 // ����
#define OP_VIEWKEJI      0x00000fa1 // �鿴�Ƽ�
#define OP_YANJIU        0x00000fa2 // �о��Ƽ�
#define OP_LIANTIAO      0x00001773 // ����
#define OP_LIANTIAO_XT   0x00001774 // ����ϵͳ
#define OP_USEGOODS      0x00001f41 // ʹ����Ʒ
#define OP_VIEWGOODS     0x00001f45 // �鿴��Ʒ
#define OP_VIEWTASK      0x00002712 // ��ѯ�����Ƿ�����
#define OP_CPTTASK       0x00002713 // �������
#define OP_VIEWCURTASK   0x00002718 // ��ѯ��һ����������
#define OP_ZHAOMULIST    0x00002af9 // ����ļ�����б� 
#define OP_ZHAOMU        0x00002afc // ��ļ
#define OP_LOGIN         0x000003e9 // ��¼������
#define OP_COOR          0x000007d1 // �������ݲ�����
#define OP_CISHAN        0x000007e4 // ���Ʋ�����
#define OP_SHENGWANG     0x000003f9 // �������ݲ�����
#define OP_XUNLIAN       0x00000bbc // ѵ��������
#define OP_JIESAN        0x00000bba // ��ɢ������
#define OP_JIANZHU_JS    0x00004269 // ��������-������
#define OP_JIANZHU_ZY    0x0000426a // ��������-��Դ��
#define OP_JIANZAO       0x0000426b // ��������
#define OP_SHENGJI       0x0000426e // ��������
#define OP_SHENGJINEED   0x0000426f // ����������������
#define OP_CHAI_JZ		 0x0000426d // ����
#define OP_SHENGJI_JIASU 0x00004272 // ������������
#define OP_FREE          0x00004275 // ��Ѷ���
#define OP_VIEWQIANDAO   0x000055fa // �鿴ǩ��
#define OP_QIANDAO       0x000055fb // ǩ��
#define RW_LIST			 0x00002711 // �����б�
#define RW_LIST_1        0x01 // �����б����
#define RW_LIST_2        0x02 // �����б����
#define OP_CITY          0x000007eb // ������Ϣ



#define JZ_MAX        64         // ���������
#define KJ_MAX        32         // ���Ƽ�����

#define FREE_SECOND   300        // ��Ϸ�������ʱ��(��)

// ��������
enum JZ_AREA
{
	JZ_AREA_JS = 0x01, // ������
	JZ_AREA_ZY = 0x02, // ������
};

// ��������
enum JZ_TYPE
{
	JZ_SHIZHENG  = 0x01,  // ������
	JZ_MINJU     = 0x02,  // ���
	JZ_NONGTIAN  = 0x03,  // ũ��
	JZ_LIANGANG  = 0x04,  // ���ֳ�
	JZ_SHIYOU    = 0x05,  // ʯ�ͻ���
	JZ_XIKUANG   = 0x06,  // ϡ��ʯ
	JZ_WEIQIANG  = 0x07,  // Χǽ
	JZ_KEYAN     = 0x08,  // ��������
	JZ_JUNXIAO   = 0x09,  // ��У
	JZ_CANMOUBU  = 0x0a,  // ��ı��
	JZ_UNKONOW2  = 0x0b,  // --
	JZ_CANGKU    = 0x0c,  // �ֿ�
	JZ_SILINGBU  = 0x0d,  // ˾�
	JZ_JUNGONG   = 0x0e,  // ������
	JZ_LIANLUO   = 0x0f,  // ��������
	JZ_QINGCHANG = 0x10,  // �Ṥ��
};

// �Ƽ�����
enum KJ_TYPE
{
	KJ_LIANGSHI = 0x01,  // ��ʳ����
	KJ_GANGTIE  = 0x02,  // ��������
	KJ_SHIYOU   = 0x03,  // ʯ������
	KJ_XIKUANG  = 0x04,  // ϡ������
};

// �������
enum LB_TYPE
{
	LB_XINSHOU_1 = 0x4e,  // �������1
	LB_XINSHOU_2 = 0x92,  // �������2
	LB_XINSHOU_3 = 0x93,  // �������3
	LB_XINSHOU_4 = 0x94,  // �������4
	LB_XINSHOU_5 = 0x95,  // �������5
	LB_XINSHOU_6 = 0x96,  // �������6
	LB_XINSHOU_7 = 0x97,  // �������7
};

// ��������
enum YX_TYPE
{
	YX_ALL = 0x00, // ����
	YX_MAX = 0x01, // ���
	YX_MIN = 0x02, // ��С
};

// ����ָ���±�
enum RW_INFO
{
	TASK_NUMBER_INDEX = 6,//����������ʼ�±�
	TASK_INFO_INDEX = 10,//������Ϣ��ʼ�±�
};

typedef struct struct_accout {
	char Mail[128];     // ����
	char Password[128]; // ����
	int  JieSanNum;     // ��ɢ����
	int  Status;        // ״̬
} Accout, *PAccout;

// ��Դ
typedef struct struct_ziyuan {
	__int64 liangshi; // ��ʳ
	__int64 gangtie;  // ����
	__int64 shiyou;   // ʯ��
	__int64 xikuang;  // ϡ��
	__int64 huangjin; // �ƽ�
	__int64 renkou;   // �˿�
} ZiYuan;

// ������Ϣ
typedef struct struct_city_info {
	__int32 minxin; // ����
	__int32 minyuan; // ��Թ
	__int32 all_hj; // �ƽ�����
	__int32 shuilv; // ˰��
	__int32 hj_cl; // �ƽ����
	__int32 dq_people; // ��ǰ�˿�
	__int32 all_people; // ���˿�
	__int32 kx_people; // �����˿�
} CityInfo;

// ���������������������
typedef struct struct_jianzhu_need {
	__int32 gangtie;  // ����
	__int32 xikuang;  // ϡ��
	__int32 renkou;   // �˿�
	__int32 liangshi; // ��ʳ
	__int32 shiyou;   // ʯ��
	struct {
		JZ_TYPE type; // ��������
		__int32 level;// �����ȼ�
	} jz[8];
	__int32 jz_count; // Ҫ��������
} JianZhu_Need;

// �����ṹ��
typedef struct struct_jianzhu {
	JianZhu_Need need;      // ���������������������
	__int64      code;      // ��������
	__int32      type;      // ��������
	__int32      level;     // �����ȼ�
	__int32      no;        // ��������λ�ñ��
	__int64      timeleft;  // ʣ�ཨ��ʱ��
	__int64      mdtime;    // ����ʱ��
	__int64      cpttime;   // ���ʱ��[���ڽ���������Ľ���]
	__int32      sendneed;  // �Ƿ��ѷ�����������
	__int32      ready;     // �Ƿ�׼���ÿ��Բ���
	__int32      exec;      // ����ִ��
	__int32      free;      // �Ƿ��Ѿ�������
	__int32      ignoreck;  // �Ƿ���Լ��  
	__int32      status;    // 0-������ 1-����
} JianZhu;

// �Ƽ��ṹ��
typedef struct struct_keji {
	KJ_TYPE type;  // �Ƽ�����
	int     level; // �ȼ�
} KeJi;

// �ص�ִ�е�����
enum BACK_TYPE
{
	BACK_WALL = 1, // Χǽ
};

class GameAccount;
class GameProc;
struct struct_account;

class Game
{
public:
	Game(GameAccount* pGameAccount);
	~Game();
	/* ��ʼ������ */
	void InitData();
	/* �������� */
	void ReleaseData();
	/* 
		��������ļ��е��ʺ� 
		�����ʺ�����
	*/
	int GetConfAccount();
	/*
		����ʺ�״̬
		index=����
	*/
	int GetAccoutStatus();
	/* ��ʼִ�� */
	void Start();
	/* ��¼ */
	bool Login(char* mail, char* password);
	/* ע�� */
	bool Regist(int big_area, int user_num, char* default_account, char* str_wid_value);
	/* ����wid */
	bool DecryptWid(char* str_wid_value, char* jm_wid_value);
	/* ������Ϸ������ */
	void Connect();
	/*
		��ȡ�û�ID
		data=����106.75.17.197��ȡ��������
	*/
	int  GetAccoutId(const char* data);
	/*
	�����ʺųɹ�
	*/
	void SetAccountSuceess();
	/*
	�����ʺ�ʧ��
	*/
	void SetAccountFailed();
	/*
		��ȡ��Ϸ������IP��ַ�Ͷ˿�
		data=����106.75.17.197��ȡ��������
	*/
	bool GetTcpServerInfo(const char* data);
	/* ��106.75.17.197��õ�¼��Ϣ������ */
	void MakeTcpLogin();
	/* ������ʽ�������ĵ�¼���� */
	void MakeTcpFirstStr();
	/* ... */
	void MakeTcpConfirmLogin();
	/* ������ȡ���� */
	void MakeTcpOP(int op, char num=0x00, bool togeth=false);
	/* ������ȡ���� */
	void MakeTcpOPInt(int op, int num = 0x00, bool togeth = false);
	/* ������ȡ���� */
	void MakeTcpOPInt64(int op, __int64 num = 0x00, bool togeth = false);
	/* ������ȡ���� */
	void MakeTcpOPMid(int op, int num, int mid, bool togeth=false);
	/* ������ȡ���� */
	void MakeOP(int op, int num, int mid, int byte, bool ismid, bool togeth);
	/* ���콨�� */
	void MakeHouse(int type, int pos);
	/* ������������ */
	void MakeUpNeed(__int64 code, JZ_TYPE type, int no);
	/* ʹ�ü��� */
	void MakeJiaSu(__int64 code, int jiasu_type);
	/* ��ȡ�������� */
	void MakeTcpGetCoor();
	/* ��ȡ�������� */
	void MakeTcpGetShengWang();
	/* ��ȡ��Դ���� */
	void MakeTcpGetZY();
	/* ���� */
	void MakeTcpCiShan();
	/*
		��ɢ����������
		num=��ɢ����
	*/
	void MakeTcpJieSan(int num);
	/*
		��������
		op=����ָ��
		id=��ԴID
	*/
	void MakeXunLian(__int64 id);
	void XunLianMore(__int64 id, __int32 num);
	/* �о��Ƽ� */
	void MakeYanJiu(KJ_TYPE type);
	/*
		��ȡ������
		data=��Ϸ���������ص�����
	*/
	int GetOpCode(const char* data);
	/*
	��ȡ����
	data=��Ϸ���������ص�����
	x=x�������
	y=y�������
	*/
	void GetCoor(const char* data, int& x, int& y);
	/*
		��ȡ����ֵ
		data=��Ϸ���������ص�����
	*/
	int GetShengWang(const char* data);
	/*
		���ѵ��ӪID
		data=��Ϸ���������ص�����
		length=data����
	*/
	int  GetXLYId(const char* data, int length);
	/* ��ý����б� */
	JianZhu* GetJianZhuList(JZ_TYPE type);
	/* �������ͻ�ý�����Ϣ */
	JianZhu* GetJianZhuByType(JZ_TYPE type, int flag=0);
	/* �������ͻ����������������Ϣ */
	JianZhu* GetJianZhuExecByType(JZ_TYPE type);
	/* ��齨���ȼ� */
	bool CheckJianZhuLevel(JZ_TYPE type, int level, bool all);
	/* �������ִ�еĶ��н������� */
	int GetJianZhuExecNum(int& no_wait);
	/* ��������ִ�н��� */
	int HandleExecJianZhu(JianZhu* jz);
	/* ��ȡ��������λ�� */
	int GetJianZhuFreeNo(JZ_AREA area);
	/* �������ͻ�ȡ��������λ�� */
	int GetJianZhuFreeNoByType(JZ_TYPE type);
	/* �������ͻ�ȡ�������� */
	int GetJianZhuNumByType(JZ_TYPE type, int level=0);
	/* ��齨���ȼ��Ƿ���� */
	bool CheckJianZhuLevel(JZ_TYPE type, int level);
	/* �����Դ�Ƿ�OK */
	bool CheckRes(JianZhu_Need& need);
	/* ���Ƽ��ȼ� */
	bool CheckKeJiLevel(KJ_TYPE type, int level);
	/* �����Ƿ����� */
	bool CheckCanFree(__int64 timeleft);
	/* ��ÿ�����ѵ���ʱ */
	int  GetFreeLeft(__int64 timeleft);
	/* �������ݵ���Ϸ������ */
	int  Send();
	/* ���Ϳ���Ϣ���ݵ���Ϸ������ */
	int  SendEmptyMsg();
	/* ����WistoneId */
	void SetWistoneId(char* wistoneId);
	/* ���ý��ܺ��WistoneId */
	void SetUid();
	void SetSocket(SOCKET);
	/* ���õ�¼�ɹ�ʱ�� */
	void SetLoginOkTime(time_t t=0);
	/* ����ʱ�������趨ʱ�� */
	bool OnLineIsTimeOut();
	Client& GetClient();
	char Hex2Char(char ch, char ch2);
	char Char2Num(char ch);
	// ���¿ؼ�����
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
	/* �Ƿ�ʼ */
	bool start;
	int    m_UpNum = 0;
	//int
	
	/* һЩ���� */
	struct {
		int  AccoutInterval;   // ÿ���ʺŵ�¼���
		int  AccountTimeOut;   // ÿ���ʺ����ʱ
		int  msLCInterval;     // ÿ������ִ�м��
		int  StartNum;         // �ʺ��б�ʼִ�����
		int  WaitSecond;       // ���еȴ����ʱ��
		int  OnlineTimeOut;    // ÿ���ʺ��������ʱ��
		int  Close;            // ���Ų���
		int  Open;             // ���Ų���
		int  JiangSheng;       // ����������
	} m_stConfig;
public:
	MYSQL m_mysqlConnect;
	// �Ƿ��ʼ��
	bool m_bIsInit = false;
	// ��¼ʱ��
	time_t m_qwLoginTime = 0;
	// ��¼�ɹ�ʱ��
	time_t m_qwLoginOkTime = 0;
	// �ʺ���
	GameAccount* m_pGameAccount = nullptr;
	// ���������
	GameProc*    m_pGameProc = nullptr;
	/* �ʺ��б� */
	struct_account* m_pAccount = nullptr;
	/* �ɹ����� */
	int m_iSuccessNum;
	//�ص���������
	int back_type = 0;
	//�ص����صȼ�
	int back_level = 0;
	/* ʧ������ */
	int m_iFailNum;
	/* */
	Client* m_pClient;
	/* */
	SOCKET m_Socket;
	/* ��Ϸ��������ַ */
	char   m_ServerHost[32];
	/* ��Ϸ�������˿� */
	int    m_ServerPort;
	/* ��Ϸ������ */
	char   m_ServerId[16];
	/* WIST */
	char   m_Wist[4];
	/* WST */
	char   m_Wst[64];
	/* WistoneId */
	char   m_WistoneId[64];
	/* ���ܺ��WistoneId */
	char   m_Uid[32];
	/* �û��ʺ�ID */
	int    m_iAccountId;
	/* ��������[��new����ռ�] */
	char  m_SendStr[1024];
	/* ���ͳ��� */
	int    m_SendLength;
	/* ָ�����һ�� */
	bool   m_SendTogeth = false;
	/* ӵ����Դ */
	ZiYuan  m_ZiYuan;
	/* ������Ϣ */
	CityInfo m_CityInfo;
	/* �����������б� */
	JianZhu m_jsJianZhu[JZ_MAX];
	/* �����������б� */
	JianZhu m_zyJianZhu[JZ_MAX];
	/* ������������ӵ������ */
	int m_iZYNum = 0x04;
	/* �Ƽ��б� */
	KeJi  m_KeJi[KJ_MAX];
	/* ����������� */
	u_int m_iXinShouCode = 0;
	/* ���һ�λ����Ϣʱ�� */
	time_t m_i64LastGetMsgTime = 0;
	/* ��������Զ���¼ */
	int    m_iAutoLoginSec = 0;
private:
	static Game* self;
};