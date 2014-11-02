#ifndef __ACL_MASTER_FLOW_INCLUDED_H__
#define __ACL_MASTER_FLOW_INCLUDED_H__

#ifdef	__cplusplus
extern "C" {
#endif

#include "stdlib/acl_define.h"
#ifdef ACL_UNIX

 /*
  * Functional interface.
  */
extern void acl_master_flow_init(void);
extern int acl_master_flow_get(int);
extern int acl_master_flow_put(int);
extern int acl_master_flow_count(void);

#endif /* ACL_UNIX */

#ifdef	__cplusplus
}
#endif

#endif
