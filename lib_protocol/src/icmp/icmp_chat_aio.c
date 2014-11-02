#include "StdAfx.h"
#include "icmp/lib_icmp.h"
#include "icmp_struct.h"
#include "icmp_private.h"

static void delay_send_pkt(ICMP_PKT*, int);
static int read_close_fn(ACL_ASTREAM*, void*);
static int read_ready_fn(ACL_ASTREAM*, void *, const char*, int);

static void check_timer(int event_type acl_unused,
	ACL_EVENT *event acl_unused, void *context)
{
	const char *myname = "check_timer";
	ICMP_CHAT *chat = (ICMP_CHAT*) context;
	ICMP_PKT *pkt, *pkt_next;

	if (chat == NULL)
		acl_msg_fatal("%s(%d): chat null", myname, __LINE__);

	while (1) {
		pkt = (ICMP_PKT*) chat->timer->popup(chat->timer);
		if (pkt == NULL)
			break;
		/* �㱨�������ʱ */
		icmp_stat_timeout(pkt);

		/* ��÷�������������һ�����ݰ� */
		pkt_next = ICMP_PKT_NEXT(&pkt->icmp_host->pkt_head, &pkt->pkt_ring);
		/* ��ʱ������һ���������ݰ� */
		if (pkt_next != NULL)
			delay_send_pkt(pkt_next, 0);
		else {
			/* ����Ը������İ�������ϣ���ص�״̬�㱨���� */
			if (pkt->icmp_host->stat_finish)
				pkt->icmp_host->stat_finish(pkt->icmp_host, pkt->icmp_host->arg);
			/* ����������������1 */
			chat->count++;
		}
	}

	/* ������һ�ζ�ʱ�� */
	acl_aio_request_timer(chat->aio, check_timer, chat, chat->check_inter, 0);
}

static void send_pkt(ICMP_PKT *pkt)
{
	ICMP_CHAT *chat = pkt->pkt_chat;
	ACL_ASTREAM *astream = chat->is->astream;
	ACL_VSTREAM *vstream = acl_aio_vstream(astream);
	int  ret;

	/* �齨�������ݰ� */
	icmp_pkt_build(pkt, chat->seq_no++);

	/* ָ����ǰ����Ŀ��������ַ����Ӵ��ݸ� acl_vstream_writen �еĻص����� */
	chat->is->curr_host = pkt->icmp_host;
	/* ����ͬ�����͵�ģʽ */
	ret = acl_vstream_writen(vstream, (const char*) pkt, (int) pkt->write_len);
	pkt->icmp_host->nsent++;

	if (ret == ACL_VSTREAM_EOF) {
		ICMP_PKT *pkt_next;

		/* �㱨�������ɴ���Ϣ */
		icmp_stat_unreach(pkt);

		pkt_next = ICMP_PKT_NEXT(&pkt->icmp_host->pkt_head, &pkt->pkt_ring);
		if (pkt_next != NULL) {
			/* �ӳٷ�����һ�����ݰ� */
			delay_send_pkt(pkt_next, pkt_next->icmp_host->delay);
		} else {
			/* ��һ�� ICMP_HOST ����� ICMP ���ѷ���, ���Խ� count ֵ�� 1 */
			chat->count++;

			/* �㱨�� ICMP_HOST ����� ICMP ���ɴ�״̬ */
			if (pkt->icmp_host->stat_finish)
				pkt->icmp_host->stat_finish(pkt->icmp_host, pkt->icmp_host->arg);
		}
	} else {
		/* ���ð����볬ʱ������ */
		chat->timer->request(chat->timer, pkt, pkt->icmp_host->timeout);
	}
}

void icmp_chat_aio(ICMP_HOST* host)
{
	const char *myname = "icmp_chat_aio";
	ICMP_PKT *pkt;

	pkt = ICMP_PKT_NEXT(&host->pkt_head, &host->pkt_head);
	if (pkt == NULL)
		acl_msg_fatal("%s(%d): first icmp pkt null", myname, __LINE__);

	/* ���͵ĵ�һ�����������Ҫ����д��ʱ�� */
	send_pkt(pkt);
}

/* ĳ�����ķ��Ͷ�ʱ������Ļص����� */
static void delay_send_timer(int event_type acl_unused,
	ACL_EVENT *event acl_unused, void *context)
{
	ICMP_PKT *pkt = (ICMP_PKT*) context;

	/* ���͸� ICMP �� */
	send_pkt(pkt);
}

static void delay_send_pkt(ICMP_PKT *pkt, int delay)
{
	const char *myname = "delay_send_pkt";

	if (pkt == NULL)
		acl_msg_fatal("%s(%d): pkt null", myname, __LINE__);

	/* ����д��ʱ������Ϊ����� delay ���뼶���� acl_aio_request_timer
	 * ��ʱ�䵥λ��΢��, ������Ҫ�� dely ����תΪ΢��
	 */
	acl_aio_request_timer(pkt->pkt_chat->aio, delay_send_timer, pkt, delay * 1000000, 0);
}

