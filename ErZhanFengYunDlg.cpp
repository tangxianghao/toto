
// ErZhanFengYunDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ErZhanFengYun.h"
#include "ErZhanFengYunDlg.h"
#include "afxdialogex.h"
#include "Common/Json.h"
#include "Common/HttpClient.h"
#include "Common/mystring.h"
#include "self/GameAccount.h"
#include "Self/Game.h"
#include "Self/GameProc.h"
#include "Encrypt/Des.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CErZhanFengYunDlg* g_dlg = nullptr;
// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CErZhanFengYunDlg 对话框



CErZhanFengYunDlg::CErZhanFengYunDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_ERZHANFENGYUN_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CErZhanFengYunDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ACCOUNT, m_ListCtrAccout);
	DDX_Control(pDX, IDC_LIST_LOG, m_ListCtrLog);
}



BEGIN_MESSAGE_MAP(CErZhanFengYunDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CErZhanFengYunDlg::OnBnClickedOk)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_ACCOUNT, &CErZhanFengYunDlg::OnNMCustomdrawListAccount)
	ON_BN_CLICKED(IDC_BUTTON_PAUSE, &CErZhanFengYunDlg::OnBnClickedButtonPause)
	ON_BN_CLICKED(IDC_REGIST, &CErZhanFengYunDlg::OnBnClickedRegist)
	ON_EN_CHANGE(IDC_EDIT_LCLEFT, &CErZhanFengYunDlg::OnEnChangeEditLcleft)
	ON_BN_CLICKED(IDC_JX_WID, &CErZhanFengYunDlg::OnBnClickedJxWid)
END_MESSAGE_MAP()


// CErZhanFengYunDlg 消息处理程序

BOOL CErZhanFengYunDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码

	g_dlg = this;
	//GetDlgItem(IDC_REGIST)->ShowWindow(SW_HIDE);
	DWORD dwStyle = m_ListCtrAccout.GetExtendedStyle();
	dwStyle |= (LVS_REPORT);
	m_ListCtrAccout.SetExtendedStyle(dwStyle);
	m_ListCtrAccout.InsertColumn(0, L"序号", LVCFMT_LEFT, 50);
	m_ListCtrAccout.InsertColumn(1, L"帐号", LVCFMT_LEFT, 200);
	m_ListCtrAccout.InsertColumn(2, L"坐标", LVCFMT_CENTER, 60);
	m_ListCtrAccout.InsertColumn(3, L"声望", LVCFMT_CENTER, 80);
	m_ListCtrAccout.InsertColumn(4, L"区服", LVCFMT_CENTER, 60);
	m_ListCtrAccout.InsertColumn(5, L"状态", LVCFMT_CENTER, 260);
	m_ListCtrAccout.InsertColumn(6, L"队列", LVCFMT_CENTER, 400);

	m_ListCtrLog.InsertColumn(0, L"消息", LVCFMT_LEFT, 450);
	m_ListCtrLog.SetExtendedStyle(dwStyle);

	GetDlgItem(IDC_EDIT_LOGIN_INTERVAL)->SetWindowText(L"100");
	GetDlgItem(IDC_EDIT_LOGIN_TIMEOUT)->SetWindowText(L"5");
	GetDlgItem(IDC_EDIT_LCLEFT)->SetWindowText(L"1500");
	GetDlgItem(IDC_EDIT_START_KEY)->SetWindowText(L"0");
	GetDlgItem(IDC_EDIT_THREAD_COUNT)->SetWindowText(L"1");
	GetDlgItem(IDC_EDIT_ONLINE_TIMEOUT)->SetWindowText(L"1800");
	GetDlgItem(IDC_EDIT_CLOSEGAME)->SetWindowText(L"100");

	((CButton*)g_dlg->GetDlgItem(IDC_CHECK_ONLINE_TIMEOUT))->SetCheck(true);
	((CButton*)g_dlg->GetDlgItem(IDC_CHECK_CLOSEGAME))->SetCheck(true);

	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE);

	AllocConsole();
	freopen("CON", "w", stdout);

	m_pGameAccount = new GameAccount;
	m_pGameAccount->ReadConfAccount();
	
	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CErZhanFengYunDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CErZhanFengYunDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CErZhanFengYunDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CErZhanFengYunDlg::InsertAccout(int row, int col, char* log)
{
	if (g_dlg) {
		CString text;
		if (col == 1) {
			text.Format(L"%d", row + 1);
			g_dlg->m_ListCtrAccout.InsertItem(row, text);
		}
		text = log;
		g_dlg->m_ListCtrAccout.SetItemText(row, col, text);
	}
}

