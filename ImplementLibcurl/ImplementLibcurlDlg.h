
// ImplementLibcurlDlg.h : header file
//

#pragma once
enum WmiInfo{
	IDD_BOARD_SERIAL,
	IDD_BIOS_SERIAL
};

// CImplementLibcurlDlg dialog
class CImplementLibcurlDlg : public CDialogEx
{
// Construction
public:
	CImplementLibcurlDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_IMPLEMENTLIBCURL_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	afx_msg void OnBnClickedButton2();

	_bstr_t GetBiosSerial(WmiInfo wmiInfo);
};
