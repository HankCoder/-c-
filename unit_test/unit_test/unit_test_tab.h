#ifndef	__UNIT_TAB_TEST_INCLUDE_H__
#define	__UNIT_TAB_TEST_INCLUDE_H__

#include "lib_acl.h"
#ifdef	__cplusplus
extern "C" {
#endif

#include "unit_test.h"

static AUT_FN_ITEM __test_fn_tab[] = {
	/* ����������		������ʾ��		�ص���������		�ص����� �Ƿ����ڲ����� */
	/* in test_unit_fn.c */
	{ "test_param",		"fn_test_param",	test_unit_param,	NULL, 0 },
	{ "test_loop",		"fn_test_loop",		test_unit_loop,		NULL, 0 },

	{ NULL, NULL, NULL, NULL, 0},
};

#ifdef	__cplusplus
}
#endif

#endif
