
// TaskProcessesDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "afxwin.h"
#include <vector>

// CTaskProcessesDlg dialog
class CTaskProcessesDlg : public CDialogEx
{
// Construction
public:
	CTaskProcessesDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_TASKPROCESSES_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	CTabCtrl m_TabControl;
	afx_msg void OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult);
	void DisplayProcessNameAndID( DWORD processID , int position);
	CListBox m_ListBox;
	void InitializeTabControl();
	afx_msg void OnSelchangeList2();
	afx_msg void OnBnClickedButton1();
};
