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

#include "aio.h"

#define	WRITE_SAFE_ENABLE(x, callback) do {  \
	if (((x)->flag & ACL_AIO_FLAG_ISWR) == 0) {  \
		(x)->flag |= ACL_AIO_FLAG_ISWR;  \
		acl_event_enable_write((x)->aio->event, (x)->stream,  \
			(x)->timeout, callback, (x));  \
	}  \
} while (0)

#define WRITE_SAFE_DIABLE(x) do {  \
	if (((x)->flag & ACL_AIO_FLAG_ISWR) != 0) {  \
		(x)->flag &= ~ACL_AIO_FLAG_ISWR;  \
		(x)->can_write_fn = NULL;  \
		(x)->can_write_ctx = NULL;  \
		acl_event_disable_write((x)->aio->event, (x)->stream);  \
	}  \
} while (0)

#define WRITE_IOCP_CLOSE(x) do {  \
	WRITE_SAFE_DIABLE((x));  \
	(x)->flag |= ACL_AIO_FLAG_IOCP_CLOSE;  \
	acl_aio_iocp_close((x));  \
} while (0)

static int write_complete_callback(ACL_ASTREAM *astream)
{
	int   ret = 0;

	/* �����ü�����1���Է�ֹ�����쳣�ر� */
	astream->nrefer++;

	if (astream->write_handles) {
		ACL_ITER iter;
		AIO_WRITE_HOOK *handle;

		/* XXX: ���뽫�����ص�����ӻص�������һһ�������һ������������,
		 * ��Ϊ ACL_AIO �ڻص��������п��ܷ���Ƕ�ף���ֹ���ظ�����
		 */

		while (1) {
			handle = astream->write_handles->pop_back(
				astream->write_handles);
			if (handle == NULL)
				break;
			astream->writer_fifo.push_back(&astream->writer_fifo, handle);
		}

		acl_foreach_reverse(iter, &astream->writer_fifo) {
			handle = (AIO_WRITE_HOOK*) iter.data;
			if (handle->disable)
				continue;

			/* �ص�д�ɹ�ע�ắ�� */
			ret = handle->callback(astream, handle->ctx);
			if (ret != 0) {
				astream->nrefer--;
				return (ret);
			}
		}
	}

	astream->nrefer--;
	return (ret);
}

/* ���Է�����д����������ݣ�����ֵΪд�����ﻹʣ������ݳ��Ȼ�дʧ�� */

static int __try_fflush(ACL_ASTREAM *astream)
{
	const char *myname = "__try_fflush";
	ACL_VSTRING *str;
	const char *ptr;
	int   n, dlen;
	int   i = 0;

	/* ���д���У�Ҳ����� writev ����ã�Ӧ���Ǹ�д��Ч�ʻ����Щ --- zsx */

	while (1) {
		/* ��ȡ��д���е�����ͷ���� */
		str = acl_fifo_head(&astream->write_fifo);
		if (str == NULL) {
			/* ˵��д�����Ѿ�Ϊ�� */
			if (astream->write_left != 0)
				acl_msg_fatal("%s: write_left(%d) != 0",
					myname, astream->write_left);
			return (astream->write_left);
		}

		/* ���㱾���ݿ�ĳ��ȼ����ݿ�ʼλ��, write_offset ���Ǳ����ݿ�����λ��,
		 * ����������ݿ����ʼλ�õ���Գ��ȣ����� dlen ���Ǳ����ݿ����ڱ���д
		 * ������Ҫ��д�����ݵĳ��ȣ��� write_left ����ȫ���Եģ�������д���е�
		 * ���ݳ��ȣ���Ҫ�����������������ֿ�������Ҳ��Ӧ�ý� write_offset Ҳ��
		 * ��Ϊȫ���Եġ�--- zsx :)
		 */

		dlen = ACL_VSTRING_LEN(str) - astream->write_offset;
		ptr = acl_vstring_str(str) + astream->write_offset;
		/* ��ʼ���з�����ʽд���� */
		n = acl_vstream_write(astream->stream, ptr, dlen);
		if (n == ACL_VSTREAM_EOF) {
			if (acl_last_error() != ACL_EAGAIN) {
				astream->flag |= ACL_AIO_FLAG_DEAD;
				return (-1);
			}
			/* ����д����δд���������ݣ�����Ҫ����ʣ�����ݳ��ȼ��� */
			return (astream->write_left);
		}

		/* ���¼���д������ʣ�����ݵ��ܳ��� */
		astream->write_left -= n;

		if (n < dlen) {
			/* δ�ܽ������ݿ�Ŀ�д����ȫ��д�룬������Ҫ���¼�������ݿ��
			 * ��д���ݵ����ƫ��λ��
			 */
			astream->write_offset += n;
			return (astream->write_left);
		}

		/* �������ݿ��д�������޳����ͷŸ����ݿ���ռ���ڴ� */

		str = acl_fifo_pop(&astream->write_fifo);
		acl_vstring_free(str);

		/* ���� write_offset ��Ϊд����һ�����ݿ���׼��, �����ñ���
		 * ��Ϊд���е����ƫ�Ʊ���������Щ. --- zsx
		 */
		astream->write_offset = 0;

		/* �������д���в�����ѭ���������࣬��Ӧ���أ��Ը���������������
		 * �ɶ�д�Ļ���, �����ˣ��ڵ��߳������½��з�����дʱ���ܻ��кܶ�
		 * ����������Ҫ��������֮��������ǰ���ƽ��:)
		 */
		if (++i >= 10) {
			if (acl_msg_verbose)
				acl_msg_warn("%s: write_left=%d, loop=%d",
					myname, astream->write_left, i);
			return (astream->write_left);
		}
	}
}

