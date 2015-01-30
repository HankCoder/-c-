#include "stdafx.h"

static bool test_zadd(acl::redis_zset& option, int i, const char* key,
	const char* big_data, size_t length, size_t base_length)
{
	// �������ݽ��зָ������ָ������ݿ����
	size_t nmember = length / base_length;
	if (length % base_length != 0)
		nmember++;

	// �����Ӷ����л��ͳһ���ڴ�ط�����󣬷���С�ڴ��
	acl::dbuf_pool* pool = option.get_client()->get_pool();
	// ��̬�������ݿ�ָ�������ڴ�
	const char** members = (const char**)
		pool->dbuf_alloc(nmember * sizeof(char*));
	// ��̬�������ݿ鳤�������ڴ�
	size_t* lens = (size_t*) pool->dbuf_alloc(nmember * sizeof(size_t));
	// ��̬�������ݿ��ֵ�����ڴ�
	double* scores = (double*) pool->dbuf_alloc(nmember * sizeof(double));

	// ���������зֳ�С���ݣ��������ݿ������У�ʹ�õ�����������Ϊ��ֵ
	size_t len;
	const char* ptr = big_data;
	char* buf, id[64];
	int n;

	for (size_t j = 0; j < nmember; j++)
	{
		len = length > base_length ? base_length : length;

		// ��ÿ��ԭʼ����ǰ���Ψһǰ׺���Ӷ����Ա�֤���򼯺��ж�����
		// ��ÿ����Ա���ݶ��ǲ�ͬ��
		n = acl::safe_snprintf(id, sizeof(id),
			"%lu:", (unsigned long) j);
		buf = (char*) pool->dbuf_alloc(len + n);
		memcpy(buf, id, n);
		memcpy(buf + n, ptr, len);
		members[j] = buf;

		lens[j] = len + n; // �����ݿ���ܳ��ȣ�Ψһǰ׺+����
		scores[j] = j;

		// ʣ�����ݿ鳤��
		length -= len;
		ptr += len;
	}

	// Ҫ�� redis ���Ӷ�������ڴ���Э����װ��ʽ�������ڲ���װ����Э��ʱ
	// ����װ�ɴ��ڴ�
	option.get_client()->set_slice_request(true);

	// ��ʼ�� redis �������
	int ret = option.zadd(key, members, lens, scores, nmember);
	if (ret < 0)
	{
		printf("add key: %s error\r\n", key);
		return false;
	}
	else if (i < 10)
		printf("add ok, key: %s, ret: %d\r\n", key, ret);

	return true;
}

static bool test_zcard(acl::redis_zset& option, int i, const char* key)
{
	// ��Ϊ��Э�����ݱȽ�С����������װ��������ʱ���ز��÷�Ƭ��ʽ
	option.get_client()->set_slice_request(false);

	int ret = option.zcard(key);
	if (ret < 0)
	{
		printf("zcard key: %s error\r\n", key);
		return false;
	}
	else if (i < 10)
		printf("zcard ok, key: %s, count: %d\r\n", key, ret);

	return true;
}

