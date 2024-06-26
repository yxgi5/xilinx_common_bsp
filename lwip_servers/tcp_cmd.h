#ifndef __TCP_CMD_H__

//#include "xparameters.h"
#if defined (XPAR_XEMACPS_NUM_INSTANCES) || defined (XPAR_XAXIETHERNET_NUM_INSTANCES)
#if defined (TCP_COMMAND_SRV)
#define __TCP_CMD_H__

#define TCP_CMD_SVR_PORT            (5555)

void tcp_cmd_svr_send_msg(const char *msg);
void transfer_tcp_cmd_data(void);
int start_tcp_cmd_application(void);
void print_tcp_cmd_header(void);

#endif // TCP_COMMAND_SRV
#endif // XPAR_XEMACPS_NUM_INSTANCES || XPAR_XAXIETHERNET_NUM_INSTANCES
#endif // __TCP_CMD_H__
