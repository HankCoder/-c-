// JawsCtrl.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif
#include "SingleCtrl.h"
#include "resource.h"		// ������


// CJawsCtrlApp:
// �йش����ʵ�֣������ JawsCtrl.cpp
//

class CJawsCtrlApp : public CWinApp
{
public:
	CJawsCtrlApp();

// ��д
	public:
	virtual BOOL InitInstance();

	CSingleCtrl m_singleCtrl;

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CJawsCtrlApp theApp;
