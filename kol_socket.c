
#include "kol_socket.h"
#include "kol_report.h"
#include "kol_utils.h"
#include "kol_handle.h"

static u8 socket_connect(int *sockfd, u8 *serverip, u16 serverport)
{
	u8 flag = 0;

	struct addrinfo hints;
    struct addrinfo *res, *cur;
    int ret;
    struct sockaddr_in *seraddr_tmp;
    char seraddr_tmp_ip[16];

    struct sockaddr_in serveraddr;	//最后要填充的网络连接结构体

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET; 		/* Allow IPv4 */
    hints.ai_flags = AI_PASSIVE; 	/* For wildcard IP address */
    hints.ai_protocol = 0; 			/* Any protocol */
    hints.ai_socktype = SOCK_STREAM;
       
    ret = getaddrinfo((char *)serverip, NULL, &hints, &res);
    
    if (ret == -1) {
        perror("getaddrinfo");
        exit(1);
    }
    
    for(cur = res; cur != NULL; cur = cur->ai_next) {
        seraddr_tmp = (struct sockaddr_in *)cur->ai_addr;
        printf("connect ip : %s\n", inet_ntop(AF_INET, &seraddr_tmp->sin_addr, seraddr_tmp_ip, 16));
    }
	
   	memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(serverport);
    
    if( inet_pton(AF_INET, (char *)seraddr_tmp_ip, &serveraddr.sin_addr) <= 0){
        debug_info("RENT:	inet_pton error.\n");
		shutdown(*sockfd,SHUT_RDWR);
		close(*sockfd);
        return 1;
    }
  



	/* 关闭的socket */  
	if(0!=*sockfd){
		shutdown(*sockfd,SHUT_RDWR);
		close(*sockfd);
		*sockfd=0;
	}	

	if((*sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
		debug_info("create_link -> create socket error");
		return 1;
	}

	#if 1
	flag = fcntl(*sockfd, F_GETFL, 0);
	
	if (flag < 0) 
	{
		debug_info("create_link -> get flag.\n");
	}
		
	flag |= O_NONBLOCK;
	
	if(fcntl(*sockfd, F_SETFL, flag) < 0)
	{
		debug_info("create_link -> set flag.\n");
	}
#endif

    if(0 != connect(*sockfd, (struct sockaddr *)&serveraddr, sizeof(struct sockaddr)))
    {
		if(errno!=EINPROGRESS){
			debug_info("create_link -> connect error: %s.\n",strerror(errno));
			shutdown(*sockfd,SHUT_RDWR);
			close(*sockfd);
			freeaddrinfo(res);
			return 1;
		}
	}
	else
	{
		
		struct timeval tm = {2, 0};
        fd_set wset,rset;
        FD_ZERO(&wset);
        FD_ZERO(&rset);
        FD_SET(*sockfd,&wset);
        FD_SET(*sockfd,&rset);
        
        int res_select = select(*sockfd+1,&rset,&wset,NULL,&tm);

        if(res_select < 0)
        {
            debug_info("create_link -> network error in connect\n");
			shutdown(*sockfd,SHUT_RDWR);
			close(*sockfd);
			freeaddrinfo(res);
			return 1;
        }
        else if(res_select == 0)
        {
            debug_info("create_link -> connect time out\n");
			shutdown(*sockfd,SHUT_RDWR);
			close(*sockfd);
			freeaddrinfo(res);
			return 1; 
        }
        else
        { 
            if(FD_ISSET(*sockfd,&wset))
            {
                debug_info("create_link -> connect succeed.\n");
                freeaddrinfo(res);
                return 0;
            }
            else
            {
                debug_info("create_link -> other error when select\n");					
				shutdown(*sockfd,SHUT_RDWR);
				close(*sockfd);
				freeaddrinfo(res);
				return 1;
            }
        }
	}
	return 1;
}




u16 user_data_socket_send(u8 *send_data_buf, u16 send_data_len)
{	
 	s32 ret = 0;
 	int msgid = 0;
 	system_v_msg_t msg;
 	
 	msgid = msgget((key_t)TBOX_DEFINE_SEND_MSG_PIPE_KEY, 0666 | IPC_CREAT);
 	
 	if(pthread_mutex_lock(&mutex_socket_data_send) != 0)
 	{
 		perror("pthread_mutex_lock error!\n");
    }
 
 
 	msg.msg_type = TBOX_DEFINE_SEND_MSG_PIPE_TYPE;
 	memcpy((void *)msg.data_buf, (void *)send_data_buf, send_data_len);
 	
 	ret = msgsnd(msgid, &msg, send_data_len, 0);
 	
 	if(ret == -1)
 	{
 		perror("msgsnd");
 	}
 
 	if(pthread_mutex_unlock(&mutex_socket_data_send) != 0)
 	{
 		perror("pthread_mutex_unlock error!\n");
    }
 	
 	return ret == 0 ? send_data_len : 0;
}


void pthread_socket_data_send()
{
	int msgid;
	u16 len_recv_from_pipe = 0;
	u16 len_sent = 0;
	s32 ret = 0;
	u8 flag_loop_send = 0;
	system_v_msg_t msg;
 	msgid = msgget((key_t)TBOX_DEFINE_SEND_MSG_PIPE_KEY, 0666 | IPC_CREAT);
 	
	while(1)
	{
		
		memset(&msg, 0, sizeof(msg));
 		len_sent = 0;
 		
 		len_recv_from_pipe = msgrcv(msgid, &msg, TBOX_DEFINE_MAX_PKG_SIZE, TBOX_DEFINE_SEND_MSG_PIPE_TYPE, 0);
 	
 		debug_info("len = %d, msg.type=%ld, msg.date=%s", len_recv_from_pipe, msg.msg_type, msg.data_buf);

		do
		{
			flag_loop_send = no;
			
	 		ret = send(socket_fd_link_1, msg.data_buf + len_sent, len_recv_from_pipe - len_sent, MSG_DONTWAIT);

			debug_info("send ret = %d, errno = %d : %s.", ret, errno, strerror(errno));
			
			if(ret >= 0)		//发送成功 
			{	
				pri_journal((uint8 *)"rent send data :", msg.data_buf + len_sent, ret);
				
				if(ret == (len_recv_from_pipe - len_sent))				//发送完整
		 		{
					debug_info("len_sent == (len_recv_from_pipe - len_sent).");
		 		}
		 		else if(ret < (len_recv_from_pipe - len_sent))			//发送未完整
		 		{
					debug_info("len_sent < (len_recv_from_pipe - len_sent).");
					len_sent += ret;
					flag_loop_send = yes;
		 		}
			}
			else if(ret < 0)
			{
				if((errno == EINTR) && (errno == EWOULDBLOCK))			//由于非阻塞造成的正常的“异常”，需要重新发送即可
		 		{
					debug_info("resend.");
					flag_loop_send = yes;
		 		}
		 		else													//真正出错，close socket 从新建立连接
		 		{
					debug_info("close socket.");
				
					close(socket_fd_link_1);
					
					g_connect_status = no;
					g_login_status = no;
					timer.connect.runable = yes;
					sem_post(&sem_keep_on_line);
				
		 		}
			}
 		}
		while(flag_loop_send);
		
	}
}


void pthread_socket_data_recv()
{
	u8 recv_data_buf[TBOX_DEFINE_MAX_PKG_SIZE];
	s16 ret = 0;

	int msgid;
	system_v_msg_t msg;
 	msgid = msgget((key_t)TBOX_DEFINE_RECV_MSG_PIPE_KEY, 0666 | IPC_CREAT);

	while(1)
	{
		if(g_connect_status == yes)
		{
			ret = recv(socket_fd_link_1, recv_data_buf, TBOX_DEFINE_MAX_PKG_SIZE, MSG_DONTWAIT);
			
			if(ret == 0)											//对端套接字关闭
	        {
	        	debug_info("recv faild : %s.", recv_data_buf);

	        	close(socket_fd_link_1);
					
				g_connect_status = no;
				g_login_status = no;
				timer.connect.runable = yes;
				sem_post(&sem_keep_on_line);
		
				continue;
	        }
	        else if(ret > 0)										//正常接收
	        {
	            memset(&msg, 0, sizeof(msg));
	            
	        	pri_journal((uint8 *)"rent recv data :", recv_data_buf, ret);

	        	/**** 接收处理 *************************/

				msg.msg_type = TBOX_DEFINE_RECV_MSG_PIPE_TYPE;

				memcpy((char *)msg.data_buf, (char *)recv_data_buf, (size_t)ret);
				
				ret = msgsnd(msgid, (void*)&msg, TBOX_DEFINE_MAX_PKG_SIZE, 0);
				
				if(ret == -1)
				{
					perror("msgsnd");
				}

	        	/***************************************/
	        }       
	        else if((errno == EINTR) && (errno == EWOULDBLOCK))		//被中断，继续接收
	        {
	            continue;
	        }
	        else													//无资源可接收，阻塞500ms
	        {
	            wait_ack(TBOX_DEFINE_WAIT_SOCKET_RECV_AGINE_MS);
	            memset(recv_data_buf, 0, TBOX_DEFINE_MAX_PKG_SIZE);
	            ret = 0;
	            continue;
	        }

		}
		else if(g_connect_status == no)
		{
			sem_wait(&sem_recv_start);
		}
	}
}




void pthread_keep_on_line(void)
{
	u8 ret = 0;
	u8 l_login_ack_err_count = 0;
	u8 l_heartbeat_ack_err_count = 0;
	u8 msg_buff_data[TBOX_DEFINE_MAX_PKG_SIZE] = {0};
	u16 msg_buff_len = 0;
	
	sem_post(&sem_keep_on_line);
	
	while(1)
	{
		sem_wait(&sem_keep_on_line);

		/* connect */
		if((g_connect_status == no) && (g_login_status == no)  && (timer.connect.runable == yes))
		{
			timer.connect.runable = no;
			debug_info("connect.");
			/**** 处理连接 ********************************/

			ret = socket_connect(&socket_fd_link_1, (u8 *)g_rent_config.server_ip, g_rent_config.server_port);
			
			/************************************/
			if(ret == 0)								//连接成功
			{	
				debug_info("connect success.");
				
				g_connect_status = yes;					//连接状态 = 成功
				timer.login.runable = yes;				//可以立即执行登入
				sem_post(&sem_recv_start);				//通知接收线程读取数据
			}
			else if(ret == 1)							//连接失败
			{
				debug_info("connect faild.");
					
				timer.connect.start = yes;  			//开始计时，重连时间
			}
		}

		/* login */
		if((g_connect_status == yes) && (g_login_status == no) && (timer.login.runable == yes))
		{
			timer.login.runable = no;
	
			debug_info("login.");
			debug_info("wait login ack.");
			/**** 处理登入 ******************************/
			
			//make and send login
			//msg_buff_len = rent_make_pkg(RENT_LOGIN, msg_buff_data);
			
			user_data_socket_send(msg_buff_data, msg_buff_len);
	
			//wait ack
			wait_ack(TBOX_DEFINE_WAIT_ACK_MS);

			/************************************/
			if(g_login_ack_status == yes)				//登入应答 - 成功
			{
				debug_info("login ack success.");
				g_login_ack_status = no;
				l_login_ack_err_count = 0;				//登入应答异常计数 - 清除
								//登入应答标志 - 清除
				g_login_status = yes;					//登入状态 = 成功
				timer.heartbeat.runable = yes;			//可以立即执行心跳检测
				/**** 处理补发 ******************************/
#if 0
				//清除老旧的补发备份数据
				//rent_clear_old_ressue_file();
				
				pthread_t pid_do_data_ressue;
				pthread_attr_t attr;
				pthread_attr_init(&attr);
				pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
				pthread_create(&pid_do_data_ressue, &attr, (void *)pthread_do_data_ressue, NULL);
				
				pthread_attr_destroy(&attr);
#endif				
				/**********************************/
			}
			else if(g_login_ack_status == no)
			{
				debug_info("login ack faild.");
				l_login_ack_err_count++;				//登入应答异常计数++	
				timer.login.start = yes;				//开启下次心跳检测
	
				if(l_login_ack_err_count >= CONFIG_MAX_LOGIN_ACK_ERR_COUNT)
				{
					l_login_ack_err_count = 0;			//登入应答异常计数 - 清除
					g_login_status = no;				//登入应答标志 - 清除	
					g_connect_status = no;				//连接状态 = 失败
					g_login_ack_status = no;			//登入应答标记清除
					timer.connect.start = yes;			//开启下次重新连接计时
				}
			}
		}

		/* heartbeat */
		if((g_login_status == yes) && (g_connect_status == yes) && (timer.heartbeat.runable == yes))
		{
			debug_info("heartbeat.");
			debug_info("wait heartbeat ack.");
			timer.heartbeat.runable = no;
			
			/***** 处理心跳 *******************************/

			//make and send heartbeat
			//msg_buff_len = rent_make_pkg(RENT_HEARTBEAT, msg_buff_data);
			
			user_data_socket_send(msg_buff_data, msg_buff_len);
			
			//wait ack
			wait_ack(TBOX_DEFINE_WAIT_ACK_MS);

			/************************************/
			if(g_heartbeat_ack_status == yes)			//心跳应答正常
			{
				debug_info("heartbeat ack success.");
				l_heartbeat_ack_err_count = 0;
				g_heartbeat_ack_status = no;
				timer.heartbeat.start = yes;			//开启下次心跳
			}
			else if(g_heartbeat_ack_status == no)		//心跳应答异常
			{
				debug_info("heartbeat ack faild.");
				l_heartbeat_ack_err_count++;
				timer.heartbeat.start = yes;			
				if(l_heartbeat_ack_err_count >= CONFIG_MAX_HEARTBEAT_ACK_ERR_COUNT)
				{
					l_heartbeat_ack_err_count = 0;
					g_login_status = no;
					g_connect_status = no;
					timer.connect.runable = yes;		//可以立即执行重新连接
					timer.heartbeat.start = yes;		//避免阻塞，使其重连连接
				}
			}
		}
	}
}

