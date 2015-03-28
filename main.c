#include "keyboard.h"
#include "internode.h"

int main(int argc, char ** argv){
	int n, err, addrlen, maxfd, fd_aux;
	char buffer[_SIZE_MAX_], instruction[_SIZE_MAX_];
	node self;
	fd_set rfds;
	void (*old_handler)(int);
	
	struct sockaddr_in addr;
	int aux_addrlen;
	
	enum {idle, busy} state;
		
	// ERROS
	
	if(argc > 7){
		printf("Error: Incorrect number of arguments\n");
		exit(-1);
	}
	
	if((old_handler = signal(SIGPIPE, SIG_IGN)) == SIG_ERR) exit(1);
	
	self = Init_Node(argv, argc);
	addr = self.id.addr;
	
	addrlen = sizeof(self.id.addr);
	aux_addrlen = addrlen;
	
	if((self.fd.listener = socket(AF_INET, SOCK_STREAM, 0)) == -1) exit(1);
	if(bind(self.fd.listener, (struct sockaddr*)&self.id.addr, addrlen) == -1) exit(1);
	if(listen(self.fd.listener, 5) == -1) exit(1);
	
	print_interface(0);  // Interface Utilizador
	state = idle;
	while(1){
		FD_ZERO(&rfds);
		if(state != busy){
			FD_SET(self.fd.keyboard, &rfds);
			FD_SET(self.fd.listener, &rfds);
			maxfd = self.fd.listener;
		}
		if (self.fd.predi != -1){
			FD_SET(self.fd.predi, &rfds);
			if(self.fd.predi > maxfd) maxfd = self.fd.predi;
		}
		if (self.fd.succi != -1){
			FD_SET(self.fd.succi, &rfds);
			if(self.fd.succi > maxfd) maxfd = self.fd.succi;
		}

		print_interface(1);
		n = select(maxfd+1, &rfds, (fd_set *) NULL, (fd_set *) NULL, (struct timeval *) NULL);
		if (n <= 0){
			printf("Error: Select\n");
			exit(1);
		}
			
		if (FD_ISSET(self.fd.keyboard, &rfds)){
			fgets(instruction, _SIZE_MAX_, stdin);
			err = switch_cmd(instruction, &self);
			memset((void *) instruction, (int) '\0', _SIZE_MAX_);
		}
		
		if (FD_ISSET(self.fd.listener, &rfds)){
			fd_aux = accept(self.fd.listener, (struct sockaddr *)&addr, &aux_addrlen);
			if(fd_aux == -1){
				printf("Error: Accept\n");
				exit(0);
			}
			sprintf(message, "Opening <outside node> socket: %d\n", fd_aux);
			print_verbose(message);
			n = read(fd_aux, buffer, _SIZE_MAX_);
			if(n != 0){
				sprintf(message, "Received from <outside node>: %s", buffer);
				print_verbose(message);
				err = switch_listen(buffer, fd_aux, &self);
				if (err == -10) state = busy;
				memset((void *) buffer, (int) '\0', _SIZE_MAX_);
			}
		}			
		
		if (FD_ISSET(self.fd.predi, &rfds) && (self.fd.predi != -1)){
			n = read(self.fd.predi, buffer, _SIZE_MAX_);
			if(n != 0){
				sprintf(message, "Received from <predi>: %s", buffer);
				print_verbose(message);
				err = switch_listen(buffer, -1, &self);
				memset((void *) buffer, (int) '\0', _SIZE_MAX_); 
			}
		}
		
		if (FD_ISSET(self.fd.succi, &rfds) && (self.fd.succi != -1)){
			n = read(self.fd.succi, buffer, _SIZE_MAX_);
			if(n != 0){
				sprintf(message, "Received from <succi>: %s", buffer);
				print_verbose(message);
				switch (state){
					case idle:
						err = switch_listen(buffer, -1, &self);
						break;
					case busy:
						err = switch_listen(buffer, fd_aux, &self);
						if(err == 12) state = idle;
						break;
					default:
						break;
				}
				memset((void *) buffer, (int) '\0', _SIZE_MAX_); 			
			}
		}
	}
	exit(0);
}

