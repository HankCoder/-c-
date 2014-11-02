#include "stdafx.h"
#include <assert.h>
#include "http_download.h"

// �����̶߳�̬������ DOWN_CTX �������������
typedef enum
{
	CTX_T_REQ_HDR,		// Ϊ HTTP ����ͷ����
	CTX_T_RES_HDR,		// Ϊ HTTP ��Ӧͷ����
	CTX_T_CONTENT_LENGTH,	// Ϊ HTTP ��Ӧ��ĳ���
	CTX_T_PARTIAL_LENGTH,	// Ϊ HTTP ����������ĳ���
	CTX_T_END
} ctx_t;

// ���̶߳�̬���������ݶ������߳̽��մ�����
struct DOWN_CTX 
{
	ctx_t type;
	long long int length;
};

// ������ȷ����ʱ��ؼ���ĺ�������ȷ�����뼶��
static double stamp_sub(const struct timeval *from,
	const struct timeval *sub_by)
{
	struct timeval res;

	memcpy(&res, from, sizeof(struct timeval));

	res.tv_usec -= sub_by->tv_usec;
	if (res.tv_usec < 0)
	{
		--res.tv_sec;
		res.tv_usec += 1000000;
	}

	res.tv_sec -= sub_by->tv_sec;
	return (res.tv_sec * 1000.0 + res.tv_usec/1000.0);
}

//////////////////////////////////////////////////////////////////////////

// ���̴߳�����
void http_download::rpc_run()
{
	acl::http_request req(addr_);  // HTTP �������
	// ���� HTTP ����ͷ��Ϣ
	req.request_header().set_url(url_.c_str())
		.set_content_type("text/html")
		.set_host(addr_.c_str())
		.set_method(acl::HTTP_METHOD_GET);

	req.request_header().build_request(req_hdr_);
	DOWN_CTX* ctx = new DOWN_CTX;
	ctx->type = CTX_T_REQ_HDR;
	rpc_signal(ctx);  // ֪ͨ���߳� HTTP ����ͷ����

	struct timeval begin, end;;
	gettimeofday(&begin, NULL);

	// ���� HTTP ��������
	if (req.request(NULL, 0) == false)
	{
		logger_error("send request error");
		error_ = false;
		gettimeofday(&end, NULL);
		total_spent_ = stamp_sub(&end, &begin);
		return;
	}

	// ��� HTTP ��������Ӷ���
	acl::http_client* conn = req.get_client();
	assert(conn);

	(void) conn->get_respond_head(&res_hdr_);
	ctx = new DOWN_CTX;
	ctx->type = CTX_T_RES_HDR;
	rpc_signal(ctx);   // ֪ͨ���߳� HTTP ��Ӧͷ����

	ctx = new DOWN_CTX;
	ctx->type = CTX_T_CONTENT_LENGTH;
	
	ctx->length = conn->body_length();  // ��� HTTP ��Ӧ���ݵ������峤��
	content_length_ = ctx->length;
	rpc_signal(ctx);  // ֪ͨ���߳� HTTP ��Ӧ�����ݳ���

	acl::string buf(8192);
	int   real_size;
	while (true)
	{
		// �� HTTP ��Ӧ������
		int ret = req.read_body(buf, true, &real_size);
		if (ret <= 0)
		{
			ctx = new DOWN_CTX;
			ctx->type = CTX_T_END;
			ctx->length = ret;
			rpc_signal(ctx);  // ֪ͨ���߳��������
			break;
		}
		ctx = new DOWN_CTX;
		ctx->type = CTX_T_PARTIAL_LENGTH;
		ctx->length = real_size;
		// ֪ͨ���̵߳�ǰ�Ѿ����صĴ�С
		rpc_signal(ctx);
	}

	// �������ع�����ʱ��
	gettimeofday(&end, NULL);
	total_spent_ = stamp_sub(&end, &begin);

	// ���ˣ����߳�������ϣ����̵߳� rpc_onover ���̽�������
}

//////////////////////////////////////////////////////////////////////////

http_download::http_download(const char* addr, const char* url,
	rpc_callback* callback)
	: addr_(addr)
	, url_(url)
	, callback_(callback)
	, error_(false)
	, total_read_(0)
	, content_length_(0)
	, total_spent_(0)
{

}

//////////////////////////////////////////////////////////////////////////

// ���̴߳�����̣��յ����߳�������ɵ���Ϣ
void http_download::rpc_onover()
{
	logger("http download(%s) over, �� %I64d �ֽڣ���ʱ %.3f ����",
		url_.c_str(), total_read_, total_spent_);
	callback_->OnDownloadOver(total_read_, total_spent_);
	delete this;  // ���ٱ�����
}

// ���̴߳�����̣��յ����̵߳�֪ͨ��Ϣ
void http_download::rpc_wakeup(void* ctx)
{
	DOWN_CTX* down_ctx = (DOWN_CTX*) ctx;

	// �������߳��д����Ĳ�ͬ�����ؽ׶ν��д���

	switch (down_ctx->type)
	{
	case CTX_T_REQ_HDR:
		callback_->SetRequestHdr(req_hdr_.c_str());
		break;
	case CTX_T_RES_HDR:
		callback_->SetResponseHdr(res_hdr_.c_str());
		break;
	case CTX_T_CONTENT_LENGTH:
		break;
	case CTX_T_PARTIAL_LENGTH:
		total_read_ += down_ctx->length;
		callback_->OnDownloading(content_length_, total_read_);
		break;
	case CTX_T_END:
		logger("%s: read over", addr_.c_str());
		break;
	default:
		logger_error("%s: ERROR", addr_.c_str());
		break;
	}

	// ɾ�������߳��ж�̬����Ķ���
	delete down_ctx;
}

//////////////////////////////////////////////////////////////////////////
