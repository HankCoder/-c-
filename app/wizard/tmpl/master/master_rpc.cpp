#include "stdafx.h"
#include "rpc_manager.h"
#include "master_service.h"

////////////////////////////////////////////////////////////////////////////////
// ����������

char *var_cfg_str;
acl::master_str_tbl var_conf_str_tab[] = {
	{ "str", "test_msg", &var_cfg_str },

	{ 0, 0, 0 }
};

int   var_cfg_bool;
acl::master_bool_tbl var_conf_bool_tab[] = {
	{ "bool", 1, &var_cfg_bool },

	{ 0, 0, 0 }
};

int   var_cfg_thread_pool_limit;
int   var_cfg_read_timeout;
acl::master_int_tbl var_conf_int_tab[] = {
	{ "thread_pool_limit", 100, &var_cfg_thread_pool_limit, 0, 0 },
	{ "read_timeout", 120, &var_cfg_read_timeout, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

long long int  var_cfg_int64;
acl::master_int64_tbl var_conf_int64_tab[] = {
	{ "int64", 120, &var_cfg_int64, 0, 0 },

	{ 0, 0 , 0 , 0, 0 }
};

////////////////////////////////////////////////////////////////////////////////

class request_rpc : public acl::rpc_request
{
public:
	request_rpc(acl::aio_socket_stream* client)
	: io_error_(false)
	, client_(client)
	{
	}

	~request_rpc()
	{
	}

protected:
	// �����麯���������̴߳���
	virtual void rpc_run()
	{
		// ����������תΪ������

		// ������������
		acl::socket_stream stream;

		// ������ get_vstream() ��õ� ACL_VSTREAM ������������
		// ���� stream ������Ϊ�� acl_cpp �� acl �е�������
		// �ͷ����������ն��ǻ��� ACL_VSTREAM���� ACL_VSTREAM ��
		// �ڲ�ά������һ����/д�������������ڳ����ӵ����ݴ����У�
		// ����ÿ�ν� ACL_VSTREAM ��Ϊ�ڲ����Ļ��������Դ�
		ACL_VSTREAM* vstream = client_->get_vstream();
		ACL_VSTREAM_SET_RWTIMO(vstream, 10);

		// ����Ϊ����ģʽ�����ͬ��д��Ч��
		acl_non_blocking(ACL_VSTREAM_SOCK(vstream), ACL_BLOCKING);

		// ��ͬ������д����
		(void) stream.open(vstream);

		// ��ʼ���������
		handle_conn(stream);

		// �� ACL_VSTREAM �������������󶨣��������ܱ�֤���ͷ�����������ʱ
		// ����ر��������ߵ����ӣ���Ϊ�����ӱ��������ڷ�����������ģ���Ҫ��
		// ���첽���رշ�ʽ���йر�
		stream.unbind();
	}

	// �����麯���������̴߳����յ����߳�������ɵ���Ϣ
	virtual void rpc_onover()
	{
		// �ر��첽��
		client_->close();

		// ������
		delete this;
	}

	/**
	 * ������ӿڣ������̵߳��ñ������ rpc_signal ʱ�������߳��л�
	 * ���ñ��ӿڣ�֪ͨ������δ���ǰ(������ rpc_onover ǰ)�յ�
	 * ���߳����е��м�״̬��Ϣ���ڲ��Զ�֧���׽ӿڻ� WIN32 ����
	 * ��Ϣ��Ӧ�ó��������磬���� HTTP ����Ӧ�ã������߳��п���
	 * һ�����أ�һ�������̷߳���(���� rpc_signal ����)���ؽ��̣�
	 * �����̻߳���ñ���ʵ���Ĵ˷������������Ϣ
	 */
	virtual void rpc_wakeup(void*)
	{
	}

private:
	bool io_error_;
	acl::aio_socket_stream* client_;

	void handle_conn(acl::socket_stream& stream)
	{
		acl::string buf;

		// ���첽��תΪͬ��������������ʽ�Ը����ӳؽ��ж�д

		if (stream.gets(buf, false) == false)
		{
			logger_warn("gets error!");
			io_error_ = true;
		}
		else if (stream.write(buf) == -1)
		{
			logger_warn("write error!");
			io_error_ = true;
		}
	}
};

////////////////////////////////////////////////////////////////////////////////

// acl::aio_callback ��������ඨ��
class io_callback : public acl::aio_callback
{
public:
	io_callback(acl::aio_socket_stream* client)
		: client_(client)
	{
	}

	~io_callback()
	{
	}

protected:
	/** 
	 * ʵ�ָ����е��麯�����ͻ������Ķ��ɹ��ص����� 
	 * @param data {char*} ���������ݵ�ַ 
	 * @param len {int} ���������ݳ��� 
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽�� 
	 */  
	virtual bool read_callback(char* data, int len)  
	{
		if (strncmp(data, "quit", len) == 0)
		{
			// ������ʽ�ص����첽���Ĺرչ��̣�Ҳ����ֱ�ӷ��� false��
			// ֪ͨ�첽����Զ��رո��첽��
			// client_->close();
			return false;
		}
		return true;
	}

	virtual bool read_wakeup()
	{
		// Ϊ��ֹ�����߳�������ͻ������Ӷ�����������ӱ��رգ�
		// ���Ա�����첽����������ȥ���Ը��첽���ļ��
		// �����ӻ��� request_rpc::rpc_onover �б��ر�
		client_->disable_read();

		// ����ͬ�����������
		request_rpc* req = new request_rpc(client_);

		// �����������ݴ������ͨ�� rpc ͨ���������̴߳���
		rpc_manager::get_instance().fork(req);

		return true;
	}

	/** 
	 * ʵ�ָ����е��麯�����ͻ�������д�ɹ��ص����� 
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽�� 
	 */  
	virtual bool write_callback()  
	{
		return true;  
	}

	/** 
	 * ʵ�ָ����е��麯�����ͻ������Ĺرջص����� 
	 */  
	virtual void close_callback()  
	{
		// �����ڴ˴�ɾ���ö�̬����Ļص�������Է�ֹ�ڴ�й¶  
		delete this;  
	}

	/** 
	 * ʵ�ָ����е��麯�����ͻ������ĳ�ʱ�ص����� 
	 * @return {bool} ���� true ��ʾ����������ϣ���رո��첽�� 
	 */  
	virtual bool timeout_callback()  
	{
		// ���� false ֪ͨ�첽��ܹرո��첽��
		return false;
	}

private:
	acl::aio_socket_stream* client_;
};

////////////////////////////////////////////////////////////////////////////////

master_service::master_service()
{
}

master_service::~master_service()
{
}

bool master_service::on_accept(acl::aio_socket_stream* client)
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
	// client->format("hello, you're welcome\r\n");

	// ����첽���Ķ�״̬���������ݿɶ�ʱ���ص� acl::aio_callback
	// �е� read_wakeup �麯��
	client->read_wait(var_cfg_read_timeout);
	return true;
}

void master_service::proc_on_init()
{
	// ����첽��ܵ��¼�������
	acl::aio_handle* handle = get_handle();

	// ��ʼ�� rpc �������
	rpc_manager::get_instance().init(handle, var_cfg_thread_pool_limit);
}

void master_service::proc_on_exit()
{
}
