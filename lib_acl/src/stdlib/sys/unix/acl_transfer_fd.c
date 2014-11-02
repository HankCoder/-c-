#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#endif

#ifdef ACL_UNIX

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "stdlib/acl_msg.h"
#include "stdlib/unix/acl_transfer_fd.h"

int acl_read_fd(int fd, void *ptr, int nbytes, int *recv_fd)
{
	struct msghdr msg;
	struct iovec iov[1];
	int n;
#ifdef HAVE_MSGHDR_MSG_CONTROL
	const char *myname = "acl_read_fd";
	union {
		struct cmsghdr cm;
	# ifdef	ACL_MACOSX
		char   control[1024];
	# else
		char   control[CMSG_SPACE(sizeof(int))];
	# endif
	} control_un;

	struct cmsghdr *cmptr;
	int *fdptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
#else
	int newfd;

	msg.msg_accrights = (caddr_t) &newfd;
	msg.msg_accrightslen = sizeof(int);
#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;

	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	if ((n = recvmsg(fd, &msg, 0)) <= 0)
		return (n);

#ifdef HAVE_MSGHDR_MSG_CONTROL
	if ((cmptr = CMSG_FIRSTHDR(&msg)) != NULL
	    && cmptr->cmsg_len == CMSG_LEN(sizeof(int)))
	{
		if (cmptr->cmsg_level != SOL_SOCKET)
			acl_msg_fatal("%s: control level != SOL_SOCKET",
				myname);
		if (cmptr->cmsg_type != SCM_RIGHTS)
			acl_msg_fatal("%s: control type != SCM_RIGHTS",
				myname);
#if 0
		*recv_fd = *((int *) CMSG_DATA(cmptr));
#else
		fdptr = (int *) CMSG_DATA(cmptr);
		*recv_fd = *fdptr;
#endif
	} else
		*recv_fd = -1;  /* descriptor was not passed */
#else
	if (msg.msg_accrightslen == sizeof(int))
		*recv_fd = newfd;
	else
		*recv_fd = -1; /* descriptor was not passed */
#endif
	
	return (n);
}

int acl_write_fd(int fd, void *ptr, int nbytes, int send_fd)
{
	struct msghdr msg;
	struct iovec  iov[1];

#ifdef HAVE_MSGHDR_MSG_CONTROL
	struct cmsghdr *cmptr;
	int *fdptr;
	union {
		struct cmsghdr cm;
	# ifdef	ACL_MACOSX
		char   control[1024];
	# else
		char   control[CMSG_SPACE(sizeof(int))];
	# endif
	} control_un;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);

	cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(int));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;

#if 0
	*((int *) CMSG_DATA(cmptr)) = send_fd;
#else
	fdptr = (int *) CMSG_DATA(cmptr);
	*fdptr = send_fd;
#endif

#else
	msg.msg_accrights = (caddr_t) &send_fd;
	msg.msg_accrightslen = sizeof(int);
#endif

	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;

	return (sendmsg(fd, &msg, 0));
}

#endif /* ACL_UNIX */
