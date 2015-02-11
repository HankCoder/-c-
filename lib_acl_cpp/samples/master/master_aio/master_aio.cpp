// master_aio.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "acl_cpp/lib_acl.hpp"

static char *var_cfg_debug_msg;

static acl::master_str_tbl var_conf_str_tab[] = {
	{ "debug_msg", "test_msg", &var_cfg_debug_msg },

	{ 0, 0, 0 }
};

static int  var_cfg_debug_enable;
static int  var_cfg_keep_alive;
static int  var_cfg_send_banner;

static acl::master_bool_tbl var_conf_bool_tab[] = {
	{ "debug_enable", 1, &var_cfg_debug_enable },
	{ "keep_alive", 1, &var_cfg_keep_alive },
	{ "send_banner", 1, &var_cfg_send_banner },

	{ 0, 0, 0 }
};

static int  var_cfg_io_timeout;

static acl::master_int_tbl var_conf_int_tab[] = {
	{ "io_timeout", 120, &var_cfg_io_timeout, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

static void (*format)(const char*, ...) = acl::log::msg1;

using namespace acl;

//////////////////////////////////////////////////////////////////////////
/**
 * �ӳٶ��ص�������
 */
class timer_reader: public aio_timer_reader
{
public:
	timer_reader(int delay)
	{
		delay_ = delay;
		format("timer_reader init, delay: %d\r\n", delay);
	}

	~timer_reader()
	{
	}

	// aio_timer_reader ������������� destroy ����
	void destroy()
	{
		format("timer_reader delete, delay: %d\r\n", delay_);
		delete this;
	}

	// ���ػ���ص�����
	virtual void timer_callback(unsigned int id)
	{
		format("timer_reader(%u): timer_callback, delay: %d\r\n", id, delay_);

		// ���û���Ĵ������
		aio_timer_reader::timer_callback(id);
	}

private:
	int   delay_;
};

/**
 * �ӳ�д�ص�������
 */
class timer_writer: public aio_timer_writer
{
public:
	timer_writer(int delay)
	{
		delay_ = delay;
		format("timer_writer init, delay: %d\r\n", delay);
	}

	~timer_writer()
	{
	}

	// aio_timer_reader ������������� destroy ����
	void destroy()
	{
		format("timer_writer delete, delay: %d\r\n", delay_);
		delete this;
	}

	// ���ػ���ص�����
	virtual void timer_callback(unsigned int id)
	{
		format("timer_writer(%u): timer_callback, delay: %u\r\n", id, delay_);

		// ���û���Ĵ������
		aio_timer_writer::timer_callback(id);
	}

private:
	int   delay_;
};

class timer_test : public aio_timer_callback
{
public:
	timer_test() : aio_timer_callback(true) {}
	~timer_test() {}
protected:
	// ���ി�麯��
	virtual void timer_callback(unsigned int id)
	{
		format("id: %u\r\n", id);
	}

	virtual void destroy(void)
	{
		delete this;
		format("timer delete now\r\n");
	}
private:
};
/**
 * �첽�ͻ������Ļص��������
 */
class io_callback : public aio_callback
{
public:
	io_callback(aio_socket_stream* client)
		: client_(client)
		, i_(0)
	{
	}

	~io_callback()
	{
		format("delete io_callback now ...\r\n");
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ������Ķ��ɹ��ص�����
	 * @param data {char*} ���������ݵ�ַ
	 * @param len {int} ���������ݳ���
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	virtual bool read_callback(char* data, int len)
	{
		if (++i_ < 10)
			format(">>gets(i: %d): %s\r\n", i_, data);

		// ���Զ�̿ͻ���ϣ���˳�����ر�֮
		if (strncmp(data, "quit", 4) == 0)
		{
			client_->format("Bye!\r\n");
			client_->close();
		}

		// ���Զ�̿ͻ���ϣ�������Ҳ�رգ�����ֹ�첽�¼�����
		else if (strncmp(data, "stop", 4) == 0)
		{
			client_->format("Stop now!\r\n");
			client_->close();  // �ر�Զ���첽��

			// ֪ͨ�첽����ر�ѭ������
			client_->get_handle().stop();
		}

		// ��Զ�̿ͻ��˻�д�յ�������

		int   delay = 0;

		if (strncmp(data, "write_delay", strlen("write_delay")) == 0)
		{
			// �ӳ�д����

			const char* ptr = data + strlen("write_delay");
			delay = atoi(ptr);
			if (delay > 0)
			{
				format(">> write delay %d second ...\r\n", delay);
				timer_writer* timer = new timer_writer(delay);
				client_->write(data, len, delay * 1000000, timer);
				client_->gets(10, false);
				return (true);
			}
		}
		else if (strncmp(data, "read_delay", strlen("read_delay")) == 0)
		{
			// �ӳٶ�����

			const char* ptr = data + strlen("read_delay");
			delay = atoi(ptr);
			if (delay > 0)
			{
				client_->write(data, len);
				format(">> read delay %d second ...\r\n", delay);
				timer_reader* timer = new timer_reader(delay);
				client_->gets(10, false, delay * 1000000, timer);
				return (true);
			}
		}

		client_->write(data, len);
		//client_->gets(10, false);
		return (true);
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ�������д�ɹ��ص�����
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	virtual bool write_callback()
	{
		return (true);
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ������ĳ�ʱ�ص�����
	 */
	virtual void close_callback()
	{
		// �����ڴ˴�ɾ���ö�̬����Ļص�������Է�ֹ�ڴ�й¶
		format("Close\r\n");
		delete this;
	}

	/**
	 * ʵ�ָ����е��麯�����ͻ������ĳ�ʱ�ص�����
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	 */
	virtual bool timeout_callback()
	{
		format("Timeout ...\r\n");
		return (true);
	}

private:
	aio_socket_stream* client_;
	int   i_;
};

//////////////////////////////////////////////////////////////////////////

class master_aio_test : public master_aio
{
public:
	master_aio_test() { timer_test_ = new timer_test(); }

	~master_aio_test() { }

protected:
	// ���ി�麯���������յ�һ���µ�����ʱ���ô˺���
	virtual bool on_accept(aio_socket_stream* client)
	{
		// �����첽�ͻ������Ļص���������첽�����а�
		io_callback* callback = new io_callback(client);

		// ע���첽���Ķ��ص�����
		client->add_read_callback(callback);

		// ע���첽����д�ص�����
		client->add_write_callback(callback);

		// ע���첽���Ĺرջص�����
		client->add_close_callback(callback);

		// ע���첽���ĳ�ʱ�ص�����
		client->add_timeout_callback(callback);

		// д��ӭ��Ϣ
		if (var_cfg_send_banner)
			client->format("hello, you're welcome\r\n");

		// ���첽����һ������
		client->gets(10, false);
		//client->read();
		return true;
	}

	// �����麯������������л��û����ǰ���ô˺���
	virtual void proc_pre_jail()
	{
		format("proc_pre_jail\r\n");
		// ֻ�е�������������ܻ���첽������
		handle_ = get_handle();
		//handle_->keep_timer(true); // ����ʱ�����ظ�����
		// ���õ�һ����ʱ����ÿ��1�봥��һ�Σ���ʱ����IDΪ0
		handle_->set_timer(timer_test_, 1000000, 0);
	}

	// �����麯������������л��û���ݺ���ô˺���
	virtual void proc_on_init()
	{
		format("proc init\r\n");
		// ���õڶ�����ʱ����ÿ��2�봥��һ�Σ���ʱ����IDΪ1
		handle_->set_timer(timer_test_, 2000000, 1);
	}

	// �����麯������������˳�ǰ���ô˺���
	virtual void proc_on_exit()
	{
		format("proc exit\r\n");
	}
private:
	timer_test* timer_test_;
	aio_handle* handle_;
};
//////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
#if 0
	int base = 8, nslice = 1024, nalloc_gc = 1000000;
	unsigned int slice_flag = ACL_SLICE_FLAG_GC2 | ACL_SLICE_FLAG_RTGC_OFF;

	acl_mem_slice_init(base, nslice, nalloc_gc, slice_flag);
#endif

	master_aio_test ma;

	// �������ò�����
	ma.set_cfg_int(var_conf_int_tab);
	ma.set_cfg_int64(NULL);
	ma.set_cfg_str(var_conf_str_tab);
	ma.set_cfg_bool(var_conf_bool_tab);

	// ��ʼ����

	if (argc >= 2 && strcmp(argv[1], "alone") == 0)
	{
		const char* addr = "127.0.0.1:8888, 127.0.0.1:8889";

		if (argc >= 3)
			addr = argv[2];
		
		const char* conf;
		if (argc >= 4)
			conf = argv[3];
		else
			conf = NULL;

		format = (void (*)(const char*, ...)) printf;
		format("listen: %s now\r\n", addr);
		ma.run_alone(addr, conf);  // �������з�ʽ
	}
	else
	{
#ifdef	WIN32
		const char* addr = "127.0.0.1:8888, 127.0.0.1:8889";
		const char* conf = "./master_aio.cf";

		format = (void (*)(const char*, ...)) printf;
		format("listen: %s now\r\n", addr);
		ma.run_alone(addr, conf);  // �������з�ʽ

#else
		ma.run_daemon(argc, argv);  // acl_master ����ģʽ����
#endif
	}
	return 0;
}
