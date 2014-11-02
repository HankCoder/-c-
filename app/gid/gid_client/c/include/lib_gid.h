#ifndef	__LIB_GID_INCLUDE_H__
#define	__LIB_GID_INCLUDE_H__

#ifdef	__cplusplus
extern "C" {
#endif

/* ͨѶЭ�鷽ʽ */
#define	GID_PROTO_CMDLINE	0	/* �����з�ʽ */
#define	GID_PROTO_JSON		1	/* http ��ʽ�����ݸ�ʽΪ json ��ʽ */
#define	GID_PROTO_XML		2	/* http ��ʽ�����ݸ�ʽΪ xml ��ʽ */

/* �������� */
#define	GID_CMD_NEXT	"new_gid"	/* ��ȡ��һ��Ψһ gid */

/* ������ */

/* �ͻ�����صĴ����� */
#define	GID_OK			200	/* ���� */
#define	GID_ERR_INIT		400	/* ��δ��ʼ����Ӧ��Ӧ�ڳ����ʼ��ʱ���ó�ʼ������ gid_client_init */
#define	GID_ERR_CONN		401	/* ���ӷ�����ʧ�� */
#define	GID_ERR_IO		402	/* �������ͨ��ʧ�� */
#define	GID_ERR_PROTO		403	/* Э���ʽ���� */
#define	GID_ERR_SERVER		404	/* �������ڲ����� */

/* �������صĴ����� */
#define GID_ERR_SID		500	/* �Ự ID �Ų��� */
#define GID_ERR_OVERRIDE	501	/* �ﵽ������ֵ */
#define GID_ERR_SAVE		502	/* �洢������ʱ���� */

/**
 * ���ʼ��������ʹ�����ڳ���������Ӧ�õ��øú�����ʼ����
 * @param proto {int} ͨ��Э���ʽ���μ����棺GID_PROTO_XXX
 * @param server_addr {const char*} gid ��������ַ����ʽ��ip:port ��
 *  domain:port �� unix �� /xxx/xxx/xxx
 */
void gid_client_init(int proto, const char *server_addr);

/**
 * ���ݴ���Ż�ô�������
 * @param errnum {int} ����ţ��μ����棺GID_ERR_XXX
 * @return {const char*} ����������Ϣ
 */
const char *gid_client_serror(int errnum);

/**
 * ���� HTTP �����е� URL���ڲ���ȱʡֵ���ɲ�ֱ�ӵ��ô˺���
 * @param url {const char*} URL �ַ���
 */
void gid_client_set_url(const char *url);

/**
 * ���� HTTP �����Ƿ񱣳ֳ����ӣ�ȱʡ����±��ֳ�����
 * @param keepalive {int} �Ƿ񱣳ֳ�����
 */
void gid_client_set_keepalive(int keepalive);

/**
 * �ڳ���������£���������м������жϣ��������Դ�����ȱʡֵΪ 1
 * @param nretry {int} ������Դ���
 */
void gid_client_set_retry_limit(int nretry);

/**
 * �������� gid �����������ӳ�ʱʱ�䣨�룩��ȱʡֵΪ 20 ��
 * @param timeout {int} ��ʱʱ�䣨�룩
 */
void gid_client_set_conn_timeout(int timeout);

/**
 * ��������ͨ�ŵĶ�д��ʱʱ�䣨�룩��ȱʡֵΪ 20 ��
 * @param timeout {int} ��ʱʱ�䣨�룩
 */
void gid_client_set_rw_timeout(int timeout);

/**
 * �����һ�� gid ��
 * @param tag {const char*} ��ʶ���ƣ����Ϊ�գ����ڲ�ȱʡʹ�� default ��ǩ��
 *  ��ֵ�ĸ�ʽΪ��tag_name[:sid]�����е� tag_name Ϊ�����ı�ʶ����sid Ϊ����
 *  �ñ�ʶ�������ȨID�ţ������ֵ�����˵� sid ��ƥ�䣬���ֹ���ʲ����ش���
 *  ������һ���µı�ʶ���󲢲�����һ�� gid ֵʱ��������������� sid ��� sid
 *  �Զ���Ϊ�ñ�ʶ�������ȨID�ţ���������������ʸñ�ʶ����� gid ������ṩ
 *  ����Ȩ ID ��
 * @param errnum {int*} ��ָ��ǿ�ʱ������¼����ʱ�Ĵ����
 * @return {long long int} ��õ���һ��Ψһ gid �ţ������ֵ < 0 ���ʾ����
 */
long long int gid_next(const char *tag, int *errnum);

/* �����ȡ gid �ĺ���ʹ���û��ṩ������������ */

/**
 * ���������з�ʽ�ӷ���˻�ȡ gid ��
 * @param fd {int} ����������ӵ��׽���
 * @param tag {const char*} ��ʶ���ƣ���Ϊ�����ڲ�ȱʡʹ��default
 * @param errnum {int*} ��ָ��ǿ�ʱ������¼����ʱ�Ĵ����
 * @return {long long int} ��õ���һ��Ψһ gid �ţ� *  �����ֵΪ < 0 ���ʾ����
 */
long long int gid_cmdline_get(int fd, const char *tag, int *errnum);

/**
 * ���� http Э�������ݸ�ʽΪ json ��ʽ���ӷ���˻�ȡ gid ��
 * @param fd {int} ����������ӵ��׽���
 * @param tag {const char*} ��ʶ���ƣ���Ϊ�����ڲ�ȱʡʹ��default
 * @param errnum {int*} ��ָ��ǿ�ʱ������¼����ʱ�Ĵ����
 * @return {long long int} ��õ���һ��Ψһ gid �ţ������ֵ < 0 ���ʾ����
 */
long long int gid_json_get(int fd, const char *tag, int *errnum);

/**
 * ���� http Э�������ݸ�ʽΪ xml ��ʽ���ӷ���˻�ȡ gid ��
 * @param fd {int} ����������ӵ��׽���
 * @param tag {const char*} ��ʶ���ƣ���Ϊ�����ڲ�ȱʡʹ��default
 * @param errnum {int*} ��ָ��ǿ�ʱ������¼����ʱ�Ĵ����
 * @return {long long int} ��õ���һ��Ψһ gid �ţ������ֵ < 0 ���ʾ����
 */
long long int gid_xml_get(int fd, const char *tag, int *errnum);

#ifdef	__cplusplus
}
#endif

#endif
