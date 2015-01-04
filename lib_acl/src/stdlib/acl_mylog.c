#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE
#include "stdlib/acl_define.h"
#include "stdlib/acl_mymalloc.h"

/* #include <time.h> */
#include <stdio.h>
#include <sys/stat.h>

#ifdef	WIN32
# include <process.h>
# ifdef	WIN32
/* #include <io.h> */
#  include <fcntl.h>
#  include <sys/stat.h>
/* #include <sys/types.h> */
# endif
#elif	defined(ACL_UNIX)
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

#else
# error "unknown OS type"
#endif

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#include "stdlib/acl_sys_patch.h"
#include "stdlib/acl_iostuff.h"
#include "stdlib/acl_iterator.h"
#include "stdlib/acl_msg.h"
#include "stdlib/acl_argv.h"
#include "net/acl_sane_inet.h"
#include "stdlib/acl_mylog.h"

#endif

#include "../private/private_fifo.h"
#include "../private/private_vstream.h"
#include "../private/thread.h"

struct ACL_LOG {
	ACL_VSTREAM *fp;
	char *path;
	int   flag;
#define ACL_LOG_F_DEAD		(1 << 0)
#define	ACL_LOG_F_FIXED		(1 << 1)

	int   type;
#define ACL_LOG_T_UNKNOWN       0
#define ACL_LOG_T_FILE          1
#define ACL_LOG_T_TCP           2
#define ACL_LOG_T_UDP           3
#define ACL_LOG_T_UNIX          4

#define	IS_NET_STREAM(x)	((x)->type == ACL_LOG_T_TCP || (x)->type == ACL_LOG_T_UNIX)

	char  logpre[256];
	acl_pthread_mutex_t *lock;
	struct sockaddr_in from;
	struct sockaddr_in dest;
	int   from_len;
	acl_uint64   count;		/**< �Ѿ���¼����־���� */
	time_t last_open;		/**< �ϴ���־��ʱ�� */
	time_t reopen_inter;		/**< ��־���´򿪵���Сʱ���� */
};

#ifdef WIN32
# define strdup _strdup
#endif

static int  __log_thread_id = 0;
static ACL_FIFO *__loggers = NULL;

void acl_log_add_tid(int onoff)
{
	__log_thread_id = onoff ? 1 : 0;
}

void acl_log_fp_set(ACL_VSTREAM *fp, const char *logpre)
{
	const char *myname = "acl_log_fp_set";
	ACL_LOG *log;
	ACL_ITER iter;

	acl_assert(fp);

	if (__loggers == NULL)
		__loggers = private_fifo_new();

	acl_foreach(iter, __loggers) {
		log = (ACL_LOG*) iter.data;
		if (strcmp(log->path, ACL_VSTREAM_PATH(fp)) == 0) {
			acl_msg_warn("%s(%d): log %s has been opened.",
				myname, __LINE__, log->path);
			return;
		}
	}

#ifdef	ACL_UNIX
	acl_close_on_exec(ACL_VSTREAM_SOCK(fp), ACL_CLOSE_ON_EXEC);
#endif
	log = (ACL_LOG*) calloc(1, sizeof(ACL_LOG));
	log->fp = fp;
	log->path = strdup(ACL_VSTREAM_PATH(fp));
	log->type = ACL_LOG_T_UNKNOWN;
	log->lock = (acl_pthread_mutex_t*) calloc(1, sizeof(acl_pthread_mutex_t));
	thread_mutex_init(log->lock, NULL);
	if (logpre && *logpre)
		snprintf(log->logpre, sizeof(log->logpre), "%s", logpre);
	else
		log->logpre[0] = 0;
	log->flag |= ACL_LOG_F_FIXED;
	private_fifo_push(__loggers, log);
}