static int read_close_fn(ACL_ASTREAM *astream acl_unused, void *arg)
{
	const char *myname = "read_close_fn";
	ICMP_CHAT *chat = (ICMP_CHAT*) arg;

	/* ���ú���������ʱ�����Ѿ��������׽��������⣬������Ҫ����һ���� */

	acl_msg_warn("%s(%d): sock read error", myname, __LINE__);
	icmp_stream_reopen(chat->aio, chat->is);

	/* ���������첽�����̣�ע����ʱchat->is->astream �Ѿ���ͬ��
	 * �ú����Ĳ��� astream ��!
	 */
	acl_aio_read(chat->is->astream);

	/* ����-1��ʹ���첽�����Զ��ر� */
	return (-1);
}

static int read_ready_fn(ACL_ASTREAM *astream, void *arg, const char *data, int dlen)
{
	const char *myname = "read_ready_fn";
	ICMP_CHAT *chat = (ICMP_CHAT*) arg;
	ICMP_PKT *pkt_next, *pkt_src, pkt;

#define	READ_RETURN(_x_) do { \
	acl_aio_read(astream); \
	return(_x_); \
} while (0)

	if (chat == NULL)
		acl_msg_fatal("%s(%d): chat null", __FILE__, __LINE__);

	if (icmp_pkt_unpack(chat, data, dlen, &pkt) < 0) {
		READ_RETURN(0);
	}

	/* �����������ݣ�ȡ���÷��Ͱ��Ķ���ʱ��ʱ�� */
	pkt_src = chat->timer->find_delete(chat->timer, pkt.hdr.seq);

	if (pkt_src == NULL) {
		acl_msg_warn("%s(%d): seq(%d) not found",
			myname, __LINE__, pkt.hdr.seq);
		READ_RETURN(0);
	}

	icmp_pkt_save(pkt_src, &pkt);

	/* �㱨״̬ */
	icmp_stat_report(pkt_src);

	pkt_next = ICMP_PKT_NEXT(&pkt_src->icmp_host->pkt_head, &pkt_src->pkt_ring);
	if (pkt_next == NULL) {
		if (pkt_src->icmp_host->stat_finish)
			pkt_src->icmp_host->stat_finish(pkt_src->icmp_host, pkt_src->icmp_host->arg);
		chat->count++;
		READ_RETURN(0);
	}

	/* ��ʱ������һ���������ݰ� */
	delay_send_pkt(pkt_next, pkt_next->icmp_host->delay);
	READ_RETURN(0);
}

static int timeout_fn(ACL_ASTREAM *astream acl_unused, void *arg acl_unused)
{
	const char *myname = "timeout_fn";

	acl_msg_fatal("%s(%d): be called", myname, __LINE__);

	/* not reached */
	return (-1);
}

static int write_ready_fn(ACL_ASTREAM *astream acl_unused, void *arg acl_unused)
{
	const char *myname = "write_ready_fn";

	acl_msg_fatal("%s(%d): be called", myname, __LINE__);
	return (-1);
}

void icmp_chat_aio_init(ICMP_CHAT *chat, ACL_AIO *aio)
{
	acl_aio_set_delay_sec(aio, 0);
	acl_aio_set_delay_usec(aio, 50);

	acl_aio_ctl(chat->is->astream,
		ACL_AIO_CTL_READ_HOOK_ADD, read_ready_fn, chat,
		ACL_AIO_CTL_WRITE_HOOK_ADD, write_ready_fn, chat,
		ACL_AIO_CTL_CLOSE_HOOK_ADD, read_close_fn, chat,
		ACL_AIO_CTL_TIMEO_HOOK_ADD, timeout_fn, chat,
		ACL_AIO_CTL_TIMEOUT, 0,  /* ��ʼ��ʱ�����ö���ʱΪ 0 */
		ACL_AIO_CTL_END);

	chat->timer = icmp_timer_new();

	chat->check_inter = 2000000;	/* one second check timer */
	acl_aio_request_timer(aio, check_timer, chat, chat->check_inter, 0);
	/* ��ʼ�첽���ð�����Ӧ���� */
	acl_aio_read(chat->is->astream);
}

static void icmp_rset(ICMP_CHAT *chat)
{
	ACL_RING *ring_ptr;
	ICMP_HOST *host;

	while ((ring_ptr = acl_ring_pop_head(&chat->host_head)) != NULL) {
		host = RING_TO_HOST(ring_ptr);
		icmp_host_free(host);
	}

	chat->count = 0;
	acl_ring_init(&chat->host_head);
}

void icmp_chat_aio_free(ICMP_CHAT *chat)
{
	icmp_rset(chat);
	icmp_timer_free(chat->timer);
	icmp_stream_close(chat->is);
	acl_myfree(chat);
}
