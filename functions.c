#include "functions.h"

/******************************************************************************
 * search()
 *
 * Arguments:	self			- Pointer to struct with node informations
 * 				k				- Value of the node identifier to find
 * 
 * Returns: (int)
 * 
 * Side-Effects: none
 *
 * Description: 	Searches for the node that is responsible for the node
 * 	 				with the specified identifier
 * 					Verifies if the node is responsible for	the identifier required.
 * 
 *****************************************************************************/

int search(node * self, int k){
	int n;
	char buffer[_SIZE_MAX_];
	
	if (dist(k, self->id.id) < dist(self->predi.id, self->id.id)){
		sprintf(message, "This node is responsible for %d\n", k);
		print_verbose(message);
		return 1;
	}else{
		sprintf(buffer, "QRY %d %d\n", self->id.id, k);
		n = write(self->fd.succi, buffer, _SIZE_MAX_);
		sprintf(message, "Sent to <succi>: %s", buffer);
		print_verbose(message);
		return 0;
	}
}

/******************************************************************************
 * join_succi()
 *
 * Arguments:	self			- Pointer to struct with node informations
 * 				new				- Value that defines if function is called
 * 									with succi already defined, or not. 
 * 								  
 * 
 * Returns: (int)
 * 
 * Side-Effects: none
 *
 *  Description:	Function called to join to a node, knowing it is succi.
 * 					Function also called when first contacting the ring.
 * 
 *****************************************************************************/


int join_succi(node * self, int new){
	int err, addrlen;
	char buffer[_SIZE_MAX_];	
	
	self->fd.succi = socket(AF_INET, SOCK_STREAM, 0);
	if(self->fd.succi == -1) exit(1);

	addrlen = sizeof(self->succi.addr);
	err = connect(self->fd.succi, (struct sockaddr *) &self->succi.addr, (socklen_t) addrlen);
	sprintf(message, "Opening <succi> socket: %d\n", self->fd.succi);
	print_verbose(message);
	
	if (err == -1){
		printf("Error: Impossible to connect to [%s %hu]\n", inet_ntoa(self->id.addr.sin_addr), ntohs(self->id.addr.sin_port));
		close(self->fd.succi);
		sprintf(message, "Closing <succi> socket: %d\n", self->fd.succi);
		print_verbose(message);
		self->succi.id = -1;
		self->id.id = -1;
		return 30;
	}
	
	memset((void *) buffer, (int) '\0', _SIZE_MAX_);
	
	if(new) 
		sprintf(buffer, "NEW %d %s %hu\n", self->id.id, inet_ntoa(self->id.addr.sin_addr), ntohs(self->id.addr.sin_port));
	else
		sprintf(buffer, "ID %d\n", self->id.id);
		
	write(self->fd.succi, buffer, (size_t) _SIZE_MAX_);
	
	sprintf(message, "Sent to %d [%s %hu]: %s", self->succi.id, inet_ntoa(self->succi.addr.sin_addr), ntohs(self->succi.addr.sin_port), buffer);
	print_verbose(message);
	return(0);
	
}

/******************************************************************************
 * join()
 *
 * Arguments:	self			- Pointer to struct with node informations
 * 				x				- Identifier of the ring
 *  
 * Returns: (int)
 * 
 * Side-Effects: none
 *
 * Description: Check if the ring is already in the server.
 * 				If the ring exists, udpdate the state succi with all informations
 * 				received and call the function join_succi with this informations.
 * 				Otherwise, create the ring with all node informations.	
 * 
 *****************************************************************************/

