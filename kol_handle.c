

#include "kol_handle.h"
#include "kol_socket.h"
#include "kol_utils.h"

void pthread_data_do_handle()
{

	u16 recv_len = 0;
	u8 msg_buff_data[TBOX_DEFINE_MAX_PKG_SIZE] = {0};
	u16 msg_buff_len = 0;
	
	int msgid;
	system_v_msg_t handle_msg;
	
 	msgid = msgget((key_t)TBOX_DEFINE_RECV_MSG_PIPE_KEY, 0666 | IPC_CREAT);
 	
	while(1)
	{
		memset(&handle_msg, 0, sizeof(handle_msg));
	
		//get msg from fifo
		
		recv_len = msgrcv(msgid, (void *)&handle_msg, TBOX_DEFINE_MAX_PKG_SIZE, TBOX_DEFINE_RECV_MSG_PIPE_TYPE, 0);
	
		
		if(SET != rent_check_bcc(handle_msg.data_buf, recv_len))
		{
			continue;
		}
			
		//msg_buff_len = rent_handle_recv_data(handle_msg.data_buf , msg_buff_data);

		msg_buff_len = 1;
		
		if(msg_buff_len > 0)
		{
			if(user_data_socket_send(msg_buff_data, msg_buff_len));
		}
		
	}
}





