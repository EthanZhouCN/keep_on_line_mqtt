#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "kol_main.h"

extern void pthread_keep_on_line(void);
extern void pthread_socket_data_send(void);
extern void pthread_socket_data_recv(void);


extern u16 user_data_socket_send(u8 *send_data_buf, u16 send_data_len);

extern volatile unsigned char PUBCOMP_PacketID;


#endif /* _SOCKET_H_ */

