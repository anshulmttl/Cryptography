
// ProjectDlg.h : header file
//

#pragma once
#include "EncryptFileDialog.h"
#include "ocx1.h"



// CProjectDlg dialog
class CProjectDlg : public CDialogEx
{
// Construction
public:
	CProjectDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_PROJECT_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	static UINT DecodingThreadProc(LPVOID pParam);

// Implementation
protected:
	HICON m_hIcon;

	
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClicked_EncryptVideo();
	COcx1 m_MediaPlayer;
	CComPtr<IWMPPlayer> m_pWMPPlayer;
	afx_msg void OnBnClickedPlayEncryptedVideo();
	afx_msg void OnBnClickedPlay();
	void DecryptionIsCompleted();

};
