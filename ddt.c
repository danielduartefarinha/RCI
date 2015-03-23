#include "aux.h"

int main(int argc, char ** argv){
	int n, err, addrlen, addr_aux_len, maxfd, fd_aux;
	char buffer[_SIZE_MAX_], instruction[_SIZE_MAX_];
	node self;
	fd_set rfds;
	void (*old_handler)(int);
	struct sockaddr_in addr_aux;
	
	// enum {idle, busy} state; Para já não está a ser usado
		
	// ERROS
	
	if(argc > 7){
		printf("Incorrect number of arguments\n");
		exit(-1);
	}
	
	if((old_handler = signal(SIGPIPE, SIG_IGN)) == SIG_ERR) exit(1);
	
	self = Init_Node(argv, argc);
	
	addrlen = sizeof(self.id.addr);
	if((self.fd.listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) exit(1);
	if(bind(self.fd.listener, (struct sockaddr*)&self.id.addr, addrlen) == -1) exit(1);
	if(listen(self.fd.listener, 5) == -1) exit(1);
	
	// Interface Utilizador
	
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
			fgets(instruction, _SIZE_MAX_, stdin);
			err = switch_cmd(instruction, &self);
		}
		
		if (FD_ISSET(self.fd.listener, &rfds)){
			fd_aux = accept(self.fd.listener, (struct sockaddr *)&addr_aux, &addr_aux_len);
			n = read(fd_aux, buffer, _SIZE_MAX_);
			printf("Este bicho: não sei o numero %s %hu mandou esta mensagem: %s", inet_ntoa(addr_aux.sin_addr), ntohs(addr_aux.sin_port), buffer);
			
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