/* �����������дʱ����������д�¼������� */

static void __writen_notify_callback(int event_type, ACL_EVENT *event acl_unused,
	ACL_VSTREAM *stream acl_unused, void *context)
{
	const char *myname = "__write_notify_callback";
	ACL_ASTREAM *astream = (ACL_ASTREAM *) context;
	int   nleft;

	WRITE_SAFE_DIABLE(astream);

	if ((event_type & ACL_EVENT_XCPT) != 0) {
		/* �������˴�������IO����ӳٹرչر� */
		WRITE_IOCP_CLOSE(astream);
		return;
	} else if ((event_type & ACL_EVENT_RW_TIMEOUT) != 0) {
		/* д������ʱ������û��Ļص��������� -1������IO����ӳٹرչ��� */
		if (aio_timeout_callback(astream) < 0) {
			WRITE_IOCP_CLOSE(astream);
		} else if (astream->flag & ACL_AIO_FLAG_IOCP_CLOSE) {
			/* ����������IO�ӳٹر�״̬����Ϊ����дIO�Ѿ��ɹ���ɣ�
			 * ������Ҫ�������IO�ӳٹرչ���
			 */
			WRITE_IOCP_CLOSE(astream);
		} else {
			/* ˵���û�ϣ�������ȴ�д�¼� */
			WRITE_SAFE_ENABLE(astream, __writen_notify_callback);
		}
		return;
	}

	if ((event_type & ACL_EVENT_WRITE) == 0)
		acl_msg_fatal("%s: unknown event: %d", myname, event_type);

	/* ���Է�������д����������� */
	nleft = __try_fflush(astream);

	if (nleft < 0) {
		/* ����дʧ��������IO����ӳٹ��� */
		WRITE_IOCP_CLOSE(astream);
	} else if (nleft == 0) {
		/* ֮ǰ����д����Ϊ�ջ��Ѿ��ɹ������д��������������� */
		int   ret;

		ret = write_complete_callback(astream);
		if (ret < 0) {
			/* �û�ϣ���رո�����������IO����ӳٹرչ��� */
			WRITE_IOCP_CLOSE(astream);
		} else if (astream->flag & ACL_AIO_FLAG_IOCP_CLOSE) {
			/* ֮ǰ�����Ѿ���������IO����ӳٹرձ�־λ��
			 * ���ٴ�����IO����ӳٹرչ���
			 */
			WRITE_IOCP_CLOSE(astream);
		}
	} else {
		/* ˵��д�����������δ������ϣ���Ҫ�ٴη��ͣ����Խ�д�¼������¼������ */
		WRITE_SAFE_ENABLE(astream, __writen_notify_callback);
	}
}

