#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"
/* System library. */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#ifdef ACL_UNIX
#include <dirent.h>
#include <unistd.h>
#elif	defined(ACL_BCB_COMPILER)
#include <dirent.h>
#endif

/* Utility library. */

#include "stdlib/acl_msg.h"
#include "stdlib/acl_mymalloc.h"
#include "stdlib/acl_mystring.h"
#include "stdlib/acl_stringops.h"
#include "stdlib/acl_scan_dir.h"

#endif

#include "stdlib/acl_dir.h"

#ifdef WIN32
# define SANE_RMDIR _rmdir
# define SANE_UNLINK _unlink
#else
# define SANE_RMDIR rmdir
# define SANE_UNLINK unlink
#endif

#include "dir_sys_patch.h"

#ifndef	MAX_PATH
#define	MAX_PATH	1024
#endif

 /*
  * The interface is based on an opaque structure, so we don't have to expose
  * the user to the guts. Subdirectory info sits in front of parent directory
  * info: a simple last-in, first-out list.
  */
typedef struct ACL_SCAN_INFO ACL_SCAN_INFO;

struct ACL_SCAN_INFO {
	char   *path;               /* directory name */
	DIR    *dir_name;           /* directory structure */
	struct acl_stat sbuf;       /* the stat of the dir or file */
	ACL_SCAN_INFO *parent;      /* linkage */
};

struct ACL_SCAN_DIR {
	ACL_SCAN_INFO *current;     /* current scan */
	int   recursive;            /* if scan subtree recursive */
	int   nfiles;               /* total files' count */
	int   ndirs;                /* total dirs' count */
	acl_int64 nsize;            /* total size of all files */
	ACL_SCAN_DIR_FN scan_fn;
	void *scan_ctx;
	char  file_name[256];       /* if is file set it */
};

#define ACL_SCAN_DIR_PATH(scan)	(scan->current->path)

/* acl_scan_dir_open - start directory scan */

ACL_SCAN_DIR *acl_scan_dir_open(const char *path, int recursive)
{
	ACL_SCAN_DIR *scan;

	scan = (ACL_SCAN_DIR *) acl_mycalloc(1, sizeof(*scan));
	scan->current = 0;
	scan->recursive = recursive;

	scan->nfiles = 0;
	scan->ndirs = 0;
	scan->nsize = 0;

	if (acl_scan_dir_push(scan, path) < 0)
		return NULL;
	return scan;
}

/* acl_scan_dir_close - terminate directory scan */

void acl_scan_dir_close(ACL_SCAN_DIR *scan)
{
	while (scan->current)
		acl_scan_dir_pop(scan);
	acl_myfree(scan);
}

void acl_scan_dir_reset(ACL_SCAN_DIR *scan)
{
	scan->nfiles = 0;
	scan->ndirs = 0;
	scan->nsize = 0;
}

/* acl_scan_dirctl - ctl interface for set option */

void acl_scan_dir_ctl(ACL_SCAN_DIR *scan, int name, ...)
{
	va_list ap;

	va_start(ap, name);

	for (; name != ACL_SCAN_CTL_END; name = va_arg(ap, int)) {
		switch(name) {
		case ACL_SCAN_CTL_FN:
			scan->scan_fn = va_arg(ap, ACL_SCAN_DIR_FN);
			break;
		case ACL_SCAN_CTL_CTX:
			scan->scan_ctx = va_arg(ap, void*);
			break;
		default:
			break;
		}
	}

	va_end(ap);
}

/* acl_scan_dir_path - return the path of the directory being read.  */

const char *acl_scan_dir_path(ACL_SCAN_DIR *scan)
{
	if (scan->current == NULL)
		return NULL;
	return ACL_SCAN_DIR_PATH(scan);
}

const char *acl_scan_dir_file(ACL_SCAN_DIR *scan)
{
	if (scan->current == NULL)
		return NULL;

	return scan->file_name;
}

int acl_scan_dir_ndirs(ACL_SCAN_DIR *scan)
{
	return scan->ndirs;
}

int acl_scan_dir_nfiles(ACL_SCAN_DIR *scan)
{
	return scan->nfiles;
}

acl_int64 acl_scan_dir_nsize(ACL_SCAN_DIR *scan)
{
	return scan->nsize;
}

