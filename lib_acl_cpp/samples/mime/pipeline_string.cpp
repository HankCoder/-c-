#include "stdafx.h"
#include "pipeline_string.h"

using namespace acl;

pipeline_string::pipeline_string()
: strip_sp_(false)
{

}

pipeline_string::~pipeline_string()
{

}

void pipeline_string::clear()
{
	strip_sp_ = false;
}

int pipeline_string::push_pop(const char* in, size_t len,
	string* out, size_t /* = 0 */)
{
	int   n = 0;

	// ��Щ�ʼ�ϵͳ�����ʱ���ܻ�����ӻس�����
	// �ַ���Ū���ˣ����Դ˴�����ȥ��س�������
	// �ո񣬴˾���ʽ����� HTML ��ʽ����������
	while (len > 0)
	{
		if ((*in) == '\r' || (*in) == '\n')
		{
			strip_sp_ = true;
			in++;
			len--;
			while (len > 0 && (*in == '\r' || *in == '\n'))
			{
				in++;
				len--;
			}
		}
		else if (strip_sp_)
		{
			strip_sp_ = false;
			while (len > 0 && (*in == ' ' || *in == '\t'))
			{
				in++;
				len--;
			}
		}
		else
		{
			(*out) << *((const unsigned char*) in);
			in++;
			len--;
			n++;
		}

	}

	return n;
}

int pipeline_string::pop_end(string* out, size_t max /* = 0 */)
{
	(void) out;
	(void) max;
	return 0;
}
