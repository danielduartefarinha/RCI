#ifndef _NODE_H_
#define _NODE_H_

#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define _SIZE_MAX_ 128

static int errno;
static int verbose;
static char message[_SIZE_MAX_];

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
	int boot;
	int ring;
	sockets fd;
} node;

#endif