void CErZhanFengYunDlg::InsertAccout(int row, int col, wchar_t* log)
{
	if (g_dlg) {
		if (col == 1) {
			CString text;
			text.Format(L"%d", row + 1);
			g_dlg->m_ListCtrAccout.InsertItem(row, text);
		}
		g_dlg->m_ListCtrAccout.SetItemText(row, col, log);
	}
}

void CErZhanFengYunDlg::InsertLog(wchar_t* log)
{
	if (g_dlg) {
		g_dlg->m_ListCtrLog.InsertItem(g_dlg->m_ListCtrLog.GetItemCount(), log);
		g_dlg->m_ListCtrLog.EnsureVisible(g_dlg->m_ListCtrLog.GetItemCount() - 1, FALSE);
	}
}

void CErZhanFengYunDlg::SetText_M(int id, int num, int num2)
{
	if (g_dlg) {
		CString text;
		if (num2 == -1) {
			text.Format(L"%d", num);
		}
		else {
			text.Format(L"%d / %d", num, num2);
		}
		g_dlg->GetDlgItem(id)->SetWindowText(text);
	}
}

void CErZhanFengYunDlg::UpdateText_M(int row, int col, char* text)
{
	CString tmp;
	tmp = text;
	/*
	LVCOLUMN lvcol;
	lvcol.mask = LVCF_TEXT;
	lvcol.cchTextMax = 256;
	lvcol.pszText = tmp.GetBuffer();
	g_dlg->m_ListCtrAccout.SetColumn(col, &lvcol);
	*/
	g_dlg->m_ListCtrAccout.SetItemText(row, col, tmp);
}

void CErZhanFengYunDlg::ListReDraw()
{
	g_dlg->m_ListCtrAccout.Invalidate();
}

UINT CErZhanFengYunDlg::Run(CErZhanFengYunDlg* dlg)
{
	return 0;
	//监听队列
	// 1222221fff1 120280217
	// chi8885@003.com 123124
	// chi8784@003.com 123124
	// chi10129@003.com 123124
	printf("Run!!!\n");
	while (true) {
		for (int i = 0; i < g_dlg->m_iThreadCount; i++) {
			GameThread* gt = &g_dlg->m_pGameThreads[i];
			Game* game = g_dlg->m_pGameThreads[i].game;
			gt->lock = true;
			if (game && game->m_bIsInit) {
				time_t now_time = getmillisecond();
				if ((now_time - game->m_stConfig.msLCInterval) > gt->exec_time) {
					if (game->m_pGameProc) {
						gt->exec_time = now_time;

						//while (game->m_pGameProc->IsLocked());
						//game->m_pGameProc->Lock();

						game->m_i64LastGetMsgTime = now_time;
						if (((CButton*)g_dlg->GetDlgItem(IDC_CHECK_ONLINE_TIMEOUT))->GetCheck()) {
							if (game->OnLineIsTimeOut()) { // 在线超时
								printf("在线已超过最大允许时间：%d秒！！！\n", game->m_stConfig.OnlineTimeOut);
								game->GetClient().Close(true); // 没有流程可以执行
								continue;
							}
						}
#if 1
						int result = game->m_pGameProc->CalcCompleteLiuCheng();
						printf("result:%d~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n", result);
						if (result != 0) {
							if ((now_time - 15000) > gt->send_time) {
								gt->send_time = now_time;
								game->m_pGameProc->SendTheData();
							}

							if (result == 1) {
								game->m_pGameProc->ExecDuiLie();  // 执行队列
							}
							if (((CButton*)g_dlg->GetDlgItem(IDC_CHECK_CLOSEGAME))->GetCheck()) {
								if (game->m_pGameProc->IsCloseGame()) {
									printf("队列已满，不等待！！！\n");
									game->GetClient().Close(true); // 没有流程可以执行
								}
							}
						}
#endif
						game->m_pGameProc->UnLock();
					}	
				}
				
			}
			gt->lock = false;
		}
		Sleep(50);
	}
	printf("Run End!!!\n");
	return 0;
}

