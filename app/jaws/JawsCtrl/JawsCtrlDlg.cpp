// JawsCtrlDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "lib_acl.h"
#include "JawsCtrl.h"
#include "JawsCtrlDlg.h"
#include ".\jawsctrldlg.h"

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


// CJawsCtrlDlg �Ի���

CJawsCtrlDlg::CJawsCtrlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJawsCtrlDlg::IDD, pParent)
	, m_trayIcon(IDR_MENU_ICON)
	, m_bShutdown(FALSE)
	, m_sJawsName(_T(""))
	, m_sJawsCtrlName(_T(""))
	, m_listenPort(8080)
	, m_listenIp(_T("127.0.0.1"))
	, m_httpVhostPath(_T(""))
	, m_httpVhostDefault(_T(""))
	, m_httpTmplPath(_T(""))
	, m_httpFilter(_T("HTTP_FILTER_HTTPD"))
	, m_nHttpFilter(IDC_RADIO_HTTPD)
	, m_pService(NULL)
	, m_regRun(_T("JawsCtrl"))
{
	char  buf[MAX_PATH];

	acl_proctl_daemon_path(buf, sizeof(buf));
	m_sJawsName.Format("%s\\Jaws.exe", buf);
	m_sJawsCtrlName.Format("%s\\JawsCtrl.exe", buf);

	//m_sJawsName.Format("C:\\\"Program Files\"\\\"Acl Project\"\\Jaws\\Jaws.exe");
	m_httpVhostPath.Format("%s\\conf\\www\\vhost", buf);
	m_httpVhostDefault.Format("%s\\conf\\www\\defaults.cf", buf);
	m_httpTmplPath.Format("%s\\conf\\www\\tmpl", buf);

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_procCtrl.RunThread();
}

void CJawsCtrlDlg::DoDataExchange(CDataExchange* pDX)
{
	BYTE field1, field2, field3, field4;

	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IPADDRESS_LISTEN, m_listenIpCtrl);
	if (pDX->m_bSaveAndValidate) {
		m_listenIpCtrl.GetAddress(field1, field2, field3, field4);
		m_listenIp.Format("%d.%d.%d.%d", field1, field2, field3, field4);
	}
	DDX_Text(pDX, IDC_EDIT_PORT, m_listenPort);
	DDV_MinMaxLong(pDX, m_listenPort, 0, 65535);
}

BEGIN_MESSAGE_MAP(CJawsCtrlDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_CREATE()
	ON_COMMAND(ID_OPEN_MAIN, OnOpenMain)
	ON_COMMAND(ID_QUIT, OnQuit)
	ON_MESSAGE(WM_MY_TRAY_NOTIFICATION, OnTrayNotification)
	ON_WM_NCPAINT()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_START, OnBnClickedButtonStart)
	ON_BN_CLICKED(IDC_BUTTON_STOP, OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_QUIT, OnBnClickedButtonQuit)
	ON_BN_CLICKED(IDC_MORE, OnBnClickedMore)
	ON_BN_CLICKED(IDC_AUTO_RUN, OnBnClickedAutoRun)
END_MESSAGE_MAP()


// CJawsCtrlDlg ��Ϣ�������

BOOL CJawsCtrlDlg::OnInitDialog()
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
	m_dVerticalExpand.Init(this, IDC_DIVIDER, TRUE);
	ExpandDialog(FALSE);

	theApp.m_singleCtrl.Register();
	m_listenIpCtrl.SetAddress(127, 0, 0, 1);
	CheckRadioButton(IDC_RADIO_HTTPD, IDC_RADIO_HTTP_PROXY, IDC_RADIO_HTTP_PROXY);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);

	// ���� Jaws.exe
	if (m_regRun.IfAutoRun()) {
		OnBnClickedButtonStart();
		CheckDlgButton(IDC_AUTO_RUN, 1);
	} else
		CheckDlgButton(IDC_AUTO_RUN, 0);

	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CJawsCtrlDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CJawsCtrlDlg::OnPaint() 
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
HCURSOR CJawsCtrlDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CJawsCtrlDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	m_trayIcon.SetNotificationWnd(this, WM_MY_TRAY_NOTIFICATION);
	m_trayIcon.SetIcon(IDI_MIN_ICON);
	return 0;
}