void acl_aio_writen(ACL_ASTREAM *astream, const char *data, int dlen)
{
	const char *myname = "acl_aio_writen";
	ACL_VSTRING *str;
	int   n;

	if ((astream->flag & (ACL_AIO_FLAG_DELAY_CLOSE | ACL_AIO_FLAG_DEAD)))
		return;

	/* ��Ƕ�׼�����1���Է�ֹǶ�ײ��̫���ʹջ��� */
	astream->write_nested++;

	/* ���Ƕ�׵��ô���С�ڷ�ֵ�����������Ƕ�׵��� */
	if (astream->write_nested < astream->write_nested_limit) {
		/* �ȳ���д����д�����е����� */
		n = __try_fflush(astream);

		if (n < 0) { /* ˵������дʧ�ܣ���Ҫ�ر��� */
			astream->write_nested--;
			WRITE_IOCP_CLOSE(astream);
			return;
		} else if (n == 0) {
			/* __try_fflush ���ص��Ƕ����е������Ѿ���գ�
			 * ���ο�����������һ��д����
			 */
			n = acl_vstream_write(astream->stream, data, dlen);
			if (n == ACL_VSTREAM_EOF) {
				if (acl_last_error() != ACL_EAGAIN) {
					astream->write_nested--;
					WRITE_IOCP_CLOSE(astream);
					astream->flag |= ACL_AIO_FLAG_DEAD;
					return;
				}
				/* ���δд�κ���������ϸ����ĶԵȵ㲢δ�رգ�
				 * �򽫴˴��������������д������
				 */
				n = 0;
			} else if (n == dlen) { /* ˵���Ѿ��ɹ�д����ȫ�������� */
				int  ret;

				ret = write_complete_callback(astream);
				if (ret < 0) {
					/* ������ϣ���ر��� */
					WRITE_IOCP_CLOSE(astream);
				} else if ((astream->flag & ACL_AIO_FLAG_IOCP_CLOSE)) {
					/* ��Ϊ����дIO�Ѿ��ɹ���ɣ�������Ҫ�������IO�ӳٹرչ��� */
					WRITE_IOCP_CLOSE(astream);
				}
				return;
			}
			/* 0 < n < dlen */
			/* ֻ�ǳɹ�д���˲�������n ��ʾ��ʣ������ݳ��� */
		} else {
			/* __try_fflush ��δȫ��д��д���е����е����ݣ�����Ҳ��Ҫ���ε�����
			 * ȫ����������д������
			 */
			n = 0;
		}
	} else {
		/* �ݹ�Ƕ��д�����ﵽ�˹涨�ķ�ֵ��ֻ��Ǹ�������Ϣ���ɣ���Ϊ��Ƕ������ */
		if (acl_msg_verbose)
			acl_msg_warn("%s(%d): write_nested(%d) >= max(%d)",
				myname, __LINE__, astream->write_nested,
				astream->write_nested_limit);
		n = 0;
	}

	/* ���򣬲��������Ƕ�ף����ǽ�д�¼������¼����ѭ���У��Ӷ�����Ƕ�ײ�� */

	astream->write_nested--;

	/* Notice: in acl_vstring_memcpy, vstring_extend should not be called! */

	/* ���������������д������ */

	str = acl_vstring_alloc(dlen - n + 1);
	acl_vstring_memcpy(str, data + n, dlen - n);
	acl_fifo_push(&astream->write_fifo, str);
	astream->write_left += dlen - n;

	/* ��������д�¼������¼������ */
	WRITE_SAFE_ENABLE(astream, __writen_notify_callback);
}