static int open_file_log(const char *filename, const char *logpre)
{
	const char *myname = "open_file_log";
#ifdef	WIN32
	int   flag = O_RDWR | O_CREAT | O_APPEND | O_BINARY;
#else
	int   flag = O_RDWR | O_CREAT | O_APPEND;
#endif
	int   mode = S_IREAD | S_IWRITE;
	ACL_LOG *log;
	ACL_ITER iter;
	ACL_VSTREAM *fp;
	ACL_FILE_HANDLE fh;

	acl_foreach(iter, __loggers) {
		log = (ACL_LOG*) iter.data;
		if (strcmp(log->path, filename) == 0) {
			acl_msg_warn("%s(%d): log %s has been opened.",
				myname, __LINE__, filename);
			return (0);
		}
	}

	fh = acl_file_open(filename, flag, mode);
	if (fh == ACL_FILE_INVALID) {
		printf("%s(%d): open %s error(%s)", myname, __LINE__,
			filename, acl_last_serror());
		return (-1);
	}

	fp = private_vstream_fhopen(fh, O_RDWR);
	acl_vstream_set_path(fp, filename);

#ifdef	ACL_UNIX
	acl_close_on_exec(fh, ACL_CLOSE_ON_EXEC);
#endif

	log = (ACL_LOG*) calloc(1, sizeof(ACL_LOG));
	log->last_open = time(NULL);
	log->fp = fp;
	log->path = strdup(filename);
	log->type = ACL_LOG_T_FILE;
	log->lock = (acl_pthread_mutex_t*) calloc(1, sizeof(acl_pthread_mutex_t));
	thread_mutex_init(log->lock, NULL);
	if (logpre && *logpre)
		snprintf(log->logpre, sizeof(log->logpre), "%s", logpre);
	else
		log->logpre[0] = 0;

	private_fifo_push(__loggers, log);
	return (0);
}

static int reopen_log(ACL_LOG *log)
{
	time_t now = time(NULL);

#undef	RETURN
#define	RETURN(x) do {  \
	if (log->lock)  \
		thread_mutex_unlock(log->lock);  \
	return (x);  \
} while (0)

	if (log->lock)
		thread_mutex_lock(log->lock);

	if (!(log->flag & ACL_LOG_F_DEAD)
		|| (log->flag & ACL_LOG_F_FIXED)
		|| !IS_NET_STREAM(log)
		|| log->fp == NULL
		|| log->reopen_inter <= 0)
	{
		RETURN (-1);
	}

	if (log->count == 0) {
		if (now - log->last_open < 5 * log->reopen_inter)
			RETURN (-1);
	} else if (now - log->last_open < log->reopen_inter)
		RETURN (-1);

	if (log->fp->path) {
		acl_myfree(log->fp->path);
		log->fp->path = NULL;
	}
	if (log->fp->addr_local) {
		acl_myfree(log->fp->addr_local);
		log->fp->addr_local = NULL;
	}
	if (log->fp->addr_peer) {
		acl_myfree(log->fp->addr_peer);
		log->fp->addr_peer = NULL;
	}

	private_vstream_close(log->fp);
	acl_assert(log->path);
	log->fp = private_vstream_connect(log->path, 60, 60);
	log->last_open = time(NULL);
	if (log->fp == NULL)
		RETURN (-1);
	log->flag &= ~ACL_LOG_F_DEAD;
	log->last_open = time(NULL);
	RETURN (0);
}

static int open_stream_log(const char *addr, const char *logpre, int type)
{
	const char *myname = "open_stream_log";
	ACL_VSTREAM *fp;
	ACL_LOG *log;
	ACL_ITER iter;

	acl_foreach(iter, __loggers) {
		log = (ACL_LOG*) iter.data;
		if (strcmp(log->path, addr) == 0 && log->type == type) {
			acl_msg_warn("%s(%d): log(%s) has been opened!",
				myname, __LINE__, addr);
			return (0);
		}
	}

	fp = private_vstream_connect(addr, 60, 60);
	if (fp == NULL) {
		printf("%s(%d): connect %s error(%s)\n",
			myname, __LINE__, addr, acl_last_serror());
		return (-1);
	}

	log = (ACL_LOG*) calloc(1, sizeof(ACL_LOG));
	log->last_open = time(NULL);
	log->reopen_inter = 60;
	log->fp = fp;
	log->path = strdup(addr);
	log->lock = (acl_pthread_mutex_t*) calloc(1, sizeof(acl_pthread_mutex_t));
	thread_mutex_init(log->lock, NULL);
	log->type = type;
	if (logpre && *logpre)
		snprintf(log->logpre, sizeof(log->logpre), "%s", logpre);
	else
		log->logpre[0] = 0;

	private_fifo_push(__loggers, log);
	return (0);
}

