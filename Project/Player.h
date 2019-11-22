#pragma once

#include <DShow.h>
// CPlayer dialog

class CPlayer : public CDialogEx
{
	DECLARE_DYNAMIC(CPlayer)

public:
	CPlayer(CWnd* pParent = NULL);   // standard constructor
	virtual ~CPlayer();
	CString filePath;
// Dialog Data
	enum { IDD = IDD_DIALOG3 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedPlayPause();

	IGraphBuilder *pGraph;
	IMediaControl *pControl;
	IMediaEvent *pEvent;
};
