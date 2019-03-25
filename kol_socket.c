
#include "kol_socket.h"
#include "kol_report.h"
#include "kol_utils.h"
#include "mqtt.h"




static int CreateTcpConnect(const char *host, unsigned short port)
{
	printf("server ip %s\n", host);
    struct sockaddr_in add;
    int fd;
    struct hostent *server;

    bzero(&add, sizeof(add));
    add.sin_family = AF_INET;
    add.sin_port = htons(port);
    server = gethostbyname(host);
    if(NULL == server) {
        printf("Failed to get the ip of the host(%s).\n", host);
        return -1;
    }

    fd = socket(AF_INET, SOCK_STREAM, 0);
    if(fd < 0) {
        printf("Failed to create socket file descriptor.\n");
        return fd;
    }

    bcopy((char*)server->h_addr, (char*)&add.sin_addr.s_addr, server->h_length);
    if(-1 == connect(fd, (struct sockaddr*)&add, sizeof(add))) {
        printf("Failed to connect to the server.\n");
        close(fd);
        return -1;
    }

    return fd;
}




uint16_t user_data_socket_send(unsigned char *send_data_buf, unsigned short send_data_len)
{	
	int i;
	printf("send len = 0x%02X.\n", send_data_len);
	for(i=0;i<send_data_len;i++)
	{
		printf("0x%02X ", send_data_buf[i]);
	}
	printf("\n");
	
	return send(socket_fd_link_1, send_data_buf, send_data_len, 0);
}


void pthread_socket_data_recv()
{
	u8 recv_data_buf[TBOX_DEFINE_MAX_PKG_SIZE];
	s16 ret = 0;
	int i;

	FixedHeader_t FixedHeader;
	unsigned short toplen = 0;
	unsigned short paylen = 0;
	unsigned char top[100] = {0};
	unsigned char pay[100] = {0};

				
	while(1)
	{
		if(g_connect_status == yes)
		{
			memset(recv_data_buf, 0, TBOX_DEFINE_MAX_PKG_SIZE);
			
			ret = recv(socket_fd_link_1, recv_data_buf, TBOX_DEFINE_MAX_PKG_SIZE, 0);

			printf("recv len = 0x%02X.\n", ret);
			for(i=0;i<ret;i++)
			{
				printf("0x%02X ", recv_data_buf[i]);
			}
			printf("\n");

			switch(recv_data_buf[0]>>4)
			{
				case MQTT_TypeCONNACK:	
					if(recv_data_buf[3] == 0x00)
					{
						g_login_ack_status = yes;	
					}
					
					break;
				case MQTT_TypePINGRESP:
					g_heartbeat_ack_status = yes;	
					
					break;

				case MQTT_TypePUBLISH:  //接收命令

					PlatfromPUBLISHAnalysis(recv_data_buf, &FixedHeader, &toplen, top, &paylen, pay);
					printf("FixedHeader.PacketType = %d.\n", FixedHeader.PacketType);
					printf("FixedHeader.RemainingLength = %d.\n", FixedHeader.RemainingLength);
					printf("toplen = %d top = %s.\n", toplen, top);
					printf("paylen = %d pay = %s.\n", paylen, pay);

					PlatfromCmdPUBLISHRsp(recv_data_buf, &FixedHeader, &toplen, top, &paylen, pay);
					printf("Cmd Rsp FixedHeader.PacketType = %d.\n", FixedHeader.PacketType);
					printf("Cmd Rsp FixedHeader.RemainingLength = %d.\n", FixedHeader.RemainingLength);
					printf("Cmd Rsp toplen = %d top = %s.\n", toplen, top);
					printf("Cmd Rsp paylen = %d pay = %s.\n", paylen, pay);
					user_data_socket_send(recv_data_buf, ret);
					
					break;
#if 0
				case MQTT_TypePUBREC:
					if(recv_data_buf[1] == 0x02)
					{
						unsigned char send_buff[5] = {0};
						memcpy(send_buff, recv_data_buf, 4);
						send_buff[0] = 0x62;
						user_data_socket_send(send_buff, 4);
						
					}	
					
					break;
				case MQTT_TypePUBCOMP:
					if(recv_data_buf[1] == 0x02)
					{
						g.PUBCOMP_PacketID = recv_data_buf[2]*128+recv_data_buf[3];
						printf("PUBCOMP_PacketID = %04X\n", g.PUBCOMP_PacketID);
					}	
					
					break;
#endif
				case MQTT_TypePUBACK:
					if(recv_data_buf[1] == 0x02)
					{
						g.PUBCOMP_PacketID = recv_data_buf[2]*128+recv_data_buf[3];
						printf("MQTT_TypePUBACK PacketID = %04X\n", g.PUBCOMP_PacketID);
					}
				default:
					break;
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
			/*183.230.40.39*/

			socket_fd_link_1 = CreateTcpConnect("admin.hlwmcu.com", 33860);
			//socket_fd_link_1 = CreateTcpConnect("183.230.40.39", 6002);
			
			/************************************/
			if(socket_fd_link_1 > 0)								//连接成功
			{	
				debug_info("connect success.");
				
				g_connect_status = yes;					//连接状态 = 成功
				timer.login.runable = yes;				//可以立即执行登入
				sem_post(&sem_recv_start);				//通知接收线程读取数据
			}
			else if(socket_fd_link_1 <= 0)							//连接失败
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
			msg_buff_len = GetDataConnet(msg_buff_data, "MQTT", 4, 1, 1, 0, 0, 0, 0, 120, "505342358", "193203", "uBJZGOHsvRD8sDttT2uPzF2BbJE=");
			
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
			msg_buff_len = GetDataPINGREQ(msg_buff_data);
			
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

