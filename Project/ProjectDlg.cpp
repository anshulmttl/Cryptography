
// ProjectDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Project.h"
#include "ProjectDlg.h"
#include "afxdialogex.h"
#include <fstream>
#include "VirtualFile.h"
#include "BoxedAppSDK.h"
#include "Player.h"
#include <DShow.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CString EncryptedVideoFile;
CString TemporaryVideoFile;

const unsigned int ChunkSizeEncrypted_TripleDES = 10008;
const unsigned int ChunkSizeEncrypted_AES = 50016;

// Modify value of ChunkSizeEncrypted to ChunkSizeEncrypted_AES or ChunkSizeEncrypted_TripleDES 
// based on encryption used
const unsigned int ChunkSizeEncrypted = ChunkSizeEncrypted_AES;
// CProjectDlg dialog
HANDLE hFile;



CProjectDlg::CProjectDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProjectDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CProjectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_OCX1, m_MediaPlayer);
}

BEGIN_MESSAGE_MAP(CProjectDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CProjectDlg::OnBnClicked_EncryptVideo)
	ON_BN_CLICKED(IDC_BUTTON2, &CProjectDlg::OnBnClickedPlayEncryptedVideo)
	ON_BN_CLICKED(IDC_BUTTON3, &CProjectDlg::OnBnClickedPlay)
END_MESSAGE_MAP()


// CProjectDlg message handlers

BOOL CProjectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	LPUNKNOWN pUnknown = m_MediaPlayer.GetControlUnknown();
	HRESULT hr = pUnknown->QueryInterface(__uuidof(IWMPPlayer), (void **)&m_pWMPPlayer);
	//if(SUCCEEDED(hr))
	//{
		//CComBSTR strMovie = _T("");
		//m_pWMPPlayer->put_URL(strMovie);
	//}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CProjectDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CProjectDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CProjectDlg::OnBnClicked_EncryptVideo()
{
	// TODO: Add your control notification handler code here
	CEncryptFileDialog encryptFileDialog;
	encryptFileDialog.DoModal();
	
	if(true == encryptFileDialog.GetEncryptionCompleted())
	{
		/*######################################################
		############  PLAY VIDEO AFTER ENCODING   ##############
		########################################################*/

		int playNow = MessageBox(_T("Encryption completed. Do you want to play video now?"), _T("Completed"), MB_OKCANCEL);
		if(IDOK == playNow)
		{
 			// If user wants to play file immediately after encoding
			// Code for "Encryption completed. Play video now"
			EncryptedVideoFile = encryptFileDialog.GetEncryptedFile();

			// Temporary file
			//TCHAR buf[1000];
			//GetTempPath(1000, buf);
			//CString tempFileName = buf;
			//tempFileName += "1.avi";
			//TemporaryVideoFile = tempFileName;

			// Delete existing 1.avi
			//CStringA fileName8bit(tempFileName);
			//char *fileNameChar = fileName8bit.GetBuffer();
			//remove(fileNameChar);
			
			 // TESTING CreateFile : BEGIN
			//HANDLE hFile;
			//char DataBuffer[] = "This is some data to write to file";
			//char DataBuffer1[] = "Hello Anshul this is your testing";
			//DWORD dwBytesToWrite = (DWORD)strlen(DataBuffer);
			//DWORD dwBytesToWrite1 = (DWORD)strlen(DataBuffer1);
			//DWORD dwBytesWritten = 0;
			//DWORD dwBytesWritten1 = 0;

			BoxedAppSDK_Init();
			BoxedAppSDK_EnableOption(DEF_BOXEDAPPSDK_OPTION__EMBED_BOXEDAPP_IN_CHILD_PROCESSES,TRUE);
			LPSTREAM pMyStream;
			CreateCryptoStream(&pMyStream, EncryptedVideoFile);

			hFile = BoxedAppSDK_CreateVirtualFileBasedOnIStream(
				_T("1.avi"),
				GENERIC_WRITE,
				FILE_SHARE_READ,
				NULL,
				CREATE_NEW,
				0,
				NULL,
				pMyStream
				);

			// DIRECTSHOW PLAY : BEGIN TESTING
			HRESULT hR = CoInitialize(NULL);
			if(FAILED(hR))
			{
			}
			IGraphBuilder *pGraph;
			HRESULT hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC_SERVER, IID_IGraphBuilder, (void **)&pGraph);

			IMediaControl *pControl;
			IMediaEvent *pEvent;
			hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
			hr = pGraph->QueryInterface(IID_IMediaEvent, (void **)&pEvent);

			hr = pGraph->RenderFile(_T("1.avi"), NULL);
			hr = pControl->Run();

			long evoCode = 0;
			pEvent->WaitForCompletion(INFINITE, &evoCode);

			pControl->Release();
			pEvent->Release();
			pGraph->Release();
			CoUninitialize();
			// DIRECTSHOW PLAY : END TESTING

			//const char* szText = "This is a virtual file. Cool! You have just loaded the virtual file into notepad.exe!\r\nDon't forget to obtain a license ;)\r\nhttp://boxedapp.com/order.html";
			//DWORD temp;
			//WriteFile(hFile, szText, lstrlenA(szText), &temp, NULL);
			//CloseHandle(hFile);
			//WinExec("notepad.exe 1.txt",SW_SHOW);
			
			//hFile = CreateFile(_T("1.avi"), GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_HIDDEN, NULL);
			// TESTING CreateFile : END
			// Create VirtualFile

			// Start new thread to decrypt video
			
			
			AfxBeginThread(DecodingThreadProc,this);
			//CComBSTR strMovie = TemporaryVideoFile;
			CComBSTR strMovie = "1.avi";
			m_pWMPPlayer->put_URL(strMovie);
		}
	}
}



