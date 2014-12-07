#pragma once

#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stream/stream_hook.hpp"

struct ACL_VSTREAM;

namespace acl {

class polarssl_conf;

/**
 * stream/aio_stream ������ײ� IO ������̵Ĵ����࣬��������еĶ�д�Ĺ��̽������
 * stream/aio_stream �������� Ĭ�ϵĵײ� IO ���̣������������Ƕ�̬������(��Ϊ�Ѷ���)��
 * stream/aio_stream ������ͨ�����ñ������� destroy()�������ͷű������
 */
class ACL_CPP_API polarssl_io : public stream_hook
{
public:
	/**
	 * ���캯��
	 * @param conf {polarssl_conf&} ��ÿһ�� SSL ���ӽ������õ������
	 * @param server_side {bool} �Ƿ�Ϊ�����ģʽ����Ϊ�ͻ���ģʽ������
	 *  ģʽ�����ַ�����ͬ������ͨ���˲�������������
	 * @param aio_mode {bool} �Ƿ�Ϊ������ģʽ
	 */
	polarssl_io(polarssl_conf& conf, bool server_side,
		bool non_block = false);

	virtual void destroy();

	/**
	 * ���ô˷������� SSL ���֣��ڷ����� IO ģʽ�¸ú�����Ҫ�� handshake_ok()
	 * �������ʹ�����ж� SSL �����Ƿ�ɹ�
	 * @return {bool}
	 *  1������ false ��ʾ����ʧ�ܣ���Ҫ�ر����ӣ�
	 *  2�������� true ʱ��
	 *  2.1�����Ϊ���� IO ģʽ���ʾ SSL ���ֳɹ�
	 *  2.2���ڷ����� IO ģʽ�½����������ֹ����� IO �ǳɹ��ģ�����Ҫ����
	 *       handshake_ok() �����ж� SSL �����Ƿ�ɹ�
	 */
	bool handshake(void);

	/**
	 * �ж� SSL �����Ƿ�ɹ�
	 * @return {bool}
	 */
	bool handshake_ok(void)
	{
		return handshake_ok_;
	}

	/**
	 * �����׽���Ϊ����ģʽ/������ģʽ
	 * @param yes {bool} ��Ϊ false ʱ����Ϊ����ģʽ��������Ϊ������ģʽ
	 * @return {polarssl_io&}
	 */
	polarssl_io& set_non_blocking(bool yes);

	/**
	 * �жϵ�ǰ���õ� SSL IO �Ƿ�����ģʽ���Ƿ�����ģʽ
	 * @return {bool} ���� true ���ʾΪ������ģʽ������Ϊ����ģʽ
	 */
	bool is_non_blocking(void) const
	{
		return non_block_;
	}


	/**
	 * ���Է�֤���Ƿ���Ч��һ�㲻�ص��ô˺�����
	 * @return {bool}
	 */
	bool check_peer(void);

private:
	~polarssl_io();

	virtual bool open(ACL_VSTREAM* s);
	virtual bool on_close(bool alive);
	virtual int read(void* buf, size_t len);
	virtual int send(const void* buf, size_t len);

private:
	polarssl_conf& conf_;
	bool  server_side_;
	bool  non_block_;
	bool  handshake_ok_;
	void* ssl_;
	void* ssn_;
	void* rnd_;
	ACL_VSTREAM* stream_;

	static int sock_read(void *ctx, unsigned char *buf, size_t len);
	static int sock_send(void *ctx, const unsigned char *buf, size_t len);
};

} // namespace acl
