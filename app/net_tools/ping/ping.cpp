#include "StdAfx.h"
#include "rpc/rpc_manager.h"
#include "ping_store.h"
#include "ping.h"

//////////////////////////////////////////////////////////////////////////

host_status::host_status(ping& p, const char* ip)
: ping_(p)
, sent_(0)
, received_(0)
, lost_(0)
, loss_(0.00)
, minimum_(0)
, maximum_(0)
, average_(0)
{
	ACL_SAFE_STRNCPY(ip_, ip, sizeof(ip_));
}

host_status::~host_status()
{
	std::vector<PING_PKT*>::iterator it = pkt_list_.begin();
	for (; it != pkt_list_.end(); ++it)
		acl_myfree(*it);
}

void host_status::add_status(const ICMP_PKT_STATUS* status)
{
	PING_PKT* pkt = (PING_PKT*) acl_mycalloc(1, sizeof(PING_PKT));

	pkt->status_ = status->status;
	if (pkt->status_ == ICMP_STATUS_OK)
	{
		pkt->bytes_ = (int) status->reply_len;
		pkt->seq_ = status->seq;
		pkt->ttl_ = status->ttl;
		pkt->rtt_ = status->rtt;
	}
	//logger("ip: %s, bytes: %d, seq: %d, ttl: %d, rtt: %.2f, status: %d",
	//	ip_, pkt->bytes_, pkt->seq_, pkt->ttl_, pkt->rtt_, pkt->status_);
	pkt_list_.push_back(pkt);
}

void host_status::set_statistics(const ICMP_STAT* status)
{
	sent_ = (int) status->nsent;
	received_ = (int) status->nreceived;
	lost_ = sent_ - received_;
	loss_ = status->loss;
	minimum_ = status->tmin;
	maximum_ = status->tmax;
	average_ = status->tave;
}

//////////////////////////////////////////////////////////////////////////

ping::ping(const char* filepath, ping_callback* callback,
	int npkt, int delay, int timeout, int pkt_size)
: filepath_(filepath)
, callback_(callback)
, npkt_(npkt)
, delay_(delay)
, timeout_(timeout)
, pkt_size_(pkt_size)
, host_list_(NULL)
, total_pkt_(0)
, curr_pkt_(0)
, error_pkt_(0)
{

}

ping::~ping()
{
	if (host_list_)
		delete host_list_;
}

//////////////////////////////////////////////////////////////////////////
// ���߳�������

void ping::rpc_onover()
{
	callback_->ping_report(total_pkt_, total_pkt_, error_pkt_);
	ping_store* s = new ping_store(host_list_, callback_);
	host_list_ = NULL;
	rpc_manager::get_instance().fork(s);
	delete this;
}

void ping::rpc_wakeup(void*)
{
	callback_->ping_report(total_pkt_, curr_pkt_, error_pkt_);
}

//////////////////////////////////////////////////////////////////////////
// ���߳�������

static ICMP_CHAT *__chat = NULL;

void ping::rpc_run()
{
	if (load_file() == true)
		ping_all();
}

bool ping::load_file()
{
	acl::ifstream in;
	if (in.open_read(filepath_) == false)
	{
		logger_error("open file(%s) failed: %s",
			filepath_.c_str(), acl::last_serror());
		return false;
	}

	host_list_ = new std::vector<host_status*>;
	acl::string line;
	while (in.eof() == false)
	{
		if (in.gets(line) == false)
			break;
		host_status* pi = new host_status(*this, line.c_str());
		host_list_->push_back(pi);
	}

	if (host_list_->empty())
	{
		logger_error("no ip in file %s", filepath_.c_str());
		return false;
	}
	return true;
}

static void display_res2(ICMP_CHAT *chat)
{
	if (chat) {
		/* ��ʾ PING �Ľ���ܽ� */
		icmp_stat(chat);
		logger(">>>max pkts: %d", icmp_chat_seqno(chat));
	}
}

static void display_res(void)
{
	if (__chat) {
		display_res2(__chat);

		/* �ͷ� ICMP ���� */
		icmp_chat_free(__chat);
		__chat = NULL;
	}
}

void ping::ping_all()
{
	if (host_list_->empty())
	{
		logger_error("ip empty");
		return;
	}

	ACL_AIO *aio;
	/* �����������첽ͨ�ž�� */
	aio = acl_aio_create(ACL_EVENT_SELECT);
	acl_aio_set_keep_read(aio, 0);
	/* ���� ICMP ���� */
	__chat = icmp_chat_create(aio, 1);

	// ���Ŀ�� IP ��ַ
	std::vector<host_status*>::iterator it = host_list_->begin();
	for (; it != host_list_->end(); ++it)
	{
		//icmp_ping_one(__chat, NULL, (*cit)->get_ip(),
		//	npkt_, delay_, timeout_);
		// ����һ�� PING �Ķ��󣬲��Ը� IP ���� PING ����
		ICMP_HOST* host = icmp_host_new(__chat, NULL,
			(*it)->get_ip(), npkt_, pkt_size_, delay_, timeout_);
		host->enable_log = 1;
		icmp_host_set(host, (*it), ping_stat_response,
			ping_stat_timeout, ping_stat_unreach,
			ping_stat_finish);
		icmp_chat(host);
	}

	total_pkt_ = npkt_ * host_list_->size();
	rpc_signal(NULL);

	time_t last_signal = time(NULL), t;

	while (1) {
		/* ��� PING ���������˳�ѭ�� */
		if (icmp_chat_finish(__chat)) {
			logger("over now!, hosts' size=%d, count=%d",
				icmp_chat_size(__chat), icmp_chat_count(__chat));
			break;
		}

		/* �첽�¼�ѭ������ */
		acl_aio_loop(aio);
		t = time(NULL);
		if (t - last_signal >= 1)
		{
			rpc_signal(NULL);  // ֪ͨ���߳�
			last_signal = t;
		}
	}

	/* ��ʾ PING ��� */
	display_res();

	/* ���ٷ�������� */
	acl_aio_free(aio);
}

void ping::ping_stat_response(ICMP_PKT_STATUS* status, void* ctx)
{
	host_status* hs = (host_status*) ctx;
	hs->get_ping().curr_pkt_++;
	hs->add_status(status);
}

void ping::ping_stat_timeout(ICMP_PKT_STATUS* status, void* ctx)
{
	host_status* hs = (host_status*) ctx;
	hs->get_ping().curr_pkt_++;
	hs->get_ping().error_pkt_++;
	hs->add_status(status);
}

void ping::ping_stat_unreach(ICMP_PKT_STATUS* status, void* ctx)
{
	host_status* hs = (host_status*) ctx;
	hs->get_ping().curr_pkt_++;
	hs->get_ping().error_pkt_++;
	hs->add_status(status);
}

void ping::ping_stat_finish(ICMP_HOST* host, void* ctx)
{
	host_status* hs = (host_status*) ctx;
	//hs->get_ping().curr_pkt_++;
	icmp_stat_host(host, 0);  // ��Ҫ�ȼ���һ�¸�������ͳ��ֵ
	hs->set_statistics(&host->icmp_stat);
}