static bool test_zrange(acl::redis_zset& option, int i, const char* key,
	const char* hmac)
{
	int start = 0, end = -1;

	// ������������Ƚ�С����������װ����Э��ʱ���ز��÷�Ƭ��ʽ
	option.get_client()->set_slice_request(false);

	// �Է��������ص�����Ҳ����Ƭ
	option.get_client()->set_slice_respond(false);

	int ret = option.zrange(key, start, end, NULL);
	if (ret <= 0)
	{
		printf("zrange return: %d\r\n", ret);
		return false;
	}

	// �������Ԫ�ؽ����
	const acl::redis_result* result = option.get_result();
	if (result == NULL)
	{
		printf("result null\r\n");
		return false;
	}

	size_t size;
	// ֱ�ӻ�����鼯��
	const acl::redis_result** children = result->get_children(&size);
	if (children == NULL || size == 0)
	{
		printf("no children: %s, size: %d\r\n",
			children ? "no" : "yes", (int) size);
		return false;
	}

	// У���õ���������Ƭ�� MD5 ֵ���봫��Ľ��бȽ�
	acl::md5* md5;
	if (hmac != NULL)
		md5 = new acl::md5;
	else
		md5 = NULL;

	const acl::redis_result* child;
	size_t len, argc, n;

	// �ȱ�����������Ԫ�ض���
	for (size_t j = 0; j < size; j++)
	{
		child = children[j];
		if (child == NULL)
			continue;

		// ��Ϊǰ�������˽�ֹ����Ӧ���ݽ��з�Ƭ������ֻ��ȡ��һ��Ԫ��
		argc = child->get_size();
		assert(argc == 1);

		const char* ptr = child->get(0, &len);
		if (ptr == NULL)
		{
			printf("first is null\r\n");
			continue;
		}

		const char* dat = strchr(ptr, ':');
		if (dat == NULL)
		{
			printf("invalid data, j: %d\n", (int) j);
			continue;
		}
		dat++;
		n = dat - ptr;
		if (len < n)
		{
			printf("invalid data, j: %d\n", (int) j);
			continue;
		}

		len -= n;

		// ȡ�����ݼ��� md5 ֵ
		if (md5 != NULL)
			md5->update(dat, len);
	}

	if (md5 != NULL)
		md5->finish();

	// ����ַ�����ʽ�� MD5 ֵ
	if (md5 != NULL)
	{
		const char* ptr = md5->get_string();
		if (strcmp(ptr, hmac) != 0)
		{
			printf("md5 error, hmac: %s, %s, key: %s\r\n",
				hmac, ptr, key);
			return false;
		}
		else if (i < 10)
			printf("md5 ok, hmac: %s, %s, key: %s\r\n",
				hmac, ptr, key);
		delete md5;
	}
	else if (i < 10)
		printf("ok, key: %s\r\n", key);

	return true;
}

static bool test_del(acl::redis_key& option, int i, const char* key)
{
	int ret = option.del(key, NULL) < 0 ? false : true;
	if (ret < 0)
		printf("del %s error, i: %d\r\n", key, i);
	else if (i < 10)
		printf("del %s ok, i: %d\r\n", key, i);
	return ret >= 0 ? true : false;
}

/////////////////////////////////////////////////////////////////////////////

static acl::string __keypre("zset_key");
static size_t __base_length = 8192;  // ��׼���ݿ鳤��
static char* __big_data;
static size_t __big_data_length = 10240000;  // �����ݿ鳤�ȣ�Ĭ���� 10 MB
static char* __hmac;

// ���߳��࣬ÿ���̶߳����� redis-server ֮�佨��һ������
class test_thread : public acl::thread
{
public:
	test_thread(acl::redis_pool& pool, const char* cmd, int n, int id)
		: pool_(pool), cmd_(cmd), n_(n), id_(id) {}

	~test_thread() {}

protected:
	virtual void* run()
	{
		bool ret;
		acl::redis_client* conn;
		acl::redis_zset option;
		acl::redis_key key_option;
		acl::string key;

		for (int i = 0; i < n_; i++)
		{
			// ��ȫ���̳߳��л�ȡһ�� redis ���Ӷ���
			conn = (acl::redis_client*) pool_.peek();
			
			if (conn == NULL)
			{
				printf("peek redis_client failed\r\n");
				break;
			}

			// ÿ���߳�һ�� ID �ţ���Ϊ��ֵ��ɲ���
			key.format("%s_%d_%d", __keypre.c_str(), id_, i);

			conn->reset();

			// �� redis ���Ӷ����� redis ��������������а󶨹���
			option.set_client(conn);

			if (cmd_ == "zadd")
				ret = test_zadd(option, i, key.c_str(),
					__big_data, __big_data_length,
					__base_length);
			else if (cmd_ == "zcard")
				ret = test_zcard(option, i, key);
			else if (cmd_ == "zrange")
				ret = test_zrange(option, i, key, __hmac);
			else if (cmd_ == "del")
			{
				key_option.set_client(conn);
				ret = test_del(key_option, i, key);
			}
			else if (cmd_ != "all")
			{
				printf("unknown cmd: %s\r\n", cmd_.c_str());
				ret = false;
			}
			else if (test_zadd(option, i, key.c_str(),
					__big_data, __big_data_length,
					__base_length) == false
				|| test_zcard(option, i, key) == false
				|| test_zrange(option, i, key, __hmac) == false)
			{
				ret = false;
			}
			else
				ret = true;

			// �� redis ���Ӷ���黹�����ӳأ��Ƿ񱣳ָ����ӣ�
			// ͨ���жϸ������Ƿ�Ͽ�����
			pool_.put(conn, !conn->eof());

			if (ret == false)
				break;
		}

		return NULL;
	}

private:
	acl::redis_pool& pool_;
	acl::string cmd_;
	int n_;
	int id_;
};

