#ifndef _LES_CURL_H_
#define _LES_CURL_H_


long les_curl_cmd(char *method,
                  char *url,
                  char *header[],
                  char *body,
                  long  body_size,
                  char *recvbuf,
                  int   recvbuf_len,
                  int   timeout);

long les_curl_file_cmd(char *method,
                       char *url,
                       char *header[],
                       char *body,
                       long  body_size,
                       char *file_path,
                       int   timeout);
#endif
