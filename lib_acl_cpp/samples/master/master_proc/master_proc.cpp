// master_proc.cpp : �������̨Ӧ�ó������ڵ㡣
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

class master_proc_test : public master_proc
{
public:
	master_proc_test() {}
	~master_proc_test() {}
protected:
	/**
	 * ���ി�麯���������յ�һ���ͻ�������ʱ���ô˺���
	 * @param stream {aio_socket_stream*} �½��յ��Ŀͻ����첽������
	 * ע���ú������غ������ӽ��ᱻ�رգ��û���Ӧ�����رո���
	 */
	virtual void on_accept(socket_stream* stream)
	{
		if (stream->format("hello, you're welcome!\r\n") == -1)
			return;
		while (true)
		{
			if (on_read(stream) == false)
				break;
		}
	}

	bool on_read(socket_stream* stream)
	{
		string buf;
		if (stream->gets(buf) == false)
		{
			format("gets error: %s", acl::last_serror());
			return false;
		}
		if (buf == "quit")
		{
			stream->puts("bye!");
			return false;
		}

		if (buf.empty())
		{
			if (stream->write("\r\n") == -1)
			{
				format("write 1 error: %s", acl::last_serror());
				return false;
			}
		}
		else if (stream->write(buf) == -1)
		{
			format("write 2 error: %s, buf(%s), len: %d",
				acl::last_serror(), buf.c_str(), (int) buf.length());
			return false;
		}
		else if (stream->write("\r\n") == -1)
		{
			format("write 3 client error: %s", acl::last_serror());
			return false;
		}
		return true;
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
private:
};
//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	master_proc_test& mp = singleton2<master_proc_test>::get_instance();

	// �������ò�����
	mp.set_cfg_int(var_conf_int_tab);
	mp.set_cfg_int64(NULL);
	mp.set_cfg_str(var_conf_str_tab);
	mp.set_cfg_bool(var_conf_bool_tab);

	// ��ʼ����

	if (argc >= 2 && strcmp(argv[1], "alone") == 0)
	{
		format = (void (*)(const char*, ...)) printf;
		mp.run_alone("127.0.0.1:8888", NULL, 5);  // �������з�ʽ
	}
	else
	{
#ifdef	WIN32
		format = (void (*)(const char*, ...)) printf;
		mp.run_alone("127.0.0.1:8888", NULL, 5);  // �������з�ʽ
#else
		mp.run_daemon(argc, argv);  // acl_master ����ģʽ����
#endif
	}

	return 0;
}