static int open_tcp_log(const char *addr, const char *logpre)
{
	return (open_stream_log(addr, logpre, ACL_LOG_T_TCP));
}

static int open_unix_log(const char *addr, const char *logpre)
{
	return (open_stream_log(addr, logpre, ACL_LOG_T_UNIX));
}

static int udp_read(ACL_SOCKET fd, void *buf, size_t size,
	int timeout acl_unused, ACL_VSTREAM *fp acl_unused, void *arg)
{
	ACL_LOG *log = (ACL_LOG*) arg;
	int   ret;

#ifdef ACL_UNIX
	ret = recvfrom(fd, buf, size, 0, (struct sockaddr*) &log->from,
		(socklen_t*) &log->from_len);
#elif defined(WIN32)
	ret = recvfrom(fd, (char*) buf, (int) size, 0,
		(struct sockaddr*) &log->from, &log->from_len);
#else
#error "unknown OS"
#endif
	return (ret);
}

static int udp_write(ACL_SOCKET fd, const void *buf, size_t size,
	int timeout acl_unused, ACL_VSTREAM *fp acl_unused, void *arg)
{
	ACL_LOG *log = (ACL_LOG*) arg;
	int   ret;

#ifdef ACL_UNIX
	ret = sendto(fd, buf, size, 0, (struct sockaddr*) &log->dest,
		sizeof(log->dest));
#elif defined(WIN32)
	ret = sendto(fd, (const char*) buf, (int) size, 0,
		(struct sockaddr*) &log->dest, sizeof(log->dest));
#else
#error	"unknown OS"
#endif
	return (ret);
}

static int open_udp_log(const char *addr, const char *logpre)
{
	const char *myname = "open_udp_log";
	ACL_LOG *log;
	ACL_ITER iter;
	ACL_SOCKET fd;
	char  ip[64], *ptr;
	int   port;

	snprintf(ip, sizeof(ip), "%s", addr);
	ptr = strchr(ip, ':');
	acl_assert(ptr);
	*ptr++ = 0;
	port = atoi(ptr);
	acl_assert(port > 0);

	acl_foreach(iter, __loggers) {
		log = (ACL_LOG*) iter.data;
		if (strcmp(log->path, addr) == 0 && log->type == ACL_LOG_T_UDP) {
			acl_msg_warn("%s(%d): log(%s) has been opened!",
				myname, __LINE__, addr);
			return (0);
		}
	}

	log = (ACL_LOG*) calloc(1, sizeof(ACL_LOG));

	fd = socket(AF_INET, SOCK_DGRAM, 0);
	acl_assert(fd != ACL_SOCKET_INVALID);
	log->fp = private_vstream_fdopen(fd, O_RDWR, 1024, 0, ACL_VSTREAM_TYPE_SOCK);
	private_vstream_ctl(log->fp,
			ACL_VSTREAM_CTL_READ_FN, udp_read,
			ACL_VSTREAM_CTL_WRITE_FN, udp_write,
			ACL_VSTREAM_CTL_CONTEXT, log,
			ACL_VSTREAM_CTL_END);
	log->last_open = time(NULL);
	log->dest.sin_family = AF_INET;
	log->dest.sin_addr.s_addr = inet_addr(ip);
	log->dest.sin_port = htons(port);

	log->from_len = sizeof(log->from);

	log->path = strdup(addr);
	log->lock = (acl_pthread_mutex_t*) calloc(1, sizeof(acl_pthread_mutex_t));
	thread_mutex_init(log->lock, NULL);
	log->type = ACL_LOG_T_UDP;
	if (logpre && *logpre)
		snprintf(log->logpre, sizeof(log->logpre), "%s", logpre);
	else
		log->logpre[0] = 0;

	private_fifo_push(__loggers, log);
	return (0);
}

/*
 * recipient:
 *  tcp:127.0.0.1:8088
 *  udp:127.0.0.1:8088
 *  unix:/var/log/unix.sock
 *  file:/var/log/unix.log
 *  /var/log/unix.log
 */
