#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#include "stdlib/acl_stdlib.h"
#include "net/acl_net.h"
#include "aio/acl_aio.h"

#endif

#include "../event/events.h"
#include "aio.h"

#if 0

#define READ_SAFE_ENABLE(x, callback) do {  \
	if (((x)->flag & ACL_AIO_FLAG_ISRD) == 0) {  \
		(x)->flag |= ACL_AIO_FLAG_ISRD;  \
		acl_event_enable_read((x)->aio->event, (x)->stream,  \
			(x)->timeout, callback, (x));  \
	}  \
} while (0)

#define READ_SAFE_DISABLE(x) do {  \
	if (((x)->flag & ACL_AIO_FLAG_ISRD) != 0) {  \
		(x)->flag &= ~ACL_AIO_FLAG_ISRD;  \
		(x)->can_read_fn = NULL;  \
		(x)->can_read_ctx = NULL;  \
		acl_event_disable_read((x)->aio->event, (x)->stream);  \
	}  \
} while (0)

#else

#define READ_SAFE_ENABLE(x, callback) do {  \
	if (((x)->flag & ACL_AIO_FLAG_ISRD) == 0) {  \
		(x)->flag |= ACL_AIO_FLAG_ISRD;  \
		(x)->aio->event->enable_read_fn((x)->aio->event,  \
			(x)->stream, (x)->timeout, callback, (x));  \
	}  \
} while (0)

#define READ_SAFE_DISABLE(x) do {  \
	if (((x)->flag & ACL_AIO_FLAG_ISRD) != 0) {  \
		(x)->flag &= ~ACL_AIO_FLAG_ISRD;  \
		(x)->can_read_fn = NULL;  \
		(x)->can_read_ctx = NULL;  \
		(x)->aio->event->disable_read_fn((x)->aio->event, (x)->stream);  \
	}  \
} while (0)

#endif

# define READ_IOCP_CLOSE(x) do {  \
	READ_SAFE_DISABLE((x));  \
	(x)->flag |= ACL_AIO_FLAG_IOCP_CLOSE;  \
	acl_aio_iocp_close((x));  \
} while (0)

/* ͳһ�Ķ��¼�����ص��ӿ� */
static void main_read_callback(int event_type, ACL_EVENT *event acl_unused,
	ACL_VSTREAM *stream acl_unused, void *context)
{
	ACL_ASTREAM *astream = (ACL_ASTREAM*) context;

	/* ���� __gets_notify_callback/__read_notify_callback/__readn_notify_callback */

	astream->event_read_callback(event_type, astream);
}

static int read_complete_callback(ACL_ASTREAM *astream, char *data, int len)
{
	int   ret = 0;

	/* ���뽫��������λ����������һ�ζ��¼�(�������ݻ����)����ʱ��
	 * ��Ϊ������ if (astream->count <= n) {} ������ fatal
	 * ---2008.11.5, zsx
	 */
	/* ��Ҫ����������գ��Է����ظ����� */
	ACL_VSTRING_RESET(&astream->strbuf);

	/* �����ü�����1���Է�ֹ�����쳣�ر� */
	astream->nrefer++;

	if (astream->read_handles) {
		ACL_ITER iter;
		AIO_READ_HOOK *handle;

		/* XXX: ���뽫�����ص�����ӻص�������һһ�������һ������������,
		 * ��Ϊ ACL_AIO �ڻص��������п��ܷ���Ƕ�ף���ֹ���ظ�����
		 */

		while (1) {
			handle = astream->read_handles->pop_back(
				astream->read_handles);
			if (handle == NULL)
				break;
			astream->reader_fifo.push_back(&astream->reader_fifo, handle);
		}

		acl_foreach_reverse(iter, &astream->reader_fifo) {
			handle = (AIO_READ_HOOK*) iter.data;
			if (handle->disable)
				continue;
			ret = handle->callback(astream, handle->ctx, data, len);
			if (ret != 0) {
				astream->nrefer--;
				return ret;
			}
		}
	}

	astream->nrefer--;
	return ret;
}

