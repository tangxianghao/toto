#pragma once

#include "../Common/HttpClient.h"
#include "../Socket/Client.h"

#define STATUS_NO_START    0x00        
#define STATUS_LOGIN_FAILD 0x01
#define STATUS_SUCCESS     0x02
#define STATUS_TIME_OUT    0x03
#define STATUS_ING         0x04        

typedef unsigned char byte;
typedef void(*InsertAccount)(int, int, char*);
typedef void (*InsertLog)(wchar_t*);
typedef void(*SetText)(int, int, int);
typedef void(*ListReDraw)();

typedef struct struct_accout {
	char Mail[128];     // �����ʺ�
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
	/* ��ȡ��Դ���� */
	void MakeTcpGetZY();
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
		�Ƿ�����Դ����
		data=��Ϸ���������ص�����
	*/
	bool IsGetZY(const char* data);
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
	static void OnConnectError(const char*, USHORT);
	static void OnConnect(const char*, USHORT, SOCKET);
	static void OnRead(const char*, USHORT, const char*, int, const char*);
	static void OnClose(const char*, USHORT);
	static void OnError(const char*, USHORT);
	static void OnTimeOut(const char*, USHORT, int);
	static Game* GetInstance();
public:
	int    m_UpNum = 0;
	/* д��List�ؼ��ʺ��б��� */
	InsertAccount InsertAccountFunc;
	/* д��List�ؼ��ռǺ��� */
	InsertLog InsertLogFunc;
	/* д��Static�ؼ����� */
	SetText SetTextFunc;
	/* �ʺ��б�ؼ��ػ� */
	ListReDraw ListReDrawFunc;
	/* һЩ���� */
	struct {
		int AccoutInterval; // ÿ���ʺŵ�¼���
		int AccountTimeOut; // ÿ���ʺ����ʱ
		int StartNum;       // �ʺ��б�ʼִ�����
	} m_stConfig;
protected:
	/* �ʺ����� */
	int    m_iAccountNum;
	/* �ʺ��б� */
	Accout* m_stAccoutList;
	/* */
	Client m_Client;
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