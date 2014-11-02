#ifndef __ICMP_INCLUDE_H__
#define __ICMP_INCLUDE_H__

#include "lib_acl.h"
#include <time.h>
#include "icmp/lib_icmp_type.h"

typedef struct ICMP_TIMER ICMP_TIMER;
typedef struct ICMP_STREAM ICMP_STREAM;
typedef struct IP_HDR IP_HDR;
typedef struct ICMP_HDR ICMP_HDR;

/* in icmp_timer.cpp */
/**< ��ʱ����Ϣ�ṹ */
struct ICMP_TIMER {
	/**< ���ö�ʱ���� */
	time_t (*request)(ICMP_TIMER* timer, ICMP_PKT* pkt, int delay);
	/**< ȡ����ʱ���� */
	time_t (*cancel)(ICMP_TIMER* timer, ICMP_PKT* pkt);
	/**< ���Ҳ�ɾ����ʱ���� */
	ICMP_PKT* (*find_delete)(ICMP_TIMER* timer, unsigned short i_seq);
	/**< ������һ����ʱ���� */
	ICMP_PKT* (*popup)(ICMP_TIMER* timer);                    

	ACL_RING timer_header;      /**< ��ʱ���������ͷ */
	time_t present;             /**< ��ǰʱ�� */
	time_t time_left;           /**< ����һ����ʱ�����ʱ�� */
};

/**< ��������� ICMP ����ͨ���� */
struct ICMP_STREAM {
	ACL_VSTREAM *vstream;       /**< ͬ����ָ�� */
	ACL_ASTREAM *astream;       /**< �첽��ָ�� */

	struct sockaddr_in from;    /**< �������ݵ�Դ��ַ */
	int   from_len;             /**< �洢�� from �еĵ�ַ��С */

	ICMP_HOST *curr_host;       /**< ��ǰ��Ŀ���������� */
};

/**< ICMP ͨ�ž�� */
struct ICMP_CHAT {
	/* ͨ�õĳ�Ա���� */
	unsigned short seq_no;      /**< ÿ�� icmp �������к� */
	ICMP_STREAM *is;            /**< ��ĳ���ͻ�����ص��� */
	ACL_RING host_head;         /**< ��ǰ ICMP ͨ�Ŷ����е�������ɵ��� */
	unsigned short pid;         /**< �ɵ�ǰ����ID��ʾ */
	unsigned int tid;           /**< ���߳�ID��ʾ */
	int   check_tid;            /**< �Ƿ�����Ӧ���е� tid ֵ */

	/* �첽IO�ĳ�Ա���� */
	ACL_AIO *aio;               /**< �첽IO��� */
	int   count;                /**< ��ǰ ICMP ͨ�Ŷ������Ѿ���ɵ������� */
	int   check_inter;          /**< ÿ����������һ�¶�ʱ��������� */
	ICMP_TIMER *timer;          /**< �������ȴ���Ӧ��Ķ�ʱ�� */
};

#define ICMP_ECHO               8
#define ICMP_ECHOREPLY          0

#define ICMP_MIN                8  /**< Minimum 8 byte icmp packet (just header) */

/**< IP Э��ͷ�ṹ */
struct IP_HDR {
#if __BYTE_ORDER == __LITTLE_ENDIAN
	unsigned int h_len:4;          /**< length of the header */
	unsigned int version:4;        /**< Version of IP */
#elif __BYTE_ORDER == __BIG_ENDIAN
	unsigned int version:4;        /**< Version of IP */
	unsigned int h_len:4;          /**< length of the header */
#else
# error "unknown __BYTE_ORDER"
#endif

	unsigned char tos;             /**< Type of service */
	unsigned short total_len;      /**< total length of the packet */
	unsigned short ident;          /**< unique identifier */
	unsigned short frag_and_flags; /**< flags */
	unsigned char  ttl;            /**< time to live */
	unsigned char proto;           /**< protocol (TCP, UDP etc) */
	unsigned short checksum;       /**< IP checksum */

	unsigned int source_ip;        /**< source IP*/
	unsigned int dest_ip;          /**< dest IP */
};

/**< ICMP header */
struct ICMP_HDR {
	unsigned char type;
	unsigned char code;          /* type sub code */
	unsigned short cksum;
	unsigned short id;
	unsigned short seq;
};

#define MIN_PACKET     32 
#define MAX_PACKET     1024 

/**< ICMP �����ݽṹ */
struct ICMP_PKT {
	/* ���͵����ݰ� */
	ICMP_HDR hdr;                   /**< icmp ͷ */
	union {
		unsigned int tid;           /**< �߳�ID�� */
		char data[MAX_PACKET];      /**< icmp ������ */
	} body;

	/* ���ش洢�����ݳ�Ա����ҪΪ�˽������� */
	ICMP_HOST *icmp_host;           /**< Ŀ������ */
#define pkt_chat icmp_host->chat    /**< ICMP_PKT ��ָ�� ICMP_CHAT �Ŀ�ݷ�ʽ */

	ACL_RING pkt_ring;              /**< �������ݰ���˫���� */
	ACL_RING timer_ring;            /**< ��ʱ����� */

	size_t write_len;               /**< ���͵����ݳ��� */
	size_t read_len;                /**< ���յ����ݳ��� */

	size_t dlen;                    /**< ���ݰ��������峤�� */
	struct timeval stamp;           /**< time stamp */

	/* ��Ӧ�����ݰ��ķ������ */
	ICMP_PKT_STATUS pkt_status;
};

#define RING_TO_PKT(r) \
	((ICMP_PKT *) ((char *) (r) - offsetof(ICMP_PKT, pkt_ring)))
#define ICMP_PKT_NEXT(head, curr) \
	(acl_ring_succ(curr) != (head) ? RING_TO_PKT(acl_ring_succ(curr)) : NULL)

#define RING_TO_HOST(r) \
	((ICMP_HOST *) ((char *) (r) - offsetof(ICMP_HOST, host_ring)))
#define ICMP_HOST_NEXT(head, curr) \
	(acl_ring_succ(curr) != (head) ? RING_TO_HOST(acl_ring_succ(curr)) : NULL)

#endif

