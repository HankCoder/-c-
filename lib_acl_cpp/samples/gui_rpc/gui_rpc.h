// gui_rpc.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error �ڰ������� PCH �Ĵ��ļ�֮ǰ������stdafx.h��
#endif

#include "resource.h"		// ������


// Cgui_rpcApp:
// �йش����ʵ�֣������ gui_rpc.cpp
//

class Cgui_rpcApp : public CWinApp
{
public:
	Cgui_rpcApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cgui_rpcApp theApp;
