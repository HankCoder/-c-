#include "stdafx.h"
#include "master_service.h"

bool var_mem_slice_on = false;

int main(int argc, char* argv[])
{
	if (0)
	{
		var_mem_slice_on = true;
		acl_mem_slice_init(8, 1024, 100000,
			ACL_SLICE_FLAG_GC2 |
			ACL_SLICE_FLAG_RTGC_OFF |
			ACL_SLICE_FLAG_LP64_ALIGN);
	}

	// ��ʼ�� acl ��
	acl::acl_cpp_init();

	master_service& ms = acl::singleton2<master_service>::get_instance();

	// �������ò�����
	ms.set_cfg_int(var_conf_int_tab);
	ms.set_cfg_str(var_conf_str_tab);
	ms.set_cfg_bool(var_conf_bool_tab);

	// ��ʼ����

	if (argc >= 2 && strcmp(argv[1], "alone") == 0)
	{
		const char* addr = "127.0.0.1:8888";
		printf("listen on: %s\r\n", addr);
		ms.run_alone(addr, NULL, acl::ENGINE_KERNEL);  // �������з�ʽ
	}
	else
	{
#ifdef	WIN32
		const char* addr = "127.0.0.1:8888";
		printf("listen on: %s\r\n", addr);

		ms.run_alone(addr, NULL, acl::ENGINE_KERNEL);  // �������з�ʽ
#else
		ms.run_daemon(argc, argv);  // acl_master ����ģʽ����
#endif
	}

	return 0;
}