/* �����Զ�һ������
 * @param astream {ACL_ASTREAM*}
 * @return {int} ����ֵ
 *  -1: ��ʾ������ϣ���ر���
 *   0: ��ʾ����δ׼���û��û������˷�������
 *   1: ��ʾ������׼���ã����Ѿ����ù�һ���û��Ļص��������û�ϣ��������
 */

static int __gets_peek(ACL_ASTREAM *astream)
{
	int   n, ready = 0;
	
	n = astream->read_ready_fn(astream->stream, &astream->strbuf, &ready);

	if (n == ACL_VSTREAM_EOF) {
#if ACL_EWOULDBLOCK == ACL_EAGAIN
		if (astream->stream->errnum == ACL_EWOULDBLOCK)
#else
		if (astream->stream->errnum == ACL_EWOULDBLOCK
			|| astream->stream->errnum == ACL_EAGAIN)
#endif
		{
			READ_SAFE_ENABLE(astream, main_read_callback);
			return 0;
		}

		/* XXX: ����鿴���������Ƿ�������,
		 * ����������ݶ����������!
		 */
		if (ACL_VSTRING_LEN(&astream->strbuf) > 0) {
			char *ptr = acl_vstring_str(&astream->strbuf);
			int   len = ACL_VSTRING_LEN(&astream->strbuf);

			(void) read_complete_callback(astream, ptr, len);
		}
		/* ��������Ҫ�ر��� */
		astream->flag |= ACL_AIO_FLAG_DEAD;
		READ_IOCP_CLOSE(astream);
		return -1;
	} else if (ready) {
		char *ptr = acl_vstring_str(&astream->strbuf);
		int   len = ACL_VSTRING_LEN(&astream->strbuf);

		/* �ص��û��Ķ��гɹ������� */
		n = read_complete_callback(astream, ptr, len);

		/* ���û�ϣ���ر���������������ӳٹر�״̬������Ҫ����
		 * ����IO�ӳٹرչ���
		 */
		if (n < 0 || (astream->flag & ACL_AIO_FLAG_IOCP_CLOSE)) {
			READ_IOCP_CLOSE(astream);
			return -1;
		} else if (astream->keep_read == 0
			|| (astream->flag & ACL_AIO_FLAG_ISRD) == 0)
		{
			return 0;
		}
		return len;
	}

	/* δ������Ҫ���һ�����ݣ�������ظ����Ķ����¼� */
	READ_SAFE_ENABLE(astream, main_read_callback);
	return 0;
}

/* ���¼���ع��̻ص������Ķ����¼�������� */

static void __gets_notify_callback(int event_type, ACL_ASTREAM *astream)
{
	const char *myname = "__gets_notify_callback";

	if (astream->keep_read == 0)
		READ_SAFE_DISABLE(astream);

	if ((event_type & ACL_EVENT_XCPT) != 0) {
		/* �������������п��ܹرյ��¼�֪ͨ����ʱ����Ȼ�ɶ���
		 * ��Ӧ�ñ�֤�����ȣ�ֱ���Ѳ���ϵͳ�������е����ݶ���
		 * Ϊֹ������ٴ���ر��¼������ر���
		 */
		int   ret;
		acl_non_blocking(ACL_VSTREAM_SOCK(astream->stream),
			ACL_NON_BLOCKING);
		do {
			astream->stream->sys_read_ready = 1;
			ret = __gets_peek(astream);
			if (astream->keep_read == 0)
				break;
		} while (ret > 0);
		READ_IOCP_CLOSE(astream);
		return;
	} else if ((event_type & ACL_EVENT_RW_TIMEOUT) != 0) {
		/* ������ʱ�����Ӧ�÷���ֵ���ڵ���0����ϣ��������,
		 * �������ֵС��0��ϣ���ر��������˻�������������
		 */
		if (aio_timeout_callback(astream) < 0) {
			READ_IOCP_CLOSE(astream);
		} else if (astream->flag & ACL_AIO_FLAG_IOCP_CLOSE) {
			/* ����������IO�ӳٹر�״̬����Ϊ���ζ�IO�Ѿ�
			 * �ɹ���ɣ�������Ҫ�������IO�ӳٹرչ���
			 */
			READ_IOCP_CLOSE(astream);
		} else {
			READ_SAFE_ENABLE(astream, main_read_callback);
		}

		return;
	}

	if ((event_type & ACL_EVENT_READ) == 0)
		acl_msg_fatal("%s: unknown event: %d", myname, event_type);

	/* �����Եض����� */
	while (1) {
		if (__gets_peek(astream) <= 0 || astream->keep_read == 0)
			break;
	}
}

