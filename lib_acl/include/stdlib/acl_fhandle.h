#ifndef	ACL_FHANDLE_INCLUDE_H
#define	ACL_FHANDLE_INCLUDE_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "acl_define.h"
#include "acl_vstream.h"
#include "acl_vstring.h"
#include "acl_ring.h"
#include "thread/acl_thread.h"
#include <time.h>

/**
 * ͨ�õĴ洢�ļ�����������Ͷ���
 */

typedef struct ACL_FHANDLE	ACL_FHANDLE;

struct ACL_FHANDLE {
	ACL_VSTREAM *fp;			/**< �洢�ļ���� */
	acl_int64 fsize;			/**< �洢�ļ���С */
	int   nrefer;				/**< �ô洢��������õļ���ֵ */
	acl_pthread_mutex_t mutex;		/**< �߳��� */
#ifdef WIN32
	unsigned long tid;			/**< �򿪸ô洢���̺߳� */
	unsigned long lock_mutex_tid;		/**< ���߳������̺߳� */
#else
	acl_pthread_t tid;			/**< �򿪸ô洢���̺߳� */
	acl_pthread_t lock_mutex_tid;		/**< ���߳������̺߳� */
#endif
	unsigned int oflags;			/**< ��ʱ�ı�־λ */
#define	ACL_FHANDLE_O_FLOCK	(1 << 0)	/**< ʹ���ļ��� */
#define	ACL_FHANDLE_O_MLOCK	(1 << 1)	/**< ʹ���߳��� */
#define	ACL_FHANDLE_O_MKDIR	(1 << 2)	/**< �Ƿ��Զ���鲢���������ڵ�Ŀ¼ */
#define	ACL_FHANDLE_O_NOATIME	(1 << 3)	/**< ���ļ�ʱ��� O_NOATIME ��־λ */
#define	ACL_FHANDLE_O_DIRECT	(1 << 4)	/**< ���ļ�ʱ��� O_DIRECT ��־λ */
#define	ACL_FHANDLE_O_SYNC	(1 << 5)	/**< ���ļ�ʱ��� O_SYNC ��־λ */
#define	ACL_FHANDLE_O_EXCL	(1 << 6)	/**< ���ļ�ʱ�Ƿ����Զ����� */

	unsigned int status;			/**< �ô洢�ļ������״̬ */
#define	ACL_FHANDLE_S_FLOCK_ON	(1 << 0)	/**< �ô洢����Ѿ����ļ��� */
#define	ACL_FHANDLE_S_MUTEX_ON	(1 << 1)	/**< �ô洢����Ѿ����߳��� */

	time_t  when_free;			/**< ���ӳٹرջ�������д���ʱ��� */
	ACL_RING ring;				/**< �������ݽ�� */
	size_t size;				/**< �� ACL_FHANDLE �����ʵ�ʴ�С >= sizeof(ACL_FHANDLE) */
	void (*on_close)(ACL_FHANDLE*);		/**< �����ļ������������ر�ʱ�Ļص�����������Ϊ�� */
};

#define	ACL_FHANDLE_PATH(x)	(ACL_VSTREAM_PATH((x)->fp))

/**
 * ��ʼ���ļ�����������ú������ڳ������г�ʼ��ʱ��������ֻ�ܱ�����һ��
 * @param cache_size {int} �ڲ������ļ������������
 * @param debug_section {int} ���Լ���
 * @param flags {unsigned int}
 */
void acl_fhandle_init(int cache_size, int debug_section, unsigned int flags);
#define	ACL_FHANDLE_F_LOCK	(1 << 0)

/**
 * �������˳�ʱ��Ҫ���ô˺������ͷ�ϵͳ��Դ
 */
void acl_fhandle_end(void);

/**
 * ��һ���ļ�
 * @param size {size_t} ����ṹ FS_HANDDLE ��Ҫ�Ŀռ��С
 * @param oflags {unsigned int} ���ļ����ʱ�ı�־λ, ACL_FHANDLE_O_XXX
 * @param file_path {const char*} �ļ���(����·��)
 * @param on_open {int (*)(ACL_FHANDLE*, void*)} �����Ϊ�գ�
 *  ���ļ�������ɹ��򿪺����ô˺���
 * @param open_arg {void *} on_open �Ļص�����֮һ
 * @param on_close {void (*)(ACL_FHANDLE*)} �����Ϊ�գ�
 *  ���ļ��������ֱ�ر�ʱ����ô˺���
 */
ACL_FHANDLE *acl_fhandle_open(size_t size, unsigned int oflags,
	const char *file_path,
	int (*on_open)(ACL_FHANDLE*, void*), void *open_arg,
	void (*on_close)(ACL_FHANDLE*));

/**
 * �ر�һ���ļ����
 * @param fs {ACL_FHANDLE*}
 * @param delay_timeout {int} ��� > 0, ���ӳٸ�ʱ���������ر�,
 *  ���������ü���Ϊ 0 �������ر�
 */
void acl_fhandle_close(ACL_FHANDLE *fs, int delay_timeout);

/**
 * ��һ���ļ��������(�ȼ��߳�������ļ���)
 * @param fs {ACL_FHANDLE*}
 */
void acl_fhandle_lock(ACL_FHANDLE *fs);

/**
 * ��һ���ļ��������(�Ƚ��ļ����ٽ��߳���)
 * @param fs {ACL_FHANDLE*}
 */
void acl_fhandle_unlock(ACL_FHANDLE *fs);

#ifdef	__cplusplus
}
#endif

#endif
