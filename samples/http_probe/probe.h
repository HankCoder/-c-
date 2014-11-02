#ifndef	__PROBE_INCLUDE_H__
#define	__PROBE_INCLUDE_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include "lib_acl.h"
#include "lib_protocol.h"

/* ����� */
#define	PROBE_HTTP_STAT_MAX	599
#define	PROBE_HTTP_STAT_200	200
#define	PROBE_HTTP_STAT_404	404
#define	PROBE_HTTP_STAT_500	500
#define	PROBE_HTTP_STAT_503	503
#define	PROBE_HTTP_STAT_504	504
#define	PROBE_HTTP_STAT_505	505

#define	PROBE_ERR_NONE		0		/* �޴��� */
#define	PROBE_ERR_CONN		-1		/* ������������ */
#define	PROBE_ERR_HTTP		-2		/* HTTPЭ�鷵�ش��� */
#define	PROBE_ERR_URL		-3		/* �������URL���ش��� */
#define	PROBE_ERR_ARG		-4		/* ����������� */
#define	PROBE_ERR_DSEARCH	-5		/* ����ѯ�����㲻���� */
#define	PROBE_ERR_HSEARCH	-6		/* ����ѯ��������㲻���� */
#define	PROBE_ERR_HDEAD		-7		/* ����ѯ����������Ѿ����� */
#define	PROBE_ERR_SOPEN		-8		/* �������� */
#define	PROBE_ERR_WRITE		-9		/* д���� */
#define	PROBE_ERR_READ		-10		/* ������ */
#define	PROBE_ERR_TOO_MANY_ITEM	-11		/* ͷ���ظ�����̫�� */

#define	HOST_STATUS_ALIVE	0		/* ���������ڴ��״̬ */
#define	HOST_STATUS_DEAD	1		/* �������Ѿ����� */

#define	HOST_FLAG_PROBE_FREE	0		/* ������δ����̽��״̬ */
#define	HOST_FLAG_PROBE_BUSY	1		/* �������Ѿ�����̽��״̬�� */


/* �������� */
#define	HTTP_HEADER_MAX_SIZE		8192
#define	HTTP_HEADER_MAX_NUM		20


typedef struct HTTP_HEADER_ITEM {
	char *name;
	char *value;
} HTTP_HEADER_ITEM;

typedef struct PROBE_STAT {
	time_t time_begin;
	time_t time_end;
	time_t time_cost;
	char http_status[32];
	int    error_num;
} PROBE_STAT;

typedef struct PROBE_SERVER {
	/* come from configure file */
	char *name;
	char *addr;
	char *url;
	int   connect_timeout;
	int   rw_timeout;
	int   retry_inter;
	int   probe_inter;
	char *http_status_errors;
	char *logfile;
	int   warn_time;

	ACL_VSTREAM *logfp;

	char  http_request_header[HTTP_HEADER_MAX_SIZE];
	int   http_request_len;

	time_t time_begin;
	time_t time_end;
	time_t time_total_cost;

	ACL_AIO      *aio;
	ACL_ASTREAM  *stream;
	HTTP_HDR_RES *hdr_res;
	HTTP_RES     *res;
} PROBE_SERVER;


#define	VAR_CFG_SERVER			"probed_server"
#define	VAR_CFG_SERVER_NAME		"server_name"
#define	VAR_CFG_SERVER_ADDR		"addr"
#define	VAR_CFG_SERVER_URL		"url"
#define	VAR_CFG_SERVER_CONNECT_TIMEOUT	"connect_timeout"
#define	VAR_CFG_SERVER_RW_TIMEOUT	"rw_timeout"
#define	VAR_CFG_SERVER_RETRY_INTER	"retry_inter"
#define	VAR_CFG_SERVER_PROBE_INTER	"probe_inter"
#define	VAR_CFG_SERVER_HTTP_ERRORS	"http_errors"
#define	VAR_CFG_SERVER_LOG		"log"
#define	VAR_CFG_SERVER_WARN_TIME	"warn_time"

#define	VAR_CFG_LOGFILE			"logfile"
#define	VAR_CFG_LOGLEVEL		"loglevel"
#define	VAR_CFG_STATLEN			"statlen"
#define	VAR_CFG_FORK_SLEEP		"fork_sleep"
#define	VAR_CFG_DEBUG_FILE		"debug_file"

/* in probe_cfg.c */
extern ACL_ARRAY *var_probe_server_link;
extern const char *var_probe_logfile;
extern int   var_probe_loglevel;
extern int   var_probe_statlen;
extern int   var_probe_fork_sleep;
extern ACL_VSTREAM *var_probe_debug_fp;

extern void probe_cfg_load(void);

/* in probe_run.c */
extern void probe_run(void);

/* in main.c */
extern char *var_cfg_file;

#ifdef	__cplusplus
}
#endif

#endif

