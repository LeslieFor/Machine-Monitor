#ifndef _SYSTEM_NET_H_
#define _SYSTEM_NET_H_

#include <net/if.h>

typedef struct sys_net_s sys_net_t;

struct sys_net_s {
	char mac[64];
	char local_ipv4[64];
	char local_ipv6[64];
	char extern_ipv4[64];
	char extern_ipv6[64];
};


typedef struct ifreq_handler_s ifreq_handler_t;

typedef int (*ifreq_handler_p)(ifreq_handler_t *, struct ifreq *);

struct ifreq_handler_s {
    ifreq_handler_p handler;
};


typedef struct sys_net_ctx_s sys_net_ctx_t;

struct sys_net_ctx_s {
    ifreq_handler_t *handle;
};


typedef struct AF_filter_s AF_filter_t;

struct AF_filter_s {
    ifreq_handler_p handler;
    int type;
};

sys_net_ctx_t *new_net_ctx(int type);
int get_net_info(sys_net_ctx_t *net_ctx);

#endif

