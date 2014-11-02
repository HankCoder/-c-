// FileDirDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "lib_acl.h"  // ע�⣺lib_acl.h ������� stdafx.h �ĺ��棬�����������:(
#include "stdlib/avl.h"
#include "FileDir.h"
#include "FileDirDlg.h"
#include "ScanDir.h"
#include ".\filedirdlg.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef WIN32
#define snprintf _snprintf
#endif

#include <direct.h>
#define __S_ISTYPE(mode, mask)  (((mode) & _S_IFMT) == (mask))
#ifndef	S_ISDIR
# define S_ISDIR(mode)    __S_ISTYPE((mode), _S_IFDIR)
#endif

typedef struct MY_TYPE
{
	char  name[256];
	char  value[256];
	avl_node_t  node;
} MY_TYPE;


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


// CFileDirDlg �Ի���

CFileDirDlg::CFileDirDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFileDirDlg::IDD, pParent)
	, m_dirPath(_T(""))
	, m_nested(TRUE)
	, m_pScan(NULL)
	, m_avlName(_T(""))
	, m_avlValue(_T(""))
	, m_outName(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	avl_create(&m_avlTree, compare_fn, sizeof(MY_TYPE), offsetof(MY_TYPE, node));
}

void CFileDirDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PATH, m_dirPath);
	DDX_Check(pDX, IDC_CHECK_NETSTED, m_nested);
	DDX_Text(pDX, IDC_EDIT_AVL_NAME, m_avlName);
	DDX_Control(pDX, IDC_EDIT_AV_RESULT, m_debugWin);
	DDX_Text(pDX, IDC_EDIT_AVL_VALUE, m_avlValue);
	DDX_Check(pDX, IDC_CHECK_OUT_NAME, m_outName);
}

BEGIN_MESSAGE_MAP(CFileDirDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_EDIT_PATH, OnEnChangeEditPath)
	ON_BN_CLICKED(IDC_CHECK_NETSTED, OnBnClickedCheckNetsted)
	ON_BN_CLICKED(IDC_BUTTON_SCAN, OnBnClickedButtonScan)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_AVL_ADD, OnBnClickedButtonAvlAdd)
	ON_BN_CLICKED(IDC_BUTTON_AVL_FIND, OnBnClickedButtonAvlFind)
	ON_BN_CLICKED(IDC_BUTTON_WALK, OnBnClickedButtonWalk)
	ON_EN_CHANGE(IDC_EDIT_AVL_NAME, OnEnChangeEditAvlName)
	ON_BN_CLICKED(IDC_BUTTON_TEST, OnBnClickedButtonTest)
	ON_EN_CHANGE(IDC_EDIT_AVL_VALUE, OnEnChangeEditAvlValue)
	ON_BN_CLICKED(IDC_BUTTON_AVL_ADD_BAT, OnBnClickedButtonAvlAddBat)
	ON_BN_CLICKED(IDC_BUTTON_AVL_DEL, OnBnClickedButtonAvlDel)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_CHECK_OUT_NAME, OnBnClickedCheckOutName)
	ON_BN_CLICKED(IDC_SVN_UPDATE, OnBnClickedSvnUpdate)
END_MESSAGE_MAP()


// CFileDirDlg ��Ϣ�������

BOOL CFileDirDlg::OnInitDialog()
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

	CheckDlgButton(IDC_CHECK_NETSTED, BST_CHECKED);
	m_debugWin.SetLimitText(81920);
	acl_msg_open("svn.log", "svn_update");

	return TRUE;  // ���������˿ؼ��Ľ��㣬���򷵻� TRUE
}

void CFileDirDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CFileDirDlg::OnPaint() 
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
HCURSOR CFileDirDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CFileDirDlg::GetDirPath(void)
{
	CString msg;

	if (m_dirPath.GetLength() == 0) {
		MessageBox(_T("Please input the dir path ..."), _T("Error"));
		return (-1);
	}

	struct stat sbuf;
	if (stat(m_dirPath.GetString(), &sbuf) < 0) {
		msg.Format("%s no exist!", m_dirPath.GetString());	
		MessageBox(msg, "Error");
		return (-1);
	} else if (!S_ISDIR(sbuf.st_mode)) {
		msg.Format("%s is not a directory!", m_dirPath.GetString());
		MessageBox(msg, "Error");
		return (-1);
	}

	return 0;
}

