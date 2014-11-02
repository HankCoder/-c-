/*++
 * NAME
 *	attr_print64 3
 * SUMMARY
 *	send attributes over byte stream
 * SYNOPSIS
 *	#include <attr.h>
 *
 *	int	attr_print64(fp, flags, type, name, ..., ATTR_TYPE_END)
 *	ACL_VSTREAM	fp;
 *	int	flags;
 *	int	type;
 *	char	*name;
 *
 *	int	attr_vprint64(fp, flags, ap)
 *	ACL_VSTREAM	fp;
 *	int	flags;
 *	va_list	ap;
 * DESCRIPTION
 *	attr_print64() takes zero or more (name, value) simple attributes
 *	and converts its input to a byte stream that can be recovered with
 *	attr_scan64(). The stream is not flushed.
 *
 *	attr_vprint64() provides an alternate interface that is convenient
 *	for calling from within variadic functions.
 *
 *	Attributes are sent in the requested order as specified with the
 *	attr_print64() argument list. This routine satisfies the formatting
 *	rules as outlined in attr_scan64(3).
 *
 *	Arguments:
 * .IP fp
 *	Stream to write the result to.
 * .IP flags
 *	The bit-wise OR of zero or more of the following.
 * .RS
 * .IP ATTR_FLAG_MORE
 *	After sending the requested attributes, leave the output stream in
 *	a state that is usable for more attribute sending operations on
 *	the same output attribute list.
 *	By default, attr_print64() automatically appends an attribute list
 *	terminator when it has sent the last requested attribute.
 * .RE
 * .IP type
 *	The type determines the arguments that follow.
 * .RS
 * .IP "ATTR_TYPE_INT (char *, int)"
 *	This argument is followed by an attribute name and an integer.
 * .IP "ATTR_TYPE_LONG (char *, long)"
 *	This argument is followed by an attribute name and a long integer.
 * .IP "ATTR_TYPE_STR (char *, char *)"
 *	This argument is followed by an attribute name and a null-terminated
 *	string.
 * .IP "ATTR_TYPE_DATA (char *, ssize_t, char *)"
 *	This argument is followed by an attribute name, an attribute value
 *	length, and an attribute value pointer.
 * .IP "ATTR_TYPE_FUNC (ATTR_PRINT_SLAVE_FN, void *)"
 *	This argument is followed by a function pointer and generic data
 *	pointer. The caller-specified function returns whatever the
 *	specified attribute printing function returns.
 * .IP "ATTR_TYPE_HASH (HTABLE *)"
 * .IP "ATTR_TYPE_NAMEVAL (NVTABLE *)"
 *	The content of the table is sent as a sequence of string-valued
 *	attributes with names equal to the table lookup keys.
 * .IP ATTR_TYPE_END
 *	This terminates the attribute list.
 * .RE
 * DIAGNOSTICS
 *	The result value is 0 in case of success, ACL_VSTREAM_EOF in case
 *	of trouble.
 *
 *	Panic: interface violation. All system call errors are fatal.
 * SEE ALSO
 *	attr_scan64(3) recover attributes from byte stream
 * LICENSE
 * .ad
 * .fi
 *	The Secure Mailer license must be distributed with this software.
 * AUTHOR(S)
 *	Wietse Venema
 *	IBM T.J. Watson Research
 *	P.O. Box 704
 *	Yorktown Heights, NY 10598, USA
 *--*/

#include "StdAfx.h"
#include <stdarg.h>
#include <string.h>

#include "attr.h"

#define STR(x)	acl_vstring_str(x)
#define LEN(x)	ACL_VSTRING_LEN(x)

static void free_vstring(void *arg)
{
    ACL_VSTRING *s = (ACL_VSTRING*) arg;

    acl_vstring_free(s);
}

/* attr_print64_str - encode and send attribute information */

static void attr_print64_str(ACL_VSTREAM *fp, const char *str, ssize_t len)
{
    static __thread ACL_VSTRING *base64_buf;

    if (base64_buf == 0) {
	base64_buf = acl_vstring_alloc(10);
	acl_pthread_atexit_add(base64_buf, free_vstring);
    }

    acl_vstring_base64_encode(base64_buf, str, len);
    acl_vstream_buffed_fputs(STR(base64_buf), fp);
}

static void attr_print64_num(ACL_VSTREAM *fp, unsigned num)
{
    static __thread ACL_VSTRING *plain;

    if (plain == 0) {
	plain = acl_vstring_alloc(10);
	acl_pthread_atexit_add(plain, free_vstring);
    }

    acl_vstring_sprintf(plain, "%u", num);
    attr_print64_str(fp, STR(plain), (int) LEN(plain));
}

static void attr_print64_long_num(ACL_VSTREAM *fp, unsigned long long_num)
{
    static __thread ACL_VSTRING *plain;

    if (plain == 0) {
	plain = acl_vstring_alloc(10);
	acl_pthread_atexit_add(plain, free_vstring);
    }

    acl_vstring_sprintf(plain, "%lu", long_num);
    attr_print64_str(fp, STR(plain), (int) LEN(plain));
}

/* attr_vprint64 - send attribute list to stream */

