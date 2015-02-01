// winaioDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "winaio.h"
#include "lib_acl.h"
#include "aio_client.h"
#include "aio_server.h"
#include "winaioDlg.h"
#include ".\winaiodlg.h"
//#include "vld.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CwinaioDlg �Ի���



CwinaioDlg::CwinaioDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CwinaioDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CwinaioDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CwinaioDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_START, OnBnClickedStart)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CONNECT, OnBnClickedConnect)
	ON_BN_CLICKED(IDC_LISTEN, OnBnClickedListen)
	ON_BN_CLICKED(IDC_SET_TIMER, OnBnClickedSetTimer)
	ON_BN_CLICKED(IDC_DEL_TIMER, OnBnClickedDelTimer)
END_MESSAGE_MAP()

static ACL_AIO *__aio = NULL;

// CwinaioDlg ��Ϣ�������

BOOL CwinaioDlg::OnInitDialog()
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
	//acl_init();
	//VLDEnable();
	aio_client_init();
	__aio = acl_aio_create(ACL_EVENT_WMSG);
	ASSERT(__aio);
	
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CwinaioDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CwinaioDlg::OnPaint() 
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
HCURSOR CwinaioDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CwinaioDlg::OnBnClickedStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	const char *addr = "127.0.0.1:30082";
	aio_client_start(__aio, addr, 100);
}

void CwinaioDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (__aio)
	{
		aio_server_end();
		acl_aio_free(__aio);
		acl_pthread_end();
		__aio = NULL;
	}
	OnOK();
}

void CwinaioDlg::OnBnClickedConnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	const char *addr = "127.0.0.1:30082";
	aio_client_start(__aio, addr, 10);
}

void CwinaioDlg::OnBnClickedListen()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	const char *addr = "127.0.0.1:30082";
	aiho_server_start(__aio, addr,  1, 0);
}

typedef struct
{
	int  i;
	ACL_AIO *aio;
} CTX;

static void OnTimerCallback(int event_type, ACL_EVENT *event, void *context)
{
	CTX *ctx = (CTX*) context;

	ASSERT(ctx->aio == __aio);
	printf(">>>timer %d get now\r\n", ctx->i);
	free(ctx);
}

static CTX *__last_ctx;

void CwinaioDlg::OnBnClickedSetTimer()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CTX *ctx = NULL;

	for (int i = 0; i < 10; i++)
	{
		ctx = (CTX*) malloc(sizeof(CTX));
		ctx->aio = __aio;
		ctx->i = i;
		printf("set timer %d\n", i);
		acl_aio_request_timer(__aio, OnTimerCallback, ctx, i + 1, 1);
	}
	__last_ctx = ctx;
}

void CwinaioDlg::OnBnClickedDelTimer()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ASSERT(__last_ctx);
	printf("cancel timer %d\n", __last_ctx->i);
	acl_aio_cancel_timer(__aio, OnTimerCallback, __last_ctx);
	free(__last_ctx);
}
