// winaioDlg.h : ͷ�ļ�
//

#pragma once
#include <list>
#include "resource.h"
#include "AioServer.h"
#include "AioClient.h"

class CAioTimer;
class acl::aio_handle;
class acl::aio_listen_stream;

// CwinaioDlg �Ի���
class CwinaioDlg : public CDialog
{
// ����
public:
	CwinaioDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CwinaioDlg();

// �Ի�������
	enum { IDD = IDD_WINAIO_DIALOG };

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
	afx_msg void OnBnClickedListen();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedSetTimer();
	afx_msg void OnBnClickedDelTimer();
protected:
	// acl::aio_handle ���麯��
	void on_increase();
	void on_decrease();
private:
	CServerCallback callback_;
	acl::aio_handle* handle_;
	acl::aio_listen_stream* sstream_;
	bool keep_timer_;
	IO_CTX client_ctx_;
	void InitCtx();
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButtonKeepTimer();
	afx_msg void OnBnClickedButtonNoKeepTimer();
	afx_msg void OnBnClickedButtonMemtest();
private:
	std::list<CAioTimer*> timers_;
public:
};