// 开始游戏
UINT CErZhanFengYunDlg::PlayGame(int index)
{
	printf("PlayGame!!!\n");
	Game* game = g_dlg->m_pGameThreads[index].game;
	//初始化连接数据库
	mysql_init(&game->m_mysqlConnect);
	if (!(mysql_real_connect(&game->m_mysqlConnect, "localhost", "root", "root", "erzhan", 3306, NULL, 0))) {
		printf("Failed to access to the database...Error: %s\n", mysql_error(&game->m_mysqlConnect));
		return 0;
	}
	if (!mysql_set_character_set(&game->m_mysqlConnect, "utf-8")) {
		printf("New client character set: %s\n", mysql_character_set_name(&game->m_mysqlConnect));
		return 0;
	}
	//查询用户信息
	//char user_info_sql[128];
	//sprintf(user_info_sql, "SELECT * FROM `cmf_account` WHERE `region`='%s' OR `user_login`='%s' ORDER BY `id` DESC", "142", "32752@s.cn");
	//printf("%s\n", user_info_sql);
	//int res = mysql_query(&game->m_mysqlConnect, user_info_sql);
	//if (res == 0) {
	//	MYSQL_RES* result = mysql_store_result(&game->m_mysqlConnect);
	//	if (result) {
	//		int num_fields = mysql_num_fields(result);
	//		MYSQL_ROW row;
	//		/*row = mysql_fetch_row(result);
	//		printf("%s\n", row[4]);*/
	//		row = mysql_fetch_row(result);
	//		if (getMorningTime() > atoi(row[38])) {
	//			printf("可以领取");
	//		}
	//		else {
	//			printf("不能领");
	//		}
	//		/*printf("值：%d\n", atoi(row[24]));
	//		while (row = mysql_fetch_row(result)) {
	//			for (int i = 0; i < num_fields; i++) {
	//				printf("--%d--", i);
	//				printf("%s ", row[i]);
	//			}
	//			printf("%s\n", row[24]);
	//		}*/
	//	}
	//}
	//return 0;
	game->Start();

	while (g_dlg->m_pGameThreads[index].lock);
	g_dlg->m_pGameThreads[index].lock = true;
	delete g_dlg->m_pGameThreads[index].game;
	g_dlg->m_pGameThreads[index].game = nullptr;
	g_dlg->m_pGameThreads[index].lock = false;

	printf("PlayGame End!!!\n");
	return 0;
}

//注册账号
UINT CErZhanFengYunDlg::PlayRegist(CErZhanFengYunDlg* dlg)
{
	Game game(nullptr);
	//default_account初始化账号有格式规范
	CString str_big_area,str_account_num;

	dlg->GetDlgItem(IDC_BIG_AREA)->GetWindowText(str_big_area);
	//哪个大区
	int big_area = _ttoi(str_big_area);

	dlg->GetDlgItem(IDC_ACCOUNT_NUM)->GetWindowText(str_account_num);
	//账号数量
	int account_num = _ttoi(str_account_num);

	//默认初始化账号
	char default_account[128];
	HWND hWnd = dlg->GetDlgItem(IDC_DEFAULT_ACCOUNT)->m_hWnd;
	::GetWindowTextA(hWnd, default_account, sizeof(default_account));

	//wid
	char str_wid_value[48];
	HWND hWnd_wid = dlg->GetDlgItem(IDC_ACCOUNT_WID)->m_hWnd;
	::GetWindowTextA(hWnd_wid, str_wid_value, sizeof(str_wid_value));
	  
	game.Regist(big_area, account_num, default_account, str_wid_value);
	return 0;
}

UINT CErZhanFengYunDlg::PlayJxWid(CErZhanFengYunDlg* dlg)
{
	Game game(nullptr);

	char str_wid_value[128];
	HWND hWnd_wid = dlg->GetDlgItem(IDC_JM_WID)->m_hWnd;
	::GetWindowTextA(hWnd_wid, str_wid_value, sizeof(str_wid_value));
	char jm_wid_value[30] = { 0 };
	game.DecryptWid(str_wid_value, jm_wid_value);

	HWND hWnd_res_wid = dlg->GetDlgItem(IDC_RES_WID)->m_hWnd;
	::SetWindowTextA(hWnd_res_wid, jm_wid_value);
	return 0;
}


void CErZhanFengYunDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CErZhanFengYunDlg::OnBnClickedButtonPause()
{
	// TODO: 在此添加控件通知处理程序代码
#if 0
	if (m_pGame->start) {
		m_pGame->start = false;
		GetDlgItem(IDC_BUTTON_PAUSE)->SetWindowText(L"继续");
	}
	else {
		m_pGame->start = true;
		GetDlgItem(IDC_BUTTON_PAUSE)->SetWindowText(L"暂停");
	}
#endif
}


