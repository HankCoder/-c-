#ifndef	__MASTER_LOG_INCLUDE_H__
#define	__MASTER_LOG_INCLUDE_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"

#ifdef	ACL_UNIX

void master_log_open(const char *procname);
void master_log_close(void);

#endif

#ifdef	__cplusplus
}
#endif

#endif
