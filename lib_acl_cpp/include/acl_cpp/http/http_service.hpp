#pragma once
#include "acl_cpp/stream/aio_handle.hpp"
#include "acl_cpp/ipc/ipc_service.hpp"
#include "acl_cpp/http/http_header.hpp"

namespace acl {

class string;

/**
 * HTTP ���������࣬�������̳и���
 */
class ACL_CPP_API http_service_request : public http_header
{
public:
	/**
	 * ���캯��
	 * @param domain {const char*} HTTP ������������(Ҳ������IP)���ǿ�
	 *  ��������˿�ֵ����� fatal
	 * @param port {unsigned short} HTTP ����˿�
	 */
	http_service_request(const char* domain, unsigned short port);

	/**
	 * ����ɹ��캯������� domain
	 * @return {const char*} ����Ϊ��
	 */
	const char* get_domain(void) const;

	/**
	 * ����ɹ��캯������� port
	 * @return {unsigned short}
	 */
	unsigned short get_port(void) const;

	/**
	 * ����������ϻ����ʱ���ڲ�������̻��Զ����� destroy �ӿڣ�
	 * ��������ڸýӿ��ڽ���һЩ�ͷŹ��̣����䵱�ö����Ƕ�̬����ʱ��
	 * ����Ӧ���ڸú����� delete this ��ɾ���Լ�����Ϊ�ú������տ϶�
	 * �ᱻ���ã��������಻Ӧ�������ط�������������
	 */
	virtual void destroy(void) {}

	//////////////////////////////////////////////////////////////////////////
	// �������ʵ�������ӿ�

	/**
	 * ��� HTTP ���������ݣ��ú���������������б�ѭ�����ã�ֱ�����ص����ݶ���
	 * �е�����Ϊ��
	 * @return {const string*} �����������ݣ�������ؿ�ָ��򷵻صĻ���������
	 *  ������Ϊ��(�� string->empty()) ���ʾ HTTP ���������ݽ���
	 * ע�⣺������������ͬ������ӿ�����������߳��б����õģ������������ʵ
	 *     ���˸ýӿڣ������Ҫ������ԭ���߳̾߱���������ԴʱӦ��ע���������
	 */
	virtual const string* get_body(void);

	/**
	 * ����� HTTP �������� HTTP ��Ӧͷʱ�Ļص��ӿ�
	 * @param addr {const char*} �������֮������ӵ�ַ����ʽ��IP:PORT
	 * @param hdr {const HTTP_HDR_RES*} HTTP ��Ӧͷ���ýṹ����μ���
	 *  acl_project/lib_protocol/include/http/lib_http_struct.h
	 */
	virtual void on_hdr(const char* addr, const HTTP_HDR_RES* hdr) = 0;

	/**
	 * ����� HTTP �������� HTTP ��Ӧ��ʱ�Ļص��ӿڣ��� HTTP ��Ӧ������
	 * �Ƚϴ�ʱ���ûص��ᱻ��ε��ã�ֱ������(����� on_error)�����ݶ���
	 * ʱ���ûص������������� 0���� data �� dlen ��Ϊ 0 ʱ�������� HTTP
	 * ��Ӧ�����
	 * @param data {const char*} ĳ�ζ�����ʱ HTTP ��Ӧ������
	 * @param dlen {size_t} ĳ�ζ�����ʱ HTTP ��Ӧ�����ݳ���
	 * ע����� HTTP ��Ӧֻ��ͷ���ݶ�û�������壬��Ҳ����øú���֪ͨ�û�
	 *     HTTP �Ự����
	 */
	virtual void on_body(const char* data, size_t dlen) = 0;

	/**
	 * �� HTTP �������Ӧ������������������ô˽ӿڣ�֪ͨ�������
	 * �ڵ��ô˽ӿں�
	 * @param errnum {http_status_t} ������
	 */
	virtual void on_error(http_status_t errnum) = 0;
protected:
	virtual ~http_service_request(void);
private:
	char* domain_;
	unsigned short port_;
};

class aio_socket_stream;

class ACL_CPP_API http_service : public ipc_service
{
public:
	/**
	 * ���캯��
	 * @param nthread {int} �����ֵ > 1 ���ڲ��Զ������̳߳أ�����
	 *  ����һ������һ���߳�
	 * @param nwait {int} ���첽������� ENGINE_WINMSG ʱ��Ϊ�˱���
	 *  �������̷߳��͵�������Ϣ��������������̵߳� WIN32 ��Ϣѭ����
	 *  �������̷߳���������Ϣʱ�Զ����ߵĺ����������������첽���棬
	 *  ��ֵҲ�����������ٹ���
	 * @param win32_gui {bool} �Ƿ��Ǵ��������Ϣ������ǣ����ڲ���
	 *  ͨѶģʽ�Զ�����Ϊ���� WIN32 ����Ϣ��������Ȼ����ͨ�õ��׽�
	 *  ��ͨѶ��ʽ
	 */
	http_service(int nthread = 1, int nwait = 1, bool win32_gui = false);
	~http_service(void);

	/**
	 * Ӧ�õ��ô˺�����ʼ HTTP �Ự���̣��� http_service �������
	 * ��������첽���� HTTP ����ͬʱ�첽��ȡ������ HTTP ����������Ӧ
	 * @param req {http_service_request*} HTTP ���������
	 */
	void do_request(http_service_request* req);
protected:
#ifdef WIN32
	/**
	 * �����麯�������յ����������̵߳� win32 ��Ϣʱ�Ļص�����
	 * @param hWnd {HWND} ���ھ��
	 * @param msg {UINT} �û��Զ�����Ϣ��
	 * @param wParam {WPARAM} ����
	 * @param lParam {LPARAM} ����
	 */
	virtual void win32_proc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
	/**
	 * �����麯�������������ӵ���ʱ����ص��˺���
	 * @param client {aio_socket_stream*} ���յ����µĿͻ�������
	 */
	virtual void on_accept(aio_socket_stream* client);

	/**
	 * �����麯�������������ɹ��򿪺�Ļص�����
	 * @param addr {const char*} ʵ�ʵļ�����ַ����ʽ��IP:PORT
	 */
	virtual void on_open(const char*addr);

	/**
	 * �����麯�������������ر�ʱ�Ļص�����
	 */
	virtual void on_close(void);
private:
	char* addr_;
	int   nwait_;
	aio_handle_type handle_type_;
};

}  // namespace acl
