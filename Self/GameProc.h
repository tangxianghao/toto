#pragma once
#include "../Common/Link.hpp"

class  Game;
class  Explode;
struct struct_jianzhu;
enum   JZ_TYPE;

// 流程结构体
typedef struct struct_liucheng
{
	int      op;         // 操作指令
	int      value;      // 操作值
	int      mid;        // 中间值
	int      extra[4];   // 额外值
	int      byte;       // 操作值字节数 
	bool     ismid;      // 是否有中间值
	bool     complete;   // 是否完成
	char     cmd[128];   // 原字符指令
	bool     sendneed;   // 是否已经发送查询
	bool     exec;       // 是否已执行
	bool     noconf;     // 不是配置里面的
	struct {
		int type;        // 加速类型
		int num;         // 加速数量
	} jiasu[8];          // 加速数组
	int      jiasunum;   // 加速数组数量
	struct_jianzhu* jz;  // 对应建筑指针
} LiuCheng;

// 执行队列结构体
typedef struct struct_duilie
{
	LiuCheng* lc;
} DuiLie;

// 游戏返回信息处理
class GameProc
{
public:
	// 设置Game指针
	GameProc(Game* game);
	~GameProc();
	// 设置响应数据
	void SetData(const char* data, int length, char flag, bool success);
	// 发送一些数据
	void SendTheData();
	// 登录响应
	void Login();
	// 坐标处理
	void Coor();
	// 声望处理
	void ShengWang();
	// 资源
	void ViewRes();
	// 建筑-军事区
	void JianZhu_JS();
	// 建筑-自由区
	void JianZhu_ZY();
	// 建筑保存
	void SaveJianZhu(const char* data, struct_jianzhu* jz, int no);
	// 设置自由区域可建造数量
	int SetZYNum(int szt_level);
	// 建筑建造
	void JianZhu_JianZao();
	// 建筑升级
	void JianZhu_ShengJi();
	//拆建筑
	void Chai_JianZhu();
	// 建筑升级所需条件
	void JianZhu_ShengJiNeed();
	// 免费队列
	void Free();
	// 科技列表
	void ViewKeJi();
	// 可招募列表
	void ZhaoMuList();
	// 查看物品
	void ViewGoods();
	// 使用物品
	void UseGoods();
	// 查看当前主线任务
	void CurrentTask();
	// 查看任务是否可完成
	void ViewTask();
	// 训练
	void XunLian();
	// 解散处理
	void JieSan();
	// 慈善
	void CiShan();
	// 查看签到
	void ViewQianDao();
	// 签到
	void QianDao();
	//读取任务列表完成已完成任务
	void RwList();
	//读取城市信息
	void City();
	// 读取升级流程
	bool ReadLiuCheng();
	// 执行队列[有流程返回true，没有返回false]
	bool ExecDuiLie();
	// 检查队列是否等待
	bool CheckWaitDuiLie(LiuCheng* p);
	// 执行流程
	bool ExecLiuCheng(LiuCheng* p);
	// 计算已完成流程
	int  CalcCompleteLiuCheng();
	// 获取当前流程
	LiuCheng* GetCurrentLiuCheng();
	// 获取下一个流程
	LiuCheng* GetNextLiuCheng();
	// 完成当前流程
	LiuCheng* CompleteLiuCheng();
	// 
	LiuCheng* TransFormOP(Explode& lcLine);
	// 流程是否可完成
	bool IsCompleteLiuCheng(LiuCheng* p);
	// 流程文字转成资源ID(建筑，科技名称代码)
	int TransFormRes(const char* data);
	// 转成加速
	int TransFormJiaSu(const char* data, LiuCheng* p);
	// 建筑类型转成文字
	void TransToTextJZ(JZ_TYPE type, char text[]);
	// 建筑是否可忽略检查
	bool CanIgnoreCk(struct_jianzhu* jz);
	// 复制到队列文字描述
	char* CopyToDuiLieText(char* text);
	// 清空队列文字内容
	char* EmptyDuiLieText();
	// 是否锁定
	bool IsLocked();
	// 锁定
	void Lock();
	// 解锁
	void UnLock();
	// 是否执行了流程

	// 是否关闭游戏
	bool IsCloseGame();
private:
	// 是否锁定
	bool  m_bLock = false;
	// 是否第一次计算
	bool m_bFirstCalc = true;
	Game* m_pGame;
	const char* m_cData;
	int  m_iDataLength;
	char m_chFlag;
	bool m_bSuccess;
	// 已经从服务器获取到了什么信息
	int m_iIsGetValue = 0;
	// 上次执行未完成的操作数量
	int m_iLastExecCount = 0;
	// 上次执行未完成的不等待数量
	int m_iLastNoWaitCount = 0;
	// 不等待队列数量
	int m_iNoWaitCount = 0;
	// 最大支持队列数
	int m_iDuiLieMax = 2;
	// 正在执行队列
	Link<DuiLie*>   m_DuiLie; 
	// 流程
	Link<LiuCheng*> m_Link;
	// 队列文字
	char* m_cDuiLie;
};