void CProjectDlg::OnBnClickedPlayEncryptedVideo()
{
	// TODO: Add your control notification handler code here
	CFileDialog fileDialog(TRUE, _T(".*"));
	if(fileDialog.DoModal() == IDOK)
	{
		CString filePath = fileDialog.GetPathName();
		EncryptedVideoFile = filePath;
		// Get temporary path for temporary video file
		//TCHAR buf[1000];
		//GetTempPath(1000,buf);
		//CString tempFileName = buf;
		//tempFileName += "1.avi";
		//TemporaryVideoFile = tempFileName;

		// Delete existing 1.avi
		//CStringA interFile(tempFileName);
		//char *fileNameChar = interFile.GetBuffer();
		//remove(fileNameChar);
		
		BoxedAppSDK_Init();
		BoxedAppSDK_EnableOption(DEF_BOXEDAPPSDK_OPTION__EMBED_BOXEDAPP_IN_CHILD_PROCESSES,TRUE);
		LPSTREAM pMyStream;
		CreateCryptoStream(&pMyStream, EncryptedVideoFile);

		hFile = BoxedAppSDK_CreateVirtualFileBasedOnIStream(
			_T("1.avi"),
			GENERIC_WRITE,
			FILE_SHARE_READ,
			NULL,
			CREATE_NEW,
			0,
			NULL,
			pMyStream
			);

		CPlayer player = new CPlayer();
		player.filePath = _T("1.avi");
		player.DoModal();
		
			// DIRECTSHOW PLAY : END TESTING
		// Start new thread to decrypt the video. Start playing the video from temporary file after creating thread
		AfxBeginThread(DecodingThreadProc, this);
		//CComBSTR strMovie = TemporaryVideoFile;
		CComBSTR strMovie = "1.avi";
		m_pWMPPlayer->put_URL(strMovie);
	}
}

void CProjectDlg::DecryptionIsCompleted()
{
	MessageBox(_T("Decryption is completed"), _T("Completed"), MB_OK);
}

UINT CProjectDlg::DecodingThreadProc(LPVOID pParam)
{
	CProjectDlg *dlg = (CProjectDlg*)pParam;
	/*std::ifstream encryptedFile;
	//std::ofstream temporaryFile;

	encryptedFile.open((LPCTSTR)EncryptedVideoFile, std::ios::binary | std::ios::in);
	if(encryptedFile.fail())
		return 0;

	//temporaryFile.open((LPCTSTR)TemporaryVideoFile, std::ios::binary | std::ios::out);
	//if(temporaryFile.fail())
	//	return 0;

	bool processFile = true;
	while(processFile)
	{
		char buffer[ChunkSizeEncrypted];
		encryptedFile.read(buffer, ChunkSizeEncrypted);
		if(encryptedFile)
		{
		}
		else
		{
			processFile = false;
		}

		int readBytes = encryptedFile.gcount();

		vector<unsigned char> decryptedChunk;
		// EncryptData *encryptor = new EncryptTripleDES(); //Use if want to use TripleDES encryption
		EncryptData *encryptor = new EncryptAES(); //Use if want to use AES encryption
		decryptedChunk = encryptor->Decrypt(buffer, readBytes);

		//Write decrypted data to temporary file
		int decryptedSize = decryptedChunk.size();
		char *res = reinterpret_cast<char*>(decryptedChunk.data());
		//temporaryFile.write(res, decryptedSize);

		DWORD temp;
		WriteFile(hFile, res, decryptedSize, &temp, NULL);
	}*/
	//dlg->DecryptionIsCompleted();
	CloseHandle(hFile);
	return 0;
}


void CProjectDlg::OnBnClickedPlay()
{
	// TODO: Add your control notification handler code here
	CComBSTR strMovie = "1.avi";
	m_pWMPPlayer->put_URL(strMovie);
}
