#include "stdafx.h"
#include "master_service.h"

int main(int argc, char* argv[])
{
	// ��ʼ�� acl ��
	acl::acl_cpp_init();

	master_service& ms = acl::singleton2<master_service>::get_instance();

	// �������ò�����
	ms.set_cfg_int(var_conf_int_tab);
	ms.set_cfg_int64(var_conf_int64_tab);
	ms.set_cfg_str(var_conf_str_tab);
	ms.set_cfg_bool(var_conf_bool_tab);

	// ��ʼ����

	if (argc >= 2 && strcmp(argv[1], "alone") == 0)
	{
		acl::log::stdout_open(true);  // ��־�������׼���
//		logger_open("check.log", "check");
		if (argc >= 3)
			ms.run_alone(argv[2], 1, 1);  // �������з�ʽ
		else
			ms.run_alone(NULL, 1, 1);  // �������з�ʽ

		printf("enter any key to exit\r\n");
		getchar();
	}
	else
		ms.run_daemon(argc, argv);  // acl_master ����ģʽ����

	return 0;
}
