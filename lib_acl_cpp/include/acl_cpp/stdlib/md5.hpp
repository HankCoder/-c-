#pragma once
#include "acl_cpp/acl_cpp_define.hpp"

namespace acl {

class istream;

class ACL_CPP_API md5
{
public:
	md5();
	~md5();

	/**
	 * ����ѭ�����ô˺��������Ҫ�� md5 ������
	 * @param dat {const void*} ���ݵ�ַ
	 * @param len {size_t} dat ���ݳ���
	 * @return {md5&}
	 */
	md5& update(const void* dat, size_t len);

	/**
	 * ������ñ�������ʾ md5 ���̽���
	 * @return {md5&}
	 */
	md5& finish();

	/**
	 * ���� md5 �㷨����״̬���Ӷ������ظ�ʹ��ͬһ�� md5 ����
	 * @return {md5&}
	 */
	md5& reset();

	/**
	 * ��ö����Ƹ�ʽ�� md5 ���ֵ
	 * @return {const char*} ����ֵ��Զ�ǿգ��һ���������Ϊ 16 �ֽ�
	 */
	const char* get_digest() const;

	/**
	 * ������ַ�����ʽ��ʾ�� m5 ���ֵ
	 * @return {const char*} ����ֵ��Զ�ǿգ����� \0 ��β�����ַ���
	 *  ����Ϊ 32 �ֽ�
	 */
	const char* get_string() const;

	/**
	 * �������� md5 �㷨����ǩ��ֵ��ȡ�� 128 λ (�� 16 �ֽ�) �����ƽ��
	 * @param dat {const void*} Դ����
	 * @param dlen {size_t} dat ���ݳ���
	 * @param key {const char*} �ǿ�ʱ��Ϊ������
	 * @param klen {size_t} key �ǿ�ʱ��ʾ key �ĳ���
	 * @param out {void*} �洢 md5 ���
	 * @param size {size_t} out ��С������Ӧ��Ϊ 16 �ֽ�
	 * @return {const char*} ���ش洢����ĵ�ַ(�� out ��ַ)
	 */
	static const char* md5_digest(const void *dat, size_t dlen,
		const void *key, size_t klen, void* out, size_t size);

	/**
	 * �������� md5 �㷨����ǩ��ֵ��ȡ���ַ�����ʽ�Ľ��
	 * @param dat {const void*} Դ����
	 * @param dlen {size_t} dat ���ݳ���
	 * @param key {const char*} �ǿ�ʱ��Ϊ������
	 * @param klen {size_t} key �ǿ�ʱ��ʾ key �ĳ���
	 * @param out {void*} �洢 md5 ���
	 * @param size {size_t} out ��С������Ӧ��Ϊ 33 �ֽ�
	 * @return {const char*} ���ش洢����ĵ�ַ(�� out ��ַ)��
	 *  �ҷ���ֵΪ�� \0 ��β�� 32 �ֽڳ���(���� \0)�ַ���
	 */
	static const char* md5_string(const void *dat, size_t dlen,
		const void *key, size_t klen, char* out, size_t size);

	/**
	 * ���ļ��е������� md5 �㷨����ǩ��ֵ����ȡ���ַ�����ʽ���
	 * @param path {const char*} �ļ�ȫ·��
	 * @param key {const char*} �ǿ�ʱ��Ϊ������
	 * @param klen {size_t} key �ǿ�ʱ��ʾ key �ĳ���
	 * @param out {void*} �洢 md5 ���
	 * @param size {size_t} out ��С������Ӧ��Ϊ 33 �ֽ�
	 * @return {int64) ��������ȡ���ļ����ݵĳ��ȣ���������·��� -1
	 *  1) ���ļ�ʧ��
	 *  2) δ���ļ��ж�������
	 *  3) out ��������С size С�� 33 �ֽڳ���
	 */
#ifdef WIN32
	static __int64 md5_file(const char* path, const void *key,
		size_t klen, char* out, size_t size);
#else
	static long long int md5_file(const char* path, const void *key,
		size_t klen, char* out, size_t size);
#endif

	/**
	 * ���ļ��е������� md5 �㷨����ǩ��ֵ����ȡ���ַ�����ʽ���
	 * @param in {istream&} �����ļ���
	 * @param key {const char*} �ǿ�ʱ��Ϊ������
	 * @param klen {size_t} key �ǿ�ʱ��ʾ key �ĳ���
	 * @param out {void*} �洢 md5 ���
	 * @param size {size_t} out ��С������Ӧ��Ϊ 33 �ֽ�
	 * @return {int64) ��������ȡ���ļ����ݵĳ��ȣ���������·��� -1:
	 *  1) δ���������ж�ȡ����ʱ
	 *  2) out ��������С size С�� 33 �ֽڳ���
	 */
#ifdef WIN32
	static __int64 md5_file(istream& in, const void *key,
		size_t klen, char* out, size_t size);
#else
	static long long int md5_file(istream& in, const void *key,
		size_t klen, char* out, size_t size);
#endif

	/**
	 * �� 16 �ֽڳ��ȵ� MD5 �����ƽ��ת��Ϊ 32 �ֽڳ��ȵ��ַ���
	 * @param in {const void*} 128 λ(�� 16 �ֽ�)�� md5 ֵ���� in �����ݳ���
	 *  ����Ӧ�� >= 16������������ڴ���Խ��
	 * @param out {char*} �洢�ַ�����ʽ�Ľ��
	 * @param size {size_t} out �ڴ��С������Ϊ 33 �ֽڣ������ڲ���������
	 * @return {const char*} ���ش洢����ĵ�ַ(�� out ��ַ)��
	 *  �ҷ���ֵΪ�� \0 ��β�� 32 �ֽڳ���(���� \0)�ַ���
	 */
	static const char* hex_encode(const void *in, char* out, size_t size);

private:
	unsigned int buf_[4];
	unsigned int bytes_[2];
	unsigned int in_[16];

	unsigned char digest_[16];
	unsigned char digest_s_[33];
};

}  // namespace acl
