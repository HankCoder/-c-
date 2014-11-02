#include <string>
#include "acl_cpp/mime/mime_base64.hpp"
#include "acl_cpp/mime/mime_uucode.hpp"
#include "acl_cpp/mime/mime_xxcode.hpp"
#include "acl_cpp/stream/fstream.hpp"
#include "acl_cpp/stream/ifstream.hpp"

static void test1(void)
{
	acl::mime_base64 mime;
	acl::string out;
	const char *x = "sg==y8bX";
	const char* ptr = x;

	while (*ptr)
	{
		mime.decode_update(ptr, 1, &out);
		ptr++;
	}
	mime.decode_finish(&out);
	printf(">>>result:|%s|\n", out.c_str());

	out.clear();
	const char *s1 = "sg==", *s2 = "y8bX";
	mime.reset();
	mime.decode_update(s1, (int) strlen(s1), &out);
	mime.decode_update(s2, (int) strlen(s2), &out);
	mime.decode_finish(&out);
	printf(">>>2result:|%s|\n", out.c_str());
}

int main(void)
{
	test1();

	const char *text = "�г��ݶ������֤����΢��Ĵ��󡣵�����֮ǰ��\r\n"
		"Google���㹻ʱ��ɳ�Ϊ����΢��ȼ�ľ��ˡ���2007�꣬΢\r\n"
		"���ò�ͨ���������Կ�Google������Ŀ������ָ�����Ż�����\r\n"
		"��Google����һ���亹��������������˵���Ż��ܾ�΢��΢��\r\n"
		"����ͼͨ���������������以����ҵ�������Ӱ�죬����2009��\r\n"
		"6���Ƴ����Լ��Ķ�����������Bing������Ʒ��ȫ����Windows\r\n"
		" �����ڽ���͹�����С������;������������Բ�������ͬ��\r\n"
		"����ʱ��΢�����ʶ������ҵ������Windowsҵ��ĸ�ӹ������\r\n"
		"��Ϊ���ĵĻ�����ҵ�����������߼���:)\r\n";
	//acl::mime_xxcode mime;
	//acl::mime_uucode mime;
	acl::mime_base64 mime;
	acl::string out;
	acl::string path_encode("base64_encode.txt");
	acl::string path_decode("base64_decode.txt");
	acl::fstream fp_out;
	const char *ptr, *end;

	/*----------------------- ������� --------------------------*/

#if 1
	if (fp_out.open_trunc(path_encode.c_str()) == false) {
		printf("open %s error\r\n", path_encode.c_str());
		getchar();
		return (1);
	}

	ptr = text;
	end = text + strlen(text);

	while (ptr < end) {
		mime.encode_update(ptr, 1, &out);
		fp_out << out.c_str();
		out.clear();
		ptr++;
	}
	mime.encode_finish(&out);
	if (out.length() > 0) {
		fp_out << out.c_str();
		out.clear();
	}
	fp_out.close();

	printf("base64 encode over\r\n");
#endif
	/*----------------------- ������� --------------------------*/

	acl::string buf;
	if (acl::ifstream::load(path_encode.c_str(), &buf) == false) {
		printf("load %s error\r\n", path_encode.c_str());
		return (1);
	}

	if (fp_out.open_trunc(path_decode.c_str()) == false) {
		printf("open %s error\r\n", path_decode.c_str());
		getchar();
		return (1);
	}

	ptr = buf.c_str();
	end = ptr + buf.length();

	mime.reset();
	mime.add_invalid(true);
	while (ptr < end) {
		mime.decode_update(ptr, 1, &out);
		if (out.length() > 0) {
			fp_out << out.c_str();
			out.clear();
		}
		ptr++;
	}
	mime.decode_finish(&out);
	if (out.length() > 0) {
		fp_out << out.c_str();
		out.clear();
	}

	fp_out.close();

	printf("base64 decode over\r\n");

	/*----------------------- ������� --------------------------*/

	const char* s2 = "PCFET0NUWVBFIEhUTUwgUFVCTElDICItLy9XM0MvL0RURCBIVE1MIDQuMCBUcmFuc2l0aW9uYWwv\r\n"
		"L0VOIj4NCjxIVE1MPjxIRUFEPg0KPE1FVEEgY29udGVudD0idGV4dC9odG1sOyBjaGFyc2V0PWdi\r\n"
		"MjMxMiIgaHR0cC1lcXVpdj1Db250ZW50LVR5cGU+DQo8TUVUQSBuYW1lPUdFTkVSQVRPUiBjb250\r\n"
		"ZW50PSJNU0hUTUwgOC4wMC43NjAwLjE2NjI1Ij48TElOSyByZWw9c3R5bGVzaGVldCANCmhyZWY9\r\n"
		"IkJMT0NLUVVPVEV7bWFyZ2luLVRvcDogMHB4OyBtYXJnaW4tQm90dG9tOiAwcHg7IG1hcmdpbi1M\r\n"
		"ZWZ0OiAyZW19Ij48L0hFQUQ+DQo8Qk9EWSBzdHlsZT0iTUFSR0lOOiAxMHB4OyBGT05ULUZBTUlM\r\n"
		"WTogdmVyZGFuYTsgRk9OVC1TSVpFOiAxMHB0Ij4NCjxESVY+PEZPTlQgc2l6ZT0yIGZhY2U9VmVy\r\n"
		"ZGFuYT48L0ZPTlQ+Jm5ic3A7PC9ESVY+DQo8RElWPjxGT05UIHNpemU9MiBmYWNlPVZlcmRhbmE+\r\n"
		"PC9GT05UPiZuYnNwOzwvRElWPg0KPERJViBhbGlnbj1sZWZ0PjxGT05UIGNvbG9yPSNjMGMwYzAg\r\n"
		"c2l6ZT0yIGZhY2U9VmVyZGFuYT4yMDEwLTA5LTIxIA0KPC9GT05UPjwvRElWPjxGT05UIHNpemU9\r\n"
		"MiBmYWNlPVZlcmRhbmE+DQo8SFIgc3R5bGU9IldJRFRIOiAxMjJweDsgSEVJR0hUOiAycHgiIGFs\r\n"
		"aWduPWxlZnQgU0laRT0yPg0KDQo8RElWPjxGT05UIGNvbG9yPSNjMGMwYzAgc2l6ZT0yIGZhY2U9\r\n"
		"VmVyZGFuYT48U1BBTj7C7crAw/Q8L1NQQU4+IA0KPC9GT05UPjwvRElWPjwvRk9OVD48L0JPRFk+\r\n"
		"PC9IVE1MPg0K\r\n"
		"\r\n"
		"\r\n";
	mime.reset();
	out.clear();

	mime.decode_update(s2, (int) strlen(s2), &out);
	mime.decode_finish(&out);
	printf("result:\n%s\n", out.c_str());
	getchar();
	return (0);
}
