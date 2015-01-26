#pragma once

struct ACL_SCAN_DIR;

namespace acl
{

class string;

class ACL_CPP_API scan_dir
{
public:
	scan_dir();
	~scan_dir();

	/**
	 * ��Ŀ¼
	 * @param path {const char*} Ŀ¼·�����ǿ�ָ��
	 * @param recursive {bool} �Ƿ�����ݹ�ɨ��Ŀ¼
	 * @return {bool} ��Ŀ¼�Ƿ�ɹ�
	 */
	bool open(const char* path, bool recursive = true);

	/**
	 * �ر�Ŀ¼��ͬʱ�ͷ��ڲ���Դ
	 */
	void close();

	/**
	 * ɨ����һ���ļ�(����Ŀ¼���Զ�����)������ open ָ��������ݹ�ɨ��ѡ��
	 * (�� recursive = true)����ú�����ݹ�ɨ������Ŀ¼��������Ŀ¼
	 * @param full {bool} �Ƿ���Ҫ�����ļ�ȫ·��
	 * @return {const char*} �� NULL ��ʾ��ɨ�赽���ļ����������ʾɨ�����
	 *  ��Ŀ¼��δ��
	 */
	const char* next_file(bool full = false);

	/**
	 * ɨ����һ��Ŀ¼(�����ļ��� "." �� ".." ������)������ open ָ��������
	 * ����ݹ�ɨ����(�� recursive = true)����ú�����ݹ�ɨ������Ŀ¼����
	 * ����Ŀ¼
	 * @param full {bool} �Ƿ���Ҫ����Ŀ¼ȫ·��
	 * @return {const char*} �� NULL ��ʾ��ɨ�赽��Ŀ¼���������ʾɨ����
	 *  �ϻ�Ŀ¼��δ��
	 */
	const char* next_dir(bool full = false);

	/**
	 * ɨ����һ��Ŀ¼���ļ������� open ָ��������ݹ�ɨ����(�� resursive
	 * = true)����ú�����ݹ�ɨ������Ŀ¼��������Ŀ¼���ļ�
	 * @param full {bool} �Ƿ���Ҫ����Ŀ¼���ļ���ȫ·�������Ϊ true ��
	 *  ��ȫ·��������ֻ�����ļ�����Ŀ¼���Ҷ�����·��
	 * @param is_file {bool*} �����ؽ���ǿ�ʱ���õ�ַ�洢��ֵ��ʾ��ɨ�赽
	 *  ���Ƿ����ļ������Ϊ true ��Ϊ�ļ�������ΪĿ¼
	 * @return {const char*} �� NULL ��ʾ��ɨ�赽��Ŀ¼�����ļ����������
	 *  ʾɨ����ϻ�Ŀ¼��δ�� 
	 */
	const char* next(bool full = false, bool* is_file = NULL);

	/**
	 * ��õ�ǰɨ��������ڵ�Ŀ¼·�������ص�·��β��������·���ָ��� '/'
	 * �� '\\' (win32)�������·����/home/zsx/����᷵�� /home/zsx�����
	 * ·��Ϊ��·����/ ��� '/' ���ᱣ������ WIN32 �£�����������
	 * C:\Users\zsx ��·��
	 * @return {const char*} ��Ŀ¼��ʱ�ú������طǿ�ָ�룬���򷵻� NULL
	 */
	const char* curr_path();

	/**
	 * ��õ�ǰ����ɨ�������ɨ�����ļ���
	 * @param full {bool} �Ƿ���Ҫͬʱ�����ļ�ȫ·��
	 * @return {bool} ���Ŀ¼δ�ҿ���ǰɨ��Ĳ����ļ����򷵻� NULL
	 */
	const char* curr_file(bool full = false);

	/**
	 * ���ص�ǰ�Ѿ�ɨ���Ŀ¼�ĸ���
	 * @return {size_t}
	 */
	size_t dir_count() const;

