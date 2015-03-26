/* 					Gestão sinais de erro
 * -------------------------------------------------------------------
 * 		<err> 		|					<meaning>
 * -------------------------------------------------------------------
 * 		  0			|	Está tudo bem, em todos os casos
 * 		  1			|	Search retorna o próprio nó (usado após ID i)
 * 		-10			|	Usado para definir o estado <busy> na main
 * 		 12			|	Usado para sair do estado <busy> na main
 *       20         |   Usado para saber erro join
 * -------------------------------------------------------------------
*/

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
#include <signal.h>

#define _SIZE_MAX_ 128

static int errno;

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

void print_interface(int n);
struct sockaddr_in getIP(char * ip, int port);
node Init_Node(char ** argv, int argc);
int search(node * self, int k);
int join_succi(node * self, int service);
int join(node * self, int x);
int show(node * self);
int leave(node * self);
void exit_app(node * self);
int switch_listen(char * command, int fd, node * self);
int switch_cmd(char * command, node * self);

#endif