void acl_aio_vfprintf(ACL_ASTREAM *astream, const char *fmt, va_list ap)
{
	const char *myname = "acl_aio_vfprintf";
	ACL_VSTRING *str;
	int   n = 0, len;

	if ((astream->flag & (ACL_AIO_FLAG_DELAY_CLOSE | ACL_AIO_FLAG_DEAD)))
		return;

	str = acl_vstring_alloc(__default_line_length);
	acl_vstring_vsprintf(str, fmt, ap);

	/* ��Ƕ�׼�����1���Է�ֹǶ�ײ��̫���ʹջ��� */
	astream->write_nested++;

	/* ���Ƕ�׵��ô���С�ڷ�ֵ�����������Ƕ�׵��� */
	if (astream->write_nested < astream->write_nested_limit) {
		/* �ȳ���д����д�����е����� */
		n = __try_fflush(astream);
	
		if (n < 0) { /* ˵������дʧ�ܣ���Ҫ�ر��� */
			astream->write_nested--;
			WRITE_IOCP_CLOSE(astream);
			return;
		} else if (n == 0) {
			/* __try_fflush ���ص��Ƕ����е������Ѿ���գ�
			 * ���ο�����������һ��д����
			 */
			const char *ptr;

			ptr = acl_vstring_str(str);
			len = ACL_VSTRING_LEN(str);
			n = acl_vstream_write(astream->stream, ptr, len);
			if (n == ACL_VSTREAM_EOF) {
				if (acl_last_error() != ACL_EAGAIN) {
					astream->flag |= ACL_AIO_FLAG_DEAD;
					astream->write_nested--;
					WRITE_IOCP_CLOSE(astream);
					return;
				}
				/* ���δд�κ���������ϸ����ĶԵȵ㲢δ�رգ�
				 * �򽫴˴��������������д������
				 */
				n = 0;
			} else if (n == len) { /* ˵���Ѿ��ɹ�д����ȫ�������� */
				int   ret;

				ret = write_complete_callback(astream);
				astream->write_nested--;
				acl_vstring_free(str);

				if (ret < 0) /* ������ϣ���ر��� */
					WRITE_IOCP_CLOSE(astream);
				else if ((astream->flag & ACL_AIO_FLAG_IOCP_CLOSE)) {
					/* ��Ϊ����дIO�Ѿ��ɹ���ɣ�������Ҫ�������IO�ӳٹرչ��� */
					WRITE_IOCP_CLOSE(astream);
				}
				return;
			}
			/* 0 < n < dlen */
			/* ֻ�ǳɹ�д���˲�������n ��ʾ��ʣ������ݳ��� */
		} else {
			/* __try_fflush ��δȫ��д��д���е����е����ݣ�����Ҳ��Ҫ���ε�����
			 * ȫ����������д������
			 */
			n = 0;
		}
	} else {
		/* �ݹ�Ƕ��д�����ﵽ�˹涨�ķ�ֵ��ֻ��Ǹ�������Ϣ���ɣ���Ϊ��Ƕ������ */
		if (acl_msg_verbose)
			acl_msg_warn("%s: write_nested(%d) >= max(%d)",
				myname, astream->write_nested, astream->write_nested_limit);
		n = 0;
	}

	/* ���򣬲��������Ƕ�ף����ǽ�д�¼������¼����ѭ���У��Ӷ�����Ƕ�ײ�� */

	astream->write_nested--;

	acl_assert(n >= 0);

	len = ACL_VSTRING_LEN(str);
	if (n < len)
		acl_vstring_memmove(str, acl_vstring_str(str) + n, len - n);

	/* ���������������д������ */
	acl_fifo_push(&astream->write_fifo, str);
	astream->write_left += ACL_VSTRING_LEN(str);

	/* ��������д�¼������¼������ */
	WRITE_SAFE_ENABLE(astream, __writen_notify_callback);
}

void acl_aio_fprintf(ACL_ASTREAM *astream, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	acl_aio_vfprintf(astream, fmt, ap);
	va_end(ap);
}

void acl_aio_writev(ACL_ASTREAM *astream, const struct iovec *vector, int count)
{
	const char *myname = "acl_aio_writev";
	ACL_VSTRING *str;
	int   n, i, j, dlen;

	acl_assert(vector);
	acl_assert(count);

	if ((astream->flag & (ACL_AIO_FLAG_DELAY_CLOSE | ACL_AIO_FLAG_DEAD)))
		return;

	/* ��Ƕ�׼�����1���Է�ֹǶ�ײ��̫���ʹջ��� */
	astream->write_nested++;

	/* ���Ƕ�׵��ô���С�ڷ�ֵ�����������Ƕ�׵��� */
	if (astream->write_nested < astream->write_nested_limit) {
		/* �ȳ���д����д�����е����� */
		n = __try_fflush(astream);
		if (n < 0) { /* ˵������дʧ�ܣ���Ҫ�ر��� */
			astream->write_nested--;
			WRITE_IOCP_CLOSE(astream);
			return;
		} else if (n == 0) {
			/* __try_fflush ���ص��Ƕ����е������Ѿ���գ�
			 * ���ο�����������һ��д����
			 */
			n = acl_vstream_writev(astream->stream, vector, count);
			if (n == ACL_VSTREAM_EOF) {
				if (acl_last_error() != ACL_EAGAIN) {
					astream->flag |= ACL_AIO_FLAG_DEAD;
					astream->write_nested--;
					WRITE_IOCP_CLOSE(astream);
					return;
				}
				/* ���δд�κ���������ϸ����ĶԵȵ㲢δ�رգ�
				 * �򽫴˴��������������д������
				 */
				n = 0;
			}
		} else {
			/* __try_fflush ��δȫ��д��д���е����е����ݣ�����Ҳ��Ҫ���ε�����
			 * ȫ����������д������
			 */
			n = 0;
		}
	} else {
		/* �ݹ�Ƕ��д�����ﵽ�˹涨�ķ�ֵ��ֻ��Ǹ�������Ϣ���ɣ���Ϊ��Ƕ������ */
		if (acl_msg_verbose)
			acl_msg_warn("%s(%d): write_nested(%d) >= max(%d)",
				myname, __LINE__, astream->write_nested, astream->write_nested_limit);
		n = 0;
	}

	/* ����ʣ���δ���͵����ݳ��� */

	for (i = 0; i < count; i++) {
		if (n >= (int) vector[i].iov_len) {
			/* written */
			n -= vector[i].iov_len;
		} else {
			/* partially written */
			break;
		}
	}

	if (i >= count) {
		int   ret;

		acl_assert(n == 0);
		ret = write_complete_callback(astream);
		astream->write_nested--;

		if (ret < 0) {
			/* ������ϣ���ر��� */
			WRITE_IOCP_CLOSE(astream);
		} else if ((astream->flag & ACL_AIO_FLAG_IOCP_CLOSE)) {
			/* ��Ϊ����дIO�Ѿ��ɹ���ɣ�������Ҫ�������IO�ӳٹرչ��� */
			WRITE_IOCP_CLOSE(astream);
		}
		return;
	}

	/* ���򣬲��������Ƕ�ף����ǽ�д�¼������¼����ѭ���У��Ӷ�����Ƕ�ײ�� */

	astream->write_nested--;

	/* ����ʣ��������ܳ��� */

	j = i;
	dlen = vector[i].iov_len - n;
	i++;  /* skipt this */
	for (; i < count; i++) {
		dlen += vector[i].iov_len;
	}

	/* ���������������д������ */

	/* �ȷ���һ���㹻��Ļ����������������е�ʣ������� */
	str = acl_vstring_alloc(dlen + 1);
 
	acl_vstring_memcpy(str, (const char*) vector[j].iov_base + n,
		vector[j].iov_len - n);
	for (i = j + 1; i < count; i++) {
		acl_vstring_memcat(str, vector[i].iov_base, vector[i].iov_len);
	}

	acl_fifo_push(&astream->write_fifo, str);
	astream->write_left += dlen;

	/* ��������д�¼������¼������ */
	WRITE_SAFE_ENABLE(astream, __writen_notify_callback);
}

