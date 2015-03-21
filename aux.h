#ifndef AUX_H
#define AUX_H

#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

typedef struct node_data{
	int  id;	
	struct sockaddr_in addr;
} node_data;

typedef struct sockets{
	int keyboard;
	int succi;
	int predi;
	int listener;
} sockets;

typedef struct node{
	node_data id;
	node_data predi;
	node_data succi;
	struct sockaddr_in udp_server;
	int ring;
	sockets fd;
} node;

struct sockaddr_in getIP(char * ip, int port);
int join(node * self, int x);
int leave(node * self);
void exit_app(node * self);
int switch_cmd(char * command, node * self);

#endif
