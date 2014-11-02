// JencodeDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Jencode.h"
#include "Gb2Utf8.h"
#include "AclTrans.h"
#include "IdxTrans.h"
#include "JencodeDlg.h"
#include "DelBOM.h"
#include "JencodeDlg.h"
#include ".\jencodedlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_USER_TRANS_OVER	WM_USER + 100
#define WM_USER_TRANS_OVER2	WM_USER + 101

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


// CJencodeDlg �Ի���



CJencodeDlg::CJencodeDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJencodeDlg::IDD, pParent)
	, m_sIdxPath(_T(""))
	, m_fsPath(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJencodeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CJencodeDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_GB2UTF, OnBnClickedButtonGb2utf)
	ON_BN_CLICKED(IDC_BUTTON_UTF2GB, OnBnClickedButtonUtf2gb)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
	ON_MESSAGE(WM_USER_TRANS_OVER, OnTransOver)
	ON_MESSAGE(WM_USER_TRANS_OVER2, OnTransOver2)
	ON_BN_CLICKED(IDC_ACL_TRANS, OnBnClickedAclTrans)
	ON_BN_CLICKED(IDC_ACL_RESTORE, OnBnClickedAclRestore)
	ON_BN_CLICKED(IDC_IDX_SELECT, OnBnClickedIdxSelect)
	ON_BN_CLICKED(IDC_TRANS_IDX, OnBnClickedTransIdx)
	ON_BN_CLICKED(IDC_DEL_BOM, OnBnClickedDelBom)
END_MESSAGE_MAP()


// CJencodeDlg ��Ϣ�������

BOOL CJencodeDlg::OnInitDialog()
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

	acl_msg_open("jencode.log", "jencode");

	// ���״̬��
	int aWidths[2] = {50, -1};
	m_wndStatus.Create(WS_CHILD | WS_VISIBLE | WS_BORDER
		| CCS_BOTTOM | SBARS_SIZEGRIP,
		CRect(0,0,0,0), this, 0);
	m_wndStatus.SetParts(2, aWidths);
	m_wndStatus.SetText("����", 0, 0);
	m_wndStatus.SetText("", 1, 0);
	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CJencodeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CJencodeDlg::OnPaint() 
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
HCURSOR CJencodeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CJencodeDlg::ButtonsEnable(void)
{
	GetDlgItem(IDC_BUTTON_GB2UTF)->EnableWindow(TRUE);
	GetDlgItem(IDC_BUTTON_UTF2GB)->EnableWindow(TRUE);
	GetDlgItem(IDC_ACL_TRANS)->EnableWindow(TRUE);
	GetDlgItem(IDC_ACL_RESTORE)->EnableWindow(TRUE);
	GetDlgItem(IDC_DEL_BOM)->EnableWindow(TRUE);
}

BOOL CJencodeDlg::CheckPath(void)
{
	UpdateData(TRUE);
	GetDlgItem(IDC_EDIT_SPATH)->GetWindowText(m_sPath);
	//MessageBox(m_sPath);
	if (m_sPath.GetLength() == 0)
	{
		MessageBox("��ѡ��ԴĿ¼...");
		return FALSE;
	}

	m_dPath = m_sPath;

	//GetDlgItem(IDC_EDIT_DPATH)->GetWindowText(m_dPath);
	//if (m_dPath.GetLength() == 0)
	//{
	//	MessageBox("��ѡ��Ŀ��Ŀ¼...");
	//	return FALSE;
	//}

	return TRUE;
}

void CJencodeDlg::ButtonsDisable(void)
{
	GetDlgItem(IDC_BUTTON_GB2UTF)->EnableWindow(FALSE);
	GetDlgItem(IDC_BUTTON_UTF2GB)->EnableWindow(FALSE);
	GetDlgItem(IDC_ACL_TRANS)->EnableWindow(FALSE);
	GetDlgItem(IDC_ACL_RESTORE)->EnableWindow(FALSE);
	GetDlgItem(IDC_DEL_BOM)->EnableWindow(FALSE);
}

void CJencodeDlg::OnBnClickedButtonGb2utf()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!CheckPath())
		return;

	static CGb2Utf8 gb2Utf8("gbk", "utf-8");

	gb2Utf8.Init(this->GetSafeHwnd(), m_sPath, m_dPath);
	gb2Utf8.OnTransEnd(WM_USER_TRANS_OVER);
	gb2Utf8.Run();
	m_wndStatus.SetText("����", 0, 0);
	m_nBegin = time(NULL);
	ButtonsDisable();
}

void CJencodeDlg::OnBnClickedButtonUtf2gb()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!CheckPath())
	{
		MessageBox(m_sPath);
		return;
	}

	static CGb2Utf8 utf2gb("utf-8", "gbk");

	utf2gb.Init(this->GetSafeHwnd(), m_sPath, m_dPath);
	utf2gb.OnTransEnd(WM_USER_TRANS_OVER);
	utf2gb.Run();
	m_wndStatus.SetText("����", 0, 0);
	m_nBegin = time(NULL);
	ButtonsDisable();
}