static void can_write_callback(int event_type, ACL_EVENT *event acl_unused,
	ACL_VSTREAM *stream acl_unused, void *context)
{
	const char *myname = "can_write_callback";
	ACL_ASTREAM *astream = (ACL_ASTREAM*) context;

	WRITE_SAFE_DIABLE(astream);

	if ((event_type & ACL_EVENT_XCPT) != 0) {
		WRITE_IOCP_CLOSE(astream);
		return;
	} else if ((event_type & ACL_EVENT_RW_TIMEOUT) != 0) {
		if (aio_timeout_callback(astream) < 0) {
			WRITE_IOCP_CLOSE(astream);
		} else if (astream->flag & ACL_AIO_FLAG_IOCP_CLOSE) {
			/* ����������IO�ӳٹر�״̬����Ϊ���ζ�IO�Ѿ��ɹ���ɣ�
			* ������Ҫ�������IO�ӳٹرչ���
			*/
			WRITE_IOCP_CLOSE(astream);
		} else {
			WRITE_SAFE_ENABLE(astream, can_write_callback);
		}
		return;
	}

	if (astream->can_write_fn == NULL)
		acl_msg_fatal("%s(%d): can_write_fn null for astream(%lx)",
			myname, __LINE__, (long) astream);

	astream->nrefer++;
	if (astream->can_write_fn(astream, astream->can_write_ctx) < 0) {
		astream->nrefer--;
		WRITE_IOCP_CLOSE(astream);
	} else if (astream->flag & ACL_AIO_FLAG_IOCP_CLOSE) {
		astream->nrefer--;
		WRITE_IOCP_CLOSE(astream);
	} else {
		astream->nrefer--;
	}
}
void acl_aio_enable_write(ACL_ASTREAM *astream,
	ACL_AIO_NOTIFY_FN can_write_fn, void *context)
{
	if ((astream->flag & (ACL_AIO_FLAG_DELAY_CLOSE | ACL_AIO_FLAG_DEAD)))
		return;
        astream->can_write_fn = can_write_fn;
        astream->can_write_ctx = context;
	WRITE_SAFE_ENABLE(astream, can_write_callback);
}

void acl_aio_disable_write(ACL_ASTREAM *astream)
{
	if ((astream->flag & ACL_AIO_FLAG_ISWR) == 0)
		return;
	astream->flag &= ~ACL_AIO_FLAG_ISWR;
        astream->can_write_fn = NULL;
        astream->can_write_ctx = NULL;
	if (astream->stream)
		acl_event_disable_write(astream->aio->event, astream->stream);
}

int acl_aio_iswset(ACL_ASTREAM *astream)
{
	const char *myname = "acl_aio_iswset";

	if (astream == NULL)
		acl_msg_fatal("%s: input invalid", myname);
	if (astream->stream == NULL)
		return (0);

	return (acl_event_iswset(astream->aio->event, astream->stream));
}
