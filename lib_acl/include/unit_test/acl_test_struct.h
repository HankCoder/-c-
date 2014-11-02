#ifndef	ACL_TEST_STRUCT_INCLUDE_H
#define	ACL_TEST_STRUCT_INCLUDE_H

# ifdef	__cplusplus
extern "C" {
# endif

#include "stdlib/acl_array.h"

typedef struct AUT_LINE {
	char  cmd_name[128];	/* ��������� */
	int   result;		/* ִ�н�� */
	int   argc;		/* �������� */
	ACL_ARRAY *argv;	/* �����б� */
	char *args_str;		/* �����б�ı��� */
	int   valid_line_idx;	/* ����Ч��������������Ч�������е��к� */
	int   line_number;	/* ����Ч�������������ļ��е��к� */
	void *arg_inner;	/* �ڲ�ע���Լ��Ĳ������ýṹ�� */
	void (*free_arg_inner)(void*);
	void *arg_outer;	/* �ⲿӦ��ע���Լ��Ĳ������ýṹ�� */
	int   obj_type;		/* ���ڲ�����������ⲿ��������־λ,
				 * defined as: AUT_OBJ_
				 */
} AUT_LINE;

typedef int (*AUT_FN) (AUT_LINE *test_line, void *arg);

/**
 * ˵��: ��Ԫ���������õ�һ�µ����ݽṹ
 */
typedef struct AUT_FN_ITEM {
	const char *cmd_name;		/* ���������� */
	const char *fn_name;		/* �������� */
	AUT_FN fn_callback;		/* �����ûص����� */
	void *arg;			/* ���Իص��������õĲ��� */
	int   inner;			/* �Ƿ����ڲ������� */
} AUT_FN_ITEM;

/* �ڲ����ݽṹ���� */
typedef struct {
	char *name;
	char *value;
} AUT_ARG_ITEM;

typedef struct {
	int match_number;		/* �ɶ��������໥���ƥ���,
					 * ����: loop_begin �� loop_end ֮��
					 * �ԷǳɶԵ���������Ч, ���:
					 * stop ������.
					 */
	AUT_LINE *peer;			/* �����Ч�����гɶԵ���һ������ */
	int flag;			/* define as: AUT_FLAG_ */
	int status;			/* define as: AUT_STAT_ */
	int valid_line_idx;		/* ��������Ч�������е��±�λ�� */

	/* ˽�����Ͷ������� */
	/* ���ѭ��ִ���������� */
	int nloop_max;			/* ���ѭ������, �������ļ��л�� */
	int nloop_cur;			/* ��ǰѭ���Ĵ��� */
	int offset_valid_line_idx;	/* �����Ч�������±����� */
	int loop_sleep;			/* ѭ��ִ��ʱ����Ϣ */
} AUT_CMD_TOKEN;

#define	AUT_OBJ_OUTER		0	/* Ĭ��Ϊ�ⲿ������� */
#define	AUT_OBJ_INNER		1	/* Ϊ�ڲ����� */

#define	AUT_FLAG_LOOP_BEGIN	1
#define	AUT_FLAG_LOOP_BREAK	2
#define	AUT_FLAG_LOOP_CONTINUE	3
#define	AUT_FLAG_LOOP_END	4

#define	AUT_FLAG_IF		5
#define	AUT_FLAG_ELSE		6
#define	AUT_FLAG_ELIF		7
#define	AUT_FLAG_ENDIF		8

#define	AUT_FLAG_STOP		9

#define	AUT_STAT_FREE		0
#define	AUT_STAT_BUSY		1


#define	AUT_LOOP_BREAK			-100

#define	VAR_AUT_LOG_PRINT	0x0001
#define	VAR_AUT_LOG_FPRINT	0x0010

/* �����ļ��еĹؼ��� */
/* ��־��¼���� */
#define	VAR_AUT_LOG			"LOG"

/* ִ��ֹͣ��־λ */
#define	VAR_AUT_STOP			"STOP"

/* ��Ϣ��־λ */
#define	VAR_AUT_SLEEP			"SLEEP"

/* ��ͣ��־λ */
#define	VAR_AUT_PAUSE			"PAUSE"

/* ѭ��ִ�п�ʼ��־λ */
#define	VAR_AUT_LOOP_BEGIN		"LOOP_BEGIN"

/* ѭ��ִ��ֹͣ��־λ */
#define	VAR_AUT_LOOP_END		"LOOP_END"

/* ����ѭ��ִ�� */
#define	VAR_AUT_LOOP_BREAK		"LOOP_BREAK"

/* ѭ������ */
#define	VAR_AUT_LOOP_CONTINUE		"LOOP_CONTINUE"

/* �����жϿ�ʼ��� */
#define	VAR_AUT_IF			"IF"

/* �����ж� else ��� */
#define	VAR_AUT_ELSE			"ELSE"

/* �����жϽ������ */
#define	VAR_AUT_ENDIF			"ENDIF"

/* ��תִ����� */
#define	VAR_AUT_GOTO			"GOTO"

/*----------------- �ڲ�������һЩ�����ļ����� -----------------------------*/
/**
 * ͨ�õ�����ֵ����������:
 *
 * ���� VAR_AUT_SLEEP ���ʾΪ��Ϣ������ֵ;
 * ���� VAR_AUT_LOOP_BEGIN ���ʾѭ���Ĵ���
 */

#define	VAR_AUT_ITEM_COUNT		"COUNT"


# ifdef	__cplusplus
}
# endif

#endif