	/**
	 * ���ص�ǰ�Ѿ�ɨ����ļ��ĸ���
	 * @return {size_t}
	 */
	size_t file_count() const;

#ifdef WIN32
	/**
	 * ��õ�ǰ�Ѿ�ɨ����ļ���Ŀ¼��С���ܺ�
	 * @return {acl_uint64}
	 */
	unsigned __int64 scaned_size() const;

	/**
	 * open �ɹ�����ñ�����ͳ�Ƹ�Ŀ¼�������ļ���Ŀ¼��С���ܺ�
	 * @param nfiles {int*} �ǿ�ʱ�洢��ɾ�����ļ�����
	 * @param ndirs {int*} �ǿ�ʱ�洢��ɾ����Ŀ¼����
	 * @return {acl_uint64} ��Ŀ¼�������ļ���Ŀ¼��С���ܺ�
	 */
	unsigned __int64 all_size(int* nfiles = NULL, int* ndirs = NULL) const;

	/**
	 * ��Ŀ¼�������ļ���Ŀ¼��С���ܺ�
	 * @param path {const char*} ��Ҫ��ɾ���ĸ�Ŀ¼
	 * @param recursive {bool} �Ƿ�ݹ�ɨ������Ŀ¼
	 * @param nfiles {int*} �ǿ�ʱ�洢��ɾ�����ļ�����
	 * @param ndirs {int*} �ǿ�ʱ�洢��ɾ����Ŀ¼����
	 * @return {acl_uint64} ��Ŀ¼�������ļ���Ŀ¼��С���ܺ�
	 */
	static unsigned __int64 all_size(const char* path, bool recursive = true,
		int* nfiles = NULL, int* ndirs = NULL);

	/**
	 * open �ɹ�����ñ�����ɾ����Ŀ¼�µ������ļ���Ŀ¼
	 * @param nfiles {int*} �ǿ�ʱ�洢��ɾ�����ļ�����
	 * @param ndirs {int*} �ǿ�ʱ�洢��ɾ����Ŀ¼����
	 * @return {acl_unint64} ɾ���������ļ���Ŀ¼��С�ܺ�(�ֽڵ�λ)
	 */
	unsigned __int64 remove_all(int* nfiles = NULL, int* ndirs = NULL) const;

	/**
	 * ɾ����Ŀ¼�µ������ļ���Ŀ¼
	 * @param path {const char*} ��Ҫ��ɾ���ĸ�Ŀ¼
	 * @param recursive {bool} �Ƿ�ݹ�ɨ������Ŀ¼
	 * @param nfiles {int*} �ǿ�ʱ�洢��ɾ�����ļ�����
	 * @param ndirs {int*} �ǿ�ʱ�洢��ɾ����Ŀ¼����
	 * @return {acl_unint64} ɾ���������ļ���Ŀ¼��С�ܺ�(�ֽڵ�λ)
	 */
	static unsigned __int64 remove_all(const char* path, bool recursive = true,
		int* nfiles = NULL, int* ndirs = NULL);
#else
	unsigned long long scaned_size() const;
	unsigned long long all_size(int* nfiles = NULL, int* ndirs = NULL) const;
	static unsigned long long all_size(const char* path, bool recursive = true,
		int* nfiles = NULL, int* ndirs = NULL);
	unsigned long long remove_all(int* nfiles = NULL, int* ndirs = NULL) const;
	static unsigned long long remove_all(const char* path, bool recursive = true,
		int* nfiles = NULL, int* ndirs = NULL);
#endif

	/**
	 * ��õ�ǰ�������е�·��
	 * @param out {string&} �洢���
	 * @return {bool} �Ƿ�ɹ���õ�ǰ��������·��
	 */
	static bool get_cwd(string& out);

private:
	char* path_;
	ACL_SCAN_DIR* scan_;
	string* path_buf_;
	string* file_buf_;
};

}  // namespace acl
