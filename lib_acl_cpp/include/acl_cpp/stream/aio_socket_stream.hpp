#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#ifdef WIN32
# include <WinSock2.h>
#endif
#include "acl_cpp/stream/aio_istream.hpp"
#include "acl_cpp/stream/aio_ostream.hpp"

namespace acl
{

/**
 * ���첽�ͻ������첽����Զ�̷�����ʱ�Ļص������࣬����Ϊ�����࣬
 * Ҫ���������ʵ�� open_callback �ص�����
 */
class ACL_CPP_API aio_open_callback : public aio_callback
{
public:
	aio_open_callback() {}
	virtual ~aio_open_callback() {}

	virtual bool open_callback() = 0;
protected:
private:
};

struct AIO_OPEN_CALLBACK 
{
	aio_open_callback* callback;
	bool enable;
};

class aio_handle;

/**
 * �����첽���࣬����̳����첽��д����ͬʱ����ֻ���ڶ��Ϸ��䣬
 * ������ջ�Ϸ��䣬���Ҹ������ʱӦ�ò����ͷŸ��������Ϊ�첽��
 * ����ڲ����Զ��ͷŸ������Ӧ�ÿ��Ե��� close �����ر���
 */
class ACL_CPP_API aio_socket_stream
	: public aio_istream
	, public aio_ostream
{
public:
	/**
	 * ���캯�������������첽�ͻ�����
	 * @param handle {aio_handle*} �첽������
	 * @param stream {ACL_ASTREAM*} ��������
	 * @param opened {bool} �����Ƿ��Ѿ��������������������ӣ���������Զ�
	 *  hook ��д���̼��ر�/��ʱ���̣������ hook �ر�/��ʱ����
	 */
	aio_socket_stream(aio_handle* handle, ACL_ASTREAM* stream, bool opened = false);

	/**
	 * ���캯�������������첽�ͻ��������� hook ��д���̼��ر�/��ʱ����
	 * @param handle {aio_handle*} �첽������
	 * @param fd {ACL_SOCKET} �����׽ӿھ��
	 */
#ifdef	WIN32
	aio_socket_stream(aio_handle* handle, SOCKET fd);
#else
	aio_socket_stream(aio_handle* handle, int fd);
#endif

	/**
	 * ����Զ�̷����������ӣ����Զ� hook ���Ĺرա���ʱ�Լ����ӳɹ�
	 * ʱ�Ļص��������
	 * @param handle {aio_handle*} �첽������
	 * @param addr {const char*} Զ�̷������ĵ�ַ����ַ��ʽΪ��
	 *  ���TCP��IP:Port �� ������׽ӿڣ�{filePath}
	 * @param timeout {int} ���ӳ�ʱʱ��(��)
	 * @return {bool} ���������������ʧ����ú������� false���������
	 *  true ֻ�Ǳ�ʾ���������ӹ����У����������Ƿ�ʱ�������Ƿ�ʧ��
	 *  Ӧͨ���ص��������ж�
	 */
	static aio_socket_stream* open(aio_handle* handle,
		const char* addr, int timeout);

	/**
	 * ������ open �����Ļص�����
	 * @param callback {aio_open_callback*} �ص�����
	 */
	void add_open_callback(aio_open_callback* callback);

	/**
	 * �� open �ص����󼯺���ɾ��
	 * @param callback {aio_open_callback*} ��ɾ���Ļص���������
	 *  ֵΪ�գ���ɾ�����еĻص�����
	 * @return {int} ���ر��ӻص����󼯺���ɾ���Ļص�����ĸ���
	 */
	int del_open_callback(aio_open_callback* callback = NULL);

	/**
	 * ��ֹ�ص������༯���е�ĳ���ص�����󣬵������ӻص������
	 * ������ɾ����ֻ�ǲ������ö���
	 * @param callback {aio_open_callback*} ����ֹ�Ļص���������
	 *  ֵΪ�գ����ֹ���еĻص�����
	 * @return {int} ���ر��ӻص����󼯺��н��õĻص�����ĸ���
	 */
	int disable_open_callback(aio_open_callback* callback = NULL);

	/**
	 * �������еĻص����󱻵���
	 * @param callback {aio_open_callback*} ����ָ���Ļص�����
	 * �����ֵΪ�գ����������еĻص�����
	 * @return {int} ���ر����õĻص�����ĸ���
	 */
	int enable_open_callback(aio_open_callback* callback = NULL);

	/**
	 * ��� open ���̣��ж��Ƿ��Ѿ����ӳɹ�
	 * @return {bool} ���� true ��ʾ���ӳɹ��������ʾ�����ӳɹ�
	 */
	bool is_opened() const;

protected:
	virtual ~aio_socket_stream();

	/**
	 * ͨ���˺�������̬�ͷ�ֻ���ڶ��Ϸ�����첽�������
	 */
	virtual void destroy();

	/**
	 * ע�������ӳɹ��Ļص�����
	 */
	void hook_open();

private:
	bool opened_;  // ��� open �������Ա�ʾ�����Ƿ�ɹ�
	bool open_hooked_;
	std::list<AIO_OPEN_CALLBACK*> open_callbacks_;

	static int open_callback(ACL_ASTREAM*, void*);
};

}  // namespace acl
