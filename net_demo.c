#include <stdio.h>
#include <stdlib.h>

#include "system_net.h"

int main(int argc, char *argv[])
{
    sys_net_ctx_t *net_ctx = NULL;

    net_ctx = new_net_ctx(0);

    get_net_info(net_ctx);

    return 0;
}


