#include <stdio.h>
#include <stdlib.h>

#include "system_net.h"

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

int main(int argc, char *argv[])
{
    sys_net_t *temp     = NULL;
    sys_net_t *net_list = NULL;

    net_list = get_net_list();
    for (temp = net_list; temp != NULL; temp = temp->next)
    {
        printf("NAME: %s\tIP:%s\t MAC:%s\n", temp->name, temp->ip, temp->mac);
    }
}

