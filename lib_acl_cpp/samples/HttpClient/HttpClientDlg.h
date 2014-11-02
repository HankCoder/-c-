// HttpClientDlg.h : ͷ�ļ�
//

#pragma once
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/http/http_service.hpp"

// CHttpClientDlg �Ի���
class CHttpClientDlg : public CDialog
{
// ����
public:
	CHttpClientDlg(CWnd* pParent = NULL);	// ��׼���캯��
	~CHttpClientDlg();

// �Ի�������
	enum { IDD = IDD_HTTPCLIENT_DIALOG };

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
	afx_msg void OnBnClickedDownload();
	afx_msg LRESULT OnDownloadOk(WPARAM wParam, LPARAM lParam);

private:
	acl::aio_handle handle_;
	acl::http_service* service_;
};
