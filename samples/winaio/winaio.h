// winaio.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// CwinaioApp:
// �йش����ʵ�֣������ winaio.cpp
//

class CwinaioApp : public CWinApp
{
public:
	CwinaioApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CwinaioApp theApp;
