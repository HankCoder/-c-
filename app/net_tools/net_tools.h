// net_tools.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "SingleCtrl.h"
#include "resource.h"		// ������


// Cnet_toolsApp:
// �йش����ʵ�֣������ net_tools.cpp
//

class Cnet_toolsApp : public CWinApp
{
public:
	Cnet_toolsApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()

public:
	CSingleCtrl m_singleCtrl;
};

extern Cnet_toolsApp theApp;
