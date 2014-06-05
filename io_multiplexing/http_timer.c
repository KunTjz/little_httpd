#include <unistd.h>
#include <time.h>

#include "sys_define.h"
#include "http_timer.h"

int is_time_out(const time_stamp& ts)
{
	time_t now = time(NULL);
	if(now - ts.start_time >= ts.time_length)
		return HTTP_TIME_OUT;
	
	return HTTP_IN_TIME;
}

void set_timer(time_stamp& ts, unsigned int time_length)
{
	time_t now = time(NULL);
	ts.start_time = now;
	ts.time_length = time_length;
}

void update_timer(time_stamp& ts)
{
	time_t now = time(NULL);
	ts.start_time = now;
}
