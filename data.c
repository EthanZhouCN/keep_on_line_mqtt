
#include "data.h"

void pthread_data_do_handle()
{
	u16 recv_len = 0;
	
	int msgid;
	system_v_msg_t msg;
 	msgid = msgget((key_t)TBOX_DEFINE_RECV_MSG_PIPE_KEY, 0666 | IPC_CREAT);
 	
	while(1)
	{
		memset(&msg, 0, sizeof(msg));
	
		//get msg from fifo
		
		recv_len = msgrcv(msgid, (void *)&msg, TBOX_DEFINE_MAX_PKG_SIZE, TBOX_DEFINE_RECV_MSG_PIPE_TYPE, 0);
		debug_info("recv fifo -> len = %d : %02X.", recv_len, msg.data_buf[0]);
		if(recv_len);//bcc
#if 1
		switch(msg.data_buf[0])
		{
			case 0xff:
				debug_info("---> recv g_login_ack.");
				g_login_ack_status = yes;
				pthread_cond_signal(&cond_wait_login_ack);
				break;

			case 0x01:
				debug_info("---> recv g_heartbeat_ack.");
				g_heartbeat_ack_status = yes;
				pthread_cond_signal(&cond_wait_heartbeat_ack);
				break;
			default:
				break;
		}
#endif

		//send and backup

	}
}


void pthread_data_do_report()
{
	while(1)
	{
		//wait

		//get msg from fifo

		//handle

		//send result
	}
}


