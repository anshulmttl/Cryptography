// EncryptFileDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Project.h"
#include "EncryptFileDialog.h"
#include "afxdialogex.h"
#include <fstream>

CString sourceVideoFile;
CString destinationVideoFile;
CWinThread *pThread;
const unsigned int ChunkSize_TripleDES = 10000;
const unsigned int ChunkSize_AES = 50000;
const unsigned int ChunkSize = ChunkSize_AES;
//const unsigned int ChunkSizeEncryptedVideo = 10008;
unsigned int DestinationChunkSize = 0;
// CEncryptFileDialog dialog

IMPLEMENT_DYNAMIC(CEncryptFileDialog, CDialogEx)

CEncryptFileDialog::CEncryptFileDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CEncryptFileDialog::IDD, pParent)
{
	m_progressDialog = new CProgressDialog(this);
	m_encryptionCompleted = false;
}

CEncryptFileDialog::~CEncryptFileDialog()
{
}

void CEncryptFileDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_EditSourceVideo);
	DDX_Control(pDX, IDC_EDIT2, m_EditEncryptedVideo);
}


BEGIN_MESSAGE_MAP(CEncryptFileDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CEncryptFileDialog::OnBnClicked_BrowseVideo)
	ON_BN_CLICKED(IDC_BUTTON3, &CEncryptFileDialog::OnBnClickedEncrypt)
END_MESSAGE_MAP()


// CEncryptFileDialog message handlers


void CEncryptFileDialog::OnBnClicked_BrowseVideo()
{
	// TODO: Add your control notification handler code here
	//CFileDialog fileDialog(TRUE, "*.*", NULL, OFN_HIDEREADONLY, "Text Files: (*.txt)|*.txt||");
	CFileDialog fileDialog(TRUE, _T(".*"));
	if(fileDialog.DoModal() == IDOK)
	{
		CString filePath = fileDialog.GetPathName();
		CString outputFilePath = filePath + ".enc";
		m_SourceVideoPath = filePath;
		m_EncryptedVideoPath = outputFilePath;

		m_EditSourceVideo.SetWindowTextW(filePath);
		m_EditEncryptedVideo.SetWindowTextW(outputFilePath);
	}
}


void CEncryptFileDialog::OnBnClickedEncrypt()
{
	// TODO: Add your control notification handler code here
	if(true == m_SourceVideoPath.IsEmpty())
	{
		int messageBoxId = MessageBox(_T("First select source file"), _T("Error") , MB_ICONWARNING | MB_CANCELTRYCONTINUE | MB_DEFBUTTON2);
		return;
	}

	// Global for access through Worker function
	sourceVideoFile = m_SourceVideoPath;
	destinationVideoFile = m_EncryptedVideoPath;
	pThread = AfxBeginThread(EncodingThreadProc, this);

	m_progressDialog->Create(CProgressDialog::IDD);
	m_progressDialog->ShowWindow(SW_SHOW);
}

UINT CEncryptFileDialog::UpdateProgressDialog(int progress)
{
	m_progressDialog->SetProgress(progress);
	return 0;
}

void CEncryptFileDialog::EncryptionCompleted(bool bEncryptionCompleted)
{
	m_encryptionCompleted = bEncryptionCompleted;
}

bool CEncryptFileDialog::GetCancelled()
{
	return m_progressDialog->GetCancelled();
}

CString CEncryptFileDialog::GetEncryptedFile()
{
	return destinationVideoFile;
}

bool CEncryptFileDialog::GetEncryptionCompleted()
{
	return m_encryptionCompleted;
}

UINT CEncryptFileDialog::EncodingThreadProc(LPVOID pParam)
{
	CEncryptFileDialog *dlg = (CEncryptFileDialog*)pParam;
	std::ifstream videoFile;
	std::ofstream encryptedVideoFile, logFile;
	
	videoFile.open((LPCTSTR)sourceVideoFile, std::ios::binary | std::ios::in);
	if(videoFile.fail())
		return 1;
	
	encryptedVideoFile.open((LPCTSTR)destinationVideoFile, ios::binary|ios::out);
	if(encryptedVideoFile.fail())
		return 1;

	std::vector<unsigned int> sourceChunkSizeList;
	std::vector<unsigned int> destinationChunkSizeList;
	unsigned int iteration = 0;

	// Setting for progressbar
	unsigned int progress = 0;
	unsigned int increment = 10;
	unsigned int begin = videoFile.tellg();
	videoFile.seekg(0, videoFile.end);
	unsigned int end = videoFile.tellg();
	videoFile.seekg(0, videoFile.beg);
	unsigned int fileSize = end - begin;
	float modulous_temp = (fileSize / ChunkSize) / 10.0; //increment 10% after these many chunks
	unsigned int modulous = 1;
	if(modulous_temp < 1)
	{
		increment = (unsigned int)(10.0 / modulous);
		modulous = 1;
	}
	else
		modulous = modulous_temp;

	bool encryptCancelled = false;
	bool processFile = true;
	char buffer[ChunkSize];
	// Read file by blocks of fixed size bytes
	while(processFile)
	{
		videoFile.read(buffer,ChunkSize);
		if(videoFile)
		{}
		else
		{
			//End of file is reached. Stop reading
			processFile = false;
		}
		int readBytes = videoFile.gcount();

		// Perform encryption on block of data
		vector<unsigned char> encryptedChunk;
		// EncryptData *encryptor = new EncryptTripleDES(); //Use if want to use TripleDES encryption
	    EncryptData *encryptor = new EncryptAES(); // Use if want to use AES encryption
		encryptedChunk = encryptor->Encrypt(buffer, readBytes);

		//Write encryoted block of data to file
		int encryptedSize = encryptedChunk.size();
		char *res = reinterpret_cast<char*>(encryptedChunk.data());
		encryptedVideoFile.write(res, encryptedSize);

		// store chunk sizes for tracing
		sourceChunkSizeList.push_back(readBytes);
		destinationChunkSizeList.push_back(encryptedSize);
		++iteration;
		
		if(iteration % modulous == 0)
		{
			progress += increment;
			dlg->UpdateProgressDialog(progress);
		}

		// Check if encryption has been cancelled
		if(dlg->GetCancelled())
		{
			encryptCancelled = true;
			break;
		}
	}

	dlg->UpdateProgressDialog(100);
	if(encryptCancelled)
	{
		// Delete the partially encrypted file
		CStringA fileDelete8bit(destinationVideoFile);
		char *fileDelete = fileDelete8bit.GetBuffer();
		remove(fileDelete);
	}
	else
	{
		dlg->EncryptionCompleted(true);
	}

	// First value is the chunk size of destination
	DestinationChunkSize = destinationChunkSizeList[0];

	// Log file to record encrypted chunks
	logFile.open("Log.log", std::ios::out);
	string totalChunks = "Total chunks : " + to_string(static_cast<long long>(iteration)) + "\n";
	logFile << totalChunks;
	//Trace source and destination chunk sizes
	for(int i = 0; i < iteration; i = i + 1)
	{
		string sourceChunkSize = "Source video chunk size : " + to_string(static_cast<long long>(sourceChunkSizeList[i])) + "\n";
		string destinationChunkSize = "Encrypted video chunk size : " + to_string(static_cast<long long>(destinationChunkSizeList[i])) + "\n";
		logFile << sourceChunkSize;
		logFile << destinationChunkSize;
	}

	logFile.close();
	return 0;
}