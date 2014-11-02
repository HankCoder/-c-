#pragma once
#include "acl_cpp/stream/aio_listen_stream.hpp"
#include "acl_cpp/stream/aio_socket_stream.hpp"

/**
* �첽�ͻ������Ļص��������
*/
class CAcceptedClientCallback : public acl::aio_callback
{
public:
	CAcceptedClientCallback(acl::aio_socket_stream* client);

	~CAcceptedClientCallback();

	/**
	* ʵ�ָ����е��麯�����ͻ������Ķ��ɹ��ص�����
	* @param data {char*} ���������ݵ�ַ
	* @param len {int} ���������ݳ���
	* @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	*/
	bool read_callback(char* data, int len);

	/**
	* ʵ�ָ����е��麯�����ͻ�������д�ɹ��ص�����
	* @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	*/
	bool write_callback();

	/**
	* ʵ�ָ����е��麯�����ͻ������ĳ�ʱ�ص�����
	*/
	void close_callback();

	/**
	* ʵ�ָ����е��麯�����ͻ������ĳ�ʱ�ص�����
	* @return {bool} ���� true ��ʾ����������ϣ���رո��첽��
	*/
	bool timeout_callback();

private:
	acl::aio_socket_stream* client_;
	int   i_;
};

/**
* �첽�������Ļص��������
*/
class CServerCallback : public acl::aio_accept_callback
{
public:
	CServerCallback();
	~CServerCallback();

	/**
	* �����麯�������������ӵ������ô˻ص�����
	* @param client {aio_socket_stream*} �첽�ͻ�����
	* @return {bool} ���� true ��֪ͨ��������������
	*/
	bool accept_callback(acl::aio_socket_stream* client);
};
