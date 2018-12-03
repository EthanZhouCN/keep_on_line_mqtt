
#include "timer.h"


void timer_1s_cbk(int arg)
{

	if(timer.connect.start  == yes)
	{
		timer.connect.sec_count++;
	}

	if(timer.login.start == yes )
	{
		timer.login.sec_count++;
	}

	if(timer.heartbeat.start == yes)
	{
		timer.heartbeat.sec_count++;
	}

	if(timer.connect.sec_count >= timer.connect.outvalue)
	{	
		timer.connect.start = no;
		timer.connect.sec_count = 0;
		timer.connect.runable = yes;
		sem_post(&sem_keep_on_line);	
	}

	if(timer.login.sec_count >= timer.login.outvalue)
	{
		timer.login.start = no;
		timer.login.sec_count = 0;
		timer.login.runable = yes;
		sem_post(&sem_keep_on_line);
	}

	if(timer.heartbeat.sec_count >= timer.heartbeat.outvalue)
	{
		timer.heartbeat.start = no;
		timer.heartbeat.sec_count = 0;
		timer.heartbeat.runable = yes;
		sem_post(&sem_keep_on_line);
	}
}


void timer_1s_init()
{
	struct itimerval new_value, old_value;
	
	signal(SIGALRM, timer_1s_cbk);

    timer.connect.outvalue = CONFIG_RECONNECT_TIME_S;
	timer.login.outvalue = CONFIG_RELOGIN_TIME_S;
	timer.heartbeat.outvalue = CONFIG_REHEARTBEAT_TIME_S;

	timer.connect.runable = yes;
	timer.login.runable = no;
	timer.heartbeat.runable = no;
	
    new_value.it_value.tv_sec = 0;
    new_value.it_value.tv_usec = 1;
    new_value.it_interval.tv_sec = 1;
    new_value.it_interval.tv_usec = 0;
    
    setitimer(ITIMER_REAL, &new_value, &old_value);
}

