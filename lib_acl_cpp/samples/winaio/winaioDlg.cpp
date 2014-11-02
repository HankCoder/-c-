// winaioDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "lib_acl.h"
#include <iostream>
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/stream/aio_stream.hpp"
#include "AioServer.h"
#include "AioClient.h"
#include "AioTimer.hpp"
#include "winaio.h"
//#include "vld.h"
//#include "heap_hook.h"
#include "winaioDlg.h"
#include ".\winaiodlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef WIN32
# ifndef snprintf
#  define snprintf _snprintf
# endif
#endif

//CHeapHook2 vld2;

using namespace acl;

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
	, sstream_(NULL)
	, handle_(NULL)
	, keep_timer_(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CwinaioDlg::~CwinaioDlg()
{
	std::list<CAioTimer*>::iterator it = timers_.begin(), it_next;
	for (it_next = it; it != timers_.end(); it = it_next)
	{
		it_next++;
		handle_->del_timer(*it);
		timers_.erase(it);
	}
	if (handle_)
		delete handle_;
}

void CwinaioDlg::InitCtx()
{
	memset(&client_ctx_, 0, sizeof(client_ctx_));
	client_ctx_.connect_timeout = 5;
	client_ctx_.nopen_limit = 10;
	client_ctx_.id_begin = 1;
	client_ctx_.nwrite_limit = 10;
	client_ctx_.debug = false;
	snprintf(client_ctx_.addr, sizeof(client_ctx_.addr), "127.0.0.1:9001");
	client_ctx_.handle = handle_;
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
	ON_BN_CLICKED(IDC_LISTEN, OnBnClickedListen)
	ON_BN_CLICKED(IDC_CONNECT, OnBnClickedConnect)
	ON_BN_CLICKED(IDC_SET_TIMER, OnBnClickedSetTimer)
	ON_BN_CLICKED(IDC_DEL_TIMER, OnBnClickedDelTimer)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_KEEP_TIMER, OnBnClickedButtonKeepTimer)
	ON_BN_CLICKED(IDC_BUTTON_NO_KEEP_TIMER, OnBnClickedButtonNoKeepTimer)
	ON_BN_CLICKED(IDC_BUTTON_MEMTEST, OnBnClickedButtonMemtest)
END_MESSAGE_MAP()


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
	GetDlgItem(IDC_SET_TIMER)->EnableWindow(TRUE);
	GetDlgItem(IDC_DEL_TIMER)->EnableWindow(FALSE);

	GetDlgItem(IDC_BUTTON_KEEP_TIMER)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_NO_KEEP_TIMER)->EnableWindow(FALSE);

	FILE *fp;
	AllocConsole();
	fp = freopen("CONOUT$","w+t",stdout);

	handle_ = new acl::aio_handle(ENGINE_WINMSG);
	InitCtx();
	
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

void CwinaioDlg::OnBnClickedListen()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	sstream_ = new aio_listen_stream(handle_);
	const char* addr = "127.0.0.1:9001";
	// ����ָ���ĵ�ַ
	if (sstream_->open(addr) == false)
	{
		std::cout << "open " << addr << " error!" << std::endl;
		sstream_->close(); // �ͷż�������ռ��Դ
		handle_->check(); // ��������첽����ռ��Դ
		acl_pthread_end(); // ��������߳���ռ��Դ
		return;
	}
	GetDlgItem(IDC_LISTEN)->EnableWindow(FALSE);
	// �����ص�����󣬵��������ӵ���ʱ�Զ����ô������Ļص�����
	sstream_->add_accept_callback(&callback_);
	std::cout << "Listen: " << addr << " ok!" << std::endl;
}

void CwinaioDlg::OnBnClickedConnect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (CConnectClientCallback::connect_server(&client_ctx_,
			client_ctx_.id_begin) == false)
	{
		std::cout << "connect " << client_ctx_.addr << " error!" << std::endl;
	}
	else
		GetDlgItem(IDC_CONNECT)->EnableWindow(FALSE);
}

void CwinaioDlg::OnBnClickedSetTimer()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	GetDlgItem(IDC_SET_TIMER)->EnableWindow(FALSE);
	GetDlgItem(IDC_DEL_TIMER)->EnableWindow(TRUE);
	__int64 inter = 1000000;
	for (int i = 0; i < 10; i++)
	{
		CAioTimer* timer = new CAioTimer(handle_, i, keep_timer_);
		handle_->set_timer(timer, (i + 1) * inter, 1);
		if (timer->keep_timer())
			timers_.push_back(timer);
	}
}

void CwinaioDlg::OnBnClickedDelTimer()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	GetDlgItem(IDC_SET_TIMER)->EnableWindow(TRUE);
	GetDlgItem(IDC_DEL_TIMER)->EnableWindow(FALSE);

	std::list<CAioTimer*>::iterator it = timers_.begin(), it_next;
	for (it_next = it; it != timers_.end(); it = it_next)
	{
		it_next++;
		handle_->del_timer(*it);
		timers_.erase(it);
	}
}

void CwinaioDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (sstream_)
	{
		sstream_->close(); // �ͷż�������Դ
		handle_->check(); // ������һ�������첽����ռ����Դ
	}
	acl_pthread_end(); // ��������߳���ռ��Դ

	OnOK();
}

void CwinaioDlg::on_increase()
{

}

void CwinaioDlg::on_decrease()
{
	/* ����첽�������ܼ�ص��첽������ */
	int nleft = handle_->length();
	if (client_ctx_.nopen_total == client_ctx_.nopen_limit && nleft == 1)
	{
		std::cout << "All client streams over! just one listen stream: "
			<< std::endl;

		InitCtx();
		GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);
	}
}
void CwinaioDlg::OnBnClickedButtonKeepTimer()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	keep_timer_ = true;
	GetDlgItem(IDC_BUTTON_KEEP_TIMER)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_NO_KEEP_TIMER)->EnableWindow(TRUE);
}

void CwinaioDlg::OnBnClickedButtonNoKeepTimer()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	keep_timer_ = false;
	GetDlgItem(IDC_BUTTON_KEEP_TIMER)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_NO_KEEP_TIMER)->EnableWindow(FALSE);
}

void CwinaioDlg::OnBnClickedButtonMemtest()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CMemory* m = new CMemory;
	delete m;
}
