#include "lib_acl.h"
#include <assert.h>

static int var_cfg_sleep_time;
static int var_cfg_create_core;

static ACL_CONFIG_INT_TABLE __conf_int_tab[] = {
	{ "sleep_time", 5, &var_cfg_sleep_time, 0, 0 },
	{ "create_core", 0, &var_cfg_create_core, 0, 0 },
	{ 0, 0, 0, 0, 0},
};

static char* var_cfg_dummy;

static ACL_CONFIG_STR_TABLE __conf_str_tab[] = {
	{ "dummy", "default", &var_cfg_dummy },
	{ 0, 0, 0 },
};

static void __service(char *buf acl_unused, int len acl_unused,
			           char *service, char **argv acl_unused)
{
	const char *myname = "__service";

	/*
	 * Sanity check. This service takes no command-line arguments.
	 */
	if (argv[0])
		acl_msg_fatal("%s(%d)->%s: unexpected command-line argument: %s",
			__FILE__, __LINE__, myname, argv[0]);

	acl_msg_info("%s(%d), %s: service name = %s, sleep %d, info: %s seconds now---",
		__FILE__, __LINE__, myname, service,
		var_cfg_sleep_time, var_cfg_dummy);
	sleep(var_cfg_sleep_time);
	acl_msg_info("%s: wakeup now", myname);
	if (var_cfg_create_core)
		assert(0);
}

static void __pre_jail_init(char *name acl_unused, char **argv acl_unused)
{
}

static void __post_jail_init(char *name acl_unused, char **argv acl_unused)
{
}

static void service_exit(char *service acl_unused, char **argv acl_unused)
{
}

int main(int argc, char *argv[])
{
	acl_trigger_server_main(argc, argv, __service,
		ACL_MASTER_SERVER_PRE_INIT, __pre_jail_init,
		ACL_MASTER_SERVER_POST_INIT, __post_jail_init,
		ACL_MASTER_SERVER_INT_TABLE, __conf_int_tab,
		ACL_MASTER_SERVER_STR_TABLE, __conf_str_tab,
		ACL_MASTER_SERVER_EXIT, service_exit,
		0);
	return (0);
}
