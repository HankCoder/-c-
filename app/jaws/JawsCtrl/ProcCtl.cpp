// ProcCtl.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "JawsCtrl.h"
#include "ProcCtl.h"
#include ".\procctl.h"


// CProcCtl

IMPLEMENT_DYNCREATE(CProcCtl, CWinThread)

CProcCtl::CProcCtl()
{
}

CProcCtl::~CProcCtl()
{
}

BOOL CProcCtl::InitInstance()
{
	// TODO: �ڴ�ִ���������̳߳�ʼ��
	return TRUE;
}

int CProcCtl::ExitInstance()
{
	// TODO: �ڴ�ִ���������߳�����
	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CProcCtl, CWinThread)
END_MESSAGE_MAP()


// CProcCtl ��Ϣ�������

int CProcCtl::Run()
{
	// TODO: �ڴ����ר�ô����/����û���

	return CWinThread::Run();
}
