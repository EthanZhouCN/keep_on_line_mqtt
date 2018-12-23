#include "kol_main.h"
#include "kol_socket.h"
#include "kol_timer.h"
#include "kol_report.h"
#include "kol_utils.h"


//pid
pthread_t pid_keeo_on_line;
pthread_t pid_socket_data_send;
pthread_t pid_socket_data_recv;
pthread_t pid_data_do_handle;
pthread_t pid_data_do_report;

//sem
sem_t sem_keep_on_line;
sem_t sem_recv_start;
sem_t sem_report_start;

//mutex
pthread_mutex_t mutex_socket_data_send;

//globle
u8 g_login_status = 0;
u8 g_connect_status = 0;

u8 volatile g_login_ack_status = 0;
u8 volatile g_heartbeat_ack_status = 0;

//timer
timer_array_t timer;

//fd
int socket_fd_link_1;

int main(int ar, char **arg)
{

	//
	//init_rent_dir();

	
	//
	sem_init(&sem_keep_on_line, 0, 0);
	sem_init(&sem_recv_start, 0, 0);
	sem_init(&sem_report_start, 0, 0);
	
	//rent_get_config();
	//PRINTF("g_rent_config.server_ip = %s.\n", g_rent_config.server_ip);
	//PRINTF("g_rent_config.server_port = %d.\n", g_rent_config.server_port);
	//PRINTF("g_rent_config.hb_interval = %d.\n", g_rent_config.hb_interval);

	//
	//init_rent_report_format();
	/*
	PRINTF("1----------.\n");
	PRINTF("rent_report_format.start_flag = %d.\n", rent_report_format.start_flag);
	PRINTF("rent_report_format.param_number = %d.\n", rent_report_format.param_number);
	PRINTF("rent_report_format.msg_interval = %d.\n", rent_report_format.msg_interval);
	PRINTF("rent_report_format.param_array = ");
	int i;
	for(i=0; i<rent_report_format.param_number ; i++)
	{
		PRINTF("%02X ", rent_report_format.param_array[i]);
	}
	PRINTF("\n");
	*/
	
	
	
	//1s
	timer_1s_init(); 
	

	//
 	pthread_create(&pid_keeo_on_line, NULL, (void *)pthread_keep_on_line, NULL);
	pthread_create(&pid_socket_data_recv, NULL, (void *)pthread_socket_data_recv, NULL);
	pthread_create(&pid_data_do_report, NULL, (void *)pthread_data_do_report, NULL);
	
 	while(1)
 	{
		sleep(1);
 	}

 	return 0;
 	
}