void CFileDirDlg::OnTimer(UINT nIDEvent)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	CDialog::OnTimer(nIDEvent);
	ScanDir();
}

void CFileDirDlg::OnEnChangeEditPath()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
}

int CFileDirDlg::ScanCallback(ACL_SCAN_DIR * scan, void * ctx)
{
	CFileDirDlg *pfdd = (CFileDirDlg *) ctx;
	const char *ptr;

	if (pfdd->m_outName) {
		ptr = acl_scan_dir_file(scan);
		if (ptr && *ptr) {
			CString msg;
			struct acl_stat sbuf;

			acl_scan_stat(scan, &sbuf);
			msg.Format(">>>file(%s), size=%I64d\r\n", ptr,
				(acl_int64) sbuf.st_size);
			pfdd->DebugWinAppend(msg);
		}
	}

	if ((acl_scan_dir_nfiles(pfdd->m_pScan)
		+ acl_scan_dir_ndirs(pfdd->m_pScan)) % 100 == 0) {
		pfdd->UpdateInfo();
		pfdd->SetTimer(TIMER_EVENT_USER_100, 10, NULL);
		return (-1);  // ��ʱֹͣɨ��
	}

	return 0;
}

void CFileDirDlg::ScanDir(void)
{
	if (acl_scan_dir_end(m_pScan)) {
		ScanClose();
		return;
	}

	if (acl_scan_dir_size2(m_pScan) < 0) {
		ScanClose();
		return;
	}
	UpdateInfo();
}

// ������ʾ��Ϣ
void CFileDirDlg::UpdateInfo(void)
{
	if (m_pScan == NULL)
		return;

	CString msg;

	msg.Format(_T("%I64u MB"), acl_scan_dir_nsize(m_pScan) / MB);
	GetDlgItem(IDC_EDIT_SIZE)->SetWindowText(msg);

	msg.Format(_T("%d ��"), acl_scan_dir_nfiles(m_pScan));
	GetDlgItem(IDC_EDIT_NFILE)->SetWindowText(msg);

	msg.Format(_T("%d ��"), acl_scan_dir_ndirs(m_pScan));
	GetDlgItem(IDC_EDIT_NDIR)->SetWindowText(msg);
}

void CFileDirDlg::ScanClose(void)
{
	KillTimer(TIMER_EVENT_USER_100);
	UpdateInfo();
	acl_scan_dir_close(m_pScan);  // �ر�ɨ��Ŀ¼����
	m_pScan = NULL;
}

void CFileDirDlg::OnBnClickedCheckNetsted()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
}

void CFileDirDlg::OnBnClickedButtonScan()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString msg;

	if (GetDirPath() < 0)
		return;

	m_pScan = acl_scan_dir_open(m_dirPath.GetString(), m_nested);
	if (m_pScan == NULL) {
		msg.Format("open dir %s error", m_dirPath.GetString());
		MessageBox(msg, "Error");
		return;
	}

	// ���ûص��������ص�����
	acl_scan_dir_ctl(m_pScan, ACL_SCAN_CTL_FN, this->ScanCallback,
		ACL_SCAN_CTL_CTX, this, ACL_SCAN_CTL_END);

	ScanDir();
}

void CFileDirDlg::OnBnClickedButtonDelete()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString msg;

	if (GetDirPath() < 0)
		return;

	msg.Format("Are you sure to delete %s ?", m_dirPath.GetString());
	if (MessageBox(msg, "Warning", MB_YESNO) != IDYES)
		return;

	CScanDir scan(m_dirPath.GetString(), m_nested);

	if (scan.BeginRemove() < 0) {
		msg.Format("%s remove error", m_dirPath.GetString());
		MessageBox(msg, "Error");
		return;
	}

	__int64 size = scan.TotalSize();

	msg.Format(_T("%I64u MB"), size / MB);
	GetDlgItem(IDC_EDIT_SIZE)->SetWindowText(msg);

	int  n = scan.FileCount();
	msg.Format(_T("%d ��"), n);
	GetDlgItem(IDC_EDIT_NFILE)->SetWindowText(msg);

	n = scan.DirCount();
	msg.Format(_T("%d ��"), n);
	GetDlgItem(IDC_EDIT_NDIR)->SetWindowText(msg);
}

