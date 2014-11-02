#ifndef __MIME_STATE_INCLUDE__
#define __MIME_STATE_INCLUDE__

#include "lib_acl.h"
#include "acl_cpp/mime/mime_define.hpp"
#include "header_token.hpp"

typedef struct MIME_NODE MIME_NODE;
typedef struct MIME_STATE MIME_STATE;
typedef struct MAIL_ADDR MAIL_ADDR;

struct MAIL_ADDR 
{
	char *addr;
	char *comment;
};

struct MIME_NODE
{
	ACL_RING children;                      /**< �ӽ�㼯�� */
	int  depth;                             /**< ��ǰ������� */
	MIME_NODE *parent;                      /**< ����� */
	MIME_STATE *state;                      /**< MIME_STATE ���� */

	/* �ʼ�ͷ */
	ACL_FIFO *header_list;                  /**< HEADER_NV ���� */
	ACL_FIFO *header_to_list;               /**< MAIL_ADDR ���� */
	ACL_FIFO *header_cc_list;               /**< MAIL_ADDR ���� */
	ACL_FIFO *header_bcc_list;              /**< MAIL_ADDR ���� */
	char *header_sender;
	char *header_from;
	char *header_replyto;
	char *header_returnpath;
	char *header_subject;

	/* multipart ͷ */
	char *header_filename;

	/* ͨ��ͷ */
	int   ctype;                            /**< MIME_CTYPE_XXX */
	int   stype;                            /**< MIME_STYPE_XXX */
	char *charset;
	char *header_name;

	int   domain;
	int   encoding;                         /**< MIME_ENC_XXX */
	int   valid_line;
	char  last_ch;                          /**< ���������м�¼��ǰһ���ֽ� */
	char  last_lf;                          /**< ����ͷ��ÿ������ʱǰһ�� \n */
	off_t last_cr_pos;                      /**< ��һ�� \r ��ƫ��λ�� */
	off_t last_lf_pos;                      /**< ��һ�� \n ��ƫ��λ�� */
	ACL_VSTRING *boundary;                  /**< ���� multipart �ʼ�ʱ�洢�ָ��� */

	/**< ���� multipart �ʼ�ʱ��¼�ָ�������һ��ƥ��λ�ã�
	  ����ֵָ��ָ���β��ʱ˵����ȫƥ����� */
	const char *bound_ptr;

	char  bound_term[3];
	ACL_VSTRING *buffer;                    /**< headers, quoted-printable body */
	ACL_VSTRING *body;
	ACL_RING node;                          /**< ��ǰ��� */

	off_t header_begin;			/**< ���ͷ��ʼλ�� */
	off_t header_end;			/**< ���ͷ����λ�� */
	off_t body_begin;			/**< ����忪ʼλ�� */
	off_t body_end;				/**< ��������λ�� */
	off_t body_data_end;			/**< ������������λ�� */
	off_t bound_end;			/**< �ָ������λ�� */

	/* for acl_iterator, ͨ�� acl_foreach �г��ý���һ���ӽ�� */

	/* ȡ������ͷ���� */
	MIME_NODE *(*iter_head)(ACL_ITER*, MIME_NODE*);
	/* ȡ��������һ������ */
	MIME_NODE *(*iter_next)(ACL_ITER*, MIME_NODE*);
	/* ȡ������β���� */
	MIME_NODE *(*iter_tail)(ACL_ITER*, MIME_NODE*);
	/* ȡ��������һ������ */
	MIME_NODE *(*iter_prev)(ACL_ITER*, MIME_NODE*);
};

#define MIME_MAX_TOKEN		3	/* tokens per attribute */

struct MIME_STATE
{
	int   depth;                    /**< ������ */
	int   node_cnt;			/**< �������, ���� root ��� */
	MIME_NODE *root;		/**< MIME_NODE ����� */
	int   use_crlf;			/**< ���� \r\n ������ \n ��Ϊ���з� */

	/* private */

	MIME_NODE *curr_node;           /**< ��ǰ���ڴ���� MIME_NODE ��� */
	const char *curr_bound;         /**< ��� multipart �ʼ�, ��ǰ�ķָ��� */
	off_t curr_off;                 /**< ����ʼ��ĵ�ǰƫ��, �����ǻ�ָ����һ��λ�� */
	int   curr_status;              /**< ״̬����ǰ����״̬ */
#define MIME_S_HEAD                     0
#define MIME_S_BODY                     1
#define MIME_S_BODY_BOUND_CRLF          2
#define MIME_S_MULTI_BOUND              3
#define MIME_S_MULTI_BOUND_CRLF         4
#define MIME_S_TERM                     5

	HEADER_TOKEN token[MIME_MAX_TOKEN]; /* header token array */
	ACL_VSTRING *token_buffer;      /* header parser scratch buffer */
	ACL_VSTRING *key_buffer;

	/* for acl_iterator, ͨ�� acl_foreach �����г������ӽ�� */

	/* ȡ������ͷ���� */
	MIME_NODE *(*iter_head)(ACL_ITER*, MIME_STATE*);
	/* ȡ��������һ������ */
	MIME_NODE *(*iter_next)(ACL_ITER*, MIME_STATE*);
	/* ȡ������β���� */
	MIME_NODE *(*iter_tail)(ACL_ITER*, MIME_STATE*);
	/* ȡ��������һ������ */
	MIME_NODE *(*iter_prev)(ACL_ITER*, MIME_STATE*);
};

MIME_STATE *mime_state_alloc(void);
void mime_state_foreach_init(MIME_STATE *state);
int mime_state_free(MIME_STATE *state);
int mime_state_reset(MIME_STATE *state);
int mime_state_update(MIME_STATE *state, const char *data, int len);
int mime_state_head_finish(MIME_STATE *state);

MIME_NODE *mime_node_new(MIME_STATE *state);
int mime_node_delete(MIME_NODE *node);
void mime_node_add_child(MIME_NODE *parent, MIME_NODE *child);
const char *mime_ctype_name(size_t ctype);
const char *mime_stype_name(size_t stype);

#endif
