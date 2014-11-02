#include "StdAfx.h"
#ifndef ACL_PREPARE_COMPILE

#include "stdlib/acl_define.h"
#ifdef	ACL_UNIX
#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#endif
#include <time.h>

#ifdef ACL_BCB_COMPILER
#pragma hdrstop
#endif

#include "stdlib/acl_stdlib.h"
#include "net/acl_vstream_net.h"
#include "net/acl_sane_inet.h"
#include "net/acl_res.h"

#endif

#include "rfc1035.h"

ACL_RES *acl_res_new(const char *dns_ip, unsigned short dns_port)
{
	const char *myname = "acl_res_new";
	ACL_RES *res;

	if (dns_ip == NULL || *dns_ip == 0)
		acl_msg_fatal("%s: dns_ip invalid", myname);
	if (dns_port <= 0)
		dns_port = 53;

	res = acl_mycalloc(1, sizeof(ACL_RES));
	res->cur_qid = (unsigned short) time(NULL);

	ACL_SAFE_STRNCPY(res->dns_ip, dns_ip, sizeof(res->dns_ip));
	res->dns_port = dns_port;

	res->conn_timeout = 10;
	res->rw_timeout = 10;
	res->transfer = ACL_RES_USE_UDP;

	return (res);
}

void acl_res_free(ACL_RES *res)
{
	if (res)
		acl_myfree(res);
}

static int udp_res_lookup(ACL_RES *res, const char *data, int dlen, char *buf, int size)
{
	const char *myname = "udp_res_lookup";
	int   ret;
	ACL_SOCKET fd;
	struct sockaddr_in addr;

	fd = socket(PF_INET, SOCK_DGRAM, 0);
	if (fd == ACL_SOCKET_INVALID)
		acl_msg_fatal("%s: socket create error", myname);

	memset(&addr, 0, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(res->dns_port);
	addr.sin_addr.s_addr = inet_addr(res->dns_ip);

	ret = sendto(fd, data, dlen, 0, (struct sockaddr *) &addr, (int) sizeof(addr));
	if (ret < 0) {
		acl_socket_close(fd);
		res->errnum = ACL_RES_ERR_SEND;
		return (-1);
	}

	ret = acl_read_wait(fd, res->rw_timeout);
	if (ret < 0) {
		acl_socket_close(fd);
		res->errnum = ACL_RES_ERR_RTMO;
		return (-1);
	}

	ret = recv(fd, buf, size, 0);
	acl_socket_close(fd);

	if (ret <= 0) {
		res->errnum = ACL_RES_ERR_SEND;
		return (-1);
	}

	return (ret);
}

static int tcp_res_lookup(ACL_RES *res, const char *data, int dlen, char *buf, int size)
{
	ACL_VSTREAM *stream = NULL;
	char  addr[256], *pbuf = NULL, *ptr, tbuf[3];
	unsigned short nsz, n;
	int   ret;

#undef RETURN
#define	RETURN(_x_) do { \
	if (pbuf) \
		acl_myfree(pbuf); \
	if (stream) \
		acl_vstream_close(stream); \
	return (_x_); \
} while (0)

	snprintf(addr, sizeof(addr), "%s:%d", res->dns_ip, res->dns_port);
	stream = acl_vstream_connect(addr, ACL_BLOCKING, res->conn_timeout,
				res->rw_timeout, 1024);
	if (stream == NULL) {
		res->errnum = ACL_RES_ERR_CONN;
		RETURN (-1);
	}

	pbuf = acl_mycalloc(1, dlen + 2);
	nsz = htons((short) dlen);
	ptr = pbuf;
	memcpy(ptr, &nsz, 2);
	ptr += 2;
	memcpy(ptr, data, dlen);

	ret = acl_vstream_writen(stream, pbuf, dlen + 2);
	if (ret == ACL_VSTREAM_EOF) {
		res->errnum = ACL_RES_ERR_SEND;
		RETURN (-1);
	}

	memset(tbuf, 0, sizeof(tbuf));

	ret = acl_vstream_readn(stream, &n, 2);
	if (ret == ACL_VSTREAM_EOF) {
		res->errnum = ACL_RES_ERR_READ;
		RETURN (-1);
	}

	nsz = ntohs(n);

	size = size > nsz ? nsz : size;
	ret = acl_vstream_readn(stream, buf, size);
	if (ret == ACL_VSTREAM_EOF) {
		res->errnum = ACL_RES_ERR_READ;
		RETURN (-1);
	}

	RETURN (ret);
#ifdef	ACL_BCB_COMPILER
	return (-1);
#endif
}

