#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"

struct ACL_VSTREAM;

namespace acl
{

class ACL_CPP_API ssl_aio_stream
	: public aio_socket_stream
	, public aio_open_callback
{
public:
	/**
	 * ���캯�������������첽�ͻ�����
	 * @param handle {aio_handle*} �첽������
	 * @param stream {ACL_ASTREAM*} ��������
	 * @param opened {bool} �����Ƿ��Ѿ��������������������ӣ���������Զ�
	 *  hook ��д���̼��ر�/��ʱ���̣������ hook �ر�/��ʱ����
	 * @param use_ssl {bool} �Ƿ�ʹ�� SSL �׽ӿ�
	 */
	ssl_aio_stream(aio_handle* handle, ACL_ASTREAM* stream,
		bool opened = false, bool use_ssl = true);

	/**
	 * ���캯�������������첽�ͻ��������� hook ��д���̼��ر�/��ʱ����
	 * @param handle {aio_handle*} �첽������
	 * @param fd {ACL_SOCKET} �����׽ӿھ��
	 * @param use_ssl {bool} �Ƿ�ʹ�� SSL �׽ӿ�
	 */
#ifdef	WIN32
	ssl_aio_stream(aio_handle* handle, SOCKET fd, bool use_ssl = true);
#else
	ssl_aio_stream(aio_handle* handle, int fd, bool use_ssl = true);
#endif

	/**
	 * ����Զ�̷����������ӣ����Զ� hook ���Ĺرա���ʱ�Լ����ӳɹ�
	 * ʱ�Ļص��������
	 * @param handle {aio_handle*} �첽������
	 * @param addr {const char*} Զ�̷������ĵ�ַ����ַ��ʽΪ��
	 *  ���TCP��IP:Port �� ������׽ӿڣ�{filePath}
	 * @param timeout {int} ���ӳ�ʱʱ��(��)
	 * @param use_ssl {bool} �Ƿ�ʹ�� SSL �׽ӿ�
	 * @return {bool} ���������������ʧ����ú������� false���������
	 *  true ֻ�Ǳ�ʾ���������ӹ����У����������Ƿ�ʱ�������Ƿ�ʧ��
	 *  Ӧͨ���ص��������ж�
	 */
	static ssl_aio_stream* open(aio_handle* handle,
		const char* addr, int timeout, bool use_ssl = true);

	/**
	 * �ú����������Ѿ��򿪵������в�������������ڽ�����Ϊ SSL ģʽ
	 * ��� SSL ģʽ
	 * @param on {bool} �Ƿ����� SSL ģʽ�����ò���Ϊ false ʱ�����
	 *  ��ǰ���Ѿ��� SSL ģʽ����ر� SSL ģʽ�������ǰ��Ϊ�� SSL
	 *  ģʽ����ֱ�ӷ��أ����ò���Ϊ true ʱ�������ǰ���Ѿ��� SSL
	 *  ģʽ����ֱ�ӷ��أ������ǰ��Ϊ�� SSL ģʽ����� SSL ģʽ
	 * @return {bool}
	 */
	bool open_ssl(bool on);

protected:
	virtual ~ssl_aio_stream();

	/**
	* ���� aio_open_callback ����ӿ�
	*/
	virtual bool open_callback();
private:
	void* ssl_;
	void* ssn_;
	void* hs_;

	bool ssl_client_init();

	static int __sock_read(void *ctx, unsigned char *buf, size_t len);
	static int __sock_send(void *ctx, const unsigned char *buf, size_t len);

#ifdef WIN32
	static int __ssl_read(SOCKET fd, void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
	static int __ssl_send(SOCKET fd, const void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
#else
	static int __ssl_read(int fd, void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
	static int __ssl_send(int fd, const void *buf, size_t len,
		int timeout, ACL_VSTREAM* stream, void *ctx);
#endif

	void clear(void);
};

} // namespace acl
