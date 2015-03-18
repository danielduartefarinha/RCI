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
	char ip[128]; // talvez uma estrutura do tip addr? é a usada nas funções
	int tcp;	
} node_data;

typedef struct node{
	node_data id;
	node_data predi;
	node_data succi;
} node;

 typedef struct host_data{
	struct sockaddr_in addr;
 } boot;

boot getBoot(char * bootip, int bootport){
	boot newBoot;
	
	struct hostent *h;
	struct in_addr *a;
	struct sockaddr_in addr;
	
	if((h = gethostbyname(bootip))==NULL)exit(1);	
	a=(struct in_addr*)h->h_addr_list[0];
	
	memset((void*)&addr,(int)'\0', sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr= *a;
	addr.sin_port=htons(bootport);
	
	newBoot.addr = addr;
	return newBoot;
}

int main(int argc, char ** argv){
	int fd,n, i, addrlen;
	char buffer[128];
	char ringport[32];
	char bootip[128] = "tejo.tecnico.utlisboa.pt";
	int bootport = 58000;
	// node self;
	boot udp_server;
	
	//ERROS
	
	if(argc > 7){
		printf("Incorrect number of arguments\n");
		exit(-1);
	}
	
	//Trata argumentos
	
	for(i = 1; i < argc-1; i++){
		if (strcmp(argv[i],"-t")==0){
			if(argv[i+1][0] == '-') continue;
			strcpy(ringport, argv[i+1]);
		}
		if (strcmp(argv[i], "-i") == 0){
			if(argv[i+1][0] == '-') continue;
			strcpy(bootip, argv[i+1]);
		}
		if (strcmp(argv[i], "-p") == 0){
			if(argv[i+1][0] == '-') continue;
			n = sscanf(argv[i+1], "%d", &bootport);
			if (n != 1) exit(2);
		}
	}
	
	fd=socket(AF_INET, SOCK_DGRAM,0);
	if(fd==-1)exit(1);
	printf("%d\n", fd);

	udp_server = getBoot(bootip, bootport);

	n=sendto(fd,"BQRY 60",50,0,(struct sockaddr*)&udp_server.addr, sizeof(udp_server.addr));
	if(n==-1)exit(1);

	addrlen=sizeof(udp_server.addr);
	n = recvfrom(fd,buffer,128,0,(struct sockaddr*)&udp_server.addr,&addrlen);
	if(n==-1)exit(1);
	
	if(strcmp(buffer, "EMPTY")==0){
		printf("EMPTY\nbora fazer um REG\n");
		n=sendto(fd,"REG 6 1 faribling 93",50,0,(struct sockaddr*)&udp_server.addr, sizeof(udp_server.addr));
		if(n==-1)exit(1);
	}
	
	close(fd);
	
}
