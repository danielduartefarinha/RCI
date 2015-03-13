#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char ** argv){
	int fd,n, addrlen;
	struct sockaddr_in addr;
	struct in_addr *a;
	struct hostent *h;
	char buffer[128];
	
	
	//ERROS
	
	
	if(argc != 7){
		printf("Incorrect number of arguments\n");
		exit(-1);
	}else{
		if((argv[1][0] != '-' || argv[3][0] != '-' || argv[5][0] != '-') || (argv[2][0] == '-' || argv[4][0] == '-' || argv[6][0] == '-')){
			printf("Invalid arguments. Format must be [-t ringport] [-i bootIP] [-p bootport]\n");
			exit(-2);
		}
	}
	
	
	
	exit(1);
		
	
	
	fd=socket(AF_INET, SOCK_DGRAM,0);
	if(fd==-1)exit(1);
	printf("%d\n", fd);
 
	if((h = gethostbyname("tejo.tecnico.ulisboa.pt"))==NULL)exit(1);	
	a=(struct in_addr*)h->h_addr_list[0];
	
	memset((void*)&addr,(int)'\0', sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr= *a;
	addr.sin_port=htons(58000);
	
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