void CJencodeDlg::OnBnClickedButton2()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString sPath;
	BROWSEINFO   bi;
	char name[MAX_PATH];
	ZeroMemory(&bi, sizeof(BROWSEINFO));

	bi.hwndOwner = GetSafeHwnd();
	bi.pszDisplayName = name;
	bi.lpszTitle = "Select folder";
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if(idl == NULL)
		return;
	SHGetPathFromIDList(idl, sPath.GetBuffer(MAX_PATH));  

	sPath.ReleaseBuffer();  

	if (sPath.Right(1) != "\\")
	{  
		sPath += "\\";
	}

	GetDlgItem(IDC_EDIT_SPATH)->SetWindowText(sPath);
//	CFileDialog file(TRUE,"�ļ�","result.txt",OFN_HIDEREADONLY,"FILE(*.*)|*.*||",NULL);
//	if(file.DoModal()==IDOK)
//	{
//		CString pathname;
//
//		pathname=file.GetPathName();
//		GetDlgItem(IDC_EDIT_SPATH)->SetWindowText(pathname);
//	}
}

void CJencodeDlg::OnBnClickedButton3()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString sPath;
	BROWSEINFO   bi;
	char name[MAX_PATH];
	ZeroMemory(&bi, sizeof(BROWSEINFO));

	bi.hwndOwner = GetSafeHwnd();
	bi.pszDisplayName = name;
	bi.lpszTitle = "Select folder";
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	LPITEMIDLIST idl = SHBrowseForFolder(&bi);
	if(idl == NULL)
		return;
	SHGetPathFromIDList(idl, sPath.GetBuffer(MAX_PATH));  

	sPath.ReleaseBuffer();  

	if (sPath.Right(1) != "\\")
	{  
		sPath += "\\";
	}

	GetDlgItem(IDC_EDIT_DPATH)->SetWindowText(sPath);
}

afx_msg LRESULT CJencodeDlg::OnTransOver(WPARAM uID, LPARAM lEvent)
{
	CString msg;

	ButtonsEnable();
	msg.Format("��ʱ��%d ��", time(NULL) - m_nBegin);
	m_wndStatus.SetText("���", 0, 0);
	m_wndStatus.SetText(msg, 1, 0);
	return (0);
}

afx_msg LRESULT CJencodeDlg::OnTransOver2(WPARAM uID, LPARAM lEvent)
{
	CString msg;

	GetDlgItem(IDC_TRANS_IDX)->EnableWindow(TRUE);
	msg.Format("��ʱ��%d ��", time(NULL) - m_nBegin);
	m_wndStatus.SetText("���!", 0, 0);
	m_wndStatus.SetText(msg, 1, 0);
	return (0);
}

void CJencodeDlg::OnBnClickedAclTrans()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!CheckPath())
		return;

	static CAclTrans aclTrans;

	aclTrans.Init(this->GetSafeHwnd(), m_sPath);
	aclTrans.OnTransEnd(WM_USER_TRANS_OVER);
	aclTrans.Run();
	m_wndStatus.SetText("����", 0, 0);
	m_nBegin = time(NULL);
	ButtonsDisable();
}

void CJencodeDlg::OnBnClickedAclRestore()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!CheckPath())
		return;

	static CAclTrans aclTrans;

	aclTrans.Init(this->GetSafeHwnd(), m_sPath);
	aclTrans.OnTransEnd(WM_USER_TRANS_OVER);
	aclTrans.Run(FALSE);
	m_wndStatus.SetText("����", 0, 0);
	m_nBegin = time(NULL);
	ButtonsDisable();
}

void CJencodeDlg::OnBnClickedDelBom()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if (!CheckPath())
		return;

	static CDelBOM delBom;

	delBom.Init(this->GetSafeHwnd(), m_sPath);
	delBom.OnDeleted(WM_USER_TRANS_OVER);
	delBom.Run();
	m_wndStatus.SetText("����", 0, 0);
	m_nBegin = time(NULL);
	ButtonsDisable();
}

void CJencodeDlg::OnBnClickedIdxSelect()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CFileDialog file(TRUE,"�ļ�","search.idx",OFN_HIDEREADONLY,"FILE(*.*)|*.*||",NULL);
	if(file.DoModal()==IDOK)
	{
		CString pathname;

		pathname=file.GetPathName();
		GetDlgItem(IDC_IDX_PATH)->SetWindowText(pathname);
	}
}

void CJencodeDlg::OnBnClickedTransIdx()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	static CIdxTrans idxTrans;

	UpdateData(TRUE);
	GetDlgItem(IDC_IDX_PATH)->GetWindowText(m_fsPath);
	if (m_fsPath.GetLength() == 0)
	{
		MessageBox("��ѡ�������ļ�...");
		return;
	}

	GetDlgItem(IDC_TRANS_IDX)->EnableWindow(FALSE);
	idxTrans.Init(this->GetSafeHwnd(), m_fsPath);
	idxTrans.OnTransEnd(WM_USER_TRANS_OVER2);
	idxTrans.Run();
	m_wndStatus.SetText("����", 0, 0);
	m_nBegin = time(NULL);
}

