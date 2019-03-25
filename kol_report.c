
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
	unsigned char leddat = 0;
	uint16_t ret = 0;
	unsigned char _ReTrySendCount = 0;
	int _DateTime[6] = {0};
	while(1)
	{

		
		timer.report.start = yes;
		
		//wait
		sem_wait(&sem_report_start);

		memset(payload_buff, 0, sizeof(payload_buff));
		//DoubleTypeDataPoint2String(payload_buff, "tmp", tmpdat+=0.1, _DateTime);	
		DataPoint2Json(payload_buff, "tmp", tmpdat+=0.1, "led", leddat+=1);	
		printf("json:\n%s\n", payload_buff);
	
		msg_buff_len = GetDataPointPUBLISH(msg_buff_data, 0, 1, 0, (char *)"$dp", 0x18, (char *)payload_buff);

		if(g_login_status == yes)
		{
			do{

				_ReTrySendCount++;

				if(_ReTrySendCount > 1)
				{
					save_log("kol_log.txt", "%04d-%02d-%02d %02d:%02d:%02d -> _ReTrySendCount = %d\n", _DateTime[0], _DateTime[1], _DateTime[2], 
																									   _DateTime[3], _DateTime[4], _DateTime[5], _ReTrySendCount);
				}
				
				ret = user_data_socket_send(msg_buff_data, msg_buff_len);	
				
				printf("send ret = %d %x\n", ret, ret);

				printf("wait_ack start.\n");
				wait_ack(1500);
				printf("wait_ack end.\n");

			}while((g.PUBCOMP_PacketID != 0x18) && (_ReTrySendCount<=3));
			
			_ReTrySendCount = 0;
			
			g.PUBCOMP_PacketID = 0;
		}
		
	}
}



