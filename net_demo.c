#include <stdio.h>
#include <stdlib.h>

#include "system_net.h"

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

