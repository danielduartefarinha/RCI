#include "internode.h"

/******************************************************************************
 * switch_listen()
 *
 * Arguments:	command			        - Command received by socket
 * 				fd						- File descriptor
 *              self                   	- Pointer to struct with node informations 				
 *
 * Returns: (int)
 * 
 * Side-Effects: none
 *
 * Description: 	Function that analyses all messages sent by other nodes
 * 
 *****************************************************************************/

int switch_listen(char * command, int fd, node * self){
	char buffer[_SIZE_MAX_], id_ip[_SIZE_MAX_];	
	int n, id, id_tcp, k, j, err;
		
	n = sscanf(command, "%s", buffer);
	if(n != 1) return 1;  //codigo de erro
	
	if(strcmp(buffer, "NEW") == 0){
		n = sscanf(command, "%*s %d %s %d", &id, id_ip, &id_tcp);
		if (n != 3) return 1; //codigo de erro
		
		
		if(self->predi.id != -1 && self->succi.id != id){
			memset((void*)buffer, (int)'\0', _SIZE_MAX_);
			sprintf(buffer, "CON %d %s %d\n", id, id_ip, id_tcp);
			write(self->fd.predi, buffer, _SIZE_MAX_);
			sprintf(message, "Sent to <predi> the message: %s", buffer);
			print_verbose(message);
			sprintf(message, "Closing <predi> socket: %d\n", self->fd.predi);
			print_verbose(message);
			close(self->fd.predi);
		}
		self->predi.id = id;
		self->predi.addr = getIP(id_ip, id_tcp);
		self->fd.predi = fd;
		sprintf(message, "Closing <outside node> socket: %d\n", fd);
		print_verbose(message);
		sprintf(message, "Opening <predi> socket: %d\n", self->fd.predi);
		print_verbose(message);
		
		if(self->succi.id == -1){
			self->succi.id = id;
			self->succi.addr = getIP(id_ip, id_tcp);
			err = join_succi(self, 1);
		}
	}	
	if(strcmp(buffer, "CON") == 0){
		n = sscanf(command, "%*s %d %s %d", &id, id_ip, &id_tcp);
		if (n != 3) return 1; //codigo de erro
		if(self->id.id == id){
			if(self->fd.predi != -1){
				sprintf(message, "Closing <predi> socket: %d\n", self->fd.predi);
				print_verbose(message);
				close(self->fd.predi);
				self->fd.predi = -1;
			}
			sprintf(message, "Closing <succi> socket: %d\n", self->fd.succi);
			print_verbose(message);			
			close(self->fd.succi);
			self->succi.id = -1;
			self->predi.id = -1;
			self->fd.succi = -1;
		}else{
			self->succi.id = id;
			self->succi.addr = getIP(id_ip, id_tcp);
			sprintf(message, "Closing <succi> socket: %d\n", self->fd.succi);
			print_verbose(message);
			close(self->fd.succi);
			err = join_succi(self, 1);
		}
	}	
	if(strcmp(buffer, "QRY") == 0){
		n = sscanf(command, "%*s %d %d", &id, &k);
		if (n != 2) return 1; //codigo de erro
		if (dist(k, self->id.id) < dist(self->predi.id, self->id.id)){
			sprintf(message, "This node is responsible for %d\n", k);
			print_verbose(message);
			sprintf(buffer, "RSP %d %d %d %s %d\n", id, k, self->id.id, inet_ntoa(self->id.addr.sin_addr), ntohs(self->id.addr.sin_port));
			n = write(self->fd.predi, buffer, _SIZE_MAX_);
			sprintf(message, "Sent to <predi>: %s", buffer);
			print_verbose(message);
		}else{
			n = write(self->fd.succi, command, _SIZE_MAX_);
			sprintf(message, "Sent to <succi>: %s", command);
			print_verbose(message);
		}
	}	
	if(strcmp(buffer, "RSP") == 0){
		n = sscanf(command, "%*s %d %d %d %s %d", &j, &k, &id, id_ip, &id_tcp);
		if (n != 5) return 1; //codigo de erro
		if(j == self->id.id){
			if(fd == -1){
				sprintf(message, "The node %d is responsible for %d\n", id, k);
				print_verbose(message);
				err = 0;
			}else{
				sprintf(buffer, "SUCC %d %s %d\n", id, id_ip, id_tcp);
				n = write(fd, buffer, _SIZE_MAX_);
				err = 12;
				sprintf(message, "Sent to <outside node>: %s", buffer);
				print_verbose(message);
				close(fd);
				sprintf(message, "Closing <outside node> socket: %d\n", fd);
				print_verbose(message);
			}
		}else{
			n = write(self->fd.predi, command, _SIZE_MAX_);
			sprintf(message, "Sent to <predi>: %s", command);
			print_verbose(message);
			err = 0;
		}
	}		
	if(strcmp(buffer, "ID") == 0){
		if(self->succi.id == -1){
			sprintf(message, "This is the only node in the ring\n");
			print_verbose(message);
			sprintf(buffer, "SUCC %d %s %d\n", self->id.id, inet_ntoa(self->id.addr.sin_addr),ntohs(self->id.addr.sin_port));
			n = write(fd, buffer, _SIZE_MAX_);
			sprintf(message, "Sent to <outside node>: %s", buffer);
			print_verbose(message);
			close(fd);
			sprintf(message, "Closing <outside node> socket: %d\n", fd);
			print_verbose(message);			
		}else{
			n = sscanf(command, "%*s %d", &k);
			if(n != 1) return -1; //codigo de erro
		
			if(search(self, k) == 1){
				sprintf(buffer, "SUCC %d %s %d\n", self->id.id, inet_ntoa(self->id.addr.sin_addr),ntohs(self->id.addr.sin_port));
				n = write(fd, buffer, _SIZE_MAX_);
				sprintf(message, "Sent to <outside node>: %s", buffer);
				print_verbose(message);
				close(fd);
				sprintf(message, "Closing <outside node> socket: %d\n", fd);
				print_verbose(message);					
			}else{
				err = -10;
			}
		}
	}	
	if(strcmp(buffer, "SUCC") == 0){
		n = sscanf(command, "%*s %d %s %d", &id, id_ip, &id_tcp);
		if (n != 3) return 1; //codigo de erro
		if(self->id.id == id){
			printf("Error: There already exists a node with same identifier\n");
					
			self->succi.id = -1;
			self->id.id = -1;
			self->ring = -1;
			close(self->fd.succi);
			sprintf(message, "Closing <succi> socket: %d\n", self->fd.succi);
			print_verbose(message);
			self->fd.succi = -1;
		}else{
			self->succi.id = id;
			self->succi.addr = getIP(id_ip, id_tcp);
			sprintf(message, "Closing <succi> socket: %d\n", self->fd.succi);
			print_verbose(message);
			close(self->fd.succi);
			err = join_succi(self, 1);
		}
	}	
	if(strcmp(buffer, "BOOT") == 0){
		self->boot = 1;
		close(self->fd.predi);
		sprintf(message, "Closing <predi> socket: %d\n", self->fd.predi);
		print_verbose(message);
		self->fd.predi = -1;
		self->predi.id = -1;
		err = 0;
	}

	return err;
}