/**
 * �첽��һ������
 * @param astream {ACL_ASTREAM*}
 * @param nonl {int} �Ƿ��Զ�ȥ��β���� \r\n
 */
static void __aio_gets(ACL_ASTREAM *astream, int nonl)
{
	const char *myname = "__aio_gets";

	if ((astream->flag & ACL_AIO_FLAG_DELAY_CLOSE))
		return;
	if (astream->stream == NULL)
		acl_msg_fatal("%s: astream->stream null", myname);

	/* ���ö������� */
	if (nonl)
		astream->read_ready_fn = acl_vstream_gets_nonl_peek;
	else
		astream->read_ready_fn = acl_vstream_gets_peek;

	if (astream->line_length > 0)
		astream->strbuf.maxlen = astream->line_length;

	astream->event_read_callback = __gets_notify_callback;

	ACL_VSTRING_RESET(&astream->strbuf);

	/* ��Ƕ�׼�����1���Է�ֹǶ�ײ��̫���ʹջ��� */
	astream->read_nested++;

	/* ������ص�����ʱ���п����Ǵ�ϵͳ�������ж�ȡ���ݣ�Ҳ�п��ܴ��û�
	 * �����������ݣ����ڳ��������̣����û��ڻص���ȡ���˶����������û�
	 * ��������������ʱ�����޷���ظ�����ϵͳ�����������Զ��ڳ������Ķ�
	 * ���������뱣֤�����ڶ�����״̬
	 */ 
	if (astream->keep_read)
		READ_SAFE_ENABLE(astream, main_read_callback);

	/* ���Ƕ�׵��ô���С�ڷ�ֵ�����������Ƕ�׵��� */
	if (astream->read_nested < astream->read_nested_limit) {
		/* �����Եض����� */
		while (1) {
			if (__gets_peek(astream) <= 0 || astream->keep_read == 0)
				break;
		}
		astream->read_nested--;
		return;
	}

	/* �ݹ�Ƕ�׶������ﵽ�˹涨�ķ�ֵ��
	 * ֻ��Ǹ�������Ϣ���ɣ���Ϊ��Ƕ������
	 */
	if (acl_msg_verbose)
		acl_msg_warn("%s: read_nested(%d) >= max(%d)", myname,
			astream->read_nested, astream->read_nested_limit);
	/* ���򣬲��������Ƕ�ף������¼������¼����ѭ���У��Լ���Ƕ�ײ�� */

	astream->read_nested--;

	/* �������Ķ��¼������¼������ */
	READ_SAFE_ENABLE(astream, main_read_callback);
}

void acl_aio_gets(ACL_ASTREAM *astream)
{
	__aio_gets(astream, 0);
}

void acl_aio_gets_nonl(ACL_ASTREAM *astream)
{
	__aio_gets(astream, 1);
}

/* �����Զ�����
 * @param astream {ACL_ASTREAM*}
 * @return {int} ����ֵ
 *  -1: ��ʾ������ϣ���ر���
 *   0: ��ʾ����δ׼���û��û������˷�������
 *   1: ��ʾ������׼���ã����Ѿ����ù�һ���û��Ļص��������û�ϣ��������
 */