static void init(const char* cmd, bool check)
{
	if (strcasecmp(cmd, "zrange") == 0 && check == false)
		return;

	acl::md5 md5;
	char ch;

	__big_data = (char*) malloc(__big_data_length);
	for (size_t i = 0; i < __big_data_length; i++)
	{
		ch = i % 255;
		__big_data[i] = ch;
		md5.update(&ch, 1);
	}

	//md5.update(__big_data, __big_data_length);
	md5.finish();

	__hmac = (char*) malloc(33);
	acl::safe_snprintf(__hmac, 33, "%s", md5.get_string());

	printf("init ok, hmac: %s, length: %lu, base: %lu, slice: %d\r\n",
		__hmac, (unsigned long) __big_data_length,
		(unsigned long) __base_length,
		(int) __big_data_length / __base_length
			+ __big_data_length % __base_length == 0 ? 0 : 1);

	md5.reset();
	md5.update(__big_data, __big_data_length);
	md5.finish();
	printf("md5 once: %s\r\n", md5.get_string());
}

static void end()
{
	if (__big_data)
		free(__big_data);
	if (__hmac)
		free(__hmac);
}

static void usage(const char* procname)
{
	printf("usage: %s -h[help]\r\n"
		"-s redis_addr[127.0.0.1:6379]\r\n"
		"-n count[default: 10]\r\n"
		"-C connect_timeout[default: 10]\r\n"
		"-I rw_timeout[default: 10]\r\n"
		"-c max_threads[default: 10]\r\n"
		"-l max_data_length\r\n"
		"-b base_length\r\n"
		"-S [if check data when cmd is zrange]\r\n"
		"-a cmd[zadd|zcard|zrange|del]\r\n",
		procname);
}

int main(int argc, char* argv[])
{
	int  ch, n = 1, conn_timeout = 10, rw_timeout = 10;
	int  max_threads = 10;
	bool check = false;
	acl::string addr("127.0.0.1:6379"), cmd;

	while ((ch = getopt(argc, argv, "hs:n:C:I:c:a:l:b:S")) > 0)
	{
		switch (ch)
		{
		case 'h':
			usage(argv[0]);
			return 0;
		case 's':
			addr = optarg;
			break;
		case 'n':
			n = atoi(optarg);
			break;
		case 'C':
			conn_timeout = atoi(optarg);
			break;
		case 'I':
			rw_timeout = atoi(optarg);
			break;
		case 'c':
			max_threads = atoi(optarg);
			break;
		case 'a':
			cmd = optarg;
			break;
		case 'l':
			__big_data_length = (unsigned long) atol(optarg);
			break;
		case 'b':
			__base_length = (size_t) atol(optarg);
			break;
		case 'S':
			check = true;
			break;
		default:
			break;
		}
	}

	acl::acl_cpp_init();

	init(cmd, check);

	acl::redis_pool pool(addr.c_str(), max_threads);
	pool.set_timeout(conn_timeout, rw_timeout);

	std::vector<test_thread*> threads;

	// ����һ���̣߳�ÿһ���߳��� redis-server ����һ������
	for (int i = 0; i < max_threads; i++)
	{
		test_thread* thread = new test_thread(pool, cmd.c_str(),
			n, i);
		threads.push_back(thread);
		// ȡ���̵߳ķ���ģʽ���Ա�����������̣߳��ȴ��߳��˳�
		thread->set_detachable(false);
		thread->start();
	}

	// ���������߳�
	std::vector<test_thread*>::iterator it = threads.begin();
	for (; it != threads.end(); ++it)
	{
		// �ȴ�ĳ���߳��˳�
		(*it)->wait();
		delete (*it);
	}

	end();

#ifdef WIN32
	printf("enter any key to exit\r\n");
	getchar();
#endif

	return 0;
}
