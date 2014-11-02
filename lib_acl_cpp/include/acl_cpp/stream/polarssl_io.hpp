#pragma once

#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stream/stream.hpp"

namespace acl {

class stream;
class polarssl_conf;

/**
 * stream ������ײ� IO ������̵Ĵ����࣬��������еĶ�д�Ĺ��̽������
 * stream �������� Ĭ�ϵĵײ� IO ���̣������������Ƕ�̬������(��Ϊ�Ѷ���)��
 * stream ������ͨ�����ñ������� destroy()�������ͷű������
 */
class ACL_CPP_API polarssl_io : public stream_hook
{
public:
	/**
	 * ���캯��
	 * @param conf {polarssl_conf&} ��ÿһ�� SSL ���ӽ������õ������
	 * @param server_side {bool} �Ƿ�Ϊ�����ģʽ����Ϊ�ͻ���ģʽ������
	 *  ģʽ�����ַ�����ͬ������ͨ���˲�������������
	 */
	polarssl_io(polarssl_conf& conf, bool server_side);

	virtual void destroy();

private:
	~polarssl_io();

	virtual bool open(stream* s);
	virtual bool on_close(bool alive);
	virtual int read(void* buf, size_t len);
	virtual int send(const void* buf, size_t len);

private:
	polarssl_conf& conf_;
	bool server_side_;
	void* ssl_;
	void* ssn_;
	void* rnd_;
	const stream* stream_;

	static int sock_read(void *ctx, unsigned char *buf, size_t len);
	static int sock_send(void *ctx, const unsigned char *buf, size_t len);
};

} // namespace acl
