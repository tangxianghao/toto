#pragma once

#ifndef IDC_EDIT_LOGIN_INTERVAL
#define IDC_EDIT_LOGIN_INTERVAL         1003
#define IDC_EDIT_LOGIN_TIMEOUT          1004
#define IDC_EDIT_START_KEY              1005
#define IDC_STATIC_ACCOUT_NUM           1008
#define IDC_STATIC_SUC                  1009
#define IDC_STATIC_FAILED               1010
#endif

#define STATUS_NO_START    0x00        
#define STATUS_LOGIN_FAILD 0x01
#define STATUS_SUCCESS     0x02
#define STATUS_TIME_OUT    0x03
#define STATUS_ING         0x04

typedef struct struct_account {
	char    Mail[128];     // ����
	char    Password[128]; // ����
	__int32 Status;        // ״̬
	__int64 UpTextTime;    // �����ڿؼ���ʱ��
	__int64 CptTime;       // ����������ʱ��
	__int32 index;         // ���б�������
	bool    Used;          // �Ƿ���ʹ��
} Account, *PAccount;

// ��Ϸ�˺�
class GameAccount
{
public:
	// ��ȡ�����ļ�������ʺ�
	int ReadConfAccount();
	// ��ȡ����Ϸ�ʺ�
	Account* GetPlayAccount();
	// ����Ƿ����
	bool AccountCanUse(Account* p, __int64 now_time=0);
	// ��һ��
	int      Next();
	// ����δʹ��
	void UnUsed(Account* p);
	// ���¿ؼ�����
	void UpdateText(Account* account, int col, char* text);
	// ����״̬������
	void UpdateStatusText(Account* account, char* text);
	// ���¶�������
	void UpdateDuiLieText(Account* account, char* text);
	// �ػ�ؼ�
	void ListReDraw();
	// д���ռǿؼ�
	void InsertLog(wchar_t* text);
	// д���ı��ؼ�����
	void SetText(int id, int num, int num2 = -1);
public:
	// ����
	bool m_bLock = false;
	// ��ǰ�ʺ�����
	int m_iCurrent = 0;
	// �ʺ�����Ŀ
	int m_iCount = 0;
	// �ʺ��б�
	Account* m_pAccounts;
protected:
private:
};