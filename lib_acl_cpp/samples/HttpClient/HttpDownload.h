#pragma once
#include "lib_acl.h"
#include "lib_protocol.h"  // http Э�����
#include "acl_cpp/stream/ofstream.hpp"
#include "acl_cpp/http/http_service.hpp"

class acl::string;
class acl::aio_handle;

class CHttpDownload : public acl::http_service_request
{
public:
	CHttpDownload(const char* domain, unsigned short port,
		acl::aio_handle* handle);

#ifdef WIN32
	void SetHWnd(HWND hWnd);
#endif

	// ������ӿڣ����ٹ��̣��� http_service �ദ����Ϻ��Զ����øûص�
	virtual void destroy();

protected:
	~CHttpDownload(void);

	//////////////////////////////////////////////////////////////////////////
	// ������ӿ�

	// ���HTTP�����������ʱ�Ļص��ӿڣ�ע��ú����ĵ��ÿռ���������������ͬ
	// һ���߳̿ռ䣬��������ú�������������������ͬ����Դʱ��Ҫע�⻥��
	virtual const acl::string* get_body();
	// �������� HTTP ��Ӧͷʱ�Ļص��ӿ�
	virtual void on_hdr(const char* addr, const HTTP_HDR_RES* hdr);
	// ������ HTTP ��Ӧ��ʱ�Ļص�����
	virtual void on_body(const char* data, size_t dlen);
	// ���������Ӧʧ��ʱ�Ļص�����
	virtual void on_error(acl::http_status_t errnum);
private:
	acl::ofstream out_;
	acl::aio_handle* handle_;
#ifdef WIN32
	HWND hWnd_;
#endif
	http_off_t  read_length_;
	http_off_t  content_length_;
	time_t begin_;
};