void CFileDirDlg::OnBnClickedButtonAvlAdd()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MY_TYPE *pm, m;

	if (m_avlName.GetLength() == 0) {
		MessageBox("Please input searching name!", "error");
		GetDlgItem(IDC_EDIT_AVL_NAME)->SetFocus();
		return;
	}
	if (m_avlValue.GetLength() == 0) {
		MessageBox("Please input value!", "error");
		GetDlgItem(IDC_EDIT_AVL_VALUE)->SetFocus();
		return;
	}

	snprintf(m.name, sizeof(m.name), "%s", m_avlName.GetString());
	pm = (MY_TYPE*) avl_find(&m_avlTree, &m, NULL);
	if (pm != NULL) {
		CString msg;

		msg.Format("key(%s) already exist, which value is %s",
			m_avlName.GetString(), pm->value);
		MessageBox(msg, "error");
		return;
	}

	pm = (MY_TYPE*) acl_mycalloc(1, sizeof(MY_TYPE));
	snprintf(pm->name, sizeof(pm->name), "%s", m_avlName.GetString());
	snprintf(pm->value, sizeof(pm->value), "%s", m_avlValue.GetString());
	avl_add(&m_avlTree, pm);
	MessageBox("Add ok!", "ok");
	// m_avlName.Empty();
	m_avlValue.Empty();
	UpdateData(FALSE);
}

void CFileDirDlg::OnBnClickedButtonAvlFind()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MY_TYPE m, *ptr;
	CString msg;

	if (m_avlName.GetLength() == 0) {
		MessageBox("Please input searching name", "error");
		GetDlgItem(IDC_EDIT_AVL_NAME)->SetFocus();
		return;
	}

	snprintf(m.name, sizeof(m.name), m_avlName.GetString());

	ptr = (MY_TYPE*) avl_find(&m_avlTree, &m, NULL);
	if (ptr) {
		m_avlValue.Format("%s", ptr->value);
	} else {
		m_avlValue.Empty();
		msg.Format("not find it, name=%s", m.name);
		MessageBox(msg);
	}

	UpdateData(FALSE);	
}

int CFileDirDlg::compare_fn(const void* v1, const void* v2)
{
	MY_TYPE *m1 = (MY_TYPE*) v1, *m2 = (MY_TYPE*) v2;

	return (strcmp(m1->name, m2->name));
}

void CFileDirDlg::OnBnClickedButtonWalk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MY_TYPE *next;
	CString msg;
	int   n = 0;

	next = (MY_TYPE*) avl_first(&m_avlTree);
	while (next) {
		msg.Format(">>name(%s), value(%s)\r\n", next->name, next->value);
		DebugWinAppend(msg);
		next = (MY_TYPE*) AVL_NEXT(&m_avlTree, next);
		n++;
	}

	msg.Format(">>ok, total count is %d\r\n", n);
	DebugWinAppend(msg);
}

void CFileDirDlg::OnEnChangeEditAvlName()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
}

void CFileDirDlg::OnBnClickedButtonTest()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	avl_node_t *node = (avl_node_t*) acl_mycalloc(1, sizeof(avl_node_t)), *pnode;
	avl_index_t where;

	where = AVL_MKINDEX(node, 1);
	pnode = AVL_INDEX2NODE(where);

	CString msg;

	msg.Format("addr=%x, %x, %d", pnode, node, pnode);
	if (pnode == node) {
		MessageBox("ok, ==");
		MessageBox(msg);
	} else {
		MessageBox("no");
	}
}

void CFileDirDlg::OnEnChangeEditAvlValue()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ�������������
	// ���͸�֪ͨ��������д CDialog::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�

	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
}

