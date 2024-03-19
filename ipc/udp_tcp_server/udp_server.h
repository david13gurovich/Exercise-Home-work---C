#ifndef H_UDP_SERVER_ 
#define H_UDP_SERVER_
/**************************************************/
/* this file define server udp */
/**************************************************/

int CreateUdpServerAndBind( int *udp_socket, ilrd::Logger *log);
void SendAndRecieveUDP(int sockfd, ilrd::Logger *log);

#endif