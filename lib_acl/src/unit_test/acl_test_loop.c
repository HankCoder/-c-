#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#include "stdlib/acl_mymalloc.h"
#include "unit_test/acl_unit_test.h"

#endif

static int __loop_nested_count = 0;
static int __loop_begin_count  = 0;
static int __loop_end_count    = 0;

AUT_LINE *aut_loop_make_begin(const ACL_CFG_LINE *cfg_line)
{
	AUT_LINE *test_line;
	AUT_CMD_TOKEN *loop_token = NULL;
	const char *ptr;

	/* ������, �� loop_begin ��Ǽ����� 1 */
	__loop_begin_count++;

	/* ѭ����Ƕ�׼� 1 */
	__loop_nested_count++;

	loop_token = (AUT_CMD_TOKEN *) acl_mycalloc(1, sizeof(*loop_token));
	loop_token->flag            = AUT_FLAG_LOOP_BEGIN;
	loop_token->status          = AUT_STAT_FREE;
	loop_token->match_number    = __loop_nested_count;
	loop_token->nloop_cur       = 0;
	loop_token->nloop_max       = 0;
	loop_token->nloop_cur       = 0;
	loop_token->peer            = NULL;

	test_line                    = aut_line_new(cfg_line);
	test_line->obj_type          = AUT_OBJ_INNER;

	/* �� loop_token ��Ϊ�ڲ������洢�� test_line->arg_inner �� */
	test_line->arg_inner         = (void *) loop_token;
	test_line->free_arg_inner   = acl_myfree_fn;

	if (cfg_line->ncount < 2) {
		loop_token->nloop_max = 0;
		return (test_line);
	}

	/* ���������в��� */
	test_line->argv = aut_parse_args_list(cfg_line->value[1]);

	/* �������ļ���ȡ��Ҫѭ���Ĵ��� */
	ptr = aut_line_getvalue(test_line, VAR_AUT_ITEM_COUNT);
	if (ptr != NULL)
		loop_token->nloop_max = atoi(ptr);

	if (loop_token->nloop_max < 0)
		loop_token->nloop_max = 0;

	return (test_line);
}

AUT_LINE *aut_loop_make_break(const ACL_CFG_LINE *cfg_line)
{
	const char *myname = "aut_loop_make_break";
	AUT_LINE *break_line, *tmp_line, *begin_line;
	AUT_CMD_TOKEN *break_token, *tmp_token, *begin_token;
	int   i, n;

	break_line = aut_line_new(cfg_line);
	break_line->obj_type = AUT_OBJ_INNER;

	break_token = (AUT_CMD_TOKEN *) acl_mycalloc(1, sizeof(AUT_CMD_TOKEN));
	break_token->flag = AUT_FLAG_LOOP_BREAK;

	/* �� breaktoken ��Ϊ�ڲ������洢�� break_line->arg_inner �� */
	break_line->arg_inner = (void *) break_token;
	break_line->free_arg_inner = acl_myfree_fn;

	n = acl_array_size(var_aut_line_array);

	begin_line = NULL;
	begin_token = NULL;

	for (i = n - 1; i >= 0; i--) {
		tmp_line = (AUT_LINE *) acl_array_index(var_aut_line_array, i);
		if (tmp_line == NULL)
			aut_log_fatal("%s(%d): loop_begin null", myname, __LINE__);
		if (tmp_line->obj_type != AUT_OBJ_INNER)
			continue;
		if (tmp_line->arg_inner == NULL)
			continue;
		tmp_token = (AUT_CMD_TOKEN *) tmp_line->arg_inner;
		if (tmp_token->flag != AUT_FLAG_LOOP_BEGIN)
			continue;
		begin_line = tmp_line;
		begin_token = tmp_token;
		break;
	}

	if (begin_line == NULL || begin_token == NULL)
		aut_log_fatal("%s(%d): no LOOP_BEGIN before LOOP_BREAK",
				myname, __LINE__);
	break_token->peer = begin_line;

	return (break_line);
}

