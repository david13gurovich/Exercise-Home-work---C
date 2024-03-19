#ifndef H_UDP_SERVER_ 
#define H_UDP_SERVER_
/**************************************************/
/* this file define server udp */
/**************************************************/

void InitServerAddr(struct sockaddr_in *server_addr);
void *StartUDPServer(void *params);

#endif