// master_trigger.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "acl_cpp/lib_acl.hpp"

static char *var_cfg_debug_msg;

static acl::master_str_tbl var_conf_str_tab[] = {
	{ "debug_msg", "test_msg", &var_cfg_debug_msg },

	{ 0, 0, 0 }
};

static int  var_cfg_debug_enable;

static acl::master_bool_tbl var_conf_bool_tab[] = {
	{ "debug_enable", 1, &var_cfg_debug_enable },

	{ 0, 0, 0 }
};

static int  var_cfg_io_timeout;

static acl::master_int_tbl var_conf_int_tab[] = {
	{ "io_timeout", 120, &var_cfg_io_timeout, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

static void (*format)(const char*, ...) = acl::log::msg1;

//////////////////////////////////////////////////////////////////////////
using namespace acl;

class master_trigger_test : public master_trigger
{
public:
	master_trigger_test() {}
	~master_trigger_test() {}

protected:
	// ���ി�麯������������ʱ�䵽ʱ���ô˺���
	virtual void on_trigger()
	{
		format("on trigger now\r\n");
	}

	// �����麯������������л��û����ǰ���ô˺���
	virtual void proc_pre_jail()
	{
		format("proc_pre_jail\r\n");
	}

	// �����麯������������л��û���ݺ���ô˺���
	virtual void proc_on_init()
	{
		format("proc init\r\n");
	}

	// �����麯������������˳�ǰ���ô˺���
	virtual void proc_on_exit()
	{
		format("proc exit\r\n");
	}
protected:
private:
};
//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	master_trigger_test mt;

	// �������ò�����
	mt.set_cfg_int(var_conf_int_tab);
	mt.set_cfg_int64(NULL);
	mt.set_cfg_str(var_conf_str_tab);
	mt.set_cfg_bool(var_conf_bool_tab);

	// ��ʼ����

	if (argc >= 2 && strcmp(argv[1], "alone") == 0)
	{
		format = (void (*)(const char*, ...)) printf;
		mt.run_alone(NULL, 5, 1);  // �������з�ʽ
	}
	else
	{
#ifdef	WIN32
		format = (void (*)(const char*, ...)) printf;
		mt.run_alone(NULL, 5, 1);  // �������з�ʽ
#else
		mt.run_daemon(argc, argv);  // acl_master ����ģʽ����
#endif
	}

	return 0;
}

