#pragma once

#include "lib_acl.h"

// CProcCtl

class CProcCtl : public CWinThread
{
	DECLARE_DYNCREATE(CProcCtl)

protected:
	CProcCtl();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CProcCtl();

public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual int Run();
};


