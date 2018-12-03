#ifndef _MAIN_H_
#define _MAIN_H_
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <sys/time.h>
#include <signal.h>
#include <semaphore.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>



#define USE_DEBUG
#ifdef USE_DEBUG
#define debug_line() 				printf("[%s:%s] line=%d\r\n",__FILE__, __func__, __LINE__)
#define debug_err(fmt, args...) 	printf("\033[46;31m[%s:%d]\033[0m "#fmt" errno=%d, %m\r\n", __func__, __LINE__, ##args, errno, errno)
#define debug_info(fmt, args...) 	printf("\033[33m[%s:%d]\033[0m "#fmt"\r\n", __func__, __LINE__, ##args)
#else
#define debug_line()
#define debug_err(fmt,...)
#define debug_info(fmt,...)
#endif




#define no 		0
#define yes 	1


#define CONFIG_MAX_LOGIN_ACK_ERR_COUNT 		3
#define CONFIG_MAX_HEARTBEAT_ACK_ERR_COUNT  3

#define CONFIG_RECONNECT_TIME_S				6
#define CONFIG_RELOGIN_TIME_S 				5
#define CONFIG_REHEARTBEAT_TIME_S 			3

#define CONFIG_SERVER_IP_LINK_1 			"192.168.31.1"
#define CONFIG_SERVER_PORT_LINK_1 			8081



#define TBOX_DEFINE_MAX_PKG_SIZE						1024
#define TBOX_DEFINE_SEND_MSG_PIPE_KEY					1234
#define TBOX_DEFINE_RECV_MSG_PIPE_KEY					4321
#define TBOX_DEFINE_SEND_MSG_PIPE_TYPE 					21
#define TBOX_DEFINE_RECV_MSG_PIPE_TYPE 					12
#define TBOX_DEFINE_WAIT_ACK_MS							2000
#define TBOX_DEFINE_WAIT_SOCKET_RECV_AGINE_MS			500




typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef signed char s8;
typedef signed short s16;
typedef signed int s32;

typedef struct{
	u8 start;
	u8 sec_count;
	u8 outvalue;
	u8 runable;
}timer_unit_t;

typedef struct{
	timer_unit_t connect;
	timer_unit_t login;
	timer_unit_t heartbeat;
}timer_array_t;

typedef struct{
	long int 	msg_type;
	u8 			data_buf[TBOX_DEFINE_MAX_PKG_SIZE];
}system_v_msg_t;



extern u8 g_connect_status;
extern u8 g_login_status;

extern u8 volatile g_login_ack_status;
extern u8 volatile g_heartbeat_ack_status;

extern timer_array_t timer;

extern sem_t sem_keep_on_line;
extern sem_t sem_recv_start;


extern pthread_mutex_t mutex_wait_login_ack;
extern pthread_mutex_t mutex_wait_heartbeat_ack;
extern pthread_mutex_t mutex_wait_socket_recv;
extern pthread_mutex_t mutex_socket_data_send;



extern pthread_cond_t cond_wait_login_ack;
extern pthread_cond_t cond_wait_heartbeat_ack;
extern pthread_cond_t cond_wait_socket_recv;


extern int socket_fd_link_1;
#endif /* _MAIN_H_ */