static int __read_peek(ACL_ASTREAM *astream)
{
	int   n;

	/* �����Եض����� */
	n = acl_vstream_read_peek(astream->stream, &astream->strbuf);

	if (n == ACL_VSTREAM_EOF) {
#if ACL_EWOULDBLOCK == ACL_EAGAIN
		if (astream->stream->errnum == ACL_EWOULDBLOCK)
#else
		if (astream->stream->errnum == ACL_EAGAIN
			|| astream->stream->errnum == ACL_EWOULDBLOCK)
#endif
		{
			READ_SAFE_ENABLE(astream, main_read_callback);
			return 0;
		}

		/* ����鿴���������Ƿ�������, ����������ݶ����������! */
		if (ACL_VSTRING_LEN(&astream->strbuf) > 0) {
			char *ptr = acl_vstring_str(&astream->strbuf);
			int   len = ACL_VSTRING_LEN(&astream->strbuf);

			(void) read_complete_callback(astream, ptr, len);
		}
		/* ��������Ҫ�ر��� */
		astream->flag |= ACL_AIO_FLAG_DEAD;
		READ_IOCP_CLOSE(astream);
		return -1;
	} else if (n > 0) {
		char *ptr = acl_vstring_str(&astream->strbuf);
		int   len = ACL_VSTRING_LEN(&astream->strbuf);

		/* �ص��û��Ķ��ɹ������� */
		n = read_complete_callback(astream, ptr, len);

		/* ���û�ϣ���ر���������������ӳٹر�״̬������Ҫ����
		 * ����IO�ӳٹرչ���
		 */
		if (n < 0 || astream->flag & ACL_AIO_FLAG_IOCP_CLOSE) {
			READ_IOCP_CLOSE(astream);
			return -1;
		} else if (astream->keep_read == 0
			|| (astream->flag & ACL_AIO_FLAG_ISRD) == 0)
		{
			return 0;
		}
		return len;
	} else {
		/* �����ݲ�����Ҫ�󣬼�����ظö��¼� */
		READ_SAFE_ENABLE(astream, main_read_callback);
		return 0;
	}
}

/* ���¼���ع��̻ص������Ķ��¼�������� */

static void __read_notify_callback(int event_type, ACL_ASTREAM *astream)
{
	const char *myname = "__read_notify_callback";

	if (astream->keep_read == 0)
		READ_SAFE_DISABLE(astream);

	if ((event_type & ACL_EVENT_XCPT) != 0) {
		/* �������������п��ܹرյ��¼�֪ͨ����ʱ����Ȼ�ɶ���
		 * ��Ӧ�ñ�֤�����ȣ�ֱ���Ѳ���ϵͳ�������е����ݶ���
		 * Ϊֹ������ٴ���ر��¼������ر���
		 */
		int   ret;
		acl_non_blocking(ACL_VSTREAM_SOCK(astream->stream),
			ACL_NON_BLOCKING);
		do {
			astream->stream->sys_read_ready = 1;
			ret = __read_peek(astream);
		} while (ret > 0);

		READ_IOCP_CLOSE(astream);
		return;
	} else if ((event_type & ACL_EVENT_RW_TIMEOUT) != 0) {
		/* ������ʱ�����Ӧ�÷���ֵ���ڵ���0����ϣ��������,
		 * �������ֵС��0��ϣ���ر��������˻�������������
		 */
		if (aio_timeout_callback(astream) < 0) {
			/* �û�ϣ���ر��� */
			READ_IOCP_CLOSE(astream);
		} else if (astream->flag & ACL_AIO_FLAG_IOCP_CLOSE) {
			/* ����������IO�ӳٹر�״̬����Ϊ���ζ�IO�Ѿ��ɹ���ɣ�
			 * ������Ҫ�������IO�ӳٹرչ���
			 */
			READ_IOCP_CLOSE(astream);
		} else {
			READ_SAFE_ENABLE(astream, main_read_callback);
		}

		return;
	}

	if ((event_type & ACL_EVENT_READ) == 0)
		acl_msg_fatal("%s: unknown event: %d", myname, event_type);

	/* �����Եض����� */
	while (1) {
		if (__read_peek(astream) <= 0 || astream->keep_read == 0)
			break;
	}
}

