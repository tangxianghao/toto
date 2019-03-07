#pragma once

#include "../Common/HttpClient.h"
#include "../Socket/Client.h"

#define STATUS_NO_START    0x00        
#define STATUS_LOGIN_FAILD 0x01
#define STATUS_SUCCESS     0x02
#define STATUS_TIME_OUT    0x03
#define STATUS_ING         0x04

// ��¼������
#define OP_LOGIN     0x000003e9
// �������ݲ�����
#define OP_SHENGWANG 0x000003f9
// �������ݲ�����
#define OP_JIANZHU   0x00004269
// ѵ��������
#define OP_XUNLIAN   0x00000bb9
// ��ɢ������
#define OP_JIESAN    0x00000bba
// ���Ʋ�����
#define OP_CISHAN    0x000007e4

typedef unsigned char byte;
typedef void(*InsertAccount)(int, int, char*);
typedef void (*InsertLog)(wchar_t*);
typedef void(*SetText)(int, int, int);
typedef void(*UpdateText)(int, int, char*);
typedef void(*ListReDraw)();

typedef struct struct_accout {
	char Mail[128];     // ����
	char Password[128]; // ����
	int  JieSanNum;     // ��ɢ����
	int  Status;        // ״̬
} Accout, *PAccout;

class Game
{
public:
	~Game();
	/* 
		��������ļ��е��ʺ� 
		�����ʺ�����
	*/
	int GetConfAccount();
	/*
		����ʺ�״̬
		index=����
	*/
	int GetAccoutStatus(int index);
	/* ��ʼִ�� */
	void Start();
	/* ��¼ */
	bool Login(char* mail, char* password);
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
	/* ���� */
	void MakeTcpConfirmLogin();
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
	void MakeTcpStr(int op, int id);
	/*
		��ȡ������
		data=��Ϸ���������ص�����
	*/
	int GetOpCode(const char* data);
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
	/* �������ݵ���Ϸ������ */
	int  Send();
	/* ����WistoneId */
	void SetWistoneId(char* wistoneId);
	/* ���ý��ܺ��WistoneId */
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
	/* �Ƿ�ʼ */
	bool start;
	int    m_UpNum = 0;
	/* д��List�ؼ��ʺ��б��� */
	InsertAccount InsertAccountFunc;
	/* ����List�ؼ����ֺ��� */
	UpdateText UpdateTextFunc;
	/* д��List�ؼ��ռǺ��� */
	InsertLog InsertLogFunc;
	/* д��Static�ؼ����� */
	SetText SetTextFunc;
	/* �ʺ��б�ؼ��ػ� */
	ListReDraw ListReDrawFunc;
	/* һЩ���� */
	struct {
		int  AccoutInterval; // ÿ���ʺŵ�¼���
		int  AccountTimeOut; // ÿ���ʺ����ʱ
		int  StartNum;       // �ʺ��б�ʼִ�����
		int Close;           // �Ƿ��ǹ��Ų���
	} m_stConfig;
protected:
	/* ��ǰִ���ʺ���� */
	int    m_iAccoutCurrentNum;
	/* �ʺ������� */
	int    m_iAccountNum;
	/* �ʺ��б� */
	Accout* m_stAccoutList;
	/* �ɹ����� */
	int m_iSuccessNum;
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
	byte*  m_SendStr;
	/* ���ͳ��� */
	int    m_SendLength;
private:
	Game();
	static Game* self;
};