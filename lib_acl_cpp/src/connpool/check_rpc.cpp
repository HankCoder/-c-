#include "acl_stdafx.hpp"
#include "acl_cpp/stream/socket_stream.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"
#include "acl_cpp/connpool/connect_monitor.hpp"
#include "acl_cpp/connpool/check_client.hpp"
#include "check_rpc.hpp"

namespace acl
{

check_rpc::check_rpc(connect_monitor& monitor, check_client& checker)
: monitor_(monitor)
, checker_(checker)
{
}

check_rpc::~check_rpc()
{
}

void check_rpc::rpc_run()
{
	// ������������
	socket_stream stream;

	// ������ get_vstream() ��õ� ACL_VSTREAM ������������
	// ���� stream ������Ϊ�� acl_cpp �� acl �е�������
	// �ͷ����������ն��ǻ��� ACL_VSTREAM���� ACL_VSTREAM ��
	// �ڲ�ά������һ����/д�������������ڳ����ӵ����ݴ����У�
	// ����ÿ�ν� ACL_VSTREAM ��Ϊ�ڲ����Ļ��������Դ�
	ACL_VSTREAM* vstream = checker_.get_conn().get_vstream();
	ACL_VSTREAM_SET_RWTIMO(vstream, 10);
	(void) stream.open(vstream);
	// ����Ϊ����ģʽ
	stream.set_tcp_non_blocking(false);

	// ����ͬ��������
	monitor_.sio_check(checker_, stream);

	// ����Ϊ������ģʽ
	stream.set_tcp_non_blocking(true);

	// �� ACL_VSTREAM �������������󶨣��������ܱ�֤���ͷ�����������ʱ
	// ����ر��������ߵ����ӣ���Ϊ�����ӱ��������ڷ�����������ģ���Ҫ��
	// ���첽���رշ�ʽ���йر�
	stream.unbind();
}

void check_rpc::rpc_onover()
{
	// ȡ���ü����������״̬
	checker_.set_blocked(false);

	// �첽�رռ�����
	checker_.close();

	// ɾ����̬ rpc �������
	delete this;
}

} // namespace acl
