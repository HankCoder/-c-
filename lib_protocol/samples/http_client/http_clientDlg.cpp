// http_clientDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "http_client.h"
#include "HttpClient.h"
#include "HttpOption.h"
#include "http_clientDlg.h"
#include ".\http_clientdlg.h"

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


// Chttp_clientDlg �Ի���



Chttp_clientDlg::Chttp_clientDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Chttp_clientDlg::IDD, pParent)
	, m_sReqUrl(_T(""))
	, m_sLocalFile(_T(""))
	, m_bHttp11(TRUE)
	, m_bZip(FALSE)
	, m_bKeepAlive(FALSE)
	, m_sHttpHdrAppend(_T(""))
	, m_sServerAddr(_T(""))
	, m_sAccept(_T("application/json, text/javascript, */*; q=0.01"))
	, m_sCType(_T(""))
	, m_bUseAddr(FALSE)
	, m_nContentLength(-1)
	, m_bForwardAuto(FALSE)
	, m_nMaxTry(10)
	, m_bPostMethod(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Chttp_clientDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_URL, m_sReqUrl);
	DDX_Text(pDX, IDC_EDIT_FILE, m_sLocalFile);
	DDX_Control(pDX, IDC_EDIT_HDR_REQ, m_reqCtlEdit);
	DDX_Control(pDX, IDC_EDIT_HDR_RES, m_resCtlEdit);
	DDX_Control(pDX, IDC_EDIT_RES_BODY, m_resBodyCtlEdit);
}

BEGIN_MESSAGE_MAP(Chttp_clientDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_BROWSER, OnBnClickedButtonBrowser)
	ON_BN_CLICKED(IDC_BUTTON_OPTION, OnBnClickedButtonOption)
	ON_BN_CLICKED(IDC_BUTTON_GET, OnBnClickedButtonGet)
	ON_MESSAGE(WM_USER_DISPLAY, OnWriteDisplay)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnBnClickedButtonClear)
	ON_WM_SIZE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// Chttp_clientDlg ��Ϣ�������

BOOL Chttp_clientDlg::OnInitDialog()
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
	GetDlgItem(IDC_EDIT_FILE)->SetWindowText("result.txt");
	CheckDlgButton(IDC_CHECK_SAVE, 0);
	CheckDlgButton(IDC_CHECK_ECHO, 0);
	m_resBodyCtlEdit.SetLimitText(0);

	// ���״̬��
	int aWidths[3] = {50, 250, -1};
	m_wndMeterBar.SetParts(3, aWidths);

	m_wndMeterBar.Create(WS_CHILD | WS_VISIBLE | WS_BORDER
		| CCS_BOTTOM | SBARS_SIZEGRIP,
		CRect(0,0,0,0), this, 0); 
	m_wndMeterBar.SetText("����", 0, 0);
	m_wndMeterBar.SetText("", 1, 0);
	m_wndMeterBar.SetText("", 2, 0); //SBT_NOBORDERS);
	//int strPartDim[3]= {100, 200, -1}; //�ָ�����
	//m_StatusBar.SetParts(3, strPartDim);
	////����״̬���ı�
	//m_StatusBar.SetText("", 0, 0);
	//m_StatusBar.SetText("", 1, 0);
	//m_StatusBar.SetText("", 2, 0);
	//��������״̬���м���ͼ��
	//Ϊ�ڶ��������мӵ�ͼ��
	//m_StatusBar.SetIcon(0,	SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE));

	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void Chttp_clientDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void Chttp_clientDlg::OnPaint() 
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
HCURSOR Chttp_clientDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Chttp_clientDlg::OnBnClickedButtonBrowser()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog file(TRUE,"�ļ�","result.txt",OFN_HIDEREADONLY,"FILE(*.*)|*.*||",NULL);
	if(file.DoModal()==IDOK)
	{
		CString pathname;

		pathname=file.GetPathName();
		GetDlgItem(IDC_EDIT_FILE)->SetWindowText(pathname);
	}
}

