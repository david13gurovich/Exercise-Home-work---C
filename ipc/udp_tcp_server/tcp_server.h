#ifndef H_TCP_SERVER_ 
#define H_TCP_SERVER_
/**************************************************/
/* this file define server tcp */
/**************************************************/

int CreateTcpServerAndBind(ilrd::Logger *log, int *tcp_socket, 
                            struct sockaddr_in *server_addr);
int HandleNewConnection(struct sockaddr_in *server_addr,
                        int *client_socket, int master_socket,
                        fd_set *readfds, ilrd::Logger *log);
int RecieveAndSend(int *client_sockets, ilrd::Logger *log, fd_set *readfds);

#endif