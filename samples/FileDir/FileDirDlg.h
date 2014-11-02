// FileDirDlg.h : ͷ�ļ�
//

#pragma once

#include "lib_acl.h"
#include "afxcmn.h"
#include "afxwin.h"

// CFileDirDlg �Ի���
class CFileDirDlg : public CDialog
{
// ����
public:
	CFileDirDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_FILEDIR_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CString m_dirPath;
	BOOL m_nested;
private:
	int GetDirPath(void);
public:
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnEnChangeEditPath();
private:
	ACL_SCAN_DIR *m_pScan;
	static int ScanCallback(ACL_SCAN_DIR * scan, void * ctx);
	static int ScanSvnCallback(ACL_SCAN_DIR * scan, void * ctx);
	void ScanDir(void);
	void ScanSvnDir(void);
	// ������ʾ��Ϣ
	void UpdateInfo(void);
	BOOL UpdateSvn(const char* filepath);
	void ScanClose(void);
public:
	afx_msg void OnBnClickedCheckNetsted();
	afx_msg void OnBnClickedButtonScan();
	afx_msg void OnBnClickedButtonDelete();
	afx_msg void OnBnClickedButtonAvlAdd();
	afx_msg void OnBnClickedButtonAvlFind();
private:
	avl_tree_t m_avlTree;
public:
	static int compare_fn(const void* v1, const void* v2);
	afx_msg void OnBnClickedButtonWalk();
public:
	afx_msg void OnEnChangeEditAvlName();
public:
	afx_msg void OnEnChangeRichedit2AvlResult();
	afx_msg void OnBnClickedButtonTest();
private:
	CEdit m_debugWin;
public:
	afx_msg void OnEnChangeEditAvlValue();
private:
	CString m_avlValue;
	CString m_avlName;
public:
	afx_msg void OnBnClickedButtonAvlAddBat();
	afx_msg void OnBnClickedButtonAvlDel();
	afx_msg void OnBnClickedButtonClear();
private:
	void DebugWinAppend(CString& msg);
public:
	// �Ƿ�ɨ��Ŀ¼���ļ������
	BOOL m_outName;
	afx_msg void OnBnClickedCheckOutName();
	afx_msg void OnBnClickedSvnUpdate();
};
