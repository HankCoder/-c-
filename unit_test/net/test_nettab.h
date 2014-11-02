#ifndef	__TEST_NETTAB_INCLUDE_H__
#define	__TEST_NETTAB_INCLUDE_H__

#include "lib_acl.h"
#ifdef	__cplusplus
extern "C" {
#endif

#include "test_net.h"

static AUT_FN_ITEM __test_fn_tab[] = {
	/* ����������		������ʾ��		�ص���������		�ص����� �Ƿ����ڲ����� */
	/* test_connect.c */
	{ "test_connect",	"test_connect",		test_connect,		NULL, 0 },

	/* test_net_misc.c */
	{ "test_mask_addr",	"test_mask_addr",	test_mask_addr,		NULL, 0 },

	{ NULL, NULL, NULL, NULL, 0},
};

#ifdef	__cplusplus
}
#endif

#endif