void acl_aio_read(ACL_ASTREAM *astream)
{
	const char *myname = "acl_aio_read";

	if ((astream->flag & ACL_AIO_FLAG_DELAY_CLOSE))
		return;
	if (astream->stream == NULL)
		acl_msg_fatal("%s: astream(%lx)->stream null",
			myname, (long) astream);

	astream->event_read_callback = __read_notify_callback;
	/* XXX: ���뽫���������� */
	ACL_VSTRING_RESET(&astream->strbuf);

	/* ������ص�����ʱ���п����Ǵ�ϵͳ�������ж�ȡ���ݣ�Ҳ�п��ܴ��û�
	 * �����������ݣ����ڳ��������̣����û��ڻص���ȡ���˶����������û�
	 * ��������������ʱ�����޷���ظ�����ϵͳ�����������Զ��ڳ������Ķ�
	 * ���������뱣֤�����ڶ�����״̬
	 */ 
	if (astream->keep_read)
		READ_SAFE_ENABLE(astream, main_read_callback);

	/* ��Ƕ�׼�����1���Է�ֹǶ�ײ��̫���ʹջ��� */
	astream->read_nested++;

	/* ���Ƕ�׵��ô���С�ڷ�ֵ�����������Ƕ�׵��� */
	if (astream->read_nested < astream->read_nested_limit) {
		/* �����Եض����� */
		while (1) {
			if (__read_peek(astream) <= 0 || astream->keep_read == 0)
				break;
		}
		astream->read_nested--;
		return;
	}

	/* �ݹ�Ƕ�׶������ﵽ�˹涨�ķ�ֵ��ֻ��Ǹ�������Ϣ����Ϊ��Ƕ������ */
	if (acl_msg_verbose)
		acl_msg_warn("%s: read_nested(%d) >= max(%d)", myname,
			astream->read_nested, astream->read_nested_limit);

	/* ���򣬲��������Ƕ�ף������¼������¼����ѭ���У�����Ƕ�ײ�� */

	astream->read_nested--;

	/* �������Ķ��¼������¼������ */
	READ_SAFE_ENABLE(astream, main_read_callback);
}

/* �����Զ��涨������������
 * @param astream {ACL_ASTREAM*}
 * @return {int} ����ֵ
 *  -1: ��ʾ������ϣ���ر���
 *   0: ��ʾ����δ׼���û��û������˷�������
 *   1: ��ʾ������׼���ã����Ѿ����ù�һ���û��Ļص��������û�ϣ��������
 */

static int __readn_peek(ACL_ASTREAM *astream)
{
	const char *myname = "__readn_peek";
	int   n, ready = 0;

	n = ACL_VSTRING_LEN(&astream->strbuf);

	if (astream->count <= n)
		acl_msg_fatal("%s: count(%d) < strlen(%d), read_netsted(%d)",
			myname, astream->count, n, astream->read_nested);

	/* �����Եض����� */
	n = acl_vstream_readn_peek(astream->stream, &astream->strbuf,
		astream->count - n, &ready);
	if (n == ACL_VSTREAM_EOF) {
#if ACL_EWOULDBLOCK == ACL_EAGAIN
		if (astream->stream->errnum == ACL_EWOULDBLOCK)
#else
		if (astream->stream->errnum == ACL_EAGAIN
			|| astream->stream->errnum == ACL_EWOULDBLOCK)
#endif
		{
			READ_SAFE_ENABLE(astream, main_read_callback);
			return 0;
		}
		/* XXX: �鿴���������Ƿ�������, ����������ݶ����������! */
		if (ACL_VSTRING_LEN(&astream->strbuf) > 0) {
			char *ptr = acl_vstring_str(&astream->strbuf);
			int   len = ACL_VSTRING_LEN(&astream->strbuf);

			acl_msg_warn("%s: nneed(%d), nread(%d),"
				" read_netsted(%d), nrefer(%d)",
				myname, astream->count, len,
				astream->read_nested, astream->nrefer);

			(void) read_complete_callback(astream, ptr, len);
		}
		/* ���������رգ���Ҫ�ر��� */
		astream->flag |= ACL_AIO_FLAG_DEAD;
		READ_IOCP_CLOSE(astream);
		return -1;
	} else if (ready) {
		/* ok, �Ѿ���������������Ѿ��������Ҫ�����ݳ��ȵ����� */
		char *ptr = acl_vstring_str(&astream->strbuf);
		int   len = ACL_VSTRING_LEN(&astream->strbuf);

		if (len != astream->count)
			acl_msg_fatal("%s: len: %d != count: %d",
				myname, len, astream->count);

		/* �ص��û��Ķ��ɹ������� */
		n = read_complete_callback(astream, ptr, len);
		if (n < 0 || astream->flag & ACL_AIO_FLAG_IOCP_CLOSE) {
			READ_IOCP_CLOSE(astream);
			return -1;
		} else if (astream->keep_read == 0
			|| (astream->flag & ACL_AIO_FLAG_ISRD) == 0)
		{
			return 0;
		}
		return len;
	} else {
		/* �����ݲ�����Ҫ�󣬼�����ظö��¼� */
		READ_SAFE_ENABLE(astream, main_read_callback);
		return 0;
	}
}

