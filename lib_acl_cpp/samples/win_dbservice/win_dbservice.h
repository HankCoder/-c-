// win_dbservice.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// Cwin_dbserviceApp:
// �йش����ʵ�֣������ win_dbservice.cpp
//

class Cwin_dbserviceApp : public CWinApp
{
public:
	Cwin_dbserviceApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cwin_dbserviceApp theApp;
