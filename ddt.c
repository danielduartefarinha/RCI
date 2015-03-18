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

typedef struct node{
	node_data id;
	node_data predi;
	node_data succi;
	struct sockaddr_in udp_server;
} node;

typedef struct host_data{
	struct sockaddr_in addr;
 } boot;

struct sockaddr_in getIP(char * bootip, int bootport){
	struct hostent *h;
	struct in_addr *a;
	struct sockaddr_in addr;
	
	if((h = gethostbyname(bootip))==NULL)exit(1);	
	a=(struct in_addr*)h->h_addr_list[0];
	
	memset((void*)&addr,(int)'\0', sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr= *a;
	addr.sin_port=htons(bootport);

	return addr;
}

int join(node * self, int x){
	char buffer[128];
	
	sprintf(buffer, "BQRY %d", x);
  	n=sendto(fd, buffer,50,0,(struct sockaddr*)&self->udp_server, sizeof(self->udp_server));
	if(n==-1)exit(1);

	addrlen=sizeof(self->udp_server);
	n = recvfrom(fd,buffer,128,0,(struct sockaddr*)&self->udp_server,&addrlen);
	if(n==-1)exit(1);
	
	
	if(strcmp(buffer, "EMPTY")==0){
		printf("EMPTY\nbora fazer um REG\n");
		sprintf(buffer, "REG %d %d %s %hu", x, self->id.id, inet_ntoa(self->id.addr.sin_addr),ntohs(self->id.addr.sin_port));
		n=sendto(fd, buffer,50,0,(struct sockaddr*)&self->udp_server, sizeof(self->udp_server));
		if(n==-1)exit(1);
	}
}


int switch_cmd(char * command, node * self){
	char buffer[128], succiIP[128], succiTCP[128];
	int n, x, i, succi;
	
	n = sscanf(command, "%s %d %d %d %s %s", buffer, &x, &self->id.id, &succi, succiIP, succiTCP);
	switch(n){
		case(1):
			if(strcmp(buffer, "leave") == 0){
				// Função de saída do anel
			}else{
				if(strcmp(buffer, "show") == 0){
					// Função de listagem de informações
				}else{
					if(strcmp(buffer, "exit") == 0){
						// O utilizador fecha a aplicação
					}else{
						printf("%s não é um comando válido, ou faltam argumentos\n", buffer);
					}
				}
			}
			break;
		case(2):
			if(strcmp(buffer, "search") == 0){
				// Função de procura de um identificador k, neste caso, o inteiro x
			}else{
				printf("%s não é um comando válido, ou faltam argumentos\n", buffer);
			}
			break;
		case(3):
			if(strcmp(buffer, "join") == 0){
				// Função de entrada no anel x, como identificador i
				
			}else{
				printf("%s não é um comando válido, ou faltam argumentos\n", buffer);
			}
			break;
		case(6):
			if(strcmp(buffer, "join") == 0){
				// Função de entrada no anel x, como identificador i, sabendo succi
			}else{
				printf("%s não é um comando válido, ou faltam argumentos\n", buffer);
			}
			break;
		default:
			printf("%s não é um comando válido, ou faltam argumentos\n", buffer);
			break;
	}
	
}


int main(int argc, char ** argv){
	int fd,n, i, err, addrlen;
	char buffer[128], instruction[128];
	char ringport[32];
	char bootip[128] = "tejo.tecnico.utlisboa.pt";
	int bootport = 58000;
	node self;
		
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
	
	if(gethostname(buffer,128)==-1)
		printf("error: %s\n", strerror(errno));
	
	
	fd=socket(AF_INET, SOCK_DGRAM,0);
	if(fd==-1)exit(1);
	printf("%d\n", fd);

	self.udp_server = getIP(bootip, bootport);
	self.id.addr = getIP(buffer, ringport);
	
	printf("Esperando um comando (join, leave, show, search, exit)\n");
	
	while(fgets(instruction, 128, stdin) != NULL){
		err = switch_cmd(instruction, &self);
			if (err == 1) exit(0) //código de erro
	}
	
	close(fd);
	
}