/* ���¼������ص������� */

static void __readn_notify_callback(int event_type, ACL_ASTREAM *astream)
{
	const char *myname = "__readn_notify_callback";

	if (astream->keep_read == 0)
		READ_SAFE_DISABLE(astream);

	if ((event_type & ACL_EVENT_XCPT) != 0) {
		/* �������������п��ܹرյ��¼�֪ͨ����ʱ����Ȼ�ɶ���
		 * ��Ӧ�ñ�֤�����ȣ�ֱ���Ѳ���ϵͳ�������е����ݶ���
		 * Ϊֹ������ٴ���ر��¼������ر���
		 */
		int   ret;
		acl_non_blocking(ACL_VSTREAM_SOCK(astream->stream),
			ACL_NON_BLOCKING);
		do {
			astream->stream->sys_read_ready = 1;
			ret = __readn_peek(astream);
		} while (astream->keep_read && ret > 0);

		READ_IOCP_CLOSE(astream);
		return;
	} else if ((event_type & ACL_EVENT_RW_TIMEOUT) != 0) {
		/* ������ʱ�����Ӧ�÷���ֵ���ڵ���0����ϣ��������,
		 * �������ֵС��0��ϣ���ر��������˻�������������
		 */
		if (aio_timeout_callback(astream) < 0) {
			READ_IOCP_CLOSE(astream);
		} else if (astream->flag & ACL_AIO_FLAG_IOCP_CLOSE) {
			/* ����������IO�ӳٹر�״̬����Ϊ���ζ�IO�Ѿ��ɹ���ɣ�
			 * ������Ҫ�������IO�ӳٹرչ���
			 */
			READ_IOCP_CLOSE(astream);
		} else {
			READ_SAFE_ENABLE(astream, main_read_callback);
		}
		return;
	}

	if ((event_type & ACL_EVENT_READ) == 0)
		acl_msg_fatal("%s: unknown event: %d", myname, event_type);

	if (astream->stream == NULL)
		acl_msg_fatal("%s: stream null", myname);

	while (1) {
		if (__readn_peek(astream) <= 0 || astream->keep_read == 0)
			break;
	}
}

