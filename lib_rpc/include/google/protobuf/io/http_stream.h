#pragma once
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/io/zero_copy_stream_impl_lite.h>
#include <google/protobuf/stubs/common.h>
#include "acl_cpp/http/http_request.hpp"
#include "acl_cpp/http/http_response.hpp"

namespace google {
namespace protobuf {

class MessageLite;

namespace io {

/**
 * ʹ�� HTTP ����Э�鷢�� protobuf ��������������ˣ�ͬʱ�ȴ��������Ӧ���
 */
class LIBPROTOBUF_EXPORT http_request
{
public:
	/**
	 * ���캯����һ����ʹ������� HTTP ��������û��������ͷ�֮��������ʱ
	 * ����ʹ�ñ����캯�����Ӷ�����ͻ�����һ�������Ϸ��Ͷ������
	 * @param request {acl::http_request*} HTTP �ͻ���������󣬷ǿ�
	 */
	explicit http_request(acl::http_request* request);

	/**
	 * ���캯�����������ڲ����� HTTP �������������Զ��ͷ�֮��������ʱ
	 * ����ʹ�ñ����캯��
	 * @param addr {const char*} http ������������ַ����ʽ��ip:port
	 * @param conn_timeout {int} ���ӷ������ĳ�ʱʱ�䣨�룩
	 * @param rw_timeout {int} IO ��ʱʱ�䣨�룩
	 */
	explicit http_request(const char* addr, int conn_timeout = 60,
		int rw_timeout = 60);

	~http_request();

	/**
	 * ���;� protobuf ���л������ݰ���ͬʱ���շ�������Ӧ�����л����ݰ�
	 * @param in {const MessageLite&} �ͻ������ɵ��������ݰ�
	 * @param out {MessageLite*} �洢�������Ӧ�����ݰ�
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool rpc_request(const MessageLite& in, MessageLite* out);

	double request_spent() const
	{
		return request_spent_;
	}

	double response_spent() const
	{
		return response_spent_;
	}

	double build_spent() const
	{
		return build_spent_;
	}

	double parse_spent() const
	{
		return parse_spent_;
	}
private:
	acl::http_request* request_;
	acl::http_request* request_inner_;
	char* addr_;
	int   conn_timeout_;
	int   rw_timeout_;

	double request_spent_;
	double response_spent_;
	double build_spent_;
	double parse_spent_;
};

/**
 * �����ʹ�ñ��������տͻ�������ͬʱ��������ظ��ͻ���
 */
class LIBPROTOBUF_EXPORT http_response
{
public:
	/**
	 * ���캯��
	 * @param response {acl::http_response*} ��ͻ��������ķ�������Ӷ���
	 */
	explicit http_response(acl::http_response* response);

	~http_response();

	/**
	 * ����˵��ñ����̶�ȡ�ͻ��˵���������
	 * @param out {MessageLite*} �洢�ͻ�����������
	 * @return {bool} �������Ƿ�ɹ�
	 */
	bool read_request(MessageLite* out);

	/**
	 * ��������ͨ�������̷��ظ��ͻ���
	 * @param int {const MessageLite&} ��������ɵĽ������
	 * @return {bool} д�����Ƿ�ɹ�
	 */
	bool send_response(const MessageLite& in);

	double header_spent() const
	{
		return header_spent_;
	}

	double body_spent() const
	{
		return body_spent_;
	}

	double parse_spent() const
	{
		return parse_spent_;
	}

	double build_spent() const
	{
		return build_spent_;
	}

	double response_spent() const
	{
		return response_spent_;
	}
private:
	acl::http_response* response_;
	double header_spent_;
	double body_spent_;
	double parse_spent_;
	double build_spent_;
	double response_spent_;
};

}  // namespace io
}  // namespace protobuf
}  // namespace google
