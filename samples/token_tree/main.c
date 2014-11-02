#include "lib_acl.h"
#include <stdio.h>
#include <stdlib.h>

#define	STR	acl_vstring_str

static void token_tree_test(const char *tokens, const char *test_tab[])
{
	ACL_TOKEN *token_tree;
	const ACL_TOKEN *token;
	const char *ptr, *psaved;
	ACL_VSTRING *buf = acl_vstring_alloc(256);
	int   i;

	token_tree = acl_token_tree_create(tokens);
	acl_token_tree_print(token_tree);

	for (i = 0; test_tab[i] != NULL; i++) {
		ptr = psaved = test_tab[i];
		token = acl_token_tree_match(token_tree, &ptr, ";", NULL);
		if (token) {
			ACL_VSTRING_RESET(buf);
			acl_token_name(token, buf);
			printf("match %s %s, token's name: %s\n", psaved,
				(token->flag & ACL_TOKEN_F_DENY) ? "DENY"
				: (token->flag & ACL_TOKEN_F_PASS ? "PASS" : "NONE"),
				STR(buf));
		} else
			printf("match %s none\n", psaved);
	}

	acl_token_tree_destroy(token_tree);
	acl_vstring_free(buf);
}

static const char *test_tab[] = {
	"�й�"
	"�й�����",
	"�й���������",
	"�й������ž�",
	NULL
};

static void test(void)
{
	const char *tokens = "�й�|p �й���|p �й�����|p �й���������|p";

	token_tree_test(tokens, test_tab);
}

static void test2(void)
{
	ACL_TOKEN *tree;
	const ACL_TOKEN *token;
	const char *n1 = "����1", *n2 = "����2", *n3 = "����3";
	const char *v1 = "����1", *v2 = "����2", *v3 = "����3";
	const char *s = "�й���������1������������ϣ������Ķ�����2? "
		"�Ҳ�֪��������ƣ����ܸ��������ҵ�����3��...";
	const char *p = s;

	if (1)
	{
		tree = acl_token_tree_create(NULL);
		acl_token_tree_add(tree, n1, ACL_TOKEN_F_STOP, v1);
		acl_token_tree_add(tree, n2, ACL_TOKEN_F_STOP, v2);
		acl_token_tree_add(tree, n3, ACL_TOKEN_F_STOP, v3);
	}
	else
		tree = acl_token_tree_create("����1|p ����2|p ����3|p");

	printf("-----------------------------------\n");

	acl_token_tree_print(tree);
	token = acl_token_tree_word_match(tree, "����1");
	if (token)
		printf("find, %s: %s\n", acl_token_name1(token),
			(const char*) token->ctx);
	else
		printf("no find\n");

	printf("-----------------------------------\n");

	while (*p) {
		token = acl_token_tree_match(tree, &p, NULL, NULL);
		if (token == NULL)
			break;
		printf("%s: %s\n", acl_token_name1(token),
			(const char*) token->ctx);
	}

	printf("-----------------------------------\n");

	acl_token_tree_destroy(tree);
}

int main(void)
{
	test();
#if	0
	acl_token_tree_test();
#endif

	test2();
#ifdef ACL_MS_WINDOWS
	printf("enter any key to exit ...\n");

	getchar();
#endif
	return (0);
}
