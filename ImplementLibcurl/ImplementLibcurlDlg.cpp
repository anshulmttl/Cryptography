
// ImplementLibcurlDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImplementLibcurl.h"
#include "ImplementLibcurlDlg.h"
#include "afxdialogex.h"
#include "curl\curl.h"
#include "document.h"
#include <string>
#include "EncryptData.h"
#include <fstream>
#include <intrin.h>
#include <sstream>
#include <comdef.h>
#include <WbemIdl.h>

#define _WIN32_DCOM
#pragma comment(lib, "wbemuuid.lib")
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CImplementLibcurlDlg dialog

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
	((std::string*)userp)->append((char*)contents, size *nmemb);
	return size * nmemb;
}


CImplementLibcurlDlg::CImplementLibcurlDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CImplementLibcurlDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CImplementLibcurlDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CImplementLibcurlDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CImplementLibcurlDlg::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, &CImplementLibcurlDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CImplementLibcurlDlg message handlers

BOOL CImplementLibcurlDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CImplementLibcurlDlg::OnPaint()
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
HCURSOR CImplementLibcurlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

_bstr_t CImplementLibcurlDlg::GetBiosSerial(WmiInfo wmiInfo)
{
	HRESULT hRes;
	hRes = CoInitializeEx(0, COINIT_MULTITHREADED);

	if(FAILED(hRes))
	{
		// return false
	}

	hRes = CoInitializeSecurity(NULL,
		-1,
		NULL,
		NULL,
		RPC_C_AUTHN_LEVEL_DEFAULT,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE,
		NULL);

	if(FAILED(hRes))
	{
	}

	IWbemLocator *pLoc = NULL;
	hRes = CoCreateInstance(CLSID_WbemLocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IWbemLocator,
		(LPVOID *) &pLoc);

	if(FAILED(hRes))
	{
	}

	IWbemServices *pSvc = NULL;
	hRes = pLoc->ConnectServer(_bstr_t(L"ROOT\\CIMV2"),
		NULL,
		NULL,
		0,
		NULL,
		0,
		0,
		&pSvc);

	if(FAILED(hRes))
	{
	}

	hRes = CoSetProxyBlanket(pSvc,
		RPC_C_AUTHN_WINNT,
		RPC_C_AUTHZ_NONE,
		NULL,
		RPC_C_AUTHN_LEVEL_CALL,
		RPC_C_IMP_LEVEL_IMPERSONATE,
		NULL,
		EOAC_NONE);

	if(FAILED(hRes))
	{
		pSvc->Release();
		pLoc->Release();
		CoUninitialize();
	}
	_bstr_t returnValue("");
	if(wmiInfo == IDD_BIOS_SERIAL)
	{
		IEnumWbemClassObject *pEnumerator = NULL;
		hRes = pSvc->ExecQuery(bstr_t("WQL"),
			bstr_t("SELECT * FROM Win32_Bios"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			&pEnumerator);

		if(FAILED(hRes))
		{
		}

		IWbemClassObject *pclsObj = NULL;
		ULONG uReturn = 0;
	
		while(pEnumerator)
		{
			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

			if(0 == uReturn)
			{
				break;
			}

			VARIANT vtProp;
			hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
			returnValue = vtProp.bstrVal;
			VariantClear(&vtProp);
			pclsObj->Release();
		}
	}
	else if(wmiInfo == IDD_BOARD_SERIAL)
	{
		IEnumWbemClassObject *pEnumerator = NULL;
		hRes = pSvc->ExecQuery(bstr_t("WQL"),
			bstr_t("SELECT * FROM Win32_BaseBoard"),
			WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
			NULL,
			&pEnumerator);

		if(FAILED(hRes))
		{
		}

		IWbemClassObject *pclsObj = NULL;
		ULONG uReturn = 0;
	
		while(pEnumerator)
		{
			HRESULT hr = pEnumerator->Next(WBEM_INFINITE, 1, &pclsObj, &uReturn);

			if(0 == uReturn)
			{
				break;
			}

			VARIANT vtProp;
			hr = pclsObj->Get(L"Name", 0, &vtProp, 0, 0); //"Product" gives baseboard serial
			returnValue = vtProp.bstrVal;
			VariantClear(&vtProp);
			pclsObj->Release();
		}
	}
	return returnValue;
}

void CImplementLibcurlDlg::OnBnClickedButton1()
{
	// TODO: Add your control notification handler code here
	// Hit curl command
	CURL *curl;
	CURLcode res;
	string readBuffer;
	curl = curl_easy_init();
	if(curl)
	{
		struct curl_slist *headers = NULL;

	    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_URL, "http://ca.prevoyancetech.in/api/restapi/activation_otp");
	
		struct curl_slist *slist1;
		slist1 = NULL;
		slist1 = curl_slist_append(slist1, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);

		// Get system name
		TCHAR szBuffer[256];
		DWORD dwSize = 256;
		GetComputerName(szBuffer, &dwSize);
		std::wstring w_compname(szBuffer);
		std::string compName(w_compname.begin(),w_compname.end());
		//ReqPara = ReqPara + compName + "\",\"MOTHER_BOARD\":\"dfsdf145454545\",\"PROCESSOR_ID\":\"adsad65656565\",\"BIOS_ID\":\"11454dfsdfsfd\",\"SYS_DATE\":\"2019-09-18 14:21:01.663\",\"VERSION\":\"18\"}";
		
		//CPUID 
		int CPUInfo[4] = {-1};
		__cpuid(CPUInfo, 0);
		std::stringstream cpuid;
		for(int i = 0; i < 4; ++i)
		{
			cpuid << CPUInfo[i];
		}

		// Motherboard
		_bstr_t motherBoard = GetBiosSerial(IDD_BOARD_SERIAL);
		std::string motherBoard_str((char *)motherBoard);

		// BIOS
		_bstr_t biosId = GetBiosSerial(IDD_BIOS_SERIAL);
		std::string biosId_str((char *)biosId);

		std::string ReqPara = "{\"KEY_CODE\":\"70E1-917B-F0D0-C07B\",\"PRODUCT_CODE\":\"47CAE3A192\",\"SYSTEM_NAME\":\"";
		ReqPara += compName;
		ReqPara += "\",\"MOTHER_BOARD\":\"";
		ReqPara += motherBoard_str;
		ReqPara += "\",\"PROCESSOR_ID\":\"";
		ReqPara += cpuid.str();
		ReqPara += "\",\"BIOS_ID\":\"";
		ReqPara += biosId_str;
		ReqPara += "\",\"SYS_DATE\":\"2019-09-18 14:21:01.663\",\"VERSION\":\"18\"}";
		
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, ReqPara.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, ReqPara.length());
        curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}
	std::string json_complex = readBuffer.substr(readBuffer.find("{"));
	
	// Read value of Key
	rapidjson::Document document;
	document.Parse(json_complex.c_str());
	assert(document.IsObject());
	assert(document.HasMember("data"));
	assert(document["data"].IsObject());
	assert(document["data"]["key"].IsString());
	std::string key = document["data"]["key"].GetString();
	assert(document.HasMember("status"));
	assert(document["status"].IsBool());
	bool status = document["status"].GetBool();
	
	// Read key only if status is true
	if(status)
	{
		// Encrypt key
		EncryptData *encrypt = new EncryptDataAES();
		string encryptedString = encrypt->Encrypt(key);
		delete encrypt;

		// Delete existing file if any
		remove("1.txt");

		// Save to file
		std::ofstream myFile("1.txt");
		myFile << encryptedString;
		myFile.close();
	}
}


