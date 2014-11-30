// http_clientDlg.h : ͷ�ļ�
//

#pragma once
#include "HttpClient.h"
#include "MeterBar.h"
#include "lib_protocol.h"
#include "afxwin.h"
#include "afxext.h"

// Chttp_clientDlg �Ի���
class Chttp_clientDlg : public CDialog
{
// ����
public:
	Chttp_clientDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_HTTP_CLIENT_DIALOG };

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
	afx_msg void OnBnClickedButtonBrowser();
	afx_msg void OnBnClickedButtonOption();
	afx_msg void OnBnClickedButtonGet();
	afx_msg LRESULT OnWriteDisplay(WPARAM wParam, LPARAM lParam);
private:
	CString m_sReqUrl;
	CString m_sLocalFile;
	CHttpClient m_hClient;
	CEdit m_reqCtlEdit;
	CEdit m_resCtlEdit;
	CEdit m_resBodyCtlEdit;
	BOOL m_bHttp11;
	BOOL m_bZip;
	BOOL m_bKeepAlive;
	BOOL m_bPostMethod;
	CString m_sBody;
	CString m_sAccept;
	CString m_sCType;

	http_off_t m_nContentLength;
	http_off_t m_nDownLen;

	void ClearWinText(void);
public:
	afx_msg void OnBnClickedButtonClear();
private:
	CString m_sHttpHdrAppend;
	CString m_sServerAddr;
	BOOL m_bUseAddr;
protected:
	CMeterBar m_wndMeterBar;
	//CStatusBarCtrl m_wndMeterBar;
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
private:
	BOOL m_bForwardAuto;
	UINT m_nMaxTry;
};
