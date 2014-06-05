#include <stdio.h>
#include <string.h>

#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "sys_define.h"
#include "http_response.h"

#define BUF_SIZE 4096

static int read_file(const char* file, char* response_str)
{
	int n;
	char *str = response_str;
	int fd = open(file, O_RDONLY);
	if(fd < 0) {
		perror("open failed");
		return L_HTTP_FAIL;
	}
	
	while((n = read(fd, str, BUF_SIZE)) > BUF_SIZE)
		str += n;
	if(n < 0) {
		perror("read failed");
		return L_HTTP_FAIL;
	}
	
	return L_HTTP_SUCCESS;
}

int generate_response(char* response_str, const char* file_name)
{
	struct stat st;
	const char *file = file_name;
	
	/*if(getcwd(file, MAX_LINE) == NULL) {
		perror("getcwd faied: ");
		return L_HTTP_FAIL;
	}

	i = strlen(file);
	if(strcmp("/", file_name) == 0)
		strncpy(file + i, DEFAULT_INDEX, strlen(DEFAULT_INDEX));
	else
		strncpy(file + i, file_name, strlen(file_name));
	*/
	printf("%s\n", file);

	if(stat(file, &st) < 0) {
		perror("stat failed: ");
		return L_HTTP_FAIL;
	}
	
	if (S_ISREG (st.st_mode)) {
		// check permission
        //TODO: getcontent
		if(read_file(file, response_str) == L_HTTP_FAIL) {
			perror("read file fail");
			return L_HTTP_FAIL;
		}
    }
	else {
		printf("invailed file type");
		return L_HTTP_FAIL;
	}

	return L_HTTP_SUCCESS;
}
