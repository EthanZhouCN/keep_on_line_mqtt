
#include "kol_report.h"
#include "kol_socket.h"


void pthread_data_do_report()
{
	u8 msg_buff_data[TBOX_DEFINE_MAX_PKG_SIZE] = {0};
	u16 msg_buff_len = 0;
	
	while(1)
	{

		
		timer.report.start = yes;
		
		//wait
		sem_wait(&sem_report_start);

		//msg_buff_len = rent_make_pkg(RENT_REPORT, msg_buff_data);
		
		if(g_login_status == yes)
		{
			user_data_socket_send(msg_buff_data, msg_buff_len);
			
		}else
		{
			//rent_record_pkg(msg_buff_data, msg_buff_len);
		}
		
	}
}



