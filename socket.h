#ifndef _SOCKET_H_
#define _SOCKET_H_

#include "main.h"
extern void pthread_keep_on_line(void);
extern void pthread_socket_data_send(void);
extern void pthread_socket_data_recv(void);

extern void wait_ack(pthread_mutex_t *mutex_wait_ack, pthread_cond_t *cond_wait_ack, u32 timeout_ms);
extern u16 user_data_socket_send(u8 *send_data_buf, u16 send_data_len);
#endif /* _SOCKET_H_ */