static int open_log(const char *recipient, const char *logpre)
{
	const char *myname = "open_log";
	const char *ptr;

	if (strncasecmp(recipient, "tcp:", 4) == 0) {
		ptr = recipient + 4;
		if (acl_ipv4_addr_valid(ptr) == 0) {
			printf("%s(%d): recipient(%s) invalid",
				myname, __LINE__, recipient);
			return (-1);
		}
		return (open_tcp_log(ptr, logpre));
	} else if (strncasecmp(recipient, "udp:", 4) == 0) {
		ptr = recipient + 4;
		if (acl_ipv4_addr_valid(ptr) == 0) {
			printf("%s(%d): recipient(%s) invalid",
				myname, __LINE__, recipient);
			return (-1);
		}
		return (open_udp_log(ptr, logpre));
	} else if (strncasecmp(recipient, "unix:", 5) == 0) {
		ptr = recipient + 5;
		if (*ptr == 0) {
			printf("%s(%d): recipient(%s) invalid",
				myname, __LINE__, recipient);
			return (-1);
		}
		return (open_unix_log(ptr, logpre));
	} else if (strncasecmp(recipient, "file:", 5) == 0) {
		ptr = recipient + 5;
		if (*ptr == 0) {
			printf("%s(%d): recipient(%s) invalid",
				myname, __LINE__, recipient);
			return (-1);
		}
		return (open_file_log(recipient, logpre));
	} else {
		return (open_file_log(recipient, logpre));
	}
}

/*
 * recipients ������������־��ʽ�����:
 *  tcp:127.0.0.1:8088
 *  udp:127.0.0.1:8088
 *  unix:/var/log/unix.sock
 *  file:/var/log/unix.log
 *  /var/log/unix.log
 * �磺tcp:127.0.0.1:8088|/var/log/unix.log
 */
int acl_open_log(const char *recipients, const char *logpre)
{
	const char *myname = "acl_open_log";
	const char *ptr;
	ACL_ARGV *argv = NULL;
	ACL_ITER  iter;

	if (recipients == NULL || *recipients == 0) {
		printf("%s(%d): recipients null\n", myname, __LINE__);
		return (-1);
	} else if (logpre == NULL || *logpre == 0) {
		printf("%s(%d): logpre null\n", myname, __LINE__);
		return (-1);
	}

	if (__loggers == NULL)
		__loggers = private_fifo_new();

	argv = acl_argv_split(recipients, "|");
	acl_foreach(iter, argv) {
		ptr = (const char*) iter.data;
		if (open_log(ptr, logpre) < 0) {
			acl_argv_free(argv);
			return (-1);
		}
	}
	acl_argv_free(argv);
	return (0);
}

void acl_logtime_fmt(char *buf, size_t size)
{
	time_t	now;
#ifdef	ACL_UNIX
	struct tm local_time;

	(void) time (&now);
	(void) localtime_r(&now, &local_time);
	strftime(buf, size, "%Y/%m/%d %H:%M:%S", &local_time);
#elif	defined(WIN32)
	struct tm *local_time;

	(void) time (&now);
	local_time = localtime(&now);
	strftime(buf, size, "%Y/%m/%d %H:%M:%S", local_time);
#else
# error "unknown OS type"
#endif
}

#ifdef	ACL_UNIX

static char *get_buf(const char *pre, const char *fmt, va_list ap, size_t *len)
{
	va_list ap_tmp;
	char *buf, *ptr;
	size_t n1, n2, n;
	int   ret;

	n1 = strlen(pre);

	n = n1 + 1024;
	buf = (char*) malloc(n);
	acl_assert(buf);

	strcpy(buf, pre);
	ptr = buf + n1;
	n2 = n - n1;
	va_copy(ap_tmp, ap);
	ret = vsnprintf(ptr, n2, fmt, ap_tmp);
	acl_assert(ret > 0);

	*len = n1 + ret;

	if (ret >= (int) n2) {
		n = n1 + ret + 1;
		buf = (char*) realloc(buf, n);
		acl_assert(buf);

		ptr = buf + n1;
		n2 = n - n1;
		va_copy(ap_tmp, ap);
		ret = vsnprintf(ptr, n2, fmt, ap_tmp);
		acl_assert(ret > 0 && ret < (int) n2);
	}

	return (buf);
}

