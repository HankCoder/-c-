// win_dbserviceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "acl_cpp/db/db_service_sqlite.hpp"
#include "acl_cpp/db/db_handle.hpp"
#include "acl_cpp/db/db_sqlite.hpp"
#include "win_dbservice.h"
#include "win_dbserviceDlg.h"
#include ".\win_dbservicedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////////


class myquery : public acl::db_query
{
public:
	myquery(int id) : id_(id)
	{

	}

	~myquery()
	{

	}

	// ������ӿڣ��� SQL ������ʱ�Ļص�����
	virtual void on_error(acl::db_status status)
	{
		(void) status;
		printf(">>on error, id: %d\r\n", id_);
	}

	// ������ӿڣ��� SQL ���ɹ�ʱ�Ļص�����
	virtual void on_ok(const acl::db_rows* rows, int affected)
	{
		if (rows)
			printf(">>on ok, id: %d, rows->legnth: %u, group_name: %s\r\n",
				id_, rows->length(), (*(*rows)[0])["group_name"]);
		else
			printf(">>on ok, id: %d, affected: %d\r\n",
				id_, affected);
	}

	// ������ӿڣ�������ʵ�������ͷ�ʱ�Ļص�����
	virtual void destroy()
	{
		printf(">> myquery destroy now\r\n");
		delete this;
	}
protected:
private:
	int   id_;
};

static acl::string __dbfile("����.db");

const char* CREATE_TBL =
"create table group_tbl\r\n"
"(\r\n"
"group_name varchar(128) not null,\r\n"
"uvip_tbl varchar(32) not null default 'uvip_tbl',\r\n"
"access_tbl varchar(32) not null default 'access_tbl',\r\n"
"access_week_tbl varchar(32) not null default 'access_week_tbl',\r\n"
"access_month_tbl varchar(32) not null default 'access_month_tbl',\r\n"
"update_date date not null default '1970-1-1',\r\n"
"disable integer not null default 0,\r\n"
"add_by_hand integer not null default 0,\r\n"
"class_level integer not null default 0,\r\n"
"primary key(group_name, class_level)\r\n"
")";

static bool tbl_create(acl::db_handle& db)
{
	if (db.tbl_exists("group_tbl"))
		return (true);
	if (db.sql_update(CREATE_TBL) == false)
	{
		printf("sql error\r\n");
		return (false);
	}
	else
	{
		printf("create table ok\r\n");
		return (true);
	}
}

static bool create_db(void)
{
	acl::db_sqlite db(__dbfile);

	if (db.open() == false)
	{
		printf("open dbfile: %s error\r\n", __dbfile.c_str());
		return (false);
	}
	db.show_conf();
	return (tbl_create(db));
}

//////////////////////////////////////////////////////////////////////////

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


// Cwin_dbserviceDlg �Ի���



Cwin_dbserviceDlg::Cwin_dbserviceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Cwin_dbserviceDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	server_ = NULL;
	handle_ = new acl::aio_handle(acl::ENGINE_WINMSG);
}

Cwin_dbserviceDlg::~Cwin_dbserviceDlg()
{
	if (server_)
		delete server_;
	delete handle_;
}

void Cwin_dbserviceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cwin_dbserviceDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_ADD_DATA, OnBnClickedAddData)
	ON_BN_CLICKED(IDC_GET_DATA, OnBnClickedGetData)
	ON_BN_CLICKED(IDC_DELETE_DATA, OnBnClickedDeleteData)
END_MESSAGE_MAP()


// Cwin_dbserviceDlg ��Ϣ�������

BOOL Cwin_dbserviceDlg::OnInitDialog()
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

	ShowWindow(SW_MINIMIZE);

	// TODO: �ڴ���Ӷ���ĳ�ʼ������
	
	// �� DOS ����
	AllocConsole();
	FILE* fp = freopen("CONOUT$","w+t",stdout);
	// �򿪿�� DOS ����
	acl::open_dos();

	logger_open("dbservice.log", "dbservice", "all:1");
	if (create_db() == false)
		printf(">>create table error\r\n");

	// ���û��� WIN32 ��Ϣģʽ��IPC��ʽ
	server_ = new acl::db_service_sqlite("DB_TEST", __dbfile, 2, 2, true);
	if (server_->open(handle_) == false)
	{
		printf("open db service failed\r\n");
		delete server_;
		server_ = NULL;
	}

	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void Cwin_dbserviceDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Cwin_dbserviceDlg::OnPaint() 
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
HCURSOR Cwin_dbserviceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

// ������ݹ���
void Cwin_dbserviceDlg::OnBnClickedAddData()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (server_ == NULL)
	{
		MessageBox("db not opened yet!");
		return;
	}

	acl::string sql;
	myquery* query;

	for (int i = 0; i < 1000; i++)
	{
		query = new myquery(i);
		sql.format("insert into group_tbl('group_name', 'uvip_tbl')"
			" values('�й���-%d', 'test')", i);
		server_->sql_update(sql.c_str(), query);
	}
}

// ��ѯ���ݹ���
void Cwin_dbserviceDlg::OnBnClickedGetData()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (server_ == NULL)
	{
		MessageBox("db not opened yet!");
		return;
	}

	acl::string sql;
	myquery* query;

	for (int i = 0; i < 1000; i++)
	{
		query = new myquery(i);
		sql.format("select * from group_tbl"
			" where group_name='�й���-%d'"
			" and uvip_tbl='test'", i);
		server_->sql_select(sql.c_str(), query);
	}
}

// ɾ�����ݹ���
void Cwin_dbserviceDlg::OnBnClickedDeleteData()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	if (server_ == NULL)
	{
		MessageBox("db not opened yet!");
		return;
	}

	acl::string sql;
	myquery* query;

	for (int i = 0; i < 1000; i++)
	{
		query = new myquery(i);
		sql.format("delete from group_tbl"
			" where group_name='�й���-%d'"
			" and uvip_tbl='test'", i);
		server_->sql_update(sql.c_str(), query);
	}
}
