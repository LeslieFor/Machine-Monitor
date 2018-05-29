#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "les_nat_check.h"

#define LES_CHECK_PORT 12304
#define LES_CHECK_WAIT_SEC 3

static int les_check_connect(char *extern_ip)
{
    int n;
    int fd;
    int flags;
    int error = 0;
    socklen_t len;
    fd_set rset, wset;
    struct timeval tval;
    struct sockaddr_in address;

    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port   = htons(LES_CHECK_PORT);

    inet_pton(AF_INET, extern_ip, &address.sin_addr);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0)
    {
        fprintf(stderr, "Function socket() ERROR!\n");
        return -1;
    }

    flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);

    if ( (n = connect(fd, (struct sockaddr *)&address, sizeof(address))) < 0)
    {
        if (errno != EINPROGRESS)
        {
            fprintf(stderr, "Function connect ERROR!\n");
            return -1;
        }
    }

    if (n == 0)
    {
        printf("connected\n");
        close(fd);
        return 0;
    }

    FD_ZERO(&rset);
    FD_SET(fd, &rset);
    wset = rset;
    tval.tv_sec = LES_CHECK_WAIT_SEC;
    tval.tv_usec = 0;

    if ( (n = select(fd + 1, &rset, &wset, NULL, &tval)) <= 0)
    {
        close(fd);
        fprintf(stderr, "connected timeout\n");
        return -1;
    }

    error = 0;
    if (FD_ISSET(fd, &rset) || FD_ISSET(fd, &wset))
    {
        len = sizeof(error);
        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &len) < 0)
        {
            close(fd);
            return -1;
        }

        if (error)
        {
            close(fd);
            return -1;
        }

        printf("Connected After Select\n");
        close(fd);
        return 0;
    }

    fprintf(stderr, "Connected After Select NOT IN SET\n");
    return -1;
}

static int les_check_listen()
{
    int listen_fd;
    struct sockaddr_in address;

    memset(&address, 0, sizeof(address));

    address.sin_family = AF_INET;
    address.sin_port   = htons(LES_CHECK_PORT);
    address.sin_addr.s_addr = INADDR_ANY;

    listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0)
    {
        fprintf(stderr, "Function socket() ERROR!\n");
    }

    if (bind(listen_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        fprintf(stderr, "Function bind() ERROR!\n");
    }

    if (listen(listen_fd, 10) < 0)
    {
        fprintf(stderr, "Function listen() ERROR!\n");
    }

    return listen_fd;
}

int les_nat_check(char *extern_ip)
{
    int i = 0;
    int j = 0;

    if (strlen(extern_ip) < 4)
    {
        fprintf(stderr, "extern_ip format error\n");
        return -1;
    }

    i = les_check_listen();
    j = les_check_connect(extern_ip);

    if (i > 0)
    {
        close(i);
    }

    return j;
}

