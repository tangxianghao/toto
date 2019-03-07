
// ErZhanFengYunDlg.cpp: 实现文件
//

#include "stdafx.h"
#include "ErZhanFengYun.h"
#include "ErZhanFengYunDlg.h"
#include "afxdialogex.h"
#include "Common/Json.h"
#include "Common/HttpClient.h"

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

	DWORD dwStyle = m_ListCtrAccout.GetExtendedStyle();
	dwStyle |= (LVS_REPORT);
	m_ListCtrAccout.SetExtendedStyle(dwStyle);
	m_ListCtrAccout.InsertColumn(0, L"序号", LVCFMT_LEFT, 80);
	m_ListCtrAccout.InsertColumn(1, L"帐号", LVCFMT_LEFT, 250);
	m_ListCtrAccout.InsertColumn(2, L"声望", LVCFMT_CENTER, 80);
	m_ListCtrAccout.InsertColumn(3, L"区服", LVCFMT_CENTER, 80);
	m_ListCtrAccout.InsertColumn(4, L"状态", LVCFMT_LEFT, 170);

	m_ListCtrLog.InsertColumn(0, L"消息", LVCFMT_LEFT, 450);
	m_ListCtrLog.SetExtendedStyle(dwStyle);

	GetDlgItem(IDC_EDIT_LOGIN_INTERVAL)->SetWindowText(L"100");
	GetDlgItem(IDC_EDIT_LOGIN_TIMEOUT)->SetWindowText(L"5");
	GetDlgItem(IDC_EDIT_START_KEY)->SetWindowText(L"0");
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(FALSE);

	//AllocConsole();
	//freopen("CON", "w", stdout);

	m_pGame = Game::GetInstance();
	AfxBeginThread((AFX_THREADPROC)Run, this);
	
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
	// 1222221fff1 120280217
	// chi8885@003.com 123124
	// chi8784@003.com 123124
	// chi10129@003.com 123124
	Game* game = dlg->m_pGame;
	game->InsertAccountFunc = dlg->InsertAccout;
	game->UpdateTextFunc = dlg->UpdateText_M;
	game->InsertLogFunc = dlg->InsertLog;
	game->SetTextFunc = dlg->SetText_M;
	game->ListReDrawFunc = dlg->ListReDraw;

	if (game->GetConfAccount()) {
		while (true) {
			Sleep(50);
			if (game->start) {
				game->Start();
				break;
			}
		}
	}
	return 0;
}


void CErZhanFengYunDlg::OnBnClickedCancel()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CErZhanFengYunDlg::OnBnClickedButtonPause()
{
	// TODO: 在此添加控件通知处理程序代码
	if (m_pGame->start) {
		m_pGame->start = false;
		GetDlgItem(IDC_BUTTON_PAUSE)->SetWindowText(L"继续");
	}
	else {
		m_pGame->start = true;
		GetDlgItem(IDC_BUTTON_PAUSE)->SetWindowText(L"暂停");
	}
}


void CErZhanFengYunDlg::OnBnClickedOk()
{
	m_ListCtrAccout.Invalidate();

	// TODO: 在此添加控件通知处理程序代码
	/* 获取一些配置 */
	CString text;
	GetDlgItem(IDC_EDIT_LOGIN_INTERVAL)->GetWindowText(text);
	m_pGame->m_stConfig.AccoutInterval = _ttoi(text);
	GetDlgItem(IDC_EDIT_LOGIN_TIMEOUT)->GetWindowText(text);
	m_pGame->m_stConfig.AccountTimeOut = _ttoi(text);
	GetDlgItem(IDC_EDIT_START_KEY)->GetWindowText(text);
	m_pGame->m_stConfig.StartNum = _ttoi(text);
	m_pGame->m_stConfig.Close = ((CButton*)(GetDlgItem(IDC_CHECK_XXX)))->GetCheck();
	m_pGame->start = true;

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_PAUSE)->EnableWindow(TRUE);
}


void CErZhanFengYunDlg::OnNMCustomdrawListAccount(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVCUSTOMDRAW pLVC = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码

	int status = m_pGame->GetAccoutStatus(pLVC->nmcd.dwItemSpec);
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
