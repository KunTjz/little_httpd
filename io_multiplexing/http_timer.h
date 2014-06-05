#ifndef __HTTP_TIMER_H__
#define __HTTP_TIMER_H__

enum timer
{
	HTTP_TIME_OUT = 0,
	HTTP_IN_TIME
};

#define DEFAULT_TIME_LENGTH 2

typedef struct time_stamp
{
	time_t start_time;
	unsigned int time_length;	
}time_stamp;

int is_time_out(const time_stamp& timer);
void set_timer(time_stamp& ts, unsigned int time_length);
void update_timer(time_stamp& ts);

#endif
