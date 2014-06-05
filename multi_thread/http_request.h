#ifndef  __HTTP_REQUEST_H__
#define  __HTTP_REQUEST_H__

typedef struct request{
    char            request_type[10];
    char            version[5];
    char            host_ip[16];
    unsigned int    host_port;
    char            file[256];
}http_request;

int http_request_parse(const char* request, http_request* rst);


#endif