#else

static char *get_buf(const char *pre, const char *fmt, va_list ap, size_t *len)
{
	char *buf, *ptr;
	size_t n1, n2, n;
	int   ret, i;

	n1 = strlen(pre);

	n = n1 + 1024;
	buf = (char*) malloc(n);
	acl_assert(buf);

	strcpy(buf, pre);
	ptr = buf + n1;
	n2 = n - n1;
	ret = vsnprintf(ptr, n2, fmt, ap);
	if (ret > 0 && ret < (int) n2) {
		*len = n1 + ret;
		return (buf);
	}

	i = 0;
	while (1) {
		n += 1024;
		buf = (char*) realloc(buf, n);
		acl_assert(buf);
		ptr = buf + n1;
		n2 = n - n1;
		ret = vsnprintf(ptr, n2, fmt, ap);
		if (ret > 0 && ret < (int) n2) {
			*len = n1 + ret;
			break;
		}
		if (++i >= 10000)
			acl_assert(0);
	}

	return (buf);
}

#endif

static void file_vsyslog(ACL_LOG *log, const char *info, const char *fmt, va_list ap)
{
	char  fmtstr[128], tbuf[1024], *buf;
	size_t len;

	acl_logtime_fmt(fmtstr, sizeof(fmtstr));

	if (__log_thread_id) {
#ifdef LINUX2
		snprintf(tbuf, sizeof(tbuf), "%s %s (pid=%d, tid=%llu)(%s): ",
			fmtstr, log->logpre, (int) getpid(),
			(unsigned long long int) pthread_self(), info);
#elif defined(SUNOS5)
		snprintf(tbuf, sizeof(tbuf), "%s %s (pid=%d, tid=%d)(%s): ",
			fmtstr, log->logpre, (int) getpid(), (int) pthread_self(), info);
#elif defined(WIN32)
		snprintf(tbuf, sizeof(tbuf), "%s %s (pid=%d, tid=%d)(%s): ",
			fmtstr, log->logpre, (int) _getpid(), (int) acl_pthread_self(), info);
#else
		snprintf(tbuf, sizeof(tbuf), "%s %s: (%s): ",
			fmtstr, log->logpre, info);
#endif
	} else {
#if defined(SUNOS5)
		snprintf(tbuf, sizeof(tbuf), "%s %s (pid=%d)(%s): ",
			fmtstr, log->logpre, (int) getpid(), info);
#elif defined(WIN32)
		snprintf(tbuf, sizeof(tbuf), "%s %s (pid=%d)(%s): ",
			fmtstr, log->logpre, (int) _getpid(), info);
#else
		snprintf(tbuf, sizeof(tbuf), "%s %s (pid=%d)(%s): ",
			fmtstr, log->logpre, (int) getpid(), info);
#endif
	}

	buf = get_buf(tbuf, fmt, ap, &len);

	if (log->lock)
		thread_mutex_lock(log->lock);

	if (private_vstream_writen(log->fp, buf, len) == ACL_VSTREAM_EOF
		|| private_vstream_writen(log->fp, "\r\n", 2) == ACL_VSTREAM_EOF)
	{
		log->flag |= ACL_LOG_F_DEAD;
	} else
		log->count++;

	if (log->lock)
		thread_mutex_unlock(log->lock);

	free(buf);
}

