#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "system_net.h"

AF_filter_t *new_AF_filter(int);
int af_filter_handler(ifreq_handler_t *hand, struct ifreq *ifq);

sys_net_ctx_t *new_net_ctx(int type)
{
    sys_net_ctx_t *net_ctx = NULL;

    net_ctx = (sys_net_ctx_t *) malloc(sizeof(*net_ctx));
    if (net_ctx == NULL)
    {
        printf("malloc sys_net_ctx_t error\n");
        return NULL;
    }

    net_ctx->handle = (ifreq_handler_t *) new_AF_filter(0);

    return net_ctx;
}


AF_filter_t *new_AF_filter(int AF_type)
{
    AF_filter_t *af_flt = NULL;
    af_flt = (AF_filter_t *) malloc(sizeof(*af_flt));
    if (af_flt == NULL)
    {
        printf("malloc AF_filter error\n");
        return NULL;
    }

    af_flt->type    = AF_type;
    af_flt->handler = &af_filter_handler;

    return af_flt;
}

int af_filter_handler(ifreq_handler_t *hand, struct ifreq *ifq)
{
    AF_filter_t *af_flt = (AF_filter_t *)hand;
    printf("type: %d \t interface: %s\n", af_flt->type, ifq->ifr_name);

    return 0;
}

int get_net_info(sys_net_ctx_t *net_ctx)
{
    int   i      = 0;
    int   len    = 0;
    int   prelen = 0;
    int   sockfd = 0;
    char *buf    = NULL;

    struct ifconf  ifc;
    struct ifreq  *ifr   = NULL;
    struct ifreq  *end   = NULL;
    struct ifreq  *start = NULL;
    

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sockfd < 0)
    {
        printf("socket error\n");
        return 0;
    }

    len = 10 * sizeof(struct ifreq);
 
    /* i for escape loop forever */
    for ( i = 0; i < 50; i++)
    {
        printf("for top len: %d\n", len);
        buf = malloc(len);
        if (buf == NULL)
        {
            printf("malloc buf error\n");
            continue;
        }

        ifc.ifc_buf = buf;

        if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0)
        {
            if (errno != EINVAL)
            {
                printf("ioctl SIOCGIFCONF error: %d\n", errno);
                free(buf);
                return -1;
            }
        }
        else
        {
            if (ifc.ifc_len == 0)
            {
                continue;
            }
            if (prelen == ifc.ifc_len)
            {
                break;
            }
            prelen = ifc.ifc_len;
        }

        len *= 2;
        free(buf);
    }

    if (i == 10)
    {
        return -1;
    }
    start = ifc.ifc_req;
    end   = ifc.ifc_req + (ifc.ifc_len / sizeof(struct ifreq));

    for (; start != end; start++)
    {
        if (net_ctx->handle != NULL && net_ctx->handle->handler != NULL)
        {
            net_ctx->handle->handler(net_ctx->handle, start);
        }
        else
        {
            printf("handle or handler is NULL\n");
            return -1;
        }
    }

    return 0;
}












