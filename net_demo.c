#include <stdio.h>
#include <stdlib.h>

#include "system_net.h"
#include "les_external_ip.h"
#include "les_nat_check.h"

int main(int argc, char *argv[])
{
    sys_net_t *temp     = NULL;
    sys_net_t *net_list = NULL;

    net_list = get_net_list();
    for (temp = net_list; temp != NULL; temp = temp->next)
    {
        printf("NAME: %s\tIP:%s\t MAC:%s\n", temp->name, temp->ip, temp->mac);
    }

    char ip[64] = "47.52.242.79";

    les_get_external_ip(ip);

    if (les_nat_check(ip) < 0)
    {
        printf("have no nat\n");
    }
    else
    {
        printf("have nat\n");
    }
}

