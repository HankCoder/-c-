#include "stdafx.h"
#include "rpc_stats.h"
#include "http_rpc.h"

http_rpc::http_rpc(acl::aio_socket_stream* client, unsigned buf_size)
: proc_quit_(false)
, handle_(client->get_handle())
, client_(client)
, buf_size_(buf_size)
{
	res_buf_ = (char*) acl_mymalloc(buf_size + 1);
	unsigned i;
	for (i = 0; i < buf_size; i++)
		res_buf_[i] = 'x';
	res_buf_[i] = 0;
}

http_rpc::~http_rpc()
{
	acl_myfree(res_buf_);
}

void http_rpc::rpc_onover()
{
	// ���� rpc ����
	rpc_del();

	if (!proc_quit_ && keep_alive_)
	{
		rpc_read_wait_add();

		// ����첽���Ƿ�ɶ�
		client_->read_wait(10);
	}
	else
		// �ر��첽������
		client_->close();

	// �ͻ��˷�����Ҫ������������˳�������
	if (proc_quit_)
		handle_.stop();
}

// ���� service_.rpc_fork ���� RPC ��������߳��е��ñ�����
// ������������ģ�鷢����������Ϣ
void http_rpc::rpc_run()
{
	// ������������
	socket_stream stream;

	// ������ get_vstream() ��õ� ACL_VSTREAM ������������
	// ���� stream ������Ϊ�� acl_cpp �� acl �е�������
	// �ͷ����������ն��ǻ��� ACL_VSTREAM���� ACL_VSTREAM ��
	// �ڲ�ά������һ����/д�������������ڳ����ӵ����ݴ����У�
	// ����ÿ�ν� ACL_VSTREAM ��Ϊ�ڲ����Ļ��������Դ�
	ACL_VSTREAM* vstream = client_->get_vstream();
	ACL_VSTREAM_SET_RWTIMO(vstream, 10);
	(void) stream.open(vstream);

	rpc_req_add();

	// ��ʼ����� HTTP ����
	handle_conn(&stream);

	rpc_req_del();

	// �� ACL_VSTREAM �������������󶨣��������ܱ�֤���ͷ�����������ʱ
	// ����ر��������ߵ����ӣ���Ϊ�����ӱ��������ڷ�����������ģ���Ҫ��
	// ���첽���رշ�ʽ���йر�
	stream.unbind();
}

void http_rpc::handle_conn(socket_stream* stream)
{
	// HTTP ��Ӧ������
	http_response res(stream);
	// ��Ӧ������Ϊ xml ��ʽ
	res.response_header().set_content_type("text/html");

	// �� HTTP ����ͷ
	if (res.read_header() == false)
	{
		keep_alive_ = false;
		return;
	}

	string buf;
	// �� HTTP ����������
	if (res.get_body(buf) == false)
	{
		keep_alive_ = false;
		return;
	}

	http_client* client = res.get_client();

	// �жϿͻ����Ƿ�ϣ�����ֳ�����
	keep_alive_ = client->keep_alive();

	// �������ݸ��ͻ���

	res.response_header()
		.set_status(200)
		.set_keep_alive(keep_alive_)
		.set_content_length(buf_size_);

	res.response(res_buf_, buf_size_);

	// ȡ�ÿͻ��˵�����
	const char* action = client->request_param("action");
	if (action && (strcasecmp(action, "stop") == 0
		|| strcasecmp(action, "quit") == 0))
	{
		proc_quit_ = true;
	}
}