int acl_scan_stat(ACL_SCAN_DIR *scan, struct acl_stat *sbuf)
{
	if (scan->current == NULL || sbuf == NULL)
		return -1;

	memcpy(sbuf, &scan->current->sbuf, sizeof(struct acl_stat));
	return 0;
}

int acl_scan_dir_end(ACL_SCAN_DIR *scan)
{
	if (scan->current == NULL)
		return 1;
	else
		return 0;
}
/* acl_scan_dir_push - enter directory */

int acl_scan_dir_push(ACL_SCAN_DIR *scan, const char *path)
{
	const char *myname = "acl_scan_dir_push";
	ACL_SCAN_INFO *info;

	if (path == NULL || *path == 0)
		acl_msg_fatal("%s(%d), %s: path null",
			__FILE__, __LINE__, myname);

	info = (ACL_SCAN_INFO *) acl_mymalloc(sizeof(*info));
	if (scan->current)
		info->path = acl_concatenate(ACL_SCAN_DIR_PATH(scan),
				PATH_SEP_S, path, (char *) 0);
	else {
		size_t len = strlen(path);
		const char *ptr = path + len - 1;

#ifdef	WIN32
		while (ptr > path && (*ptr == '/' || *ptr == '\\'))
#else
		while (ptr > path && *ptr == '/')
			ptr--;
#endif
		len = ptr - path + 1;
		info->path = (char*) acl_mymalloc(len + 1);
		memcpy(info->path, path, len);
		info->path[len] = 0;
	}

	if ((info->dir_name = opendir(info->path)) == 0) {
		acl_msg_error("%s(%d), %s: open directory(%s) error(%s)",
			__FILE__, __LINE__, myname,
			info->path, acl_last_serror());
		return -1;
	}

	info->parent = scan->current;
	scan->current = info;

	return 0;
}

/* acl_scan_dir_pop - leave directory */

ACL_SCAN_DIR *acl_scan_dir_pop(ACL_SCAN_DIR *scan)
{
	const char *myname = "acl_scan_dir_pop";
	ACL_SCAN_INFO *info = scan->current;
	ACL_SCAN_INFO *parent;

	if (info == NULL)
		return NULL;

	parent = info->parent;
	if (closedir(info->dir_name)) {
		acl_msg_fatal("%s(%d), %s: close directory(%s) error(%s)",
			__FILE__, __LINE__, myname,
			info->path, acl_last_serror());
	}

	acl_myfree(info->path);
	acl_myfree(info);
	scan->current = parent;

	return parent ? scan : NULL;
}

/* acl_scan_dir_next - find next entry */

const char *acl_scan_dir_next(ACL_SCAN_DIR *scan)
{
	ACL_SCAN_INFO *info = scan->current;
	struct dirent *dp;

#define STREQ(x,y)	(strcmp((x),(y)) == 0)

	if (info == NULL)
		return NULL;

	while ((dp = readdir(info->dir_name)) != 0) {
		if (STREQ(dp->d_name, ".") || STREQ(dp->d_name, ".."))
			continue;

		return dp->d_name;
	}

	return NULL;
}

/* acl_scan_dir_next_file - find next valid file */

const char *acl_scan_dir_next_file(ACL_SCAN_DIR *scan)
{
	const char *myname = "acl_scan_dir_next_file";
	const char *name;
	char  pathbuf[MAX_PATH];
	struct acl_stat sbuf;

	for (;;) {
		if ((name = acl_scan_dir_next(scan)) == NULL) {
			if (acl_scan_dir_pop(scan) == 0)
				return NULL;
			continue;
		}

		snprintf(pathbuf, sizeof(pathbuf), "%s%c%s",
			ACL_SCAN_DIR_PATH(scan), PATH_SEP_C, name);

		if (acl_stat(pathbuf, &sbuf) < 0) {
			acl_msg_error("%s(%d), %s: stat file(%s) error(%s)",
				__FILE__, __LINE__, myname, pathbuf,
				acl_last_serror());
			return NULL;
		}

		memcpy(&scan->current->sbuf, &sbuf, sizeof(sbuf));

		scan->nsize += sbuf.st_size;

		if (!S_ISDIR(sbuf.st_mode)) {
			scan->nfiles++;
			return name;
		}

		scan->ndirs++;

		if (scan->recursive && acl_scan_dir_push(scan, name) < 0)
			return NULL;
	}
}

