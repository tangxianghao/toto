#pragma once
#include "../Common/Link.hpp"

class  Game;
class  Explode;
struct struct_jianzhu;
enum   JZ_TYPE;

// ���̽ṹ��
typedef struct struct_liucheng
{
	int      op;         // ����ָ��
	int      value;      // ����ֵ
	int      mid;        // �м�ֵ
	int      extra[4];   // ����ֵ
	int      byte;       // ����ֵ�ֽ��� 
	bool     ismid;      // �Ƿ����м�ֵ
	bool     complete;   // �Ƿ����
	char     cmd[128];   // ԭ�ַ�ָ��
	bool     sendneed;   // �Ƿ��Ѿ����Ͳ�ѯ
	bool     exec;       // �Ƿ���ִ��
	bool     noconf;     // �������������
	struct {
		int type;        // ��������
		int num;         // ��������
	} jiasu[8];          // ��������
	int      jiasunum;   // ������������
	struct_jianzhu* jz;  // ��Ӧ����ָ��
} LiuCheng;

// ִ�ж��нṹ��
typedef struct struct_duilie
{
	LiuCheng* lc;
} DuiLie;

// ��Ϸ������Ϣ����
class GameProc
{
public:
	// ����Gameָ��
	GameProc(Game* game);
	~GameProc();
	// ������Ӧ����
	void SetData(const char* data, int length, char flag, bool success);
	// ����һЩ����
	void SendTheData();
	// ��¼��Ӧ
	void Login();
	// ���괦��
	void Coor();
	// ��������
	void ShengWang();
	// ��Դ
	void ViewRes();
	// ����-������
	void JianZhu_JS();
	// ����-������
	void JianZhu_ZY();
	// ��������
	void SaveJianZhu(const char* data, struct_jianzhu* jz, int no);
	// ������������ɽ�������
	int SetZYNum(int szt_level);
	// ��������
	void JianZhu_JianZao();
	// ��������
	void JianZhu_ShengJi();
	//����
	void Chai_JianZhu();
	// ����������������
	void JianZhu_ShengJiNeed();
	// ��Ѷ���
	void Free();
	// �Ƽ��б�
	void ViewKeJi();
	// ����ļ�б�
	void ZhaoMuList();
	// �鿴��Ʒ
	void ViewGoods();
	// ʹ����Ʒ
	void UseGoods();
	// �鿴��ǰ��������
	void CurrentTask();
	// �鿴�����Ƿ�����
	void ViewTask();
	// ѵ��
	void XunLian();
	// ��ɢ����
	void JieSan();
	// ����
	void CiShan();
	// �鿴ǩ��
	void ViewQianDao();
	// ǩ��
	void QianDao();
	//��ȡ�����б�������������
	void RwList();
	//��ȡ������Ϣ
	void City();
	// ��ȡ��������
	bool ReadLiuCheng();
	// ִ�ж���[�����̷���true��û�з���false]
	bool ExecDuiLie();
	// �������Ƿ�ȴ�
	bool CheckWaitDuiLie(LiuCheng* p);
	// ִ������
	bool ExecLiuCheng(LiuCheng* p);
	// �������������
	int  CalcCompleteLiuCheng();
	// ��ȡ��ǰ����
	LiuCheng* GetCurrentLiuCheng();
	// ��ȡ��һ������
	LiuCheng* GetNextLiuCheng();
	// ��ɵ�ǰ����
	LiuCheng* CompleteLiuCheng();
	// 
	LiuCheng* TransFormOP(Explode& lcLine);
	// �����Ƿ�����
	bool IsCompleteLiuCheng(LiuCheng* p);
	// ��������ת����ԴID(�������Ƽ����ƴ���)
	int TransFormRes(const char* data);
	// ת�ɼ���
	int TransFormJiaSu(const char* data, LiuCheng* p);
	// ��������ת������
	void TransToTextJZ(JZ_TYPE type, char text[]);
	// �����Ƿ�ɺ��Լ��
	bool CanIgnoreCk(struct_jianzhu* jz);
	// ���Ƶ�������������
	char* CopyToDuiLieText(char* text);
	// ��ն�����������
	char* EmptyDuiLieText();
	// �Ƿ�����
	bool IsLocked();
	// ����
	void Lock();
	// ����
	void UnLock();
	// �Ƿ�ִ��������

	// �Ƿ�ر���Ϸ
	bool IsCloseGame();
private:
	// �Ƿ�����
	bool  m_bLock = false;
	// �Ƿ��һ�μ���
	bool m_bFirstCalc = true;
	Game* m_pGame;
	const char* m_cData;
	int  m_iDataLength;
	char m_chFlag;
	bool m_bSuccess;
	// �Ѿ��ӷ�������ȡ����ʲô��Ϣ
	int m_iIsGetValue = 0;
	// �ϴ�ִ��δ��ɵĲ�������
	int m_iLastExecCount = 0;
	// �ϴ�ִ��δ��ɵĲ��ȴ�����
	int m_iLastNoWaitCount = 0;
	// ���ȴ���������
	int m_iNoWaitCount = 0;
	// ���֧�ֶ�����
	int m_iDuiLieMax = 2;
	// ����ִ�ж���
	Link<DuiLie*>   m_DuiLie; 
	// ����
	Link<LiuCheng*> m_Link;
	// ��������
	char* m_cDuiLie;
};