static void net_vsyslog(ACL_LOG *log, const char *info, const char *fmt, va_list ap)
{
	char  tbuf[1024], *buf;
	size_t len;

	if (__log_thread_id) {
#ifdef LINUX2
		snprintf(tbuf, sizeof(tbuf), " %s (pid=%d, tid=%llu)(%s): ",
			log->logpre, (int) getpid(),
			(unsigned long long int) pthread_self(), info);
#elif defined(SUNOS5)
		snprintf(tbuf, sizeof(tbuf), " %s (pid=%d, tid=%d)(%s): ",
			log->logpre, (int) getpid(), (int) pthread_self(), info);
#elif defined(WIN32)
		snprintf(tbuf, sizeof(tbuf), " %s (pid=%d, tid=%d)(%s): ",
			log->logpre, (int) _getpid(), (int) acl_pthread_self(), info);
#else
		snprintf(tbuf, sizeof(tbuf), " %s: (%s): ", log->logpre, info);
#endif
	} else {
#if defined(SUNOS5)
		snprintf(tbuf, sizeof(tbuf), " %s (pid=%d)(%s): ",
			log->logpre, (int) getpid(), info);
#elif defined(WIN32)
		snprintf(tbuf, sizeof(tbuf), " %s (pid=%d)(%s): ",
			log->logpre, (int) _getpid(), info);
#else
		snprintf(tbuf, sizeof(tbuf), " %s (pid=%d)(%s): ",
			log->logpre, (int) getpid(), info);
#endif
	}

	buf = get_buf(tbuf, fmt, ap, &len);

	if (log->lock)
		thread_mutex_lock(log->lock);

	if (log->type == ACL_LOG_T_UDP) {
		(void) private_vstream_write(log->fp, buf, len);
		log->count++;
	} else if (private_vstream_buffed_writen(log->fp, buf, len) == ACL_VSTREAM_EOF
		|| private_vstream_writen(log->fp, "\r\n", 2) == ACL_VSTREAM_EOF)
	{
		log->flag |= ACL_LOG_F_DEAD;
	} else
		log->count++;

	if (log->lock)
		thread_mutex_unlock(log->lock);

	free(buf);
}

int acl_write_to_log2(const char *info, const char *fmt, va_list ap)
{
	ACL_ITER iter;
	ACL_LOG *log;
#ifdef ACL_UNIX
	va_list tmp;
#endif

	if (__loggers == NULL)
		return (0);

#ifdef ACL_UNIX
	acl_foreach(iter, __loggers) {
		log = (ACL_LOG*) iter.data;
		if ((log->flag & ACL_LOG_F_DEAD)) {
			if (reopen_log(log) < 0)
				continue;
		}
		va_copy(tmp, ap);
		if (log->type == ACL_LOG_T_FILE)
			file_vsyslog(log, info, fmt, tmp);
		else if (log->type == ACL_LOG_T_TCP
			|| log->type == ACL_LOG_T_UDP
			|| log->type == ACL_LOG_T_UNIX)
		{
			net_vsyslog(log, info, fmt, tmp);
		}
	}
#else
	acl_foreach(iter, __loggers) {
		log = (ACL_LOG*) iter.data;
		if ((log->flag & ACL_LOG_F_DEAD)) {
			if (reopen_log(log) < 0)
				continue;
		}
		if (log->type == ACL_LOG_T_FILE)
			file_vsyslog(log, info, fmt, ap);
		else if (log->type == ACL_LOG_T_TCP
			|| log->type == ACL_LOG_T_UDP
			|| log->type == ACL_LOG_T_UNIX)
		{
			net_vsyslog(log, info, fmt, ap);
		}
	}
#endif

	return (0);
}

int acl_write_to_log(const char *fmt, ...)
{
	va_list	ap;
	int   ret;

	va_start (ap, fmt);
	ret = acl_write_to_log2("-", fmt, ap);
	va_end(ap);
	return (ret);
}

void acl_close_log()
{
	ACL_LOG *log;

	if (__loggers == NULL)
		return;

	while (1) {
		log = (ACL_LOG*) private_fifo_pop(__loggers);
		if (log == NULL)
			break;

		if ((log->flag & ACL_LOG_F_FIXED))
			continue;

		if (log->fp) {
			if (log->fp->path) {
				acl_myfree(log->fp->path);
				log->fp->path = NULL;
			}
			if (log->fp->addr_local) {
				acl_myfree(log->fp->addr_local);
				log->fp->addr_local = NULL;
			}
			if (log->fp->addr_peer) {
				acl_myfree(log->fp->addr_peer);
				log->fp->addr_peer = NULL;
			}
			private_vstream_close(log->fp);
		}
		if (log->path)
			free(log->path);
		if (log->lock)
			thread_mutex_destroy(log->lock);
		free(log);
	}

	free(__loggers);
	__loggers = NULL;
}
