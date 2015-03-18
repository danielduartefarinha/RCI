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

int join(boot udp_server, node * self, int x, int i){
	char buffer[128];
	
		
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
}


int switch_cmd(char * command, boot udp_server, node * self){
	char buffer[128], succiIP[128], succiTCP[128];
	int n, x, i, succi;
	
	n = sscanf(command, "%s %d %d %d %s %s", buffer, &x, &i, &succi, succiIP, succiTCP);
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
	
	printf("Esperando um comando (join, leave, show, search, exit)\n");
	
	while(fgets(instruction, 128, stdin) != NULL){
		err = switch_cmd(instruction, udp_server, &self);
			if (err == 1) exit(0) //código de erro
	}
	
	close(fd);
	
}