int join(node * self, int x){
	int fd, addrlen, n, j, tcp, err;
	char command[_SIZE_MAX_], ip[_SIZE_MAX_];
	char buffer[_SIZE_MAX_];	
	
	fd=socket(AF_INET, SOCK_DGRAM,0);
	if(fd==-1)exit(1);
	
	sprintf(buffer, "BQRY %d\n", x);
  	n=sendto(fd, buffer,50,0,(struct sockaddr*)&self->udp_server, sizeof(self->udp_server));
	if(n==-1)exit(1);
	sprintf(message, "Sent to <%s:%hu> the message: %s", inet_ntoa(self->udp_server.sin_addr), ntohs(self->udp_server.sin_port), buffer);
	print_verbose(message);
	
	memset(buffer, '\0', _SIZE_MAX_);

	addrlen=sizeof(self->udp_server);
	n = recvfrom(fd,buffer,_SIZE_MAX_,0,(struct sockaddr*)&self->udp_server,&addrlen);
	if(n==-1)exit(1);
	sprintf(message, "Received from <%s:%hu> the message: %s\n", inet_ntoa(self->udp_server.sin_addr), ntohs(self->udp_server.sin_port), buffer);
	print_verbose(message);

	
	if(strcmp(buffer, "EMPTY")==0){
		sprintf(buffer, "REG %d %d %s %hu\n", x, self->id.id, inet_ntoa(self->id.addr.sin_addr),ntohs(self->id.addr.sin_port));
		n=sendto(fd, buffer,50,0,(struct sockaddr*)&self->udp_server, sizeof(self->udp_server));
		if(n==-1)exit(1);
		sprintf(message, "Sent to <%s:%hu> the message: %s", inet_ntoa(self->udp_server.sin_addr), ntohs(self->udp_server.sin_port), buffer);
		print_verbose(message);
		
		memset(buffer, '\0', _SIZE_MAX_);
		n = recvfrom(fd,buffer,_SIZE_MAX_,0,(struct sockaddr*)&self->udp_server,&addrlen);
		if(n==-1)exit(1);
		sprintf(message, "Received from <%s:%hu> the message: %s\n", inet_ntoa(self->udp_server.sin_addr), ntohs(self->udp_server.sin_port), buffer);
		print_verbose(message);

		if (strcmp(buffer, "OK") == 0){
			sprintf(message, "Ring %d created\n", x);
			print_verbose(message);
			self->ring = x;
			self->boot = 1;
			close(fd);
			return 0;
		}
	}else{
		sprintf(message, "Ring %d already exists\n", x);
		print_verbose(message);
		n = sscanf(buffer, "%s %d %d %s %d", command, &x, &j, ip, &tcp);
		if(n != 5) return 1;
		if(strcmp(command, "BRSP") == 0){
			if(self->id.id == j){
				printf("Error: There already exists a node with same identifier\n");
				close(fd);
				return 1;
			}else{
			self->succi.id = j;
			self->succi.addr = getIP(ip, tcp);
			self->ring = x;
			err = join_succi(self, 0);
			}
		}
	}
	close(fd);
	return err;
}


/******************************************************************************
 * leave()
 *
 * Arguments:	self			- Pointer to struct with node informations
 * 
 * Returns: (int)
 * 
 * Side-Effects: none
 *
 * Description: Function to leave the ring.
 * 				Check if it's boot node and if true, apoints succi as boot.
 * 				Erase the ring if it is the last node on the ring.
 * 				Send CON to its predi to connect to its succi.
 * 
 *****************************************************************************/

