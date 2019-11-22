#pragma once
#include "afxcmn.h"


// CProgressDialog dialog

class CProgressDialog : public CDialogEx
{
	DECLARE_DYNAMIC(CProgressDialog)

public:
	CProgressDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProgressDialog();

// Dialog Data
	enum { IDD = IDD_DIALOG2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_ProgressCtrl;
	afx_msg void OnBnClickedCancel();

	void SetProgress(int progress);
	CDialog *m_pEncryptFileDialog;
	bool GetCancelled();
private:
	bool m_bIsCancelled;
};