void acl_aio_readn(ACL_ASTREAM *astream, int count)
{
	const char *myname = "acl_aio_readn";

	if ((astream->flag & ACL_AIO_FLAG_DELAY_CLOSE))
		return;
	if (count <= 0)
		acl_msg_fatal("%s: count(%d) <= 0", myname, count);

	/* ���ûص����� */
	astream->event_read_callback = __readn_notify_callback;
	/* count ��ʾ�û�ϣ�����������ܳ��� */
	astream->count = count;

	ACL_VSTRING_RESET(&astream->strbuf);

	/* ������ص�����ʱ���п����Ǵ�ϵͳ�������ж�ȡ���ݣ�Ҳ�п��ܴ��û�
	 * �����������ݣ����ڳ��������̣����û��ڻص���ȡ���˶����������û�
	 * ��������������ʱ�����޷���ظ�����ϵͳ�����������Զ��ڳ������Ķ�
	 * ���������뱣֤�����ڶ�����״̬
	 */ 
	if (astream->keep_read)
		READ_SAFE_ENABLE(astream, main_read_callback);

	/* ��Ƕ�׼�����1���Է�ֹǶ�ײ��̫���ʹջ��� */
	astream->read_nested++;

	/* ���Ƕ�׵��ô���С�ڷ�ֵ�����������Ƕ�׵��� */
	if (astream->read_nested < astream->read_nested_limit) {
		/* �����Եض����� */
		while (1) {
			if (__readn_peek(astream) <= 0 || astream->keep_read == 0)
				break;
		}
		astream->read_nested--;
		return;
	}

	/* �ݹ�Ƕ�׶������ﵽ�˹涨�ķ�ֵ��ֻ��Ǹ�������Ϣ����Ϊ��Ƕ������ */
	if (acl_msg_verbose)
		acl_msg_warn("%s: read_nested(%d) >= max(%d)", myname,
			astream->read_nested, astream->read_nested_limit);

	/* ���򣬲��������Ƕ�ף������¼������¼����ѭ���У�����Ƕ�ײ�� */

	astream->read_nested--;
	
	/* �������Ķ��¼������¼������ */
	READ_SAFE_ENABLE(astream, main_read_callback);
}

ACL_VSTRING *acl_aio_gets_peek(ACL_ASTREAM *astream)
{
	int   ready = 0;

	if ((astream->flag & ACL_AIO_FLAG_DELAY_CLOSE))
		return NULL;
	if (acl_vstream_gets_peek(astream->stream,
			&astream->strbuf, &ready) == ACL_VSTREAM_EOF
#if ACL_EWOULDBLOCK == ACL_EAGAIN
		&& astream->stream->errnum != ACL_EAGAIN
#endif
		&& astream->stream->errnum != ACL_EWOULDBLOCK)
	{
		astream->flag |= ACL_AIO_FLAG_DEAD;
		if (ACL_VSTRING_LEN(&astream->strbuf) > 0)
			return (&astream->strbuf);
		else
			return NULL;
	} else if (ready)
		return &astream->strbuf;
	else
		return NULL;
}

ACL_VSTRING *acl_aio_gets_nonl_peek(ACL_ASTREAM *astream)
{
	int   ready = 0;

	if ((astream->flag & ACL_AIO_FLAG_DELAY_CLOSE))
		return NULL;
	if (acl_vstream_gets_nonl_peek(astream->stream,
		&astream->strbuf, &ready) == ACL_VSTREAM_EOF
#if ACL_EWOULDBLOCK == ACL_EAGAIN
		&& astream->stream->errnum != ACL_EAGAIN
#endif
		&& astream->stream->errnum != ACL_EWOULDBLOCK)
	{
		astream->flag |= ACL_AIO_FLAG_DEAD;
		if (ACL_VSTRING_LEN(&astream->strbuf) > 0)
			return &astream->strbuf;
		else
			return NULL;
	} else if (ready)
		return &astream->strbuf;
	else
		return NULL;
}

ACL_VSTRING *acl_aio_read_peek(ACL_ASTREAM *astream)
{
	int   n;

	if ((astream->flag & ACL_AIO_FLAG_DELAY_CLOSE))
		return NULL;
	if ((n = acl_vstream_read_peek(astream->stream,
		&astream->strbuf)) == ACL_VSTREAM_EOF
#if ACL_EWOULDBLOCK == ACL_EAGAIN
		&& astream->stream->errnum != ACL_EAGAIN
#endif
		&& astream->stream->errnum != ACL_EWOULDBLOCK)
	{
		astream->flag |= ACL_AIO_FLAG_DEAD;
		if (ACL_VSTRING_LEN(&astream->strbuf) > 0)
			return &astream->strbuf;
		else
			return NULL;
	} else if (n > 0)
		return &astream->strbuf;
	else
		return NULL;
}

