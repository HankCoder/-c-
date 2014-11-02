#pragma once
#include "acl_cpp/acl_cpp_define.hpp"
#include "acl_cpp/stdlib/pipe_stream.hpp"

typedef struct z_stream_s z_stream;

namespace acl {

/**
 * ѹ���������Ͷ��壬�ü��϶�����ѹ���ٶȼ�ѹ���ȵ�һ����ѡ��ʽ
 * ��ѡ��ѹ��ֵԽ�ߣ���ѹ���Ȼ���󣬵�ѹ���ٶȻ�Խ��
 */
typedef enum
{
	zlib_default = -1,      // ȱʡ��ѹ����
	zlib_level0 = 0,        // ��͵�ѹ���ȣ���ʵ���ǲ�ѹ��
	zlib_best_speed = 1,    // ѹ���ٶ�����ѹ����
	zlib_level1 = zlib_best_speed,
	zlib_level2 = 2,
	zlib_level3 = 3,
	zlib_level4 = 4,
	zlib_level5 = 5,
	zlib_level6 = 6,
	zlib_level7 = 7,
	zlib_level8 = 8,
	zlib_best_compress = 9, // ��ߵ�ѹ���ȣ���͵�ѹ���ٶ�
	zlib_level9 = zlib_best_compress
} zlib_level_t;

/**
 * ѹ�����ѹ�����еĻ���ģʽ������ѹ�����ѹ�������Ƿ�����ˢ��
 * ��������Ϊ�˻�ñȽϸߵ�ѹ���ȣ�Ӧ��ѡ�� zlib_flush_off ��ʽ
 */
typedef enum
{
	zlib_flush_off = 0,     // ������ˢ�����û�����
	zlib_flush_partial = 1, // ˢ�²������û�����
	zlib_flush_sync = 2,    // ͬ��ˢ��
	zlib_flush_full = 3,    // ��ȫˢ��
	zlib_flush_finish = 4   // ��ȫˢ�²�ֹͣѹ�����ѹ����
} zlib_flush_t;

class string;

class ACL_CPP_API zlib_stream : public pipe_stream
{
public:
	zlib_stream();
	~zlib_stream();

	/**
	 * ����ʽѹ��
	 * @param in {const char*} Դ����
	 * @param len {int} Դ���ݳ���
	 * @param out {string*} �洢ѹ��������û�������
	 * @param level {zlib_level_t} ѹ�����𣬼���Խ����ѹ����Խ�ߣ�
	 *  ��ѹ���ٶ�Խ��
	 * @return {bool} ѹ�������Ƿ�ɹ�
	 */
	bool zlib_compress(const char* in, int len, string* out,
		zlib_level_t level = zlib_default);

	/**
	 * ����ʽ��ѹ��
	 * @param in {const char*} Դѹ������
	 * @param len {int} Դ���ݳ���
	 * @param out {string*} �洢��ѹ��������û�������
	 * @param have_zlib_header {bool} �Ƿ��� zlib_header ͷ����
	 *  HTTP ����Э�����Ӧ�ý���ֵ��Ϊ false
	 * @param wsize {int} ��ѹ�����еĻ������ڴ�С
	 * @return {bool} ��ѹ�������Ƿ�ɹ�
	 */
	bool zlib_uncompress(const char* in, int len, string* out,
		bool have_zlib_header = true, int wsize = 15);

	///////////////////////////////////////////////////////////////
	//
	//           ����Ϊ��ʽѹ������ʽ��ѹ������
	//
	///////////////////////////////////////////////////////////////

	/**
	 * ��ʼѹ�����̣����������ʽѹ����ʽ�������˳������ǣ�
	 * zip_begin->zip_update->zip_finish������м��κ�һ��
	 * ����ʧ�ܣ���Ӧ�õ��� zip_reset
	 * @param level {zlib_level_t} ѹ�����𣬼���Խ�ߣ���ѹ����
	 *  Խ�ߣ���ѹ���ٶ�Խ��
	 * @return {bool} ѹ����ʼ�������Ƿ�ɹ���ʧ�ܵ�ԭ��һ��
	 *  Ӧ��������Ĳ����Ƿ�
	 */
	bool zip_begin(zlib_level_t level = zlib_default);

	/**
	 * ѭ�����ô˺�����Դ���ݽ���ѹ��
	 * @param in {const char*} Դ����
	 * @param len {int} Դ���ݳ���
	 * @param out {string*} �û����������ú�������ӷ�ʽ���û�
	 *  �ṩ�Ļ����������ѹ���Ľ�����û�Ӧ�������жϵ��ñ�����
	 *  ǰ��Ļ��������ȣ���ȷ���ɸú�����ӵ����ݳ��ȣ�������
	 *  ѡ��� zlib_flush_t �Ĳ�ͬ���û����������ݿ���δ�ش�ȡ
	 *  ���еĽ��
	 * @param flag {zlib_flush_t} ѹ�������е����ݻ��巽ʽ
	 *  zlib_flush_off: ���ݽ�����ܲ�������ˢ�����û���������
	 *    zlib �Ȿ�����ˢ�µķ�ʽ���Ӷ����ܻ��ýϸߵ�ѹ����
	 *  zlib_flush_partial: ���ݽ�����ܻᲿ��ˢ�����û�������
	 *  zlib_flush_sync: ��������ͬ��ˢ�����û�������
	 *  zlib_flush_full: �� zlib �⻺�����ݽ��ȫ��ˢ�����û�������
	 *  zlib_flush_finish: ���ñ����������ѹ�����̽�����ͬʱ�Ὣ
	 *    ���н������ˢ�����û���������һ��ò�������Ҫ���ã���Ϊ��
	 *    ���� zip_finish �󣬻��Զ������еĻ�������ˢ�����û�������
	 * @return {bool} ѹ�������Ƿ�ʧ��
	 */
	bool zip_update(const char* in, int len, string* out,
		zlib_flush_t flag = zlib_flush_off);

