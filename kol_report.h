#ifndef _REPORT_H_
#define _REPORT_H_


#include "kol_main.h"


#define RENT_LOGIN			0x01
#define RENT_REPORT			0x02
#define	RENT_RESSUE			0x03
#define RENT_LOGOUT			0x04
#define RENT_MALFUNCTION	0x05
#define RENT_HEARTBEAT		0x07
#define RENT_QUERY			0x80
#define RENT_SET			0x81
#define RENT_CTRL			0x82

#define RENT_ACK_SUCCESS	0x01
#define RENT_ACK_FAILD		0x02
#define RENT_ACK_REPETITION	0x03
#define RENT_ACK_CMD		0xFE
#define RENT_UNACK_CMD		0xFF



extern void pthread_do_data_ressue(void);
extern void pthread_data_do_report(void);

#endif /* _REPORT_H_ */