AUT_LINE *aut_loop_make_end(const ACL_CFG_LINE *cfg_line)
{
	const char *myname = "aut_loop_make_end";
	AUT_LINE *test_line, *test_line_peer;
	AUT_CMD_TOKEN *loop_token = NULL, *loop_token_peer;
	int   n, i;

	/* ������, �� loop_end ��Ǽ�����  1 */
	__loop_end_count++;

	loop_token = (AUT_CMD_TOKEN *) acl_mycalloc(1, sizeof(*loop_token));
	loop_token->flag   = AUT_FLAG_LOOP_END;
	loop_token->status = AUT_STAT_BUSY;

	/* ��ǰ��� loop_begin ��ƥ�� */
	loop_token->match_number = __loop_nested_count;
	loop_token->peer = NULL;

	/* ѭ����Ƕ�׼� 1 */
	__loop_nested_count--;

	test_line = aut_line_new(cfg_line);
	test_line->obj_type = AUT_OBJ_INNER;

	/* �� loop_token ��Ϊ�ڲ������洢�� test_line->arg_inner �� */
	test_line->arg_inner = (void *) loop_token;
	test_line->free_arg_inner = acl_myfree_fn;

	n = acl_array_size(var_aut_line_array);

	/* ������ѭ��������־����Ե�ѭ����ʼ���� */

	for (i = 0; i < n; i++) {
		test_line_peer = (AUT_LINE *)
				acl_array_index(var_aut_line_array, i);

		/* xxx: ��Ӧ�÷����������, ���Ƕ�̬������˹��� */
		if (test_line_peer == NULL)
			break;

		/* ���ж��Ƿ����ڲ����� */
		if (test_line_peer->obj_type != AUT_OBJ_INNER)
			continue;

		/* �Ȳ鿴�Ƿ����ڲ����ݲ����洢�� test_line_peer �� */
		if (test_line_peer->arg_inner == NULL)
			continue;

		loop_token_peer = (AUT_CMD_TOKEN *) test_line_peer->arg_inner;

		/* ���� loopbegin �����Ƿ��Ѿ��� һ�� loopend �����ƥ���� */
		if (loop_token_peer->status == AUT_STAT_BUSY)
			continue;

		/* �Ƿ��� ѭ����ʼ��־, ���ж��Ƿ���һ��ѭ���Ŀ�ʼ */
		if (loop_token_peer->flag != AUT_FLAG_LOOP_BEGIN)
			continue;

		/* �Ƚ�ƥ���к��Ƿ���� */
		if (loop_token_peer->match_number != loop_token->match_number)
			continue;

		/* �ҵ�ƥ���ѭ����ʼ�ԵȽ�� */
		loop_token_peer->peer   = test_line;
		loop_token_peer->status = AUT_STAT_BUSY;
		loop_token->peer        = test_line_peer;
	}

	if (loop_token->peer == NULL) {
		aut_log_fatal("%s(%d)->%s: line_number=%d, cmd=%s, "
				"loop_nested=%d, loop_begin=%d, "
				"loop_end=%d, please check configure, "
				"err_msg=not found peer loop begin",
				__FILE__, __LINE__, myname,
				test_line->line_number,
				test_line->cmd_name, __loop_nested_count,
				__loop_begin_count, __loop_end_count);
	}

	return (test_line);
}

/*-------------------------- ������ѭ����صĺ����ӿ� ----------------------*/
const AUT_LINE *aut_loop_end(const AUT_LINE *test_begin)
{
	if (test_begin == NULL)
		return (NULL);
	return (aut_line_peer(test_begin));
}

int aut_loop_count(const AUT_LINE *test_line)
{
	AUT_CMD_TOKEN *token;

	if (test_line == NULL || test_line->arg_inner == NULL)
		return (-1);

	token = (AUT_CMD_TOKEN *) test_line->arg_inner;
	if (token->flag == AUT_FLAG_LOOP_BEGIN)
		return (token->nloop_max);
	else if (token->flag == AUT_FLAG_LOOP_END) {
		token = aut_line_peer_token(test_line);
		if (token == NULL)
			return (-1);
		if (token->flag == AUT_FLAG_LOOP_BEGIN)
			return (token->nloop_max);
	}

	return (-1);
}

int aut_loop_from(const AUT_LINE *test_line)
{
	AUT_CMD_TOKEN *token;
	const AUT_LINE *test_begin;
	int   ret;

	if (test_line == NULL || test_line->arg_inner == NULL)
		return (-1);

	token = (AUT_CMD_TOKEN *) test_line->arg_inner;
	if (token->flag == AUT_FLAG_LOOP_BEGIN)
		test_begin = test_line;
	else if (token->flag == AUT_FLAG_LOOP_END)
		test_begin = aut_line_peer(test_line);
	else
		return (-1);

	if (test_begin == NULL || test_begin->arg_inner == NULL)
		return (-1);

	token = (AUT_CMD_TOKEN *) test_begin->arg_inner;
	if (token->flag != AUT_FLAG_LOOP_BEGIN)
		return (-1);

	ret = aut_line_valid_linenum(test_begin);
	if (ret < 0)
		return (-1);
	return (ret + 1);
}

int aut_loop_to(const AUT_LINE *test_line)
{
	AUT_CMD_TOKEN *token;
	const AUT_LINE *test_end;

	if (test_line == NULL || test_line->arg_inner == NULL)
		return (-1);

	token = (AUT_CMD_TOKEN *) test_line->arg_inner;
	if (token->flag == AUT_FLAG_LOOP_END)
		test_end = test_line;
	else if (token->flag == AUT_FLAG_LOOP_BEGIN)
		test_end = aut_line_peer(test_line);
	else
		return (-1);

	if (test_end == NULL || test_end->arg_inner == NULL)
		return (-1);

	token = (AUT_CMD_TOKEN *) test_end->arg_inner;
	if (token->flag != AUT_FLAG_LOOP_END)
		return (-1);

	return (aut_line_valid_linenum(test_end));
}

