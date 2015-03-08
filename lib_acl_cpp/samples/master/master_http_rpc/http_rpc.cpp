#include "stdafx.h"
#include "rpc_stats.h"
#include "http_rpc.h"

http_rpc::http_rpc(acl::aio_socket_stream* client, unsigned buf_size)
: client_(client)
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
	//logger("rpc_request destroyed!");
	acl_myfree(res_buf_);
}

// ���� service_.rpc_fork ���� RPC ��������߳��е��ñ�����
// ������������ģ�鷢����������Ϣ
void http_rpc::rpc_run()
{
	// ������������
	acl::socket_stream stream;

	// ������ get_vstream() ��õ� ACL_VSTREAM ������������
	// ���� stream ������Ϊ�� acl_cpp �� acl �е�������
	// �ͷ����������ն��ǻ��� ACL_VSTREAM���� ACL_VSTREAM ��
	// �ڲ�ά������һ����/д�������������ڳ����ӵ����ݴ����У�
	// ����ÿ�ν� ACL_VSTREAM ��Ϊ�ڲ����Ļ��������Դ�
	ACL_VSTREAM* vstream = client_->get_vstream();
	ACL_VSTREAM_SET_RWTIMO(vstream, 10);
	(void) stream.open(vstream);
	// ����Ϊ����ģʽ
	stream.set_tcp_non_blocking(false);

	rpc_req_add();

	// ��ʼ����� HTTP ����
	handle_conn(&stream);

	rpc_req_del();

	// ����Ϊ������ģʽ
	stream.set_tcp_non_blocking(true);

	// �� ACL_VSTREAM �������������󶨣��������ܱ�֤���ͷ�����������ʱ
	// ����ر��������ߵ����ӣ���Ϊ�����ӱ��������ڷ�����������ģ���Ҫ��
	// ���첽���رշ�ʽ���йر�
	stream.unbind();
}

void http_rpc::handle_conn(acl::socket_stream* stream)
{
	// HTTP ��Ӧ������
	acl::http_response res(stream);
	// ��Ӧ������Ϊ xml ��ʽ
	res.response_header().set_content_type("text/html");

	// �� HTTP ����ͷ
	if (res.read_header() == false)
	{
		keep_alive_ = false;
		return;
	}

	acl::string buf;
	// �� HTTP ����������
	if (res.get_body(buf) == false)
	{
		keep_alive_ = false;
		return;
	}

	acl::http_client* client = res.get_client();

	// �жϿͻ����Ƿ�ϣ�����ֳ�����
	keep_alive_ = client->keep_alive();

	// �������ݸ��ͻ���

	res.response_header().set_status(200).set_keep_alive(keep_alive_);
	res.response(res_buf_, buf_size_);
}

void http_rpc::rpc_onover()
{
	// ���� rpc ����
	rpc_del();

	if (keep_alive_)
	{
		rpc_read_wait_add();

		// ����첽���Ƿ�ɶ�
		client_->read_wait(10);
	}
	else
		// �ر��첽������
		client_->close();
}