void CFileDirDlg::OnBnClickedButtonAvlAddBat()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MY_TYPE *pm, m;
	int   i, n = 0;
	CString msg;

	for (i = 0; i < 100; i++) {
		snprintf(m.name, sizeof(m.name), "%d", i);
		pm = (MY_TYPE*) avl_find(&m_avlTree, &m, NULL);
		if (pm != NULL) {
			msg.Format(">>key(%s) already exist, value(%s)\r\n",
				pm->name, pm->value);
			DebugWinAppend(msg);
			continue;
		}

		pm = (MY_TYPE*) acl_mycalloc(1, sizeof(MY_TYPE));
		snprintf(pm->name, sizeof(pm->name), "%d", i);
		snprintf(pm->value, sizeof(pm->value), "value(%d)", i);
		avl_add(&m_avlTree, pm);
		msg.Format(">>add one, key(%s), value(%s)\r\n", pm->name, pm->value);
		DebugWinAppend(msg);
		n++;
	}
	
	msg.Format(">>ok, %d records were added\r\n", n);
	DebugWinAppend(msg);
}

void CFileDirDlg::OnBnClickedButtonAvlDel()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	MY_TYPE m, *pm;

	if (m_avlName.GetLength() == 0) {
		MessageBox("Please input searching name!", "error");
		GetDlgItem(IDC_EDIT_AVL_NAME)->SetFocus();
		return;
	}

	snprintf(m.name, sizeof(m.name), "%s", m_avlName.GetString());
	pm = (MY_TYPE*) avl_find(&m_avlTree, &m, NULL);
	if (!pm) {
		MessageBox("not find", "ok");
	} else {
		avl_remove(&m_avlTree, pm);
		MessageBox("Delete ok!", "ok");
	}
	m_avlName.Empty();
	m_avlValue.Empty();
	UpdateData(FALSE);
}

void CFileDirDlg::OnBnClickedButtonClear()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	m_avlName.Empty();
	m_avlValue.Empty();
	UpdateData(FALSE);
	m_debugWin.SetSel(0, m_debugWin.GetWindowTextLength());
	m_debugWin.ReplaceSel("");
}

void CFileDirDlg::DebugWinAppend(CString& msg)
{
	m_debugWin.SetSel(m_debugWin.GetWindowTextLength(),
		m_debugWin.GetWindowTextLength());
	m_debugWin.ReplaceSel(msg.GetString());
}

void CFileDirDlg::OnBnClickedCheckOutName()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
}

void CFileDirDlg::OnBnClickedSvnUpdate()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString msg;

	if (GetDirPath() < 0)
		return;

	m_pScan = acl_scan_dir_open(m_dirPath.GetString(), m_nested);
	if (m_pScan == NULL) {
		msg.Format("open dir %s error", m_dirPath.GetString());
		MessageBox(msg, "Error");
		return;
	}

	// ���ûص��������ص�����
	acl_scan_dir_ctl(m_pScan, ACL_SCAN_CTL_FN, ScanSvnCallback,
		ACL_SCAN_CTL_CTX, this, ACL_SCAN_CTL_END);

	ScanSvnDir();
}

int CFileDirDlg::ScanSvnCallback(ACL_SCAN_DIR * scan, void * ctx)
{
	CFileDirDlg *pfdd = (CFileDirDlg *) ctx;

	if ((acl_scan_dir_nfiles(pfdd->m_pScan)
		+ acl_scan_dir_ndirs(pfdd->m_pScan)) % 100 == 0)
	{
		pfdd->UpdateInfo();
		pfdd->SetTimer(TIMER_EVENT_USER_100, 10, NULL);
		return (-1);  // ��ʱֹͣɨ��
	}

	if (!pfdd->m_outName)
		return 0;

	const char* filename = acl_scan_dir_file(scan);
	const char* path = acl_scan_dir_path(scan);
	if (!filename || strcasecmp(filename, "entries") || !path)
		return 0;

	CString filepath;
	filepath.Format("%s/%s", path, filename);

	if (acl_strrncasecmp(filepath.GetString(),
		".svn/entries", sizeof(".svn/entries") - 1) != 0)
	{
		return 0;
	}

	CString msg;
	struct acl_stat sbuf;

	acl_assert(path);
	acl_scan_stat(scan, &sbuf);
	msg.Format(">>>file(%s/%s), size=%I64d\r\n",
		path, filename, (acl_int64) sbuf.st_size);
	pfdd->DebugWinAppend(msg);
	return 0;
}

