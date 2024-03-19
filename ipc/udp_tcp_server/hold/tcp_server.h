#ifndef H_TCP_SERVER_ 
#define H_TCP_SERVER_
/**************************************************/
/* this file define server tcp */
/**************************************************/

void* CreateTcpServer(void *param);
void *ReceiveAndSendTCP(void *params);
void InitServerAddr(struct sockaddr_in *server_addr);

#endif