#include "stdafx.h"
#include "master_service.h"

int main(int argc, char* argv[])
{
	// ��ʼ�� acl ��
	acl::acl_cpp_init();
	acl::log::stdout_open(true);

	master_service& ms = acl::singleton2<master_service>::get_instance();

	// �������ò�����
	ms.set_cfg_int(var_conf_int_tab);
	ms.set_cfg_bool(var_conf_bool_tab);

	// ��ʼ����

	if (argc >= 2 && strcmp(argv[1], "alone") == 0)
	{
		const char* addr = "127.0.0.1:8888";
		if (argc >= 3)
			addr = argv[2];
		printf("listen on: %s\r\n", addr);
		ms.run_alone(addr, NULL, acl::ENGINE_SELECT);  // �������з�ʽ
	}
	else
		ms.run_daemon(argc, argv);  // acl_master ����ģʽ����

	return 0;
}
