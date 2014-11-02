#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#include "stdlib/acl_stdlib.h"
#include "aio/acl_aio.h"

#endif

#include "aio.h"

int aio_timeout_callback(ACL_ASTREAM *astream)
{
	int   ret = 0;

	/* ��Ҫ����������գ��Է����ظ����� */
	ACL_VSTRING_RESET(&astream->strbuf);

	/* �����ü�����1���Է�ֹ�����쳣�ر� */
	astream->nrefer++;

	if (astream->timeo_handles) {
		ACL_ITER iter;
		ACL_FIFO timeo_handles;

		/* XXX: ���뽫�����ص�����ӻص�������һһ�������һ������������,
		 * ��Ϊ ACL_AIO �ڻص��������п��ܷ���Ƕ�ף���ֹ���ظ�����
		 */

		acl_fifo_init(&timeo_handles);
		acl_foreach_reverse(iter, astream->timeo_handles) {
			AIO_TIMEO_HOOK *handle = (AIO_TIMEO_HOOK*) iter.data;
			if (handle->disable)
				continue;
			acl_fifo_push(&timeo_handles, handle);
		}

		while (1) {
			AIO_TIMEO_HOOK *handle = acl_fifo_pop(&timeo_handles);
			if (handle == NULL)
				break;
			ret = handle->callback(astream, handle->ctx);
			if (ret < 0) {
				astream->nrefer--;
				return (ret);
			}
		}
	}

	astream->nrefer--;
	return (ret);
}

void aio_close_callback(ACL_ASTREAM *astream)
{
	/* ��Ҫ����������գ��Է����ظ����� */
	ACL_VSTRING_RESET(&astream->strbuf);

	/* �����ü�����1���Է�ֹ�����쳣�ر� */
	astream->nrefer++;

	if (astream->close_handles) {
		ACL_ITER iter;
		ACL_FIFO close_handles;

		/* XXX: ���뽫�����ص�����ӻص�������һһ�������һ������������,
		 * ��Ϊ ACL_AIO �ڻص��������п��ܷ���Ƕ�ף���ֹ���ظ�����
		 */

		acl_fifo_init(&close_handles);
		acl_foreach_reverse(iter, astream->close_handles) {
			AIO_CLOSE_HOOK *handle = (AIO_CLOSE_HOOK*) iter.data;
			if (handle->disable)
				continue;
			acl_fifo_push(&close_handles, handle);
		}

		while (1) {
			AIO_CLOSE_HOOK *handle = acl_fifo_pop(&close_handles);
			void *ctx;

			if (handle == NULL)
				break;
			/* xxx: �رջص����ܱ�����һ�� */
			ctx = handle->ctx;
			handle->disable = 1;
			handle->ctx = NULL;
			if (handle->callback(astream, ctx) < 0) {
				astream->nrefer--;
				return;
			}
		}
	}

	astream->nrefer--;
}
