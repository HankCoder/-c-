#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#endif

#ifdef ACL_UNIX

#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <sys/types.h>
#include <unistd.h>

#include "stdlib/unix/acl_mychown.h"


/* ע��, �˺������̲߳���ȫ��, ���ÿ�ε� s_owner �� s_group ��һ��������ν */
int acl_mychown(const char *path, const char *s_owner, const char *s_group)
{
	struct	passwd	*pwd;
	struct	group	*grp;
	uid_t	uid;
	gid_t	gid;
	int	ret;

	if(path == NULL || s_owner == NULL || s_group == NULL)
		return(-1);
	if ((pwd = getpwnam(s_owner)) != 0) {
		uid = pwd->pw_uid;
		grp = getgrnam(s_group);
		if(grp == NULL)
			return(-1);
		gid = grp->gr_gid;
		ret = chown(path, uid, gid);
		if(ret < 0)
			return(-1);
		return(0);
	}
	/* no such user */
	return(-1);
}

/* ע��, �˺������̲߳���ȫ��, ���ÿ�ε� s_owner �� s_group ��һ��������ν */
int acl_myfchown(const int fd, const char *s_owner, const char *s_group)
{
	struct	passwd	*pwd;
	struct	group	*grp;
	uid_t	uid;
	gid_t	gid;
	int	ret;

	if(fd < 0 || s_owner == NULL || s_group == NULL)
		return(-1);
	if ((pwd = getpwnam(s_owner)) != 0) {
		uid = pwd->pw_uid;
		grp = getgrnam(s_group);
		if(grp == NULL)
			return(-1);
		gid = grp->gr_gid;
		ret = fchown(fd, uid, gid);
		if(ret < 0)
			return(-1);
		return(0);
	}
	/* no such user */
	return(-1);
}
#endif /* ACL_UNIX */