static int res_lookup(ACL_RES *res, const char *data, int dlen,
					  char *buf, int size)
{
	if (res->transfer == ACL_RES_USE_TCP)
		return (tcp_res_lookup(res, data, dlen, buf, size));
	else
		return (udp_res_lookup(res, data, dlen, buf, size));
}

ACL_DNS_DB *acl_res_lookup(ACL_RES *res, const char *domain)
{
	const char *myname = "acl_res_lookup";
	ACL_DNS_DB *dns_db;
	char  buf[1024];
	int   ret, i;
	rfc1035_message *answers;
	ACL_HOSTNAME *phost;
	time_t  begin;

	if (res == NULL || domain == NULL || *domain == 0)
		acl_msg_fatal("%s: input invalid", myname);

	memset(buf, 0, sizeof(buf));
	ret = rfc1035BuildAQuery(domain, buf, sizeof(buf), res->cur_qid++, NULL);

	(void) time(&begin);
	ret = res_lookup(res, buf, ret, buf, sizeof(buf));
	res->tm_spent = time(NULL) - begin;

	if (ret <= 0)
		return (NULL);

	ret = rfc1035MessageUnpack(buf, ret, &answers);
	if (ret < 0) {
		res->errnum = ret;
		return (NULL);
	} else if (ret == 0) {
		rfc1035MessageDestroy(answers);
		res->errnum = ACL_RES_ERR_NULL;
		return (NULL);
	}

	dns_db = acl_netdb_new(domain);
	if (dns_db == NULL) {
		rfc1035MessageDestroy(answers);
		acl_msg_error("%s: acl_netdb_new error", myname);
		return (NULL);
	}

	for (i = 0; i < ret; i++) {
		if (answers->answer[i].type == RFC1035_TYPE_A) {
			phost = acl_mycalloc(1, sizeof(ACL_HOSTNAME));
			if (phost == NULL) {
				acl_msg_error("%s: calloc error(%s)",
					myname, acl_last_strerror(buf, sizeof(buf)));
				acl_netdb_free(dns_db);
				rfc1035MessageDestroy(answers);
				return (NULL);
			}

			memcpy(&phost->saddr.sin_addr, answers->answer[i].rdata, 4);
			/* bugfix: 2009.12.8
			 *  ACL_SAFE_STRNCPY(phost->ip, inet_ntoa(phost->saddr.sin_addr),
			 *  	sizeof(phost->ip));
			 */
			acl_inet_ntoa(phost->saddr.sin_addr, phost->ip, sizeof(phost->ip));
			phost->ttl = answers->answer[i].ttl;

			if (acl_array_append(dns_db->h_db, phost) < 0) {
				acl_msg_error("%s, %s(%d): array append error(%s)",
					__FILE__, myname, __LINE__,
					acl_last_strerror(buf, sizeof(buf)));
				acl_netdb_free(dns_db);
				rfc1035MessageDestroy(answers);
				return (NULL);
			}       

			dns_db->size++;
		} else {
			if (acl_msg_verbose)
				acl_msg_error("%s: can't print answer type %d, domain %s",
					myname, (int) answers->answer[i].type, domain);
		}
	}

	rfc1035MessageDestroy(answers);
	
	return (dns_db);
}

const char *acl_res_strerror(int errnum)
{
	int   i;
	struct __ERRMSG {
		int  errnum;
		const char *msg;
	};
	static const struct __ERRMSG errmsg[] = {
		{ ACL_RES_ERR_SEND, "send msg error" },
		{ ACL_RES_ERR_READ, "read msg error" },
		{ ACL_RES_ERR_RTMO, "read timeout" },
		{ ACL_RES_ERR_NULL, "result emplty" },
		{ ACL_RES_ERR_CONN, "connect error" },
		{ 0, NULL },
	};

	for (i = 0; errmsg[i].errnum != 0; i++) {
		if (errmsg[i].errnum == errnum)
			return (errmsg[i].msg);
	}

	return (rfc1035Strerror(errnum));
}

const char *acl_res_errmsg(const ACL_RES *res)
{
	const char *myname = "acl_res_errmsg";

	if (res == NULL)
		acl_msg_fatal("%s: res null", myname);

	return (acl_res_strerror(res->errnum));
}