int     attr_vprint64(ACL_VSTREAM *fp, int flags, va_list ap)
{
    const char *myname = "attr_print64";
    int     attr_type;
    char   *attr_name;
    unsigned int_val;
    unsigned long long_val;
    char   *str_val;
    ACL_HTABLE_INFO **ht_info_list;
    ACL_HTABLE_INFO **ht;
    ssize_t len_val;
    ATTR_PRINT_SLAVE_FN print_fn;
    void   *print_arg;

    /*
     * Sanity check.
     */
    if (flags & ~ATTR_FLAG_ALL)
	acl_msg_panic("%s: bad flags: 0x%x", myname, flags);

    /*
     * Iterate over all (type, name, value) triples, and produce output on
     * the fly.
     */
    while ((attr_type = va_arg(ap, int)) != ATTR_TYPE_END) {
	switch (attr_type) {
	case ATTR_TYPE_INT:
	    attr_name = va_arg(ap, char *);
	    attr_print64_str(fp, attr_name, (int) strlen(attr_name));
	    int_val = va_arg(ap, int);
	    ACL_VSTREAM_PUTC(':', fp);
	    attr_print64_num(fp, (unsigned) int_val);
	    ACL_VSTREAM_PUTC('\n', fp);
	    if (acl_msg_verbose)
		acl_msg_info("send attr %s = %u", attr_name, int_val);
	    break;
	case ATTR_TYPE_LONG:
	    attr_name = va_arg(ap, char *);
	    attr_print64_str(fp, attr_name, (int) strlen(attr_name));
	    long_val = va_arg(ap, long);
	    ACL_VSTREAM_PUTC(':', fp);
	    attr_print64_long_num(fp, (unsigned long) long_val);
	    ACL_VSTREAM_PUTC('\n', fp);
	    if (acl_msg_verbose)
		acl_msg_info("send attr %s = %lu", attr_name, long_val);
	    break;
	case ATTR_TYPE_STR:
	    attr_name = va_arg(ap, char *);
	    attr_print64_str(fp, attr_name, (int) strlen(attr_name));
	    str_val = va_arg(ap, char *);
	    ACL_VSTREAM_PUTC(':', fp);
	    attr_print64_str(fp, str_val, (int) strlen(str_val));
	    ACL_VSTREAM_PUTC('\n', fp);
	    if (acl_msg_verbose)
		acl_msg_info("send attr %s = %s", attr_name, str_val);
	    break;
	case ATTR_TYPE_DATA:
	    attr_name = va_arg(ap, char *);
	    attr_print64_str(fp, attr_name, (int) strlen(attr_name));
	    len_val = va_arg(ap, ssize_t);
	    str_val = va_arg(ap, char *);
	    ACL_VSTREAM_PUTC(':', fp);
	    attr_print64_str(fp, str_val, len_val);
	    ACL_VSTREAM_PUTC('\n', fp);
	    if (acl_msg_verbose)
		acl_msg_info("send attr %s = [data %ld bytes]",
			 attr_name, (long) len_val);
	    break;
	case ATTR_TYPE_FUNC:
	    print_fn = va_arg(ap, ATTR_PRINT_SLAVE_FN);
	    print_arg = va_arg(ap, void *);
	    print_fn(attr_print64, fp, flags | ATTR_FLAG_MORE, print_arg);
	    break;
	case ATTR_TYPE_HASH:
	    ht_info_list = acl_htable_list(va_arg(ap, ACL_HTABLE *));
	    for (ht = ht_info_list; *ht; ht++) {
		attr_print64_str(fp, ht[0]->key.c_key, (int) strlen(ht[0]->key.c_key));
		ACL_VSTREAM_PUTC(':', fp);
		attr_print64_str(fp, ht[0]->value, (int) strlen(ht[0]->value));
		ACL_VSTREAM_PUTC('\n', fp);
		if (acl_msg_verbose)
		    acl_msg_info("send attr name %s value %s",
			     ht[0]->key.c_key, (char*) ht[0]->value);
	    }
	    acl_myfree(ht_info_list);
	    break;
	default:
	    acl_msg_panic("%s: unknown type code: %d", myname, attr_type);
	}
    }
    if ((flags & ATTR_FLAG_MORE) == 0)
	ACL_VSTREAM_PUTC('\n', fp);
    return (acl_vstream_fflush(fp));
}

int     attr_print64(ACL_VSTREAM *fp, int flags,...)
{
    va_list ap;
    int     ret;

    va_start(ap, flags);
    ret = attr_vprint64(fp, flags, ap);
    va_end(ap);
    return (ret);
}

#ifdef TEST

 /*
  * Proof of concept test program.  Mirror image of the attr_scan64 test
  * program.
  */

int     main(int unused_argc, char **argv)
{
    ACL_HTABLE *table = acl_htable_create(1);

    acl_msg_verbose = 1;
    acl_htable_enter(table, "foo-name", mystrdup("foo-value"));
    acl_htable_enter(table, "bar-name", mystrdup("bar-value"));
    attr_print64(ACL_VSTREAM_OUT, ATTR_FLAG_NONE,
		 ATTR_TYPE_INT, ATTR_NAME_INT, 4711,
		 ATTR_TYPE_LONG, ATTR_NAME_LONG, 1234,
		 ATTR_TYPE_STR, ATTR_NAME_STR, "whoopee",
		 ATTR_TYPE_DATA, ATTR_NAME_DATA, strlen("whoopee"), "whoopee",
		 ATTR_TYPE_HASH, table,
		 ATTR_TYPE_END);
    attr_print64(ACL_VSTREAM_OUT, ATTR_FLAG_NONE,
		 ATTR_TYPE_INT, ATTR_NAME_INT, 4711,
		 ATTR_TYPE_LONG, ATTR_NAME_LONG, 1234,
		 ATTR_TYPE_STR, ATTR_NAME_STR, "whoopee",
		 ATTR_TYPE_DATA, ATTR_NAME_DATA, strlen("whoopee"), "whoopee",
		 ATTR_TYPE_END);
    if (acl_vstream_fflush(ACL_VSTREAM_OUT) == ACL_VSTREAM_EOF)
	acl_msg_fatal("%s: write error", __FUNCTION__);

    acl_htable_free(table, myfree);
    return (0);
}

#endif
