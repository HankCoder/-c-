#pragma once

namespace acl {

struct HTTP_PARAM 
{
	char* name;
	char* value;
};

// HTTP ��Ӧ״̬
typedef enum
{
	HTTP_OK,                // һ������
	HTTP_ERR_DNS,           // ��������ʧ��
	HTTP_ERR_CONN,          // ���ӷ�����ʧ��
	HTTP_ERR_REQ,           // ��������Э��ʧ��
	HTTP_ERR_READ,          // ������ʧ��
	HTTP_ERR_SEND,          // д����ʧ��
	HTTP_ERR_TIMO,          // ��д���ݳ�ʱ
	HTTP_ERR_READ_HDR,      // �� HTTP ��Ӧͷʧ��
	HTTP_ERR_READ_BODY,     // �� HTTP ��Ӧ��ʧ��
	HTTP_ERR_INVALID_HDR,   // HTTP ��Ӧͷ��Ч
	HTTP_ERR_UNKNOWN,       // ������δ֪����
	HTTP_ERR_REDIRECT_MAX,	// HTTP ��Ӧͷ���ض������̫��
} http_status_t;

// HTTP ���󷽷�
typedef enum
{
	HTTP_METHOD_UNKNOWN,    // δ֪����
	HTTP_METHOD_GET,        // GET ����
	HTTP_METHOD_POST,       // POST ����
	HTTP_METHOD_PUT,        // PUT ����
	HTTP_METHOD_CONNECT,    // CONNECT ����
	HTTP_METHOD_PURGE       // PURGE ����
} http_method_t;

typedef enum
{
	// Content-Type: application/x-www-form-urlencoded
	HTTP_REQUEST_NORMAL,

	// Content-Type: multipart/form-data; boundary=xxx
	HTTP_REQUEST_MULTIPART_FORM,

	// Content-Type: application/octet-stream
	HTTP_REQUEST_OCTET_STREAM,

	// ��������
	HTTP_REQUEST_OTHER
} http_request_t;

typedef enum
{
	// ok
	HTTP_REQ_OK,

	// network io error
	HTTP_REQ_ERR_IO,

	// invalid request method
	HTTP_REQ_ERR_METHOD
} http_request_error_t;

typedef enum
{
	HTTP_MIME_PARAM,        // http mime ���Ϊ��������
	HTTP_MIME_FILE          // http mime ���Ϊ�ļ�����
} http_mime_t;

} // namespace acl end
