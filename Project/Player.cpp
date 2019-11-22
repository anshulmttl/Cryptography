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

}

CPlayer::~CPlayer()
{
}

void CPlayer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPlayer, CDialogEx)
	ON_BN_CLICKED(IDC_PLAY_PAUSE, &CPlayer::OnBnClickedPlayPause)
END_MESSAGE_MAP()


// CPlayer message handlers


void CPlayer::OnBnClickedPlayPause()
{
	// TODO: Add your control notification handler code here
	// DIRECTSHOW PLAY : BEGIN TESTING
			HRESULT hR = CoInitialize(NULL);
			if(FAILED(hR))
			{
			}
			//IGraphBuilder *pGraph;
			HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);

			IMediaSeeking *g_pSeek = 0;
			hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
			hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);
			hr = pGraph->QueryInterface(IID_IMediaSeeking, (void **)&g_pSeek);


			hr = pGraph->RenderFile(filePath, NULL);

			/*// Check if seekable
			BOOL bCanSeek = FALSE;
			DWORD caps = AM_SEEKING_CanSeekAbsolute | AM_SEEKING_CanGetDuration;
			bCanSeek = (S_OK == g_pSeek->CheckCapabilities(&caps));
			if(bCanSeek)
			{
				LONGLONG *pDuration;
				g_pSeek->GetDuration(pDuration);
			}*/

			hr = pControl->Run();

			long evoCode = 0;
			pEvent->WaitForCompletion(INFINITE, &evoCode);

			pControl->Release();
			pEvent->Release();
			pGraph->Release();
			CoUninitialize();
}
