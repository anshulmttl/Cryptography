// Player.cpp : implementation file
//

#include "stdafx.h"
#include "Project.h"
#include "Player.h"

#include "afxdialogex.h"


// CPlayer dialog

IMPLEMENT_DYNAMIC(CPlayer, CDialogEx)

CPlayer::CPlayer(CWnd* pParent /*=NULL*/)
	: CDialogEx(CPlayer::IDD, pParent)
{
	hOwner = NULL;
	timeElapsed = 0;
	m_CurrentDuration = 0;
	m_PlayState = ID_NOT_PLAYING;
}

CPlayer::~CPlayer()
{
	pControl->Stop();
}

void CPlayer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TIMER_TEXT, m_VideoDuration);
	DDX_Control(pDX, IDC_SLIDER1, m_pSeekBar);
	DDX_Control(pDX, IDC_PLAY_PAUSE, m_ButtonPlayPause);
}


BEGIN_MESSAGE_MAP(CPlayer, CDialogEx)
	ON_BN_CLICKED(IDC_PLAY_PAUSE, &CPlayer::OnBnClickedPlayPause)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
END_MESSAGE_MAP()


// CPlayer message handlers


void CPlayer::OnBnClickedPlayPause()
{
	// TODO: Add your control notification handler code here
	// DIRECTSHOW PLAY : BEGIN TESTING
	hOwner = AfxGetMainWnd()->m_hWnd;
	if(m_PlayState == ID_NOT_PLAYING)
	{
		HRESULT hR = CoInitialize(NULL);
			if(FAILED(hR))
			{
			}
			//IGraphBuilder *pGraph;
			HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);

			hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
			hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
			hr = pGraph->QueryInterface(IID_IMediaSeeking, (void **)&g_pSeek);

			if(!pControl || !pEvent || !g_pSeek)
			{
				MessageBox(_T("Critical error getting references"),_T("Error"),MB_OK);
			}
			hr = pGraph->RenderFile(filePath, NULL);

			// Check if seekable
			BOOL bCanSeek = FALSE;
			DWORD caps = AM_SEEKING_CanSeekAbsolute | AM_SEEKING_CanGetDuration;
			bCanSeek = (S_OK == g_pSeek->CheckCapabilities(&caps));

			if(!bCanSeek)
			{
				MessageBox(_T("Cannot seek video file"), _T("Error"), MB_OK);
			}

			//Duration
			g_pSeek->GetDuration(&m_Duration);

			m_iDuration = m_Duration * 0.0000001; //Duration in seconds

			step = (float)100.0 / m_iDuration;
			m_pSeekBar.SetRange(0, m_iDuration, FALSE);

			UINT timerVal;
			timerVal = SetTimer(IDT_TIMER_0, 1000, NULL);

			if(timerVal == 0)
			{
				MessageBox(_T("Unable to obtain timer"), _T("Error"), MB_OK);
			}

			hr = pControl->Run();

			m_PlayState = ID_PLAYING;
			m_ButtonPlayPause.SetWindowTextW(_T("Pause"));
			long evoCode = 0;
			//pEvent->WaitForCompletion(INFINITE, &evoCode);

			pControl->Release();
			pEvent->Release();
			pGraph->Release();
			CoUninitialize();
	}
	else if(m_PlayState == ID_PLAYING)
	{
		pControl->Pause();
		m_PlayState = ID_PAUSED;
		m_ButtonPlayPause.SetWindowTextW(_T("Play"));
	}
	else if(m_PlayState == ID_PAUSED)
	{
		pControl->Run();
		m_PlayState = ID_PLAYING;
		m_ButtonPlayPause.SetWindowTextW(_T("Pause"));
	}
			
	
}

void CPlayer::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	char time[MAX_SIZE] = "";
	int pos = 0;

	timeElapsed++;

	// Calculate position of progress bar
	pos = (int)((float)step * (float)timeElapsed);

	CTimeSpan ts(timeElapsed);
	CString strTime = ts.Format(_T("%M:%S"));	
	CTimeSpan te(m_iDuration);
	CString eTime = te.Format(_T("%M:%S"));
	m_VideoDuration.SetWindowText(strTime+ " / " + eTime);

	m_pSeekBar.SetPos(timeElapsed);
	CDialogEx::OnTimer(nIDEvent);
}


void CPlayer::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	// TODO: Add your message handler code here and/or call default
	if(IDC_SLIDER1 == pScrollBar->GetDlgCtrlID())
	{
		// Seeking video playback
		LONGLONG llCurrent, llStop;
		g_pSeek->GetPositions(&llCurrent, &llStop);

		if(llStop > 0)
		{
			KillTimer(IDT_TIMER_0);

			int p = m_pSeekBar.GetPos() + 5;

			LONGLONG llNewPos = p * ONE_SECOND;

			// Set position in IMediaSeeking
			g_pSeek->SetPositions(&llNewPos, AM_SEEKING_AbsolutePositioning, &m_Duration, AM_SEEKING_NoPositioning);

			m_CurrentDuration = llCurrent * 0.0000001;

			timeElapsed = p;
			// Set window text 
			double nCurrent = (p + 5) * 0.0000001;
			CTimeSpan ts(p);
			CString strTime = ts.Format(_T("%M:%S"));	
			CTimeSpan te(m_iDuration);
			CString eTime = te.Format(_T("%M:%S"));
			m_VideoDuration.SetWindowText(strTime+ " / " + eTime);

			// Set timer again after processing
			SetTimer(IDT_TIMER_0, 1000, NULL);
		}
	}

	CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
}
