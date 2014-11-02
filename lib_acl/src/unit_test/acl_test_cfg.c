#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"
/**
 *
 * System Library.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

/**
 *
 * Util Library.
 *
 */
#include "stdlib/acl_msg.h"
#include "stdlib/acl_mystring.h"
#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_loadcfg.h"
#include "stdlib/acl_array.h"

/**
 *
 * Application Specific.
 *
 */
#include "unit_test/acl_unit_test.h"

#endif

/* �ֲ����� */

/* ����������Ч�����еĶ�̬����ָ�� */
ACL_ARRAY *var_aut_line_array = NULL;

int var_aut_valid_line_idx = 0;

int var_aut_log_level = 0;

/*--------------------------------------------------------------------------*/
/* ���������ļ��еĵ��ĸ�����, ������зֽⲢ���붯̬����֮�� */
ACL_ARRAY *aut_parse_args_list(const char *str_in)
{
	const char *myname = "aut_parse_args_list";
	ACL_ARRAY *argvs_array = NULL;
	AUT_ARG_ITEM *arg_item = NULL;
	char *ptr_item, *pstr, *pstr_saved, *pname, *pvalue;
	char *ptr;
	int   len;
	char  tbuf[256];

	argvs_array = acl_array_create(10);
	pstr = acl_mystrdup(str_in);
	pstr_saved = pstr;

#define	SKIP_WHILE(_cond, _ptr) { while (*_ptr && (_cond)) _ptr++; }
#define	SKIP_WHILE_DEC(_cond, _ptr) { while (*_ptr && (_cond)) _ptr--; }

	len = strlen("=");
	while (1) {
		/* �ҵ�ÿһ������, �ָ���Ϊ���� */
		ptr_item = acl_mystrtok(&pstr, ",");
		if (ptr_item == NULL)
			break;

		/* ɾ��������ǰ�Ŀո�� tab */
		SKIP_WHILE((*ptr_item == ' ' || *ptr_item == '\t'), ptr_item);
		pname = ptr_item;

		/* ���ҵ����ںŷָ��� */
		pvalue = strstr(ptr_item, "=");
		if (pvalue == NULL) /* not found '=' */
			continue;

		ptr = pvalue;

		/* ɾ���Ⱥ���ߵĿո�� tab */
		SKIP_WHILE_DEC((*ptr == ' ' || *ptr == '\t'), ptr);
		if (ptr < pvalue)
			*(++ptr) = 0;

		*pvalue = 0;
		pvalue += len; /* skip '=' */

		/* ɾ���Ⱥ��ұߵĿո�͡tab */
		SKIP_WHILE((*pvalue == ' ' || *pvalue == '\t'), pvalue);
		if (*pvalue == 0)
			continue;

		/* ����һ�������� */
		arg_item = (AUT_ARG_ITEM *) acl_mycalloc(1, sizeof(AUT_ARG_ITEM));
		arg_item->name = acl_mystrdup(pname);
		arg_item->value = acl_mystrdup(pvalue);

		/* �Ѹò�������뵽��̬����֮�� */
		if (acl_array_append(argvs_array, (void *) arg_item) < 0)
			aut_log_fatal("%s(%d): append to array error(%s)",
				myname, __LINE__, acl_last_strerror(tbuf, sizeof(tbuf)));
	}

	acl_myfree(pstr_saved);

	return (argvs_array);
}

static void free_arg_item(void* arg)
{
	AUT_ARG_ITEM *item = (AUT_ARG_ITEM*) arg;
	acl_myfree(item->name);
	acl_myfree(item->value);
	acl_myfree(item);
}

void aut_free_args_list(ACL_ARRAY *a)
{
	if (a == NULL)
		return;
	acl_array_free(a, free_arg_item);
}

/*------------------------- ���е���Ч��������� ---------------------------*/

static void __parse_cfg_line(const ACL_CFG_LINE *line)
{
/*
 * ���������|Ԥ����[0:��ʾ�ɹ�, 1:��ʾʧ��]|��������|�����б�[name=����1, name=����2...]
 * test_line->value[0]: ���������
 * test_line->value[1]: Ԥ����
 * test_line->value[2]: ��������
 * test_line->value[3]: �����б�
 * ˵��: Ҫ��ǰ 3 ������������, ��һ��������ѡ
 */

	/* ���Ȳ����ڲ��������Ч����ѡ�� */
	if (aut_cfg_add_general_line(line) == 0)
		return;

	/* �ٲ����Ƿ����ڲ�����ѡ�� */
	if (aut_add_inner_cmd(line))
		return;

	/* ��Ӧ����ص�����ѡ��Ĵ��� */
	(void) aut_add_outer_cmd(line);
}

/* ��ʼ�� */
static void __init(void)
{
	const char *myname = "__init";

	if (var_aut_line_array != NULL)
		return;

	var_aut_line_array = acl_array_create(10);
	if (var_aut_line_array == NULL) {
		char tbuf[256];
		aut_log_fatal("%s: acl_array_create error(%s)",
			myname, acl_last_strerror(tbuf, sizeof(tbuf)));
	}
	var_aut_valid_line_idx = 0;
}

/* �������ļ������ڴ沢���зֽ� */
int aut_cfg_parse(const char *pathname)
{
	const char *myname = "aut_cfg_parse";
	ACL_CFG_PARSER *cfg_parser;
	ACL_CFG_LINE *cfg_line;
	int   i, n;

	if (pathname == NULL || *pathname == 0) {
		aut_log_error("%s: invalid configure name", myname);
		return (-1);
	}

	__init();

	cfg_parser = acl_cfg_parser_load(pathname, "|");

	if (cfg_parser == NULL) {
		char tbuf[256];
		aut_log_fatal("%s: open cfg_file(%s) error(%s)",
			myname, pathname, acl_last_strerror(tbuf, sizeof(tbuf)));
	}

	n = acl_cfg_parser_size(cfg_parser);
	for (i = 0; i < n; i++) {
		cfg_line = (ACL_CFG_LINE *) acl_cfg_parser_index(cfg_parser, i);
		if (cfg_line == NULL)
			break;
		if (cfg_line->ncount == 0)
			continue;
		__parse_cfg_line(cfg_line);
	}

	acl_cfg_parser_free(cfg_parser);

	return (0);
}

/*--------------------------------------------------------------------------*/
/* ��ӡ��������ļ��е���Ч������ */
int aut_cfg_print(void)
{
	const char *myname = "aut_cfg_print";
	AUT_LINE *line = NULL;
	AUT_ARG_ITEM *arg;
	int   i, j, n, m, first_line_arg;

	if (var_aut_line_array == NULL) {
		printf("%s: var_aut_line_array=NULL\n", myname);
		return (-1);
	}

	n = acl_array_size(var_aut_line_array);

	for (i = 0; i < n; i++) {
		line = (AUT_LINE *) acl_array_index(var_aut_line_array, i);
		if (line == NULL)
			break;
		printf("%s|%d|%d|", line->cmd_name, line->result, line->argc);

		m = acl_array_size(line->argv);
		first_line_arg = 1;
		for (j = 0; j < m; j++) {
			arg = (AUT_ARG_ITEM *) acl_array_index(line->argv, j);
			if (arg == NULL)
				break;

			/* �ж��Ƿ��ǵ�һ�������� */
			if (first_line_arg) {
				printf("%s=%s", arg->name, arg->value);
				first_line_arg = 0;
			} else {
				printf(",%s=%s", arg->name, arg->value);
			}
		}
		printf("\n");
	}

	return (0);
}