int leave(node * self){	
	int fd, addrlen, n;
	char buffer[_SIZE_MAX_];
	
	// Caso o nó não esteja em nenhum anel 
	
	if (self->ring == -1){
		sprintf(message, "This node is not inserted in any ring\n");
		print_verbose(message);
		return 1;
	}

	fd=socket(AF_INET, SOCK_DGRAM,0);
	if(fd==-1)exit(1);
		
	if(self->succi.id == -1){
		// Sou único?
		sprintf(message, "This node isn't connected to any other node\n");
		print_verbose(message);
		
		memset(buffer, '\0', _SIZE_MAX_);
		sprintf(buffer, "UNR %d\n", self->ring);
		n=sendto(fd, buffer,50,0,(struct sockaddr*)&self->udp_server, sizeof(self->udp_server));
		if(n==-1)exit(1);
		sprintf(message, "Sent to <%s:%hu> the message: %s", inet_ntoa(self->udp_server.sin_addr), ntohs(self->udp_server.sin_port), buffer);
		print_verbose(message);	
		
		memset(buffer, '\0', _SIZE_MAX_);
		n = recvfrom(fd,buffer,_SIZE_MAX_,0,(struct sockaddr*)&self->udp_server,&addrlen);
		sprintf(message, "Received from <%s:%hu> the message: %s\n", inet_ntoa(self->udp_server.sin_addr), ntohs(self->udp_server.sin_port), buffer);
		print_verbose(message);
		
		if(strcmp(buffer, "OK")==0){
			sprintf(message, "Ring %d erased\n", self->ring);
			print_verbose(message);
			self->ring = -1;
			self->id.id = -1;
			self->boot = 0;
			close(fd);
			return 0;
		}else return 1;
	}else{ 
		sprintf(message, "This node is connected to other node(s)\n");
		print_verbose(message);
		if(self->boot){				
		// Sou BOOT?
			sprintf(message, "BOOT\n");
			print_verbose(message);
			
			memset(buffer, '\0', _SIZE_MAX_);
			sprintf(buffer, "REG %d %d %s %d\n", self->ring, self->succi.id, inet_ntoa(self->succi.addr.sin_addr), ntohs(self->succi.addr.sin_port));
			n=sendto(fd, buffer,50,0,(struct sockaddr*)&self->udp_server, sizeof(self->udp_server));
			if(n==-1)exit(1);
			sprintf(message, "Sent to <%s:%hu> the message: %s", inet_ntoa(self->udp_server.sin_addr), ntohs(self->udp_server.sin_port), buffer);
			print_verbose(message);
			
			memset(buffer, '\0', _SIZE_MAX_);
			addrlen=sizeof(self->udp_server);
			n = recvfrom(fd,buffer,_SIZE_MAX_,0,(struct sockaddr*)&self->udp_server,&addrlen);
			if(n==-1)exit(1);
			sprintf(message, "Received from <%s:%hu> the message: %s\n", inet_ntoa(self->udp_server.sin_addr), ntohs(self->udp_server.sin_port), buffer);
			print_verbose(message);
			
			n = write(self->fd.succi, "BOOT\n", _SIZE_MAX_);
			if(n==-1)exit(1);
			sprintf(message, "Sent to <succi>: BOOT\n");
			print_verbose(message);
			self->boot = 0;
		}
				
		memset(buffer, '\0', _SIZE_MAX_);
		sprintf(buffer, "CON %d %s %d\n", self->succi.id, inet_ntoa(self->succi.addr.sin_addr), ntohs(self->succi.addr.sin_port));
		n = write(self->fd.predi, buffer, _SIZE_MAX_);
		if(n==-1)exit(1);
		sprintf(message, "Sent to <predi>: %s", buffer);
		print_verbose(message);
		close(self->fd.succi);
		close(self->fd.predi);
		sprintf(message, "Closing <predi> socket: %d\nClosing <succi> socket: %d\n", self->fd.predi, self->fd.succi);
		print_verbose(message);
		self->fd.predi = -1;
		self->fd.succi = -1;
		self->predi.id = -1;
		self->succi.id = -1;
		self->ring = -1;
		self->id.id = -1;
	}
	
	sprintf(message,"This node has successfully left the ring\n");
	print_verbose(message);
	return 0;
}

/******************************************************************************
 * exit_app()
 *
 * Arguments:	self			- Pointer to struct with node informations
 * 
 * Returns: (void)
 * 
 * Side-Effects: nenhum
 *
 * Description:   Function to close the program.	
 * 				  Call the fuction leave and close the program	
 * 
 *****************************************************************************/

void exit_app(node * self){
	leave(self);
	// Fazer frees à memória alocada
	sprintf(message, "Exited successfully.\n");
	print_verbose(message);
	exit(0);
}

