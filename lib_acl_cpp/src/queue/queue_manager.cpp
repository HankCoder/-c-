#include "acl_stdafx.hpp"
#include "acl_cpp/stdlib/snprintf.hpp"
#include "acl_cpp/stdlib/util.hpp"
#include "acl_cpp/stdlib/log.hpp"
#include "acl_cpp/queue/queue_manager.hpp"

#ifdef WIN32
#define PATH_SEP	'\\'
#else
#define PATH_SEP	'/'
#endif

namespace acl
{

queue_manager::queue_manager(const char* home, const char* queueName,
	unsigned sub_width)
: m_scanDir(NULL)
, m_home(home)
, m_queueName(queueName)
{
	if (sub_width == 0)
		sub_width_ = 2;
	else
		sub_width_ = sub_width;

	string buf = home;
	buf << PATH_SEP << queueName;

	// �ȴ�����Ŀ¼
	if (acl_make_dirs(buf.c_str(), 0700) == -1)
		logger_error("create dir: %s error %s", buf.c_str(),
			last_serror());
	else
		logger("create dir: %s ok", buf.c_str());

	char  node[32];
	for (unsigned i = 0; i < sub_width_; i++)
	{
		buf.clear();
		safe_snprintf(node, sizeof(node), "%d", i);
		buf << home << PATH_SEP << queueName
			<< PATH_SEP << node;
		// ������������Ŀ¼
		if (acl_make_dirs(buf.c_str(), 0700) == -1)
			logger_error("create dir: %s error %s",
				buf.c_str(), last_serror());
		else
			logger("create dir: %s ok", buf.c_str());
	}
}

queue_manager::~queue_manager()
{
	if (m_scanDir)
		acl_scan_dir_close(m_scanDir);
}

const char* queue_manager::get_home() const
{
	return m_home.c_str();
}

const char* queue_manager::get_queueName() const
{
	return m_queueName.c_str();
}

queue_file* queue_manager::create_file(const char* extName)
{
	queue_file* fp = NEW queue_file;

	if (fp->create(m_home.c_str(), m_queueName.c_str(),
		extName, sub_width_) == false)
	{
		delete fp;
		return NULL;
	}

	if (cache_add(fp) == false)
		logger_fatal("%s already exist in table", fp->key());
	return fp;
}

queue_file* queue_manager::open_file(const char* filePath, bool no_cache /* = true */)
{
	string home, queueName, queueSub, partName, extName;
	if (queue_manager::parse_filePath(filePath, &home, &queueName, &queueSub,
		&partName, &extName) == false)
	{
		logger_error("filePath(%s) invalid", filePath);
		return NULL;
	}

	queue_file* fp;

	// ������ļ��������ڴ�����ֱ�ӷ���֮
	fp = cache_find(partName);
	if (fp != NULL)
	{
		if (no_cache)
		{
			logger_warn("file: %s locked", filePath);
			return NULL;
		}
		return fp;
	}

	// �Ӵ��̴��Ѿ����ڵĶ����ļ�
	fp = NEW queue_file;
	if (fp->open(home.c_str(), queueName.c_str(), queueSub.c_str(),
		partName.c_str(), extName.c_str()) == false)
	{
		delete fp;
		return NULL;
	}
	cache_add(fp);
	return fp;
}

bool queue_manager::close_file(queue_file* fp)
{
	string key(fp->key());
	delete fp;
	cache_del(key.c_str());
	return true;
}

bool queue_manager::delete_file(queue_file* fp)
{
	string key(fp->key());
	fp->remove();
	delete fp;
	cache_del(key.c_str());
	return true;
}

bool queue_manager::rename_extname(queue_file* fp, const char* extName)
{
	if (cache_check(fp) == false)
	{
		logger_warn("file(%s)'s key(%s) not exist",
			fp->get_filePath(), fp->key());
		return false;
	}
	return fp->move_file(fp->get_queueName(), extName);
}

bool queue_manager::move_file(queue_file* fp, const char* queueName, const char* extName)
{
	string key(fp->key());
	bool ret = fp->move_file(queueName, extName);
	cache_del(key.c_str());
	return ret;
}

bool queue_manager::move_file(queue_file* fp, queue_manager* toQueue, const char* extName)
{
	bool ret = move_file(fp, toQueue->get_queueName(), extName);
	if (ret == false)
		return false;
	return toQueue->cache_add(fp);
}

bool queue_manager::parse_filePath(const char* filePath, string* home,
	string* queueName, string* queueSub,
	string* partName, string* extName)
{
	if (filePath == NULL || *filePath == 0)
	{
		logger_error("filePath invalid!");
		return false;
	}

	// ��ʽΪ: /home/queue_name/queue_sub_node/file_name.file_ext

	ACL_ARGV *argv = acl_argv_split(filePath, "/\\");
	if (argv->argc < 4)
	{
		logger_error("filePath(%s) invalid", filePath);
		acl_argv_free(argv);
		return false;
	}

	home->clear();

	// �����һ���ַ�Ϊ PATH_SEP ����Ҫ����
	if (*filePath == PATH_SEP)
		home->push_back(PATH_SEP);

	// ���洢��·������
	for (int i = 0; i < argv->argc - 3; i++)
	{
		if (i > 0 && home->length() > 0)
			(*home) += PATH_SEP;
		(*home) += argv->argv[i];
	}

	// ȡ�ö�����
	*queueName = argv->argv[argv->argc - 3];
	*queueSub = argv->argv[argv->argc - 2];

	// ���������ļ�������
	bool ret  = parse_fileName(argv->argv[argv->argc - 1], partName, extName);

	acl_argv_free(argv);
	return ret;
}

bool queue_manager::parse_fileName(const char* fileName, string* partName, string* extName)
{
	const char* extSep = strrchr(fileName, '.');
	if (extSep == NULL || extSep == fileName)
	{
		logger_error("fileName(%s) invalid", fileName);
		return false;
	}

	// �����ļ���
	partName->copy(fileName, extSep - fileName);
	extSep++;
	if (*extSep == 0)
	{
		logger("fileName(%s) invalid", fileName);
		return false;
	}

	// ������չ��
	*extName = extSep;
	return true;
}

bool queue_manager::parse_path(const char* path, string* home,
		       string* queueName, string* queueSub)
{
	if (path == NULL || *path == 0)
	{
		logger_error("path invalid!");
		return false;
	}

	/* WINDOWS ֧�� '/' �� '\\' ���ַָ��� */
	// ���ݸ�ʽ: /home/queueName/queueSub
	ACL_ARGV *argv = acl_argv_split(path, "/\\");
	if (argv->argc < 3)
	{
		logger_error("path(%s) invalid", path);
		acl_argv_free(argv);
		return false;
	}

	// ȡ��home
	home->clear();
	// �����һ���ַ�Ϊ PATH_SEP ����Ҫ����
	if (*path == PATH_SEP)
		home->push_back(PATH_SEP);
	*home += argv->argv[argv->argc - 3];

	// ȡ�ö�����
	*queueName = argv->argv[argv->argc - 2];
	// ȡ�ö�����Ŀ¼��
	*queueSub = argv->argv[argv->argc - 1];
	acl_argv_free(argv);
	return true;
}

unsigned int queue_manager::hash_queueSub(const char* partName, unsigned width)
{
	acl_assert(width > 0);
	unsigned int n = acl_hash_crc32(partName, strlen(partName));
	return n % width;
}

bool queue_manager::busy(const char* partName)
{
	if (cache_find(partName))
		return true;
	else
		return false;
}

queue_file* queue_manager::cache_find(const char* key)
{
	queue_file* fp = NULL;
	std::map<acl::string, queue_file*>::iterator it;
	m_queueLocker.lock();
	it = m_queueList.find(key);
	if (it != m_queueList.end())
	{
		fp = it->second;
	}
	m_queueLocker.unlock();
	return fp;
}

bool queue_manager::cache_check(queue_file* fp)
{
	std::map<acl::string, queue_file*>::iterator it;
	m_queueLocker.lock();
	it = m_queueList.find(fp->key());
	if (it == m_queueList.end())
	{
		m_queueLocker.unlock();
		logger_error("%s not exist in table", fp->key());
		return false;
	}
	else if (it->second != fp)
	{
		m_queueLocker.unlock();
		logger_error("%s not match, %lu, %lu", fp->key(),
			(unsigned long) fp, (unsigned long)it->second);
		return false;
	}
	m_queueLocker.unlock();
	return true;
}

bool queue_manager::cache_add(queue_file* fp)
{
	bool  ret;

	std::map<acl::string, queue_file*>::iterator it;
	m_queueLocker.lock();
	it = m_queueList.find(fp->key());
	if (it == m_queueList.end())
	{
		m_queueList[fp->key()] = fp;
		ret = true;
	} else {
		ret = false;
	}
	m_queueLocker.unlock();
	return ret;
}

bool queue_manager::cache_del(const char* key)
{
	bool  ret;

	std::map<acl::string, queue_file*>::iterator it;
	m_queueLocker.lock();
	it = m_queueList.find(key);
	if (it != m_queueList.end())
	{
		m_queueList.erase(it);
		ret = true;
	}
	else
	{
		ret = false;
	}

	m_queueLocker.unlock();
	return ret;
}

bool queue_manager::remove(queue_file* fp)
{
	string key(fp->key());
	bool ret = fp->remove();
	delete fp;
	cache_del(key.c_str());
	return ret;
}

bool queue_manager::scan_open(bool scanSub /* = true */)
{
	string path = m_home.c_str();
	path << PATH_SEP << m_queueName.c_str();
	m_scanDir = acl_scan_dir_open(path.c_str(), scanSub ? 1 : 0);
	if (m_scanDir == NULL)
	{
		logger_error("open %s error(%s)", path.c_str(), acl_last_serror());
		return false;
	} else
		return true;
}

void queue_manager::scan_close()
{
	if (m_scanDir)
	{
		acl_scan_dir_close(m_scanDir);
		m_scanDir = NULL;
	}
}

queue_file* queue_manager::scan_next()
{
	if (m_scanDir == NULL)
		logger_fatal("call scan_open first!");

	queue_file* fp = NULL;
	string filePath;

	while (1)
	{
		// ɨ����һ�������ļ�
		const char* fileName = acl_scan_dir_next_file(m_scanDir);
		if (fileName == NULL)
			return NULL;

		string partName, extName;

		if (parse_fileName(fileName, &partName, &extName) == false)
			continue;
		// ����ö����ļ��Ѿ��������ڴ������������
		if (busy(partName.c_str()))
			continue;

		const char* path = acl_scan_dir_path(m_scanDir);
		if (path == NULL)
		{
			logger_error("acl_scan_dir_path error for %s", fileName);
			continue;
		}

		filePath.clear();
		filePath << path << PATH_SEP << fileName;
		fp = NEW queue_file;
		// �Ӵ��̴��Ѿ����ڵĶ����ļ�
		if (fp->open(filePath.c_str()) == false)
		{
			logger_error("open %s error(%s)", filePath.c_str(),
				acl_last_serror());
			delete fp;
			fp = NULL;
			continue;
		}
		if (cache_add(fp) == false)
		{
			logger_error("file(%s) locked", filePath.c_str());
			delete fp;
			fp = NULL;
			continue;
		}
		else
			break;
	}

	return fp;
}

} // namespace acl