void Chttp_clientDlg::OnBnClickedButtonOption()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CHttpOption option;

	option.m_bHttp11 = m_bHttp11;
	option.m_bZip = m_bZip;
	option.m_bKeepAlive = m_bKeepAlive;
	option.m_sHttpHdrAppend = m_sHttpHdrAppend;
	option.m_sServerAddr = m_sServerAddr;
	option.m_bUseAddr = m_bUseAddr;
	option.m_bForwardAuto = m_bForwardAuto;
	option.m_nMaxTry = m_nMaxTry;
	option.m_bPostMethod = m_bPostMethod;
	option.m_sCType = m_sCType;
	option.m_sAccept = m_sAccept;
	option.m_sHttpBody = m_sBody;
	option.m_bPostMethod = m_bPostMethod;

	if (option.DoModal() == IDOK) {
		m_bHttp11 = option.m_bHttp11;
		m_bZip = option.m_bZip;
		m_bKeepAlive = option.m_bKeepAlive;
		m_sHttpHdrAppend = option.m_sHttpHdrAppend;
		m_sServerAddr = option.m_sServerAddr;
		if (m_sServerAddr.GetLength() > 0)
			m_bUseAddr = option.m_bUseAddr;
		else
			m_bUseAddr = FALSE;
		m_bForwardAuto = option.m_bForwardAuto;
		m_nMaxTry = option.m_nMaxTry;
		m_bPostMethod = option.m_bPostMethod;
		if (m_bPostMethod)
		{
			m_sBody = option.m_sHttpBody;
			if (m_sBody.IsEmpty())
				m_bPostMethod = FALSE;
		}
		m_sCType = option.m_sCType;
		m_sAccept = option.m_sAccept;
		m_bPostMethod = option.m_bPostMethod;
	}
}

void Chttp_clientDlg::OnBnClickedButtonGet()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString sLocalFile, sReqUrl;

	GetDlgItem(IDC_EDIT_FILE)->GetWindowText(sLocalFile);
	if (sLocalFile.GetLength() == 0) {
		MessageBox("��������ȷ���ļ���!");
		return;
	}

	GetDlgItem(IDC_EDIT_URL)->GetWindowText(sReqUrl);
	if (sReqUrl.GetLength() == 0) {
		MessageBox("��������ȷ�� URL!");
		return;
	}

	ClearWinText();

	if (IsDlgButtonChecked(IDC_CHECK_SAVE))
		m_hClient.SaveAs(sLocalFile);
	if (IsDlgButtonChecked(IDC_CHECK_ECHO))
		m_hClient.DisplayResBody(TRUE);
	else
		m_hClient.DisplayResBody(FALSE);
	m_hClient.EnableHttp11(m_bHttp11);
	m_hClient.EnableZip(m_bZip);
	m_hClient.EnableKeepAlive(m_bKeepAlive);
	m_hClient.m_sHttpHdrAppend = m_sHttpHdrAppend;
	m_hClient.m_bUseAddr = m_bUseAddr;
	m_hClient.m_sServerAddr = m_sServerAddr;
	m_hClient.m_bForwardAuto = m_bForwardAuto;
	m_hClient.m_nMaxTry = m_nMaxTry;
	m_hClient.m_sHttpBody = m_sBody;
	m_hClient.m_bPostMethod = m_bPostMethod;
	m_hClient.m_sAccept = m_sAccept;
	m_hClient.m_sCtype = m_sCType;
	m_hClient.OnDataCallback(this->GetSafeHwnd(), WM_USER_DISPLAY);

	// ��������й�
	m_nContentLength = -1;
	m_nDownLen = 0;
	m_wndMeterBar.GetProgressCtrl().SetPos(0);

	m_wndMeterBar.SetText("����", 0, 0);

	m_hClient.GetUrl(sReqUrl);
	GetDlgItem(IDC_BUTTON_GET)->EnableWindow(FALSE);
}