void CJawsCtrlDlg::OnOpenMain()
{
	// TODO: �ڴ���������������
	ShowWindow(SW_NORMAL);
}

void CJawsCtrlDlg::OnQuit()
{
	// TODO: �ڴ���������������
	m_bShutdown = TRUE;		// really exit
	SendMessage(WM_CLOSE);
}

afx_msg LRESULT CJawsCtrlDlg::OnTrayNotification(WPARAM uID, LPARAM lEvent)
{
	// let tray icon do default stuff
	return m_trayIcon.OnTrayNotification(uID, lEvent);
}
void CJawsCtrlDlg::OnNcPaint()
{
	// TODO: �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CDialog::OnNcPaint()
	static int i = 2;
	if(i > 0)
	{
		i --;
		ShowWindow(SW_HIDE);
	} else
	{
		CDialog::OnNcPaint();
	}
}

void CJawsCtrlDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	if (m_bShutdown) {
		if (m_pService) {
			m_procCtrl.StopOne(*m_pService);

			delete m_pService;
			m_pService = NULL;
		}
		CDialog::OnClose();
	} else {
		ShowWindow(SW_HIDE);
	}
}

void CJawsCtrlDlg::OnDestroy()
{
	CDialog::OnDestroy();

	// TODO: �ڴ˴������Ϣ����������
	theApp.m_singleCtrl.Remove();
}

void CJawsCtrlDlg::OnBnClickedButtonStart()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString addr;

	addr.Format("%s:%d", m_listenIp, m_listenPort);
	m_nHttpFilter = GetCheckedRadioButton(IDC_RADIO_HTTPD, IDC_RADIO_HTTP_PROXY);
	switch (m_nHttpFilter) {
	case IDC_RADIO_HTTPD:
		m_httpFilter.Format("HTTP_FILTER_HTTPD");
		break;
	case IDC_RADIO_HTTP_PROXY:
		m_httpFilter.Format("HTTP_FILTER_PROXY");
		break;
	default:
		m_httpFilter.Format("HTTP_FILTER_HTTPD");
		break;
	}

	ASSERT(m_pService == NULL);
	m_pService = new CHttpService(
		m_sJawsName.GetString(),
		addr.GetString(),
		m_httpVhostPath.GetString(),
		m_httpVhostDefault.GetString(),
		m_httpTmplPath.GetString(),
		m_httpFilter.GetString());

	m_pService->DebugArgv();
	m_procCtrl.StartOne(*m_pService);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_HTTPD)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_HTTP_PROXY)->EnableWindow(FALSE);
}

void CJawsCtrlDlg::OnBnClickedButtonStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ASSERT(m_pService != NULL);
	GetDlgItem(IDC_BUTTON_START)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_STOP)->EnableWindow(FALSE);
	m_procCtrl.StopOne(*m_pService);
	GetDlgItem(IDC_RADIO_HTTPD)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_HTTP_PROXY)->EnableWindow(TRUE);

	delete m_pService;
	m_pService = NULL;
}

void CJawsCtrlDlg::OnBnClickedButtonQuit()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_bShutdown = TRUE;		// really exit
	SendMessage(WM_CLOSE);
}

void CJawsCtrlDlg::OnBnClickedMore()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	static BOOL bExpand = TRUE;

	ExpandDialog(bExpand);
	bExpand = !bExpand;
}

void CJawsCtrlDlg::ExpandDialog(BOOL bExpand)
{
	CString sExpand;

	m_dVerticalExpand.Expand(bExpand);
	if (bExpand) {
		sExpand = "<<(&L)����";
	} else {
		sExpand = "(&M)����>>";
	}

	SetDlgItemText(IDC_MORE, sExpand);
}

void CJawsCtrlDlg::OnBnClickedAutoRun()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (IsDlgButtonChecked(IDC_AUTO_RUN))
		m_regRun.AutoRun(TRUE, m_sJawsCtrlName.GetString());
	else
		m_regRun.AutoRun(FALSE, m_sJawsCtrlName.GetString());
}
