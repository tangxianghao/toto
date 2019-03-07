
// ErZhanFengYunDlg.h: 头文件
//

#pragma once

#include "Self/Game.h"


// CErZhanFengYunDlg 对话框
class CErZhanFengYunDlg : public CDialogEx
{
// 构造
public:
	CErZhanFengYunDlg(CWnd* pParent = nullptr);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ERZHANFENGYUN_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持

	static void InsertAccout(int, int, char*);
	static void InsertAccout(int, int, wchar_t*);
	static void InsertLog(wchar_t*);
	static void SetText_M(int, int, int num2=-1);
	static void UpdateText_M(int, int, char*);
	static void ListReDraw();
	static UINT Run(CErZhanFengYunDlg*);
// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	Game* m_pGame;
	CListCtrl m_ListCtrAccout;
	CListCtrl m_ListCtrLog;
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMCustomdrawListAccount(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheck1();
	afx_msg void OnBnClickedButtonPause();
};
