
// TaskProcessesDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TaskProcesses.h"
#include "TaskProcessesDlg.h"
#include "afxdialogex.h"
#include "psapi.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CTaskProcessesDlg dialog




CTaskProcessesDlg::CTaskProcessesDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CTaskProcessesDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTaskProcessesDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB1, m_TabControl);
	DDX_Control(pDX, IDC_LIST2, m_ListBox);
}

BEGIN_MESSAGE_MAP(CTaskProcessesDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, &CTaskProcessesDlg::OnSelchangeTab1)
	ON_LBN_SELCHANGE(IDC_LIST2, &CTaskProcessesDlg::OnSelchangeList2)
	ON_BN_CLICKED(IDC_BUTTON1, &CTaskProcessesDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CTaskProcessesDlg message handlers

BOOL CTaskProcessesDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	TC_ITEM ti, ti1;
	ti.mask = TCIF_TEXT;
	ti.pszText = _T("Running process");

	ti1.mask = TCIF_TEXT;
	ti1.pszText = _T("Capture processes");
	m_TabControl.InsertItem(0,&ti);
	m_TabControl.InsertItem(1,&ti1);

	CString str;
	str += "Process ID";
	str += "               ";
	str += "Process Name";
	m_ListBox.AddString(str);

	InitializeTabControl();

	//HWND hWnd = ::GetActiveWindow();
	HWND hWnd = AfxGetApp()->m_pMainWnd->m_hWnd;
	if(::IsWindow(hWnd))
	{
		BOOL hRa = SetWindowDisplayAffinity(hWnd, WDA_MONITOR);
	}
	return TRUE;  // return TRUE   unless you set the focus to a control
}
void CTaskProcessesDlg::InitializeTabControl()
{
			//Process[] localAll = Process.GetProcesses();
		   // Get the list of process identifiers.  
   DWORD aProcesses[1024], cbNeeded, cProcesses;
   unsigned int i;
  
   //This returns a list of handles to processes running on the system as an array.
   if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
      return;
   m_ListBox.ResetContent();

	//m_ListBox.A
   // Calculate how many process identifiers were returned.  
   cProcesses = cbNeeded / sizeof(DWORD);
  	
   // Display the name and process identifier for each process.  
   for ( i = 0; i < cProcesses; i++ )
      if( aProcesses[i] != 0 )
         DisplayProcessNameAndID( aProcesses[i] , i+1);

    CString str;
	str += "Process ID";
	str += "               ";
	str += "Process Name";
	m_ListBox.InsertString(0,str);
}

void CTaskProcessesDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTaskProcessesDlg::OnPaint()
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
HCURSOR CTaskProcessesDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CTaskProcessesDlg::OnSelchangeTab1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	int iSel = m_TabControl.GetCurSel();
	if(iSel == 0)
	{
		//Process[] localAll = Process.GetProcesses();
		   // Get the list of process identifiers.  
   DWORD aProcesses[1024], cbNeeded, cProcesses;
   unsigned int i;
  
   //This returns a list of handles to processes running on the system as an array.
   if ( !EnumProcesses( aProcesses, sizeof(aProcesses), &cbNeeded ) )
      return;
   m_ListBox.ResetContent();

	//m_ListBox.A
   // Calculate how many process identifiers were returned.  
   cProcesses = cbNeeded / sizeof(DWORD);
  	
   // Display the name and process identifier for each process.  
   for ( i = 0; i < cProcesses; i++ )
      if( aProcesses[i] != 0 )
         DisplayProcessNameAndID( aProcesses[i] , i+1);

    CString str;
	str += "Process ID";
	str += "               ";
	str += "Process Name";
	m_ListBox.InsertString(0,str);
	}
	else if(iSel == 1)
	{
		m_ListBox.ResetContent();

		CString str;
		str += "Process ID";
		str += "               ";
		str += "Process Name";
		m_ListBox.InsertString(0,str);
	}


	*pResult = 0;
}

void CTaskProcessesDlg::DisplayProcessNameAndID( DWORD processID , int position)
{
   TCHAR szProcessName[MAX_PATH] = TEXT("<unknown>");
  
   // Get a handle to the process.  
   HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION |
      PROCESS_VM_READ,
      FALSE, processID );
  
   // Get the process name.  
   if (NULL != hProcess )
   {
      HMODULE hMod;
      DWORD cbNeeded;
     
      //Given a handle to a process, this returns all the modules running within the process.
      //The first module is the executable running the process,
      //and subsequent handles describe DLLs loaded into the process.
      if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
      {
         //This function returns the short name for a module,
         //typically the file name portion of the EXE or DLL
         GetModuleBaseName( hProcess, hMod, szProcessName,
            sizeof(szProcessName)/sizeof(TCHAR) );
      }

	  CString str1 = szProcessName;
	  CString str2 = _T("<unknown>");
   // Display the process name and identifier.
	  if(str1.Compare(str2) != 0)
	  {
		   CString str, prcId;
		   prcId.Format(_T("%u"), processID);
		   str += prcId;
		   str += "                    ";
		   str += szProcessName;
  
		   m_ListBox.AddString(str);
		   
	  }
   //str += processID;
   //str.Format("Text:%s, PID : %u", szProcessName, processID );
   //AfxMessageBox(str);
   }
  

  
   //close the process handle
   CloseHandle( hProcess );
}

void CTaskProcessesDlg::OnSelchangeList2()
{
	// TODO: Add your control notification handler code here
}


void CTaskProcessesDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	int nSel = m_ListBox.GetCurSel();
	if(nSel != LB_ERR)
	{
		CString itemSelected;
		m_ListBox.GetText(nSel, itemSelected);
		int curPos = 0;
		CString resToken = itemSelected.Tokenize(_T(" "), curPos);
		CStringA strA (resToken);
		DWORD processID =  (DWORD)atoi(strA);
		//AfxMessageBox(resToken);

		HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_TERMINATE, FALSE, processID);
		TerminateProcess(hProcess, 0);
		CloseHandle(hProcess);
	}
}