void CFileDirDlg::ScanSvnDir(void)
{
	if (acl_scan_dir_end(m_pScan)) {
		ScanClose();
		return;
	}

	if (acl_scan_dir_size2(m_pScan) < 0) {
		ScanClose();
		return;
	}
	UpdateInfo();
}

BOOL CFileDirDlg::UpdateSvn(const char* filepath)
{
	ssize_t file_size;
	char* buf = acl_vstream_loadfile2(filepath, &file_size);
	if (buf == NULL)
	{
		acl_msg_error("loadfile(%s) error(%s)",
			filepath, acl_last_serror());
		return FALSE;
	}

	ACL_VSTREAM* fp = acl_vstream_fopen(filepath,
		O_WRONLY | O_BINARY | O_TRUNC, 0700, 4096);
	if (fp == NULL)
	{
		acl_myfree(buf);
		acl_msg_error("fopen(%s) error(%s)",
			filepath, acl_last_serror());
		return FALSE;
	}

	ACL_VSTRING* vbuf = acl_vstring_alloc(2048);
	const char* ptr = buf;
	const char* old_svn = "svn://192.168.1.231";
	size_t old_npre = strlen(old_svn);
	const char* new_svn = "svn://122.49.0.188";
	size_t new_npre = strlen(new_svn);

#define STR	acl_vstring_str
#define LEN	ACL_VSTRING_LEN

	int   i = 0;
	while (TRUE)
	{
		// ֻ��Ҫ�޸�ǰ 10 ���ݼ���
		if (i++ > 10)
			break;

		// �ӻ������л��һ������
		const ACL_VSTRING* vp = acl_buffer_gets(vbuf, &ptr, strlen(ptr));
		if (vp == NULL)
			break;

		// �Ƚ�ǰ׺�Ƿ����
		if (strncasecmp(old_svn, STR(vp), old_npre) != 0)
		{
			if (acl_vstream_writen(fp, STR(vp), LEN(vp)) == ACL_VSTREAM_EOF)
			{
				acl_msg_error("%s(%d): write error(%s)",
					__FUNCTION__, __LINE__,
					acl_last_serror());
				goto end;
			}
			ACL_VSTRING_RESET(vbuf);
			continue;
		}

		// д���µ�ǰ׺
		if (acl_vstream_writen(fp, new_svn, new_npre) == ACL_VSTREAM_EOF)
		{
			acl_msg_error("%s(%d): write error(%s)",
				__FUNCTION__, __LINE__, acl_last_serror());
			goto end;
		}

		const char* pleft = STR(vp) + old_npre;
		if (*pleft == 0)
		{
			ACL_VSTRING_RESET(vbuf);
			continue;
		}

		// д�����������ʣ�ಿ��
		if (acl_vstream_writen(fp, pleft, strlen(pleft)) == ACL_VSTREAM_EOF)
		{
			acl_msg_error("%s(%d): write error(%s)",
				__FUNCTION__, __LINE__, acl_last_serror());
			goto end;
		}
		ACL_VSTRING_RESET(vbuf);
	}

	ssize_t nleft = (ssize_t) (file_size - (ptr - buf));
	if (*ptr && nleft > 0)
	{
		if (acl_vstream_writen(fp, ptr, nleft) == ACL_VSTREAM_EOF)
			acl_msg_error("%s(%d): write nleft(%d) error(%s)",
				__FUNCTION__, __LINE__,
				(int) nleft, acl_last_serror());
	}

end:
	acl_vstring_free(vbuf);
	acl_vstream_fclose(fp);
	acl_myfree(buf);
	return TRUE;
}