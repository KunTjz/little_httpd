#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#include "sys_define.h"
#include "http_request.h"

#define DEFAULT_INDEX "/index.html"
#define MAX_PATH	256

int http_request_parse(const char* request, http_request* http_rst)
{
    const char*  p = request;
    int i = 0;
	char file[MAX_PATH];
	memset(file, 0x00, MAX_PATH);
	
    assert(request);
   
    // get request_type
    while(*p != ' ' && i < 9)
        http_rst->request_type[i++] = *p++;
    http_rst->request_type[i] = '\0';
    
    // get file
    i = 0;
    p++;
    while(*p != ' ')
        http_rst->file[i++] = *p++;
    http_rst->file[i] = '\0';
	
	if(getcwd(file, MAX_PATH) == NULL) {
		perror("getcwd faied: ");
		return L_HTTP_FAIL;
	}

	i = strlen(file);
	if(strcmp("/", http_rst->file) == 0)
		strncpy(file + i, DEFAULT_INDEX, strlen(DEFAULT_INDEX));
	else
		strncpy(file + i, http_rst->file, strlen(http_rst->file));
	
	strcpy(http_rst->file, file);

    // get version
    i = 0;
    p++;
    while(*p != '\r')
        http_rst->version[i++] = *p++;
    http_rst->version[i] = '\0';

    return L_HTTP_SUCCESS;
}
