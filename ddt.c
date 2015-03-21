#include "aux.h"

int main(int argc, char ** argv){
	int n, i, err, errno, ringport, addrlen, maxfd;
	char buffer[128], instruction[128];
	char bootip[128] = "tejo.tecnico.ulisboa.pt";
	int bootport = 58000;
	node self;
	fd_set rfds;
	enum {idle, busy} state;
		
	// ERROS
	
	if(argc > 7){
		printf("Incorrect number of arguments\n");
		exit(-1);
	}
	
	//Trata argumentos
	
	for(i = 1; i < argc-1; i++){
		if (strcmp(argv[i],"-t")==0){
			if(argv[i+1][0] == '-') continue;
			n = sscanf(argv[i+1], "%d", &ringport);
			if (n != 1) exit(2);
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

	// Inicialização
	
	if(gethostname(buffer,128)==-1) printf("error: %s\n", strerror(errno));
	self.id.addr = getIP(buffer, ringport);
	self.id.id = -1;
	self.predi.id = -1;
	self.succi.id = -1;
	self.udp_server = getIP(bootip, bootport);
	self.ring = -1;	// Inicialização do numero do anel a -1
	addrlen = sizeof(self.id.addr);
	
	self.fd.keyboard = 0;
	self.fd.predi = -1;
	self.fd.succi = -1;
	
	if((self.fd.listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) exit(1);
	if(bind(self.fd.listener, (struct sockaddr*)&self.id.addr, addrlen) == -1) exit(1);
	if(listen(self.fd.listener, 5) == -1) exit(1);
	
	while(1){
		FD_ZERO(&rfds);
		FD_SET(self.fd.keyboard, &rfds);
		FD_SET(self.fd.listener, &rfds);
		maxfd = self.fd.listener;
		if (self.fd.predi != -1){
			FD_SET(self.fd.predi, &rfds);
			if(self.fd.predi > maxfd) maxfd = self.fd.predi;
		}
		if (self.fd.succi != -1){
			FD_SET(self.fd.succi, &rfds);
			if(self.fd.succi > maxfd) maxfd = self.fd.succi;
		}
		
		// Sempre que se fizer close tem de se colocar self.fd.* = -1
		
		n = select(maxfd+1, &rfds, (fd_set *) NULL, (fd_set *) NULL, (struct timeval *) NULL);
		if (n <= 0) exit(1);
		
		if (FD_ISSET(self.fd.keyboard, &rfds)){
			fgets(instruction, 128, stdin);
			err = switch_cmd(instruction, &self);
		}
		
		if (FD_ISSET(self.fd.listener, &rfds)){
			
		}
		
		if (FD_ISSET(self.fd.predi, &rfds)){
			
		}
		
		if (FD_ISSET(self.fd.succi, &rfds)){
			
		}
		
	}
		
	// err = switch_cmd(instruction, &self);
	// if (err == 1) exit(1); //código de erro
	
	exit(0);
}

