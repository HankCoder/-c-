// win_dbserviceDlg.h : ͷ�ļ�
//

#pragma once

#include "acl_cpp/db/db_service.hpp"

// Cwin_dbserviceDlg �Ի���
class Cwin_dbserviceDlg : public CDialog
{
// ����
public:
	Cwin_dbserviceDlg(CWnd* pParent = NULL);	// ��׼���캯��

	// ����
	~Cwin_dbserviceDlg();

// �Ի�������
	enum { IDD = IDD_WIN_DBSERVICE_DIALOG };

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
public:
	afx_msg void OnBnClickedAddData();

private:
	acl::db_service* server_;
	acl::aio_handle* handle_;
public:
	afx_msg void OnBnClickedGetData();
	afx_msg void OnBnClickedDeleteData();
};