void CImplementLibcurlDlg::OnBnClickedButton2()
{
	// TODO: Add your control notification handler code here
	
	// Getting encrypted string from file
	std::ifstream myFile("1.txt");
	string encryptedString((std::istreambuf_iterator<char>(myFile)), std::istreambuf_iterator<char>());
	myFile.close();

	// Decrypt encrypted string
	EncryptData *decrypt = new EncryptDataAES();
	string decryptedString = decrypt->Decrypt(encryptedString);
	delete decrypt;
	
	// Create AUTH_KEY from decrypted key
	string authKey = "{\"AUTH_KEY\":\"" + decryptedString + "\"}";

	// Hit command with CURL
	string returnValue;
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	if(curl)
	{
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
		curl_easy_setopt(curl, CURLOPT_URL, "http://ca.prevoyancetech.in/api/restapi/auth_check");

		struct curl_slist *slist1;
		slist1 = NULL;
		slist1 = curl_slist_append(slist1, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);

		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, authKey.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, authKey.length());

		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &returnValue);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
	}

	// Extract JSON string
	std::string json_complex = returnValue.substr(returnValue.find("{"));

	// Process JSON string to check value of status
	// Read value of Key
	rapidjson::Document document;
	document.Parse(json_complex.c_str());
	assert(document.IsObject());
	assert(document.HasMember("status"));
	assert(document["status"].IsBool());
	bool status = document["status"].GetBool();

	if(status)
	{
		// If status is true process further
	}
}