	/**
	 * ���ñ�������ʾѹ�����̽���
	 * @param out {string} �û����������ú����Ὣ zlib �⻺������
	 *  ����������ӵķ�ʽ��ˢ�����û�������
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool zip_finish(string* out);

	/**
	 * ����ѹ����״̬��һ��ֻ�е�ѹ�����̳���ʱ�Ż���ñ�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool zip_reset();

	/**
	 * ��ʼ��ѹ�����̣����������ʽ��ѹ����ʽ�������˳������ǣ�
	 * unzip_begin->unzip_update->unzip_finish������м��κ�һ��
	 * ����ʧ�ܣ���Ӧ�õ��� unzip_reset
	 * @param have_zlib_header {bool} �Ƿ��� zlib_header ͷ����
	 *  HTTP ����Э�����Ӧ�ý���ֵ��Ϊ false
	 * @param wsize {int} ��ѹ�����еĻ������ڴ�С
	 * @return {bool} ��ѹ����ʼ�������Ƿ�ɹ���ʧ�ܵ�ԭ��һ��
	 *  Ӧ��������Ĳ����Ƿ�
	 */
	bool unzip_begin(bool have_zlib_header = true, int wsize = 15);

	/**
	 * ѭ�����ô˺�����Դ���ݽ��н�ѹ��
	 * @param in {const char*} ѹ����Դ����
	 * @param len {int} Դ���ݳ���
	 * @param out {string*} �û����������ú�������ӷ�ʽ���û�
	 *  �ṩ�Ļ���������ӽ�ѹ�Ľ�����û�Ӧ�������жϵ��ñ�����
	 *  ǰ��Ļ��������ȣ���ȷ���ɸú�����ӵ����ݳ��ȣ�������
	 *  ѡ��� zlib_flush_t �Ĳ�ͬ���û����������ݿ���δ�ش�ȡ
	 *  ���еĽ��
	 * @param flag {zlib_flush_t} ��ѹ�������е����ݻ��巽ʽ
	 *  zlib_flush_off: ���ݽ�����ܲ�������ˢ�����û���������
	 *    zlib �Ȿ�����ˢ�µķ�ʽ
	 *  zlib_flush_partial: ���ݽ�����ܻᲿ��ˢ�����û�������
	 *  zlib_flush_sync: ��������ͬ��ˢ�����û�������
	 *  zlib_flush_full: �� zlib �⻺�����ݽ��ȫ��ˢ�����û�������
	 *  zlib_flush_finish: ���ñ������������ѹ�����̽�����ͬʱ�Ὣ
	 *    ���н������ˢ�����û���������һ��ò�������Ҫ���ã���Ϊ��
	 *    ���� zip_finish �󣬻��Զ������еĻ�������ˢ�����û�������
	 * @return {bool} ��ѹ�������Ƿ�ʧ��
	 */
	bool unzip_update(const char* in, int len, string* out,
		zlib_flush_t flag = zlib_flush_off);

	/**
	 * ���ñ�������ʾ��ѹ�����̽���
	 * @param out {string} �û����������ú����Ὣ zlib �⻺������
	 *  ����������ӵķ�ʽ��ˢ�����û�������
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool unzip_finish(string* out);

	/**
	 * ���ý�ѹ����״̬��һ��ֻ�е���ѹ�����̳���ʱ�Ż���ñ�����
	 * @return {bool} �Ƿ�ɹ�
	 */
	bool unzip_reset();

	///////////////////////////////////////////////////////////////

	bool pipe_zip_begin(zlib_level_t level = zlib_default,
		zlib_flush_t flag = zlib_flush_off);
	bool pipe_unzip_begin(zlib_flush_t flag = zlib_flush_off);

	// pipe_stream �麯������

	virtual int push_pop(const char* in, size_t len,
		string* out, size_t max = 0);
	virtual int pop_end(string* out, size_t max = 0);
	virtual void clear();
protected:
private:
	z_stream* zstream_;
	bool finished_;
	bool is_compress_;
	zlib_flush_t flush_;

	bool update(int (*func)(z_stream*, int), zlib_flush_t flag,
		const char* in, int len, string* out);
	bool flush_out(int (*func)(z_stream*, int),
		zlib_flush_t flag, string* out);
};

} // namespace acl
