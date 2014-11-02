#include "lib_acl.h"

static ACL_DLINK *build(void)
{
	const char *myname = "build";
	ACL_DLINK *dlink = acl_dlink_create(100);

	acl_dlink_insert(dlink, 0, 10);
	acl_dlink_insert(dlink, 8, 10);
	acl_dlink_insert(dlink, 20, 21);
	acl_dlink_insert(dlink, 21, 28);
	acl_dlink_insert(dlink, 31, 38);
	acl_dlink_insert(dlink, 41, 48);
	acl_dlink_insert(dlink, 51, 58);

	printf("\r\n%s:\r\n", myname);
	acl_dlink_list(dlink);
	return (dlink);
}

static void delete_check(acl_off_t ibegin, acl_off_t iend)
{
	ACL_DLINK *dlink = build();

	printf(">>> after delete " ACL_FMT_I64D "-" ACL_FMT_I64D ":\r\n", ibegin, iend);
	acl_dlink_delete_range(dlink, ibegin, iend);
	acl_dlink_list(dlink);
	acl_dlink_free(dlink);
}

static void test(void)
{
	delete_check(0, 5);
	delete_check(0, 10);
	delete_check(0, 20);
	delete_check(0, 21);
	delete_check(0, 27);
	delete_check(0, 28);
	delete_check(0, 40);
	delete_check(0, 41);
	delete_check(0, 44);
	delete_check(0, 47);
	delete_check(0, 48);
	delete_check(0, 50);
	delete_check(0, 51);
	delete_check(0, 52);
	delete_check(0, 57);
	delete_check(0, 58);
	printf("Enter any key to continue ...\r\n");
	getchar();
	delete_check(5, 7);
	delete_check(5, 9);
	delete_check(5, 10);
	delete_check(5, 20);
	delete_check(5, 21);
	delete_check(5, 27);
	delete_check(5, 28);
	delete_check(5, 41);
	delete_check(5, 47);
	delete_check(5, 48);
	delete_check(5, 50);
	delete_check(5, 51);
	delete_check(5, 57);
	delete_check(5, 58);
	printf("Enter any key to continue ...\r\n");
	getchar();
	delete_check(10, 10);
	delete_check(10, 11);
	delete_check(10, 20);
	delete_check(10, 21);
	delete_check(10, 27);
	delete_check(10, 28);
	delete_check(10, 31);
	delete_check(10, 37);
	delete_check(10, 38);
	delete_check(10, 41);
	delete_check(10, 48);
	delete_check(10, 50);
	delete_check(10, 57);
	delete_check(10, 58);
	printf("Enter any key to continue ...\r\n");
	getchar();
	delete_check(20, 20);
	delete_check(20, 21);
	delete_check(20, 42);
	delete_check(20, 48);
	delete_check(20, 51);
	delete_check(20, 58);
	delete_check(41, 57);
	delete_check(41, 59);
	delete_check(50, 57);
	delete_check(50, 59);
	delete_check(51, 57);
	delete_check(51, 58);
	delete_check(51, 59);

	printf("Enter any key to quit\r\n");
	getchar();
}
int main(int argc acl_unused, char *argv[] acl_unused)
{
	test();
	return (0);
}