/* acl_scan_dir_next_dir - find next valid dir */

const char *acl_scan_dir_next_dir(ACL_SCAN_DIR *scan)
{
	const char *myname = "acl_scan_dir_next_dir";
	const char *name;
	char  pathbuf[MAX_PATH];
	struct acl_stat sbuf;

	for (;;) {
		if ((name = acl_scan_dir_next(scan)) == NULL) {
			if (acl_scan_dir_pop(scan) == 0)
				return NULL;
			continue;
		}
		snprintf(pathbuf, sizeof(pathbuf), "%s%c%s",
			ACL_SCAN_DIR_PATH(scan), PATH_SEP_C, name);
		if (acl_stat(pathbuf, &sbuf) < 0) {
			acl_msg_error("%s(%d), %s: stat file(%s) error(%s)",
				__FILE__, __LINE__, myname, pathbuf,
				acl_last_serror());
			return NULL;
		}

		memcpy(&scan->current->sbuf, &sbuf, sizeof(sbuf));

		scan->nsize += sbuf.st_size;

		if (!S_ISDIR(sbuf.st_mode))
			continue;

		scan->ndirs++;

		if (scan->recursive && acl_scan_dir_push(scan, name) < 0)
			return NULL;
		else
			return name;
	}
}

const char *acl_scan_dir_next_name(ACL_SCAN_DIR *scan, int *is_file)
{
	const char *myname = "acl_scan_dir_next_name";
	const char *name;
	char  pathbuf[MAX_PATH];
	struct acl_stat sbuf;

	for (;;) {
		if ((name = acl_scan_dir_next(scan)) == NULL) {
			if (acl_scan_dir_pop(scan) == 0)
				return NULL;
			continue;
		}
		snprintf(pathbuf, sizeof(pathbuf), "%s%c%s",
			ACL_SCAN_DIR_PATH(scan), PATH_SEP_C, name);
		if (acl_stat(pathbuf, &sbuf) < 0) {
			acl_msg_error("%s(%d), %s: stat file(%s) error(%s)",
				__FILE__, __LINE__, myname, pathbuf,
				acl_last_serror());
			return NULL;
		}

		memcpy(&scan->current->sbuf, &sbuf, sizeof(sbuf));

		scan->nsize += sbuf.st_size;

		if (!S_ISDIR(sbuf.st_mode)) {
			if (is_file)
				*is_file = 1;
			return name;
		}

		scan->ndirs++;

		if (scan->recursive && acl_scan_dir_push(scan, name) < 0)
			return NULL;
		if (is_file)
			*is_file = 0;
		return name;
	}
}

acl_int64 acl_scan_dir_size2(ACL_SCAN_DIR *scan, int *nfile, int *ndir)
{
	const char *myname = "acl_scan_dir_size2";
	const char *name;
	char  pathbuf[MAX_PATH];
	struct acl_stat sbuf;

	while (1) {
		if ((name = acl_scan_dir_next(scan)) == NULL) {
			if (acl_scan_dir_pop(scan) == NULL)
				break;
			continue;
		}
		snprintf(pathbuf, sizeof(pathbuf), "%s%c%s",
			ACL_SCAN_DIR_PATH(scan), PATH_SEP_C, name);
		if (acl_stat(pathbuf, &sbuf) < 0) {
			acl_msg_error("%s(%d), %s: stat file(%s) error(%s)",
				__FILE__, __LINE__, myname, pathbuf,
				acl_last_serror());
			break;
		}

		memcpy(&scan->current->sbuf, &sbuf, sizeof(sbuf));

		if (S_ISDIR(sbuf.st_mode)) {
			scan->ndirs++;
			scan->file_name[0] = 0;
			if (scan->recursive && acl_scan_dir_push(scan, name) < 0)
				break;
		} else {
			ACL_SAFE_STRNCPY(scan->file_name, name,
				sizeof(scan->file_name));
			scan->nfiles++;
			scan->nsize += sbuf.st_size;
		}

		if (scan->scan_fn && scan->scan_fn(scan, scan->scan_ctx) < 0)
			break;
	}

	if (nfile)
		*nfile = scan->nfiles;
	if (ndir)
		*ndir = scan->ndirs;

	return scan->nsize;
}

