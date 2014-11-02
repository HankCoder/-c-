// rpc_download.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include <assert.h>
#include "acl_cpp/lib_acl.hpp"

using namespace acl;

typedef enum
{
	CTX_T_CONTENT_LENGTH,
	CTX_T_PARTIAL_LENGTH,
	CTX_T_END
} ctx_t;

struct DOWN_CTX 
{
	ctx_t type;
	int length;
};

static int __download_count = 0;

class http_down : public rpc_request
{
public:
	http_down(aio_handle& handle, const char* addr, const char* url)
		: handle_(handle)
		, addr_(addr)
		, url_(url)
		, error_(false)
		, total_read_(0)
		, content_length_(0)
	{}
	~http_down() {}
protected:

	// ���̴߳�����
	void rpc_run()
	{
		http_request req(addr_);  // HTTP �������

		// ���� HTTP ����ͷ��Ϣ
		req.request_header().set_url(url_.c_str())
			.set_content_type("text/html")
			.set_host(addr_.c_str())
			.set_method(HTTP_METHOD_GET);

		// �����ã���ʾ HTTP ����ͷ��Ϣ����
		string header;
		req.request_header().build_request(header);
		printf("request: %s\r\n", header.c_str());

		// ���� HTTP ��������
		if (req.request(NULL, 0) == false)
		{
			printf("send request error\r\n");
			error_ = false;
			return;
		}

		// ��� HTTP ��������Ӷ���
		http_client* conn = req.get_client();
		assert(conn);
		DOWN_CTX* ctx = new DOWN_CTX;
		ctx->type = CTX_T_CONTENT_LENGTH;

		// ��� HTTP ��Ӧ���ݵ������峤��
		ctx->length = (int) conn->body_length();
		content_length_ = ctx->length;

		// ֪ͨ���߳�
		rpc_signal(ctx);

		char buf[8192];
		while (true)
		{
			// �� HTTP ��Ӧ������
			int ret = req.read_body(buf, sizeof(buf));
			if (ret <= 0)
			{
				ctx = new DOWN_CTX;
				ctx->type = CTX_T_END;
				ctx->length = ret;
				// ֪ͨ���߳�
				rpc_signal(ctx);
				break;
			}
			ctx = new DOWN_CTX;
			ctx->type = CTX_T_PARTIAL_LENGTH;
			ctx->length = ret;
			// ֪ͨ���߳�
			rpc_signal(ctx);
		}
	}

	// ���̴߳�����̣��յ����߳�������ɵ���Ϣ
	void rpc_onover()
	{
		printf("%s: read over now, total read: %d, content-length: %d\r\n",
			addr_.c_str(), total_read_, content_length_);

		// �� HTTP ��Ӧ�����ʱ��֪ͨ���߳�ֹͣ�¼�ѭ������
		__download_count--;
		if (__download_count == 0)
			handle_.stop();
	}

	// ���̴߳�����̣��յ����̵߳�֪ͨ��Ϣ
	void rpc_wakeup(void* ctx)
	{
		DOWN_CTX* down_ctx = (DOWN_CTX*) ctx;
		switch (down_ctx->type)
		{
		case CTX_T_CONTENT_LENGTH:
			printf("%s: content-length: %d\r\n",
				addr_.c_str(), down_ctx->length);
			break;
		case CTX_T_PARTIAL_LENGTH:
			total_read_ += down_ctx->length;
			printf("%s: partial-length: %d, total read: %d\r\n",
				addr_.c_str(), down_ctx->length, total_read_);
			break;
		case CTX_T_END:
			printf("%s: read over\r\n", addr_.c_str());
			break;
		default:
			printf("%s: ERROR\r\n", addr_.c_str());
			break;
		}
		delete down_ctx;
	}
private:
	aio_handle& handle_;
	string addr_;
	string url_;
	bool  error_;
	int   total_read_;
	int   content_length_;
};

static void run(void)
{
	aio_handle handle;
	rpc_service* service = new rpc_service(10);  // ���� rpc �������

	// ����Ϣ������
	if (service->open(&handle) == false)
	{
		printf("open service error: %s\r\n", last_serror());
		return;
	}

	// ����ҳ������

	http_down down1(handle, "www.sina.com.cn:80", "http://www.sina.com.cn/");
	service->rpc_fork(&down1);  // ����һ�������Ự����
	__download_count++;

	http_down down2(handle, "www.hexun.com:80", "/");
	service->rpc_fork(&down2);  // ����ڶ��������Ự����
	__download_count++;

	// �첽�¼�ѭ������
	while (true)
	{
		if (handle.check() == false)
			break;
	}

	delete service;
	handle.check(); // ��֤�ͷ������ӳٹرյ��첽����
}

int main(void)
{
#ifdef WIN32
	acl_cpp_init();
#endif

	run();
	printf("Enter any key to continue\r\n");
	getchar();
	return 0;
}
