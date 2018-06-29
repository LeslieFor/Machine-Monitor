#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <curl/curl.h>

#include "les_curl.h"

#define LES_DEBUG 0
#define LES_USERAGENT "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_3) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/66.0.3359.181 Safari/537.36"

static long les_curl_cmd_raw(char *method, char *url, char *header[], char *body, long body_size, char *recvbuf, int recvbuf_len, char *file_path, int timeout);

typedef struct les_curl_buffer_s les_curl_buffer_t;

struct les_curl_buffer_s {
    char  *buffer;
    size_t size;
};



static size_t les_curl_file_writecb(void *buf, size_t size, size_t nmemb, void *recv)
{
    FILE *fp = (FILE *) recv;
    printf("fwrite\n");
    return fwrite(buf, size, nmemb, fp);
}


static size_t les_curl_writecb(void *buf, size_t size, size_t nmemb, void *recv)
{
    size_t realsize = size * nmemb;

    les_curl_buffer_t *chunk = (les_curl_buffer_t *)recv;

    chunk->buffer = (char *) realloc(chunk->buffer, chunk->size + realsize + 1);

    if (NULL == chunk->buffer) {

        chunk->size = 0;

        return 0;
    }

    memcpy(chunk->buffer + chunk->size, buf, realsize);

    chunk->size += realsize;

    // chunk->buffer[chunk->size] = 0;

    return realsize;
}


long les_curl_file_cmd(char *method, char *url, char *header[], char *body, long body_size, char *file_path, int timeout)
{
    return les_curl_cmd_raw(method, url, header, body, body_size, NULL, 0, file_path, timeout);
}


long les_curl_cmd(char *method, char *url, char *header[], char *body, long body_size, char *recvbuf, int recvbuf_len, int timeout)
{
    return les_curl_cmd_raw(method, url, header, body, body_size, recvbuf, recvbuf_len, NULL, timeout);
}


static long les_curl_cmd_raw(char *method, char *url, char *header[], char *body, long body_size, char *recvbuf, int recvbuf_len, char *file_path, int timeout)
{
    long   retcode;
    FILE  *fp = NULL;
    CURL  *curl_handle = NULL;
    char   err_buf[CURL_ERROR_SIZE];
    struct curl_slist *curl_headers = NULL;

    les_curl_buffer_t chunk;

    /* Init Buffer */
    chunk.buffer = malloc(1);
    chunk.size   = 0;

    curl_global_init(CURL_GLOBAL_ALL);

    curl_handle = curl_easy_init();

    /* 
     * For Debug:
     * curl_easy_setopt(curl_handle, CURLOPT_DEBUGFUNCTION, pear_curl_trace);
     * curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1L);
     */

    curl_easy_setopt(curl_handle, CURLOPT_ERRORBUFFER, err_buf);

    /*
     * Add header
     *
     */
    if (header != NULL)
    {
        for ( ; *header; header++)
        {
            curl_headers = curl_slist_append(curl_headers, *header);
        }

        curl_easy_setopt(curl_handle, CURLOPT_HTTPHEADER, curl_headers);
    }

    /*
     * Set callback function
     *
     */
    if (file_path == NULL)
    {
        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, les_curl_writecb);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &chunk);
    }
    else
    {
        if ((fp = fopen(file_path, "wb+")) == NULL)
        {
            fprintf(stderr, "fopen error: %s\n", strerror(errno));
            retcode = -1;
            goto ret;
        }

        /* Clear File Length */
        if (ftruncate(fileno(fp), 0) < 0)
        {
            fprintf(stderr, "ftruncate error\n");
        }

        curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, les_curl_file_writecb);
        curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)fp);
    }


    /* 
     * POST Method Has Body
     *
     */
    if (strcmp(method, "POST") == 0)
    {

        curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);

        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, body_size);

        curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, body);

    }
    else if (strcmp(method, "GET") == 0)
    {

    }
    else
    {
        return -1;
    }

    /*
     * Other Options
     */
    curl_easy_setopt(curl_handle, CURLOPT_URL, url);
    curl_easy_setopt(curl_handle, CURLOPT_NOBODY, 0L);
    curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, timeout);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0);
    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYHOST, 0);
    curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, LES_USERAGENT);

    /* Easy Perform */
    curl_easy_perform(curl_handle);


    /* Get Result */
    //CURLcode result = curl_easy_getinfo(curl_handle, CURLINFO_HTTP_CODE, &retcode);
    curl_easy_getinfo(curl_handle, CURLINFO_HTTP_CODE, &retcode);


    /* Copy Data */
    if (file_path == NULL)
    {
        if (recvbuf_len >= chunk.size)
        {
            memcpy(recvbuf, chunk.buffer, chunk.size);
            recvbuf[chunk.size] = 0;
        }
        else
        {
            fprintf(stderr, "Error: recvbuf length not enough\n");
            retcode = -1;
        }
    }

ret:

    /* Show Error */
    if (retcode != 200)
    {
        fprintf(stderr, "err: %s\n", err_buf);
    }

    /* Close File */
    if (fp != NULL)
    {
        fflush(fp);
        fclose(fp);
    }

    /* Free Buffer */
    free(chunk.buffer);

    /* Free Headers */
    curl_slist_free_all(curl_headers);

    /* Free Curl Handle */
    curl_easy_cleanup(curl_handle);

    /* Free Curl Global */
    curl_global_cleanup();

    return retcode;
}