acl_int64 acl_scan_dir_size(const char *pathname, int recursive,
	int *nfile, int *ndir)
{
	const char *myname = "acl_scan_dir_size";
	ACL_SCAN_DIR *scan;
	acl_int64 size;

	if (pathname == NULL || *pathname == 0) {
		acl_msg_error("%s(%d), %s: pathname null",
			__FILE__, __LINE__, myname);
		return -1;
	}

	scan = acl_scan_dir_open(pathname, recursive);
	if (scan == NULL) {
		acl_msg_error("%s(%d), %s: dir_open error: %s, path: %s",
			__FILE__, __LINE__, myname,
			acl_last_serror(), pathname);
		return -1;
	}

	size = acl_scan_dir_size2(scan, nfile, ndir);
	acl_scan_dir_close(scan);

	return size;
}

/* acl_scan_dir_rmall - remove all directoies and file in the dir */

acl_int64 acl_scan_dir_rm2(ACL_SCAN_DIR *scan, int *ndir, int *nfile)
{
	const char *myname = "acl_scan_dir_rm2";
	const char *name;
	char  path[MAX_PATH];
	struct acl_stat sbuf;

	for (;;) {
		if ((name = acl_scan_dir_next(scan)) == NULL) {
			if (scan->current != NULL)
				snprintf(path, sizeof(path), "%s",
					ACL_SCAN_DIR_PATH(scan));
			else
				path[0] = 0;

			/* �����˳��ÿ�Ŀ¼��ſ���ɾ����Ŀ¼ */

			if (acl_scan_dir_pop(scan) == 0) {
				/* ɾ������Ŀ¼ */
				if (path[0] != 0 && SANE_RMDIR(path) == 0)
					scan->ndirs++;
				break;
			}

			/* ɾ����Ŀ¼ */
			if (path[0] != 0 && SANE_RMDIR(path) == 0)
				scan->ndirs++;
			continue;
		}

		snprintf(path, sizeof(path), "%s%c%s",
			ACL_SCAN_DIR_PATH(scan), PATH_SEP_C, name);

		if (acl_stat(path, &sbuf) < 0) {
			acl_msg_error("%s(%d), %s: stat file(%s) error(%s)",
				__FILE__, __LINE__, myname,
				path, acl_last_serror());
			break;
		}
		if (S_ISDIR(sbuf.st_mode)) {
			scan->file_name[0] = 0;
			if (scan->recursive && acl_scan_dir_push(scan, name) < 0)
				break;
			if (scan->scan_fn && scan->scan_fn(scan, scan->scan_ctx) < 0)
				break;
			continue;
		}

		ACL_SAFE_STRNCPY(scan->file_name, name, sizeof(scan->file_name));

		if (scan->scan_fn && scan->scan_fn(scan, scan->scan_ctx) < 0)
			break;
		scan->nfiles++;
		scan->nsize += sbuf.st_size;

		SANE_UNLINK(path);
	}

	if (ndir)
		*ndir = scan->ndirs;
	if (nfile)
		*nfile = scan->nfiles;

	return scan->nsize;
}

acl_int64 acl_scan_dir_rm(const char *pathname, int recursive,
	int *ndir, int *nfile)
{
	const char *myname = "acl_scan_dir_rmall";
	ACL_SCAN_DIR *scan;
	struct acl_stat sbuf;
	acl_int64 nsize;

	if (ndir)
		*ndir = 0;
	if (nfile)
		*nfile = 0;

	if (acl_stat(pathname, &sbuf) < 0) {
		acl_msg_error("%s(%d), %s: stat pathname(%s) error(%s)",
			__FILE__, __LINE__, myname, pathname,
			acl_last_serror());
		return -1;
	}
	if (S_ISDIR(sbuf.st_mode) == 0) {
		if (nfile)
			*nfile = 1;
		SANE_UNLINK(pathname);

		return 1;
	}

	scan = acl_scan_dir_open(pathname, recursive);
	if (scan == NULL) {
		acl_msg_error("%s(%d), %s: open path(%s) error(%s)",
			__FILE__, __LINE__, myname,
			pathname, acl_last_serror());
		return -1;
	}

	acl_scan_dir_rm2(scan, ndir, nfile);

	nsize = scan->nsize;
	acl_scan_dir_close(scan);

	return nsize;
}
