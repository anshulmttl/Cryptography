#pragma once

#include <DShow.h>
#include "afxwin.h"
#include "afxcmn.h"

#define MY_TIMEREVENT 9991
#define IDT_TIMER_0 WM_USER + 200

#define MAX_SIZE 256
#define ONE_SECOND 10000000
#define NANO_SEC_UNIT 10000000.0f

enum State{
	ID_NOT_PLAYING,
	ID_PLAYING,
	ID_PAUSED,
	ID_STOPPED
};
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
	IMediaSeeking *g_pSeek;

	LONGLONG m_Duration;
	float step;
	HWND hOwner;
	long timeElapsed;
	double m_iDuration;
	double m_CurrentDuration;

public:
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CStatic m_VideoDuration;
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CSliderCtrl m_pSeekBar;

	State m_PlayState;
	CButton m_ButtonPlayPause;
};
