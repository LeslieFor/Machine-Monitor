#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#include "../les_curl.h"
#include "../les_json.h"
#include "les_external_ip.h"

int les_get_external_ip(char *value)
{
    char *api_url[] = {
        "https://api.ipify.org?format=json",
        "http://ipinfo.io/json",
        "http://myexternalip.com/json"
    };

    int  i = 0;
    int  s = 0;
    char temp[1024] = {0x00};

    for (i = 0; i < 3; i++)
    {
        s = les_curl_cmd("GET", api_url[i], NULL, NULL, 0, temp, 1024, 10);
        if (s != 200)
        {
            continue;
        }

        printf("temp: %s\n", temp);

        if (les_json_get_string(temp, "ip", value) > 0)
        {
            return 0;
        }
    }

    return -1;
    fprintf(stderr, "Can't get external ip!\n");
}
