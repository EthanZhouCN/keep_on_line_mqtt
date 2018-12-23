
#include "kol_report.h"
#include "kol_socket.h"



	


	

void pthread_data_do_report()
{
	unsigned char msg_buff_data[TBOX_DEFINE_MAX_PKG_SIZE] = {0};
	unsigned short msg_buff_len = 0;

	unsigned char payload_buff[256] = {0};
	double tmpdat = 10.1;
	uint16_t ret = 0;
	
	while(1)
	{

		
		timer.report.start = yes;
		
		//wait
		sem_wait(&sem_report_start);

		DoubleTypeDataPoint2String(payload_buff, "tmp", ++tmpdat);	
			
		printf("json:\n%s\n", payload_buff);
	
		msg_buff_len = GetDataPointPUBLISH(msg_buff_data, 0, 2, 0, (char *)"$dp", (char *)payload_buff);

		if(g_login_status == yes)
		{
			ret = user_data_socket_send(msg_buff_data, msg_buff_len);	
			printf("send ret = %d %x\n", ret, ret);
		}
		
	}
}



