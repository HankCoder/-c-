#pragma once

#include "acl_cpp/stream/aio_socket_stream.hpp"

typedef struct
{
	char  addr[64];
	acl::aio_handle* handle;
	int   connect_timeout;
	int   read_timeout;
	int   nopen_limit;
	int   nopen_total;
	int   nwrite_limit;
	int   nwrite_total;
	int   nread_total;
	int   id_begin;
	bool  debug;
} IO_CTX;

class CConnectClientCallback : public acl::aio_open_callback
{
public:
	CConnectClientCallback(IO_CTX* ctx, acl::aio_socket_stream* client, int id);
	~CConnectClientCallback();

	/**
	 * �����麯��, ���첽��������Ҫ�������ʱ���ô˻ص�����
	 * @param data {char*} ���������ݵ�ַ
	 * @param len {int�� ���������ݳ���
	 * @return {bool} ���ظ������� true ��ʾ�����������ʾ��Ҫ�ر��첽��
	 */
	bool read_callback(char* data, int len);

	/**
	 * �����麯��, ���첽��д�ɹ�ʱ���ô˻ص�����
	 * @return {bool} ���ظ������� true ��ʾ�����������ʾ��Ҫ�ر��첽��
	 */
	bool write_callback();

	/**
	 * �����麯��, �����첽���ر�ʱ���ô˻ص�����
	 */
	void close_callback();

	/**
	 * �����麯�������첽����ʱʱ���ô˺���
	 * @return {bool} ���ظ������� true ��ʾ�����������ʾ��Ҫ�ر��첽��
	 */
	bool timeout_callback();

	/**
	 * �����麯��, ���첽���ӳɹ�����ô˺���
	 * @return {bool} ���ظ������� true ��ʾ�����������ʾ��Ҫ�ر��첽��
	 */
	bool open_callback();

	static bool CConnectClientCallback::connect_server(IO_CTX* ctx, int id);
private:
	acl::aio_socket_stream* client_;
	IO_CTX* ctx_;
	int   nwrite_;
	int   id_;
};