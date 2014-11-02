#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"

#include <stdlib.h>			/* 44BSD stdarg.h uses abort() */
#include <stdarg.h>
#include <string.h>

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

/* Application-specific. */

#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_msg.h"
#include "stdlib/acl_argv.h"

#endif

#define SPACE_LEFT(a) ((a)->len - (a)->argc - 1)

/* argv_extend - extend array */

static void argv_extend(ACL_ARGV *argvp)
{
	int     new_len;

	new_len = argvp->len * 2;
	if (argvp->slice)
		argvp->argv = (char **) acl_slice_pool_realloc(__FILE__, __LINE__,
			argvp->slice, argvp->argv, (new_len + 1) * sizeof(char *));
	else
		argvp->argv = (char **) acl_myrealloc((char *) argvp->argv,
			(new_len + 1) * sizeof(char *));
	argvp->len = new_len;
}

static void argv_push_back(struct ACL_ARGV *argvp, const char *s)
{
	acl_argv_add(argvp, s, 0);
}

static void argv_push_front(struct ACL_ARGV *argvp, const char *s)
{
	int   i;

	/* Make sure that always argvp->argc < argvp->len. */

	if (SPACE_LEFT(argvp) <= 0)
		argv_extend(argvp);
	for (i = argvp->argc; i > 0; i--) {
		argvp->argv[i] = argvp->argv[i - 1];
	}
	if (argvp->slice)
		argvp->argv[0] = acl_slice_pool_strdup(__FILE__, __LINE__,
			argvp->slice, s);
	else
		argvp->argv[0] = acl_mystrdup(s);
	argvp->argc++;
}

static char *argv_pop_back(struct ACL_ARGV *argvp)
{
	if (argvp->argc <= 0)
		return (NULL);
	return (argvp->argv[--argvp->argc]);
}

static char *argv_pop_front(struct ACL_ARGV *argvp)
{
	char *s;
	int   i;

	if (argvp->argc <= 0)
		return (NULL);
	s = argvp->argv[0];
	argvp->argc--;
	for (i = 0; i < argvp->argc; i++) {
		argvp->argv[i] = argvp->argv[i + 1];
	}
	return (s);
}

/* argv_iter_head - get the head of the array */

static void *argv_iter_head(ACL_ITER *iter, struct ACL_ARGV *argv)
{
	iter->dlen = -1;
	iter->key = NULL;
	iter->klen = -1;

	iter->i = 0;
	iter->size = argv->argc;
	iter->ptr = argv->argv[0];

	iter->data = iter->ptr;
	return (iter->ptr);
}

/* argv_iter_next - get the next of the array */

static void *argv_iter_next(ACL_ITER *iter, struct ACL_ARGV *argv)
{
	iter->i++;
	if (iter->i >= argv->argc)
		iter->data = iter->ptr = 0;
	else
		iter->data = iter->ptr = argv->argv[iter->i];
	return (iter->ptr);
}
 
/* argv_iter_tail - get the tail of the array */

static void *argv_iter_tail(ACL_ITER *iter, struct ACL_ARGV *argv)
{
	iter->dlen = -1;
	iter->key = NULL;
	iter->klen = -1;

	iter->i = argv->argc - 1;
	iter->size = argv->argc;
	if (iter->i < 0)
		iter->data = iter->ptr = 0;
	else
		iter->data = iter->ptr = argv->argv[iter->i];
	return (iter->ptr);
}

/* argv_iter_prev - get the prev of the array */

static void *argv_iter_prev(ACL_ITER *iter, struct ACL_ARGV *argv)
{
	iter->i--;
	if (iter->i < 0)
		iter->data = iter->ptr = 0;
	else
		iter->data = iter->ptr = argv->argv[iter->i];
	return (iter->ptr);
}

/* acl_argv_free - destroy string array */

ACL_ARGV   *acl_argv_free(ACL_ARGV *argvp)
{
	char  **cpp;

	for (cpp = argvp->argv; cpp < argvp->argv + argvp->argc; cpp++) {
		if (argvp->slice)
			acl_slice_pool_free(__FILE__, __LINE__, *cpp);
		else
			acl_myfree(*cpp);
	}
	if (argvp->slice) {
		acl_slice_pool_free(__FILE__, __LINE__, argvp->argv);
		acl_slice_pool_free(__FILE__, __LINE__, argvp);
	} else {
		acl_myfree(argvp->argv);
		acl_myfree(argvp);
	}
	return (NULL);
}

/* acl_argv_alloc - initialize string array */

ACL_ARGV   *acl_argv_alloc(int size)
{
	return (acl_argv_alloc2(size, NULL));
}

ACL_ARGV   *acl_argv_alloc2(int len, ACL_SLICE_POOL *slice)
{
	ACL_ARGV   *argvp;
	int     sane_len;

	/* Make sure that always argvp->argc < argvp->len. */

	if (slice) {
		argvp = (ACL_ARGV *) acl_slice_pool_alloc(__FILE__, __LINE__,
			slice, sizeof(*argvp));
		argvp->slice = slice;
	} else {
		argvp = (ACL_ARGV *) acl_mymalloc(sizeof(*argvp));
		argvp->slice = NULL;
	}
	argvp->len = 0;
	sane_len = (len < 2 ? 2 : len);
	if (argvp->slice)
		argvp->argv = (char **) acl_slice_pool_alloc(__FILE__, __LINE__,
			argvp->slice, (sane_len + 1) * sizeof(char *));
	else
		argvp->argv = (char **) acl_mymalloc((sane_len + 1) * sizeof(char *));
	argvp->len = sane_len;
	argvp->argc = 0;
	argvp->argv[0] = 0;

	argvp->push_back = argv_push_back;
	argvp->push_front = argv_push_front;
	argvp->pop_back = argv_pop_back;
	argvp->pop_front = argv_pop_front;

	/* set the iterator callback */
	argvp->iter_head = argv_iter_head;
	argvp->iter_next = argv_iter_next;
	argvp->iter_tail = argv_iter_tail;
	argvp->iter_prev = argv_iter_prev;

	return (argvp);
}

/* acl_argv_add - add string to vector */

void    acl_argv_add(ACL_ARGV *argvp,...)
{
	const char *arg;
	va_list ap;

	/* Make sure that always argvp->argc < argvp->len. */

	va_start(ap, argvp);
	while ((arg = va_arg(ap, const char *)) != 0) {
		if (SPACE_LEFT(argvp) <= 0)
			argv_extend(argvp);
		if (argvp->slice)
			argvp->argv[argvp->argc++] = acl_slice_pool_strdup(__FILE__, __LINE__,
				argvp->slice, arg);
		else
			argvp->argv[argvp->argc++] = acl_mystrdup(arg);
	}
	va_end(ap);
	argvp->argv[argvp->argc] = 0;
}

void    acl_argv_addv(ACL_ARGV *argvp, va_list ap)
{
	const char *arg;

	/* Make sure that always argvp->argc < argvp->len. */

	while ((arg = va_arg(ap, const char *)) != 0) {
		if (SPACE_LEFT(argvp) <= 0)
			argv_extend(argvp);
		if (argvp->slice)
			argvp->argv[argvp->argc++] = acl_slice_pool_strdup(__FILE__, __LINE__,
				argvp->slice, arg);
		else
			argvp->argv[argvp->argc++] = acl_mystrdup(arg);
	}
	argvp->argv[argvp->argc] = 0;
}

/* acl_argv_addn - add string to vector */

void    acl_argv_addn(ACL_ARGV *argvp,...)
{
	const char *myname = "acl_argv_addn";
	const char *arg;
	int     len;
	va_list ap;

	/* Make sure that always argvp->argc < argvp->len. */

	va_start(ap, argvp);
	while ((arg = va_arg(ap, const char *)) != 0) {
		if ((len = va_arg(ap, int)) < 0)
			acl_msg_panic("%s: bad string length %d", myname, len);
		if (SPACE_LEFT(argvp) <= 0)
			argv_extend(argvp);
		if (argvp->slice)
			argvp->argv[argvp->argc++] = acl_slice_pool_strndup(__FILE__, __LINE__,
				argvp->slice, arg, len);
		else
			argvp->argv[argvp->argc++] = acl_mystrndup(arg, len);
	}
	va_end(ap);
	argvp->argv[argvp->argc] = 0;
}

void    acl_argv_addnv(ACL_ARGV *argvp, va_list ap)
{
	const char *myname = "acl_argv_addnv";
	const char *arg;
	int     len;

	/* Make sure that always argvp->argc < argvp->len. */

	while ((arg = va_arg(ap, const char *)) != 0) {
		if ((len = va_arg(ap, int)) < 0)
			acl_msg_panic("%s: bad string length %d", myname, len);
		if (SPACE_LEFT(argvp) <= 0)
			argv_extend(argvp);
		if (argvp->slice)
			argvp->argv[argvp->argc++] = acl_slice_pool_strndup(__FILE__, __LINE__,
				argvp->slice, arg, len);
		else
			argvp->argv[argvp->argc++] = acl_mystrndup(arg, len);
	}
	argvp->argv[argvp->argc] = 0;
}

/* acl_argv_terminate - terminate string array */

void    acl_argv_terminate(ACL_ARGV *argvp)
{

	/* Trust that argvp->argc < argvp->len. */
	argvp->argv[argvp->argc] = 0;
}

char  *acl_argv_index(ACL_ARGV *argvp, int idx)
{
	if (argvp == NULL || idx < 0 || idx > argvp->argc - 1)
		return(NULL);

	return(argvp->argv[idx]);
}

int   acl_argv_size(ACL_ARGV *argvp)
{
	if (argvp == NULL)
		return (-1);

	return (argvp->argc);
}

