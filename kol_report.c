
#include "kol_report.h"
#include "kol_socket.h"
#include "kol_utils.h"

#include "cjson_utils.h"
#include "mqtt.h"

	


	

void pthread_data_do_report()
{
	unsigned char msg_buff_data[TBOX_DEFINE_MAX_PKG_SIZE] = {0};
	unsigned short msg_buff_len = 0;

	unsigned char payload_buff[256] = {0};
	double tmpdat = 0;
	uint16_t ret = 0;
	unsigned char _ReTrySendCount = 0;
	while(1)
	{

		
		timer.report.start = yes;
		
		//wait
		sem_wait(&sem_report_start);

		DoubleTypeDataPoint2String(payload_buff, "tmp", tmpdat+=0.000001);	
			
		printf("json:\n%s\n", payload_buff);
	
		msg_buff_len = GetDataPointPUBLISH(msg_buff_data, 0, 2, 0, (char *)"$dp", 0x18, (char *)payload_buff);

		if(g_login_status == yes)
		{
			do{

			_ReTrySendCount++;
		
			PUBCOMP_PacketID = 0;
			
			ret = user_data_socket_send(msg_buff_data, msg_buff_len);	
			
			printf("send ret = %d %x\n", ret, ret);

			printf("wait_ack start.\n");
			wait_ack(500);
			printf("wait_ack end.\n");

			}while((PUBCOMP_PacketID != 0x18) && (_ReTrySendCount<=3));
			

			_ReTrySendCount = 0;
		}
		
	}
}



