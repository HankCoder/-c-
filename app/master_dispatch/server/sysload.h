#pragma once

class sysload {

public:
	sysload() {}
	~sysload() {}

	/**
	 * ���ϵͳ��ǰ�ĸ���
	 * @param out {acl::string*} ����ǿգ���洢�ַ�����ʽ�Ľ��
	 * @return {double}
	 */
	static double get_load(acl::string* out);
};
