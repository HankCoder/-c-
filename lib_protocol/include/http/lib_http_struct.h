
#ifndef	__LIB_HTTP_STRUCT_INCLUDE_H__
#define	__LIB_HTTP_STRUCT_INCLUDE_H__

#include "lib_acl.h"

#ifdef	__cplusplus
extern "C" {
#endif

#ifdef HTTP_DLL
# ifdef HTTP_EXPORTS
#  define HTTP_API __declspec(dllexport)
# else
#  define HTTP_API __declspec(dllimport)
# endif
#else
#  define HTTP_API
#endif

typedef	acl_int64	http_off_t;

/* �ṹ���Ͷ��� */
typedef struct HTTP_HDR HTTP_HDR;
typedef struct HTTP_HDR_REQ HTTP_HDR_REQ;
typedef struct HTTP_HDR_RES HTTP_HDR_RES;
typedef struct HTTP_REQ HTTP_REQ;
typedef struct HTTP_RES HTTP_RES;
typedef struct HTTP_HDR_ENTRY HTTP_HDR_ENTRY;

/* �������Ͷ��� */

/**
 * ����ͷ�����еĻػص��������Ͷ���
 * @param status {int} HTTP_CHAT_XXX
 *    status:
 *      HTTP_CHAT_OK: ��������������ͷ
 *      HTTP_CHAT_ERR_TOO_MANY_LINES: ����ͷ�е�����̫��
 * @param arg {void*} �ص������Ĳ���
 * @return {int} �ûص������������ֵΪ -1 ���ϼ������߱����; ������ 0 
 *    �ϼ������߼���
 */
typedef int  (*HTTP_HDR_NOTIFY)(int status, void *arg);

/**
 * ��������������еĻص��������Ͷ���
 * @param status {int} HTTP_CHAT_XXX
 *  status:
 *    HTTP_CHAT_OK: �Ѿ��������������壬�� data �������һ��������, dlen ��ʾ
 *      data �����ݳ���
 *    HTTP_CHAT_DATA: ��Ϊ�鴫�䷽ʽʱ����ʾÿ�����ݿ��е��������еĲ������ݣ�
 *      ���ǿ鴫�䷽ʽʱ����ʾ�����������һ��������
 *    HTTP_CHAT_CHUNK_HDR: ��ʾ�鴫�䷽ʽ�е�ĳ�����ݿ��ͷ����
 *    HTTP_CHAT_CHUNK_TRAILER: ��ʾ�鴫�䷽ʽ�е����һ�����ݿ��ͷ����
 *    HTTP_CHAT_CHUNK_DATA_ENDL: ��ʾ�鴫�䷽ʽ��ÿ�����������ķָ�������
 *    HTTP_CHAT_ERR_PROTO: ��ʾЭ�����
 * @param data {char *} �����������ݿ�ʼ��ַ����Զ��Ϊ��
 * @param dlen {int} ��ʾ��ǰ data ���ݳ���
 * @return {int} �ûص������������ֵΪ -1 ���ϼ������߱����; ������ 0 
 *    �ϼ������߼���
 */
typedef int  (*HTTP_BODY_NOTIFY)(int status, char *data, int dlen, void *arg);

/* ͨ�Ź���״̬�ֶ��� */
#define	HTTP_CHAT_OK                    0       /**< �������������� */
#define	HTTP_CHAT_CONTINUE              1       /**< �ڲ��� */
#define	HTTP_CHAT_DATA                  2       /**< �������еĲ������� */
#define	HTTP_CHAT_CHUNK_HDR             3       /**< ������ͷ�е����� */
#define HTTP_CHAT_CHUNK_DATA_ENDL       4       /**< ���������еķָ������� */
#define	HTTP_CHAT_CHUNK_TRAILER         5       /**< ���һ�����ݿ��ͷ�������� */
#define HTTP_CHAT_ERR_MIN               100     /**< ��Ϊ����ֵ����Сֵ */
#define	HTTP_CHAT_ERR_IO                101     /**< IO���� */
#define	HTTP_CHAT_ERR_PROTO             102     /**< �������ݻ���Ӧ���ݵ�Э����� */
#define	HTTP_CHAT_ERR_TOO_MANY_LINES    103     /**< ����ͷ̫���� */
#define HTTP_CHAT_ERR_MAX               1000    /**< ������Χ */

/* ���õı�־λ */
#define	HTTP_CHAT_FLAG_BUFFED           0x0001

/* HTTP Э��ͷ���ֶεĶ��� */
#define	HTTP_HDR_ENTRY_VIA              "via"   /**< HTTP ͷ����ֶΣ���ֹ�ݹ����� */
#define	HTTP_HDR_ENTRY_FORWARD_FOR      "X-Forwarded-For"  /**< HTTP ����ͷ����ֶ� */

/* HTTP Э������ṹ */
struct HTTP_REQ {
	HTTP_HDR_REQ *hdr_req;  /**< �� client ��� */
	int  status;            /**< �Ƿ����, defined above: HTTP_STATUS_ */
	unsigned int flag;      /**< defined as: HTTP_CHAT_FLAG_XXX */
	void *ctx;
	void (*free_ctx)(void*);
};

struct HTTP_RES {
	HTTP_HDR_RES *hdr_res;  /**< �� client ��� */
	int   read_cnt;
	int   status;           /**< �Ƿ����, defined above: HTTP_STATUS_ */
	unsigned int flag;      /**< defined as: HTTP_CHAT_FLAG_XXX */
	void *ctx;
	void (*free_ctx)(void*);
};

/* name-value ��ʽ����Ŀ */
struct HTTP_HDR_ENTRY {
	char *name;
	char *value;
	int   off;
};

/* HTTP Э��ͷ */

struct HTTP_HDR {
	/* ͨ��ʵ�� */
	char  proto[32];        /**< ֧�ֵ�Э��: HTTP */
	struct {
		unsigned int major; /**< ���汾�� */
		unsigned int minor; /**< �ΰ汾�� */
	} version;

	int   keep_alive;       /**< �Ƿ񱣳ֳ����� */
	http_off_t   content_length; /**< HTTPЭ�������ݳ��� */
	int   chunked;          /**
                                 * ���ֶα�����HTTPЭ����Ӧ������,
                                 * Ϊ�˽�������չ, �ʶ����ڴ�
                                 */

	/* �ڲ����� */
	int   cur_lines;
	int   max_lines;
	int   valid_lines;
	int   status;
	int   keep_alive_count; /**< ������� */

	ACL_ARRAY  *entry_lnk;  /**< �洢�� HTTP_HDR_ENTRY ���͵�Ԫ�� */
	void *chat_ctx;
	void (*chat_free_ctx_fn)(void*);

	int   debug;            /**< ������Ϣͷ�ı�־λ */
};

#define HDR_RESTORE(hdr_ptr, hdr_type, hdr_member) \
	((hdr_type *) (((char *) (hdr_ptr)) - offsetof(hdr_type, hdr_member)))

/* HTTP ����ͷ */
struct HTTP_HDR_REQ {
	HTTP_HDR hdr;       /**< ������ͨ�õ�HDRͷ, ����ͨ�÷��� */

	int   port;         /**< ������ķ���˵ķ���˿ں� */
	/* ����ʵ�� */
	char  method[32];   /**< ���󷽷�: POST, GET, CONNECT */
	char  host[512];    /**< ����ʾ��������������IP��ַ */
	ACL_VSTRING *url_part; /**
                                * �洢�������� URL �еĺ�벿��,
                                * ��:
                                * 1) http://test.com.cn/cgi-bin/test?name=value
                                *    => /cgi-bin/test?name=value
                                */
	ACL_VSTRING *url_path;  /**
                                 * �洢�������� URL �е����·����(��������������),
                                 * ����� /path/test.cgi?name=value,
                                 * ���洢 /path/test.cgi, ʣ���
                                 * ������������ url_params �洢.
                                 */
	ACL_VSTRING *url_params; /**< �洢�� URL �еĲ������� */
	ACL_VSTRING *file_path;

	ACL_HTABLE *params_table; /**< �洢�� URL �����еĸ����ֶε����� */
	ACL_HTABLE *cookies_table; /**< �洢�ŵ� cookie �� */
	unsigned int flag;        /**< ��־λ */
#define	HTTP_HDR_REQ_FLAG_PARSE_PARAMS	(1 << 0)
#define	HTTP_HDR_REQ_FLAG_PARSE_COOKIE	(1 << 1)
};

/* HTTP ��Ӧͷ */

struct HTTP_HDR_RES {
	HTTP_HDR hdr;           /**< ������ͨ�õ�HDRͷ, ����ͨ�÷��� */

	/* ��Ӧʵ�� */
	int   reply_status;     /**< ����������Ӧ���룬��: 100, 200, 404, 304, 500 */
};

#ifdef	__cplusplus
}
#endif

#endif