ACL_VSTRING *acl_aio_readn_peek(ACL_ASTREAM *astream, int count)
{
	int   ready = 0;

	if ((astream->flag & ACL_AIO_FLAG_DELAY_CLOSE))
		return NULL;
	if (acl_vstream_readn_peek(astream->stream,
		&astream->strbuf, count, &ready) == ACL_VSTREAM_EOF
#if ACL_EWOULDBLOCK == ACL_EAGAIN
		&& astream->stream->errnum != ACL_EAGAIN
#endif
		&& astream->stream->errnum != ACL_EWOULDBLOCK)
	{
		astream->flag |= ACL_AIO_FLAG_DEAD;
		if (ACL_VSTRING_LEN(&astream->strbuf) > 0)
			return &astream->strbuf;
		else
			return NULL;
	} else if (ready)
		return &astream->strbuf;
	else
		return NULL;
}

int acl_aio_can_read(ACL_ASTREAM *astream)
{
	return acl_vstream_can_read(astream->stream);
}

static void can_read_callback(int event_type, ACL_EVENT *event acl_unused,
	ACL_VSTREAM *stream acl_unused, void *context)
{
	ACL_ASTREAM *astream = (ACL_ASTREAM*) context;

	if (astream->keep_read == 0)
		READ_SAFE_DISABLE(astream);

	if ((event_type & ACL_EVENT_XCPT) != 0) {
		READ_IOCP_CLOSE(astream);
		return;
	} else if ((event_type & ACL_EVENT_RW_TIMEOUT) != 0) {
		if (aio_timeout_callback(astream) < 0) {
			READ_IOCP_CLOSE(astream);
		} else if (astream->flag & ACL_AIO_FLAG_IOCP_CLOSE) {
			/* ����������IO�ӳٹر�״̬����Ϊ���ζ�IO�Ѿ��ɹ���ɣ�
			 * ������Ҫ�������IO�ӳٹرչ���
			 */
			READ_IOCP_CLOSE(astream);
		} else {
			READ_SAFE_ENABLE(astream, can_read_callback);
		}
		return;
	}

	astream->nrefer++;
	if (astream->can_read_fn(astream, astream->can_read_ctx) < 0) {
		astream->nrefer--;
		READ_IOCP_CLOSE(astream);
	} else if (astream->flag & ACL_AIO_FLAG_IOCP_CLOSE) {
		astream->nrefer--;
		READ_IOCP_CLOSE(astream);
	} else {
		astream->nrefer--;
		if (astream->keep_read) {
			READ_SAFE_ENABLE(astream, can_read_callback);
		}
	}
}

void acl_aio_enable_read(ACL_ASTREAM *astream,
	ACL_AIO_NOTIFY_FN can_read_fn, void *context)
{
	int   ret;

	if ((astream->flag & ACL_AIO_FLAG_DELAY_CLOSE))
		return;

	astream->can_read_fn = can_read_fn;
	astream->can_read_ctx = context;

	++astream->read_nested;

	if ((ret = acl_vstream_can_read(astream->stream)) == ACL_VSTREAM_EOF) {
		READ_IOCP_CLOSE(astream);
		astream->flag |= ACL_AIO_FLAG_DEAD;
	} else if (ret > 0 && astream->read_nested < astream->read_nested_limit) {
		can_read_callback(ACL_EVENT_READ, astream->aio->event,
			astream->stream , astream);
	} else {
		READ_SAFE_ENABLE(astream, can_read_callback);
	}

	--astream->read_nested;
}

void acl_aio_disable_read(ACL_ASTREAM *astream)
{
	if ((astream->flag & ACL_AIO_FLAG_ISRD) == 0)
		return;
	astream->flag &= ~ACL_AIO_FLAG_ISRD;
	astream->can_read_fn = NULL;
	astream->can_read_ctx = NULL;
	if (astream->stream)
		acl_event_disable_read(astream->aio->event, astream->stream);
}

int acl_aio_isrset(ACL_ASTREAM *astream)
{
	if (astream->stream == NULL)
		return 0;

	return acl_event_isrset(astream->aio->event, astream->stream);
}

void acl_aio_stream_set_line_length(ACL_ASTREAM *astream, int len)
{
	astream->line_length = len;
}

int acl_aio_stream_get_line_length(ACL_ASTREAM *astream)
{
	return astream->line_length;
}
