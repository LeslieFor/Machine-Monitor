#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "system_net.h"

AF_filter_t *new_AF_filter();
int af_filter_handler(ifreq_handler_t *hand, struct ifreq *ifq);

sys_net_t *get_net_list()
{
    sys_net_t     *net_list = NULL;
    AF_filter_t   *af_flt   = NULL;
    sys_net_ctx_t *net_ctx  = NULL;

    net_ctx = new_net_ctx(0);
    get_net_info(net_ctx);

    af_flt = (AF_filter_t *)net_ctx->handle;
    net_list = af_flt->head;

    free(net_ctx);
    free(af_flt);

    return net_list;
}

sys_net_ctx_t *new_net_ctx(int type)
{
    sys_net_ctx_t *net_ctx = NULL;

    net_ctx = (sys_net_ctx_t *) malloc(sizeof(*net_ctx));
    if (net_ctx == NULL)
    {
        printf("malloc sys_net_ctx_t error\n");
        return NULL;
    }

    net_ctx->handle = (ifreq_handler_t *) new_AF_filter();

    return net_ctx;
}


AF_filter_t *new_AF_filter()
{
    AF_filter_t *af_flt = NULL;
    af_flt = (AF_filter_t *) malloc(sizeof(*af_flt));
    if (af_flt == NULL)
    {
        printf("malloc AF_filter error\n");
        return NULL;
    }
    memset(af_flt, 0x00, sizeof(*af_flt));

    af_flt->handler = &af_filter_handler;

    return af_flt;
}

int af_filter_handler(ifreq_handler_t *hand, struct ifreq *ifrq)
{
    //printf("\n===== start =====\n");
    int sockfd;
    struct ifreq   ifr;
    sys_net_t     *temp   = NULL;
    AF_filter_t   *af_flt = NULL;
    unsigned char  mac[6] = {0x00};

    af_flt = (AF_filter_t *)hand;

    temp = (sys_net_t *) malloc(sizeof(*temp));
    if (temp == NULL)
    {
        printf("malloc sys_net_t error\n");
        return -1;
    }
    memset(temp, 0x00, sizeof(sizeof(*temp)));

    sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if (sockfd < 0)
    {
        printf("sockfd error\n");
    }

    strcpy(ifr.ifr_name, ifrq->ifr_name);
    strcpy(temp->name, ifr.ifr_name);

    /*
     * Get Net Interface IP Address
     * Only support IPv4
     */
    if (inet_ntop(AF_INET, &((struct sockaddr_in *)&(ifrq->ifr_addr))->sin_addr, temp->ip, sizeof(temp->ip)) == NULL)
    {
        printf("inet_ntop error: %d\n", errno);
    }

    /*
     * Get Net Interface Mac Address
     */
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) < 0)
    {
        printf("ioctl SIOCGIFHWADDR error:%d\n", errno);
        close(sockfd);
        return -1;
    }

    memcpy(mac, ifr.ifr_hwaddr.sa_data, 6);
    sprintf(temp->mac, "%02x:%02x:%02x:%02x:%02x:%02x", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);

    /*add temp to list*/
    temp->next   = af_flt->head;
    af_flt->head = temp;

    /* printf("NAME: %s\tIP:%s\t MAC:%s\n", temp->name, temp->ip, temp->mac); */

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
        /* printf("loop len :%d\n", len); */
        buf = malloc(len);
        if (buf == NULL)
        {
            printf("malloc buf error\n");
            continue;
        }

        ifc.ifc_buf = buf;
        ifc.ifc_len = len;

        if (ioctl(sockfd, SIOCGIFCONF, &ifc) < 0)
        {
            if (errno != EINVAL)
            {
                printf("ioctl SIOCGIFCONF error: %d\n", errno);
                free(buf);
                close(sockfd);
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

    close(sockfd);

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












