#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char ** argv){
	int fd,n, i, addrlen;
	struct sockaddr_in addr;
	struct in_addr *a;
	struct hostent *h;
	char buffer[128];
	char ringport[32];
	char bootip[100] = "tejo.tecnico.utlisboa.pt";
	int bootport = 58000;
	
	
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
 
	if((h = gethostbyname(bootip))==NULL)exit(1);	
	a=(struct in_addr*)h->h_addr_list[0];
	
	memset((void*)&addr,(int)'\0', sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr= *a;
	addr.sin_port=htons(bootport);
	
	n=sendto(fd,"BQRY 60",50,0,(struct sockaddr*)&addr, sizeof(addr));
	if(n==-1)exit(1);

	addrlen=sizeof(addr);
	n = recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen);
	if(n==-1)exit(1);
	
	if(strcmp(buffer, "EMPTY")==0){
		printf("EMPTY\nbora fazer um REG\n");
		n=sendto(fd,"REG 6 1 faribling 93",50,0,(struct sockaddr*)&addr, sizeof(addr));
		if(n==-1)exit(1);
	}
	
	h=gethostbyaddr(&addr.sin_addr, sizeof(addr.sin_addr), AF_INET);
	if(h==NULL)
		printf("set by [%s:%hu]\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
	else
		printf("sent by [%s:%hu]\n", h->h_name, ntohs(addr.sin_port));	
	
	close(fd);
	
}