void CErZhanFengYunDlg::OnBnClickedOk()
{
	m_ListCtrAccout.Invalidate();

	// TODO: 在此添加控件通知处理程序代码
	/* 获取一些配置 */
	CString text;
	GetDlgItem(IDC_EDIT_THREAD_COUNT)->GetWindowText(text);
	m_iThreadCount = _ttoi(text);
	if (m_iThreadCount <= 0)
		m_iThreadCount = 1;

	m_pGameThreads = new GameThread[g_dlg->m_iThreadCount];
	//for (int i = 0; i < m_iThreadCount; i++) {
		int i = 0;
		Game* game = new Game(m_pGameAccount);

		GetDlgItem(IDC_EDIT_LOGIN_INTERVAL)->GetWindowText(text);
		game->m_stConfig.AccoutInterval = _ttoi(text);
		GetDlgItem(IDC_EDIT_LOGIN_TIMEOUT)->GetWindowText(text);
		game->m_stConfig.AccountTimeOut = _ttoi(text);
		GetDlgItem(IDC_EDIT_LCLEFT)->GetWindowText(text);
		game->m_stConfig.msLCInterval = _ttoi(text);
		GetDlgItem(IDC_EDIT_START_KEY)->GetWindowText(text);
		game->m_stConfig.StartNum = _ttoi(text);
		game->m_stConfig.Close = ((CButton*)(GetDlgItem(IDC_RADIO_CLOSE)))->GetCheck();
		game->m_stConfig.Open = ((CButton*)(GetDlgItem(IDC_RADIO_OPEN)))->GetCheck();
		game->m_stConfig.JiangSheng = ((CButton*)(GetDlgItem(IDC_RADIO_JIANGSHENG)))->GetCheck();
		GetDlgItem(IDC_EDIT_ONLINE_TIMEOUT)->GetWindowText(text);
		game->m_stConfig.OnlineTimeOut = _ttoi(text);
		GetDlgItem(IDC_EDIT_CLOSEGAME)->GetWindowText(text);
		game->m_stConfig.WaitSecond = _ttoi(text);
		game->start = true;

		g_dlg->m_pGameThreads[i].game = game;
		g_dlg->m_pGameThreads[i].exec_time = 0;
		g_dlg->m_pGameThreads[i].cpt_time = 0;
		g_dlg->m_pGameThreads[i].lock = false;

		AfxBeginThread((AFX_THREADPROC)PlayGame, (LPVOID)i);
	//}


	GetDlgItem(IDOK)->EnableWindow(FALSE);
	//GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(TRUE);

	AfxBeginThread((AFX_THREADPROC)Run, this);
}


void CErZhanFengYunDlg::OnNMCustomdrawListAccount(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW pLVC = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	int status = 0;// m_pGame->GetAccoutStatus(pLVC->nmcd.dwItemSpec);
	//printf("dwItemSpec:%d %d\n", pLVC->nmcd.dwItemSpec, status);
	if (status == STATUS_NO_START) {
		pLVC->clrText = RGB(0x00, 0x00, 0x00);
	}
	else if (status == STATUS_SUCCESS) {
		pLVC->clrText = RGB(0x66, 0x66, 0x66);
		//pLVC->clrText = RGB(0x68, 0x8E, 0x23);
	}
	else if (status == STATUS_ING) {
		pLVC->clrText = RGB(0xFF, 0x00, 0xFF);
		m_ListCtrAccout.EnsureVisible(pLVC->nmcd.dwItemSpec, FALSE);
	}
	else {
		pLVC->clrText = RGB(0xCD, 0x26, 0x26);
	}

	*pResult = 0;
	*pResult |= CDRF_NOTIFYPOSTPAINT;		//必须有，不然就没有效果
	*pResult |= CDRF_NOTIFYITEMDRAW;		//必须有，不然就没有效果
}


void CErZhanFengYunDlg::OnBnClickedCheck1()
{
	//m_pGame->start = false;
	// TODO: 在此添加控件通知处理程序代码
}

//账号注册
void CErZhanFengYunDlg::OnBnClickedRegist()
{
	// TODO: 在此添加控件通知处理程序代码
	AfxBeginThread((AFX_THREADPROC)PlayRegist, (LPVOID)this);
}


void CErZhanFengYunDlg::OnEnChangeEditLcleft()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

/*
解密wid
*/
void CErZhanFengYunDlg::OnBnClickedJxWid()
{
	// TODO: 在此添加控件通知处理程序代码
	AfxBeginThread((AFX_THREADPROC)PlayJxWid, (LPVOID)this);
}
