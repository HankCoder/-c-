#pragma once
#include "afxwin.h"


// COptionOnClose �Ի���

class COptionOnClose : public CDialog
{
	DECLARE_DYNCREATE(COptionOnClose)

public:
	COptionOnClose(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~COptionOnClose();

	void init(BOOL QuitOnClose);

// �Ի�������
	enum { IDD = IDD_DIALOG_QUIT};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	BOOL m_QuitClose;
	BOOL m_MinOnClose;
	BOOL m_SaveOption;
};
