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

#define PRINTF printf

/**************************  PUBLIC  ********************************/

#define no 		0
#define yes 	1
#define SET		1
#define CLR		0
typedef unsigned char 	u8;
typedef u8 				u8t;
typedef u8 				uint8;
typedef unsigned short 	u16;
typedef u16 			u16t;
typedef u16 			uint16;
typedef unsigned int 	u32;
typedef u32 			u32t;
typedef u32 			uint32;
typedef signed char 	s8;
typedef signed short 	s16;
typedef signed int 		s32;


/**************************  SOCKET  ********************************/


#define CONFIG_MAX_LOGIN_ACK_ERR_COUNT 					3
#define CONFIG_MAX_HEARTBEAT_ACK_ERR_COUNT  			3

#define CONFIG_RECONNECT_TIME_S							20
#define CONFIG_RELOGIN_TIME_S 							10

//#define CONFIG_REHEARTBEAT_TIME_S 					200
//#define CONFIG_SERVER_IP_LINK_1 						"192.168.31.1"
//#define CONFIG_SERVER_PORT_LINK_1 					8081

#define TBOX_DEFINE_MAX_PKG_SIZE						1024
#define TBOX_DEFINE_SEND_MSG_PIPE_KEY					1234
#define TBOX_DEFINE_RECV_MSG_PIPE_KEY					4321
#define TBOX_DEFINE_SEND_MSG_PIPE_TYPE 					21
#define TBOX_DEFINE_RECV_MSG_PIPE_TYPE 					12
#define TBOX_DEFINE_WAIT_ACK_MS							500
#define TBOX_DEFINE_WAIT_SOCKET_RECV_AGINE_MS			300

/**************************  CONFIG  ********************************/
#define RENT_RESSUE_JOURNAL_FILE_DIR					"."
#define RENT_CONFIG_DIR 								"./RentConfig.txt" 
#define RENT_CONFIG_MAX_LINE 							20
#define RENT_CONFIG_LINE_SIZE							60	

/**************************  IPC  ********************************/

typedef struct{
	long int 	msg_type;
	u8 			data_buf[TBOX_DEFINE_MAX_PKG_SIZE];
}system_v_msg_t;


/**************************  globle var  ********************************/

extern u8 g_connect_status;
extern u8 g_login_status;

extern u8 volatile g_login_ack_status;
extern u8 volatile g_heartbeat_ack_status;

extern int socket_fd_link_1;


/**************************  TIMER  ********************************/

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
	timer_unit_t report;
}timer_array_t;


extern timer_array_t timer;


/**************************  sem & mutex  ********************************/

extern sem_t sem_keep_on_line;
extern sem_t sem_recv_start;
extern sem_t sem_report_start;

extern pthread_mutex_t mutex_socket_data_send;


#endif /* _MAIN_H_ */
