#include "main.h"
#include "socket.h"
#include "timer.h"
#include "data.h"

//pid
pthread_t pid_keeo_on_line;
pthread_t pid_socket_data_send;
pthread_t pid_socket_data_recv;
pthread_t pid_data_do_handle;
pthread_t pid_data_do_report;

//sem
sem_t sem_keep_on_line;
sem_t sem_recv_start;

//cond
pthread_cond_t cond_wait_login_ack;
pthread_cond_t cond_wait_heartbeat_ack;
pthread_cond_t cond_wait_socket_recv;

//mutex
pthread_mutex_t mutex_socket_data_send;

pthread_mutex_t mutex_wait_login_ack;
pthread_mutex_t mutex_wait_heartbeat_ack;
pthread_mutex_t mutex_wait_socket_recv;


//globle
u8 g_login_status = 0;
u8 g_connect_status = 0;

u8 volatile g_login_ack_status = 0;
u8 volatile g_heartbeat_ack_status = 0;

//timer
timer_array_t timer;

//fd
int socket_fd_link_1;



int main(int argc, char **argv)
{
	
	//定时器

	//
	sem_init(&sem_keep_on_line, 0, 0);
	sem_init(&sem_recv_start, 0, 0);
	
	//
	pthread_cond_init(&cond_wait_login_ack, NULL); 
	pthread_cond_init(&cond_wait_heartbeat_ack, NULL);
	pthread_cond_init(&cond_wait_socket_recv, NULL);

	pthread_mutex_init(&mutex_wait_login_ack, NULL); 
	pthread_mutex_init(&mutex_wait_heartbeat_ack, NULL); 
	pthread_mutex_init(&mutex_wait_socket_recv, NULL); 

	//1s
	timer_1s_init(); 

	//
 	pthread_create(&pid_keeo_on_line, NULL, (void *)pthread_keep_on_line, NULL);
	pthread_create(&pid_socket_data_send, NULL, (void *)pthread_socket_data_send, NULL);
	pthread_create(&pid_socket_data_recv, NULL, (void *)pthread_socket_data_recv, NULL);
	pthread_create(&pid_data_do_handle, NULL, (void *)pthread_data_do_handle, NULL);
	//pthread_create(&pid_data_do_report, NULL, (void *)pthread_data_do_report, NULL);
	
 	while(1)
 	{
		sleep(1);
 	}
 	
	return 0;
}
