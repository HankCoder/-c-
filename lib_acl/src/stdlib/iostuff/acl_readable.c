/* System library. */
#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#include <errno.h>

#endif

#ifdef ACL_UNIX
# include <sys/time.h>
# include <poll.h>
#endif
#include <string.h>

#ifdef USE_SYS_SELECT_H
#include <sys/select.h>
#endif

/* Utility library. */

#include "stdlib/acl_msg.h"
#include "stdlib/acl_iostuff.h"

/* acl_readable - see if file descriptor is readable */

#ifdef ACL_UNIX

int acl_readable(ACL_SOCKET fd)
{
	const char *myname = "poll_read_wait";
	struct pollfd fds;
	int   delay = 0;

	fds.events = POLLIN | POLLHUP | POLLERR;
	fds.fd = fd;

	acl_set_error(0);

	for (;;) {
		switch (poll(&fds, 1, delay)) {
		case -1:
			if (acl_last_error() == ACL_EINTR)
				continue;

			acl_msg_error("%s(%d), %s: poll error(%s), fd: %d",
				__FILE__, __LINE__, myname,
				acl_last_serror(), (int) fd);
			return -1;
		case 0:
			return 0;
		default:
			if (fds.revents & (POLLHUP | POLLERR))
				return -1;
			else if ((fds.revents & POLLIN))
				return 1;
			else
				return 0;
		}
	}
}

#else

int acl_readable(ACL_SOCKET fd)
{
	const char *myname = "acl_readable";
	struct timeval tv;
	fd_set  rfds, xfds;
	int   errnum;

	/*
	 * Sanity checks.
	 */
	if ((unsigned) fd >= FD_SETSIZE)
		acl_msg_fatal("%s(%d), %s: fd %d does not fit in "
			"FD_SETSIZE: %d", __FILE__, __LINE__, myname,
			(int) fd, FD_SETSIZE);

	/*
	 * Initialize.
	 */
	FD_ZERO(&rfds);
	FD_SET(fd, &rfds);
	FD_ZERO(&xfds);
	FD_SET(fd, &xfds);
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	acl_set_error(0);

	/*
	 * Loop until we have an authoritative answer.
	 */
	for (;;) {
		switch (select(fd + 1, &rfds, (fd_set *) 0, &xfds, &tv)) {
		case -1:
			errnum = acl_last_error();
#ifdef	WIN32
			if (errnum == WSAEINPROGRESS
				|| errnum == WSAEWOULDBLOCK
				|| errnum == ACL_EINTR)
			{
				continue;
			}
#else
			if (errnum == ACL_EINTR)
				continue;
#endif
			acl_msg_error("%s(%d), %s: select error(%s), fd: %d",
				__FILE__, __LINE__, myname,
				acl_last_serror(), (int) fd);
			return -1;
		case 0:
			return 0;
		default:
			return FD_ISSET(fd, &rfds);
		}
	}
}

#endif