LRESULT Chttp_clientDlg::OnWriteDisplay(WPARAM wParam, LPARAM lParam)
{
	CHttpReport *pReport = (CHttpReport*) wParam;

	if (pReport->m_type == TYPE_HDR_REQ) {
		m_reqCtlEdit.SetSel(m_reqCtlEdit.GetWindowTextLength(),
			m_reqCtlEdit.GetWindowTextLength());
		m_reqCtlEdit.ReplaceSel(pReport->m_pBuf);
	} else if (pReport->m_type == TYPE_HDR_RES) {
		m_resCtlEdit.SetSel(m_resCtlEdit.GetWindowTextLength(),
			m_resCtlEdit.GetWindowTextLength());
		m_resCtlEdit.ReplaceSel(pReport->m_pBuf);
	} else if (pReport->m_type == TYPE_BODY_RES) {
		m_resBodyCtlEdit.SetSel(m_resBodyCtlEdit.GetWindowTextLength(),
			m_resBodyCtlEdit.GetWindowTextLength());
		m_resBodyCtlEdit.ReplaceSel(pReport->m_pBuf);
	} else if (pReport->m_type == TYPE_TOTAL_LEN) {
		m_nContentLength = pReport->m_nContentLength;
		ASSERT(pReport->m_pBuf == NULL);
	} else if (pReport->m_type == TYPE_DOWN_LEN) {
		m_nDownLen += pReport->m_nDownLen;
		if (m_nContentLength > 0) {
			http_off_t  nStept;

			nStept = (m_nDownLen * 100) / (m_nContentLength);
			m_wndMeterBar.GetProgressCtrl().SetPos((int) nStept);
		}
		CString msg;
		msg.Format("�� %I64d �ֽ�, ��ʱ %.3f ����",
			m_nDownLen, pReport->m_timeRes);
		m_wndMeterBar.SetText(msg, 1, 0);
	} else if (pReport->m_type == TYPE_ERROR_CONNECT) {
		CString msg;

		msg.Format("%s", pReport->m_pBuf);
		m_wndMeterBar.GetProgressCtrl().SetText(msg.GetString());
		//m_wndMeterBar.GetProgressCtrl.SetWindowText(msg);
	} else if (pReport->m_type == TYPE_TIME_RES) {
		CString msg;
		msg.Format("�� %I64d �ֽ�, ��ʱ %.3f ����",
			m_nDownLen, pReport->m_timeRes);
		m_wndMeterBar.SetText(msg, 1, 0);
	} else if (pReport->m_type == TYPE_COMPLETE) {
		CString msg;
		msg.Format("���");
		m_wndMeterBar.SetText(msg, 0, 0);
		GetDlgItem(IDC_BUTTON_GET)->EnableWindow(TRUE);
	}

	ASSERT(pReport);
	delete pReport;

	return (0);
}

void Chttp_clientDlg::ClearWinText(void)
{
	m_reqCtlEdit.SetSel(0, m_reqCtlEdit.GetWindowTextLength());
	m_reqCtlEdit.ReplaceSel("");
	m_resCtlEdit.SetSel(0, m_resCtlEdit.GetWindowTextLength());
	m_resCtlEdit.ReplaceSel("");
	m_resBodyCtlEdit.SetSel(0, m_resBodyCtlEdit.GetWindowTextLength());
	m_resBodyCtlEdit.ReplaceSel("");
	m_wndMeterBar.SetText("����", 0, 0);
	m_wndMeterBar.SetText("", 1, 0);
	m_wndMeterBar.GetProgressCtrl().SetPos(0);

	//m_reqCtlEdit.Clear();
	//m_resCtlEdit.Clear();
	//m_resBodyCtlEdit.Clear();
}

void Chttp_clientDlg::OnBnClickedButtonClear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	ClearWinText();
}
void Chttp_clientDlg::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	if (m_wndMeterBar.GetSafeHwnd()) {
		int aWidths[3] = {cx / 3, cx / 3, -1};
		m_wndMeterBar.SetParts(3, aWidths);
		m_wndMeterBar.SetText("����", 0, 0);
		m_wndMeterBar.SetText("����2", 1, 0); //SBT_NOBORDERS);
		m_wndMeterBar.SetText("", 2, SBT_NOBORDERS);

		m_wndMeterBar.SendMessage(WM_SIZE, nType, MAKELONG(cy, cx));
	}
}

int Chttp_clientDlg::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������

	return 0;
}
