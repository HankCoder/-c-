// winaioDlg.h : ͷ�ļ�
//

#pragma once


// CwinaioDlg �Ի���
class CwinaioDlg : public CDialog
{
// ����
public:
	CwinaioDlg(CWnd* pParent = NULL);	// ��׼���캯��

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
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedConnect();
	afx_msg void OnBnClickedListen();
	afx_msg void OnBnClickedSetTimer();
	afx_msg void OnBnClickedDelTimer();
};
