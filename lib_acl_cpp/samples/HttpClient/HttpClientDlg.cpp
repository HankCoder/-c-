// HttpClientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "acl_cpp/stdlib/string.hpp"
#include "acl_cpp/stream/aio_handle.hpp"
#include "HttpClient.h"
#include "HttpClientDlg.h"
#include "HttpDownload.h"
#include ".\httpclientdlg.h"

//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CHttpClientDlg �Ի���



CHttpClientDlg::CHttpClientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CHttpClientDlg::IDD, pParent)
	, handle_(acl::ENGINE_WINMSG)
	, service_(NULL)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CHttpClientDlg::~CHttpClientDlg()
{
	if (service_)
	{
		delete service_;

		// service_ ����һ���첽IO�󶨣���ɾ���󲢲��������ر�
		// ���첽 IO����Ϊ�첽��������ӳٹرջ��ƣ�������Ҫ��
		// �� handle_check() �������ͷ��ӳ��ͷŶ�������첽 IO
		handle_.check();
	}
}

void CHttpClientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CHttpClientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_DOWNLOAD, OnBnClickedDownload)
	ON_MESSAGE(WM_USER_DOWNLOAD_OVER, OnDownloadOk)
END_MESSAGE_MAP()


// CHttpClientDlg ��Ϣ�������

BOOL CHttpClientDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ��\������...\���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	FILE *fp;
	AllocConsole();
	fp = freopen("CONOUT$","w+t",stdout);

	service_ = NEW acl::http_service(2, 0, true);
	// ʹ��Ϣ���������� 127.0.0.1 �ĵ�ַ
	//if (service_->open(&handle_) == false)
	//{
	//	printf(">>open message service error!\n");
	//	delete service_;
	//	service_ = NULL;
	//}
	
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CHttpClientDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CHttpClientDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ��������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù����ʾ��
HCURSOR CHttpClientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CHttpClientDlg::OnBnClickedDownload()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (service_ == NULL)
	{
		printf("service_ null\r\n");
		return;
	}

	// ���� HTTP �������
	acl::string domain;
	domain = "www.banmau.com";
	//domain = "192.168.1.229";
	//domain = "www.renwou.net";
	CHttpDownload* req = NEW CHttpDownload(domain.c_str(), 80, &handle_);
	req->SetHWnd(this->GetSafeHwnd());
	//req->set_url("/index.html");
	req->set_url("/download/banma_install.exe");
	req->set_host(domain);
	req->set_keep_alive(false);
	req->set_method(acl::HTTP_METHOD_GET);
	req->add_cookie("x-cookie-name", "cookie-value");
	//req->set_redirect(1); // �����Զ��ض���Ĵ�������

	// ֪ͨ�첽��Ϣ����������� HTTP �������

	//////////////////////////////////////////////////////////////////////////
	//acl::string buf;
	//req->build_request(buf);
	//printf("-----------------------------------------------\n");
	//printf("%s", buf.c_str());
	//printf("-----------------------------------------------\n");
	//////////////////////////////////////////////////////////////////////////

	GetDlgItem(IDC_DOWNLOAD)->EnableWindow(FALSE);
	service_->do_request(req);
}

LRESULT CHttpClientDlg::OnDownloadOk(WPARAM wParam, LPARAM lParam)
{
	GetDlgItem(IDC_DOWNLOAD)->EnableWindow(TRUE);
	return (0);
}