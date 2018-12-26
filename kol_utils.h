#ifndef _RENT_CONFIG_H_
#define _RENT_CONFIG_H_

#include "kol_main.h"


typedef struct{
	u8t  server_ip[70];					//租赁平台 IP地址
	u16t server_port;						//租赁平台 端口
	u16t hb_interval;			//租赁心跳发送周期
}rent_config_t;

extern rent_config_t g_rent_config;

extern void pri_journal(uint8 *title, uint8 *pkg, uint16 position);
extern uint8 rent_check_bcc(uint8 *pkg, uint16 len);
extern int getfirstfile(char *basePath,char *filename);
extern void wait_ack(u32 timeout_ms);


extern void init_rent_dir(void);
extern void rent_set_config(void);
extern void rent_get_config(void);
extern void save_log(char    *filename, char *fmt, ...);


#endif /* _RENT_CONFIG_H_ */
