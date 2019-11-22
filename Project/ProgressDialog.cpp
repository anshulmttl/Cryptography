// ProgressDialog.cpp : implementation file
//

#include "stdafx.h"
#include "Project.h"
#include "ProgressDialog.h"
#include "afxdialogex.h"


// CProgressDialog dialog

IMPLEMENT_DYNAMIC(CProgressDialog, CDialogEx)

CProgressDialog::CProgressDialog(CWnd* pParent /*=NULL*/)
	: CDialogEx(CProgressDialog::IDD, pParent)
{
	m_bIsCancelled = false;
}

CProgressDialog::~CProgressDialog()
{
}

void CProgressDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PROGRESS1, m_ProgressCtrl);
}


BEGIN_MESSAGE_MAP(CProgressDialog, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON1, &CProgressDialog::OnBnClickedCancel)
END_MESSAGE_MAP()


// CProgressDialog message handlers

bool CProgressDialog::GetCancelled()
{
	return m_bIsCancelled;
}

void CProgressDialog::OnBnClickedCancel()
{
	// TODO: Add your control notification handler code here
	m_bIsCancelled = true;
}

void CProgressDialog::SetProgress(int progress)
{
	m_ProgressCtrl.SetPos(progress);
}

