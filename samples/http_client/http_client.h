// http_client.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// Chttp_clientApp:
// �йش����ʵ�֣������ http_client.cpp
//

class Chttp_clientApp : public CWinApp
{
public:
	Chttp_clientApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Chttp_clientApp theApp;
