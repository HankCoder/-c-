// HttpOption.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "http_client.h"
#include "HttpOption.h"
#include ".\httpoption.h"


// CHttpOption �Ի���

IMPLEMENT_DYNAMIC(CHttpOption, CDialog)
CHttpOption::CHttpOption(CWnd* pParent /*=NULL*/)
	: CDialog(CHttpOption::IDD, pParent)
	, m_bHttp11(FALSE)
	, m_bZip(FALSE)
	, m_bKeepAlive(FALSE)
	, m_sHttpHdrAppend(_T(""))
	, m_sHttpBody(_T(""))
	, m_sServerAddr(_T(""))
	, m_bUseAddr(FALSE)
	, m_bForwardAuto(FALSE)
	, m_nMaxTry(10)
{
}

CHttpOption::~CHttpOption()
{
}

void CHttpOption::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Check(pDX, IDC_CHECK_PROTO, m_bHttp11);
	DDX_Check(pDX, IDC_CHECK_ZIP, m_bZip);
	DDX_Check(pDX, IDC_CHECK_KEEP_ALIVE, m_bKeepAlive);
	DDX_Text(pDX, IDC_EDIT_HTTP_HDR, m_sHttpHdrAppend);
	DDX_Text(pDX, IDC_EDIT_HTTP_BODY, m_sHttpBody);
	DDX_Text(pDX, IDC_EDIT_ADDR, m_sServerAddr);
	DDX_Check(pDX, IDC_CHECK_USE_ADDR, m_bUseAddr);
	DDX_Check(pDX, IDC_CHECK_FORWARD, m_bForwardAuto);
	DDX_Text(pDX, IDC_EDIT_MAX_TRY, m_nMaxTry);
	DDX_Text(pDX, IDC_CONTENT_TYPE, m_sCType);
	DDX_Text(pDX, IDC_ACCEPT, m_sAccept);
	DDX_Check(pDX, IDC_POST_METHOD, m_bPostMethod);
}


BEGIN_MESSAGE_MAP(CHttpOption, CDialog)
	ON_BN_CLICKED(IDC_CHECK_PROTO, OnBnClickedCheckProto)
	ON_BN_CLICKED(IDC_CHECK_ZIP, OnBnClickedCheckZip)
	ON_BN_CLICKED(IDC_CHECK_KEEP_ALIVE, OnBnClickedCheckKeepAlive)
	ON_BN_CLICKED(IDC_POST_METHOD, OnBnClickedPostMethod)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CHttpOption ��Ϣ�������

void CHttpOption::OnBnClickedCheckProto()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (IsDlgButtonChecked(IDC_CHECK_PROTO))
		m_bHttp11 = TRUE;
	else
		m_bHttp11 = FALSE;
}

void CHttpOption::OnBnClickedCheckZip()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (IsDlgButtonChecked(IDC_CHECK_ZIP))
		m_bZip = TRUE;
	else
		m_bZip = FALSE;
}

void CHttpOption::OnBnClickedCheckKeepAlive()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (IsDlgButtonChecked(IDC_CHECK_KEEP_ALIVE))
		m_bKeepAlive = TRUE;
	else
		m_bKeepAlive = FALSE;
}

BOOL CHttpOption::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣: OCX ����ҳӦ���� FALSE
}


void CHttpOption::OnBnClickedPostMethod()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (IsDlgButtonChecked(IDC_POST_METHOD))
		m_bPostMethod = TRUE;
	else
		m_bPostMethod = FALSE;
}

void CHttpOption::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	OnOK();
}
