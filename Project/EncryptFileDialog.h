#pragma once
#include "afxwin.h"
#include "EncryptData.h"
#include "ProgressDialog.h"


// CEncryptFileDialog dialog

class CEncryptFileDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CEncryptFileDialog)

public:
	CEncryptFileDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CEncryptFileDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG1 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClicked_BrowseVideo();
	CEdit m_EditSourceVideo;
	CEdit m_EditEncryptedVideo;
	UINT UpdateProgressDialog(int progress);
	static UINT EncodingThreadProc(LPVOID pParam);
private:
	CString m_SourceVideoPath;
	CString m_EncryptedVideoPath;
	CProgressDialog *m_progressDialog;
	bool GetCancelled();
	bool m_encryptionCompleted;
	void EncryptionCompleted(bool bEncryptCompleted);
public:
	afx_msg void OnBnClickedEncrypt();

	CString GetEncryptedFile();
	bool GetEncryptionCompleted();
};
