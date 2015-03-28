#include "functions.h"

/******************************************************************************
 * search()
 *
 * Argumentos:	self			- estrutura com a informação do nó
 * 				k				- valor do identificador que se quer usar
 * 
 * Retorna: (void)
 * 
 * Side-Effects: nenhum
 *
 * Descrição: 	verfica se o nó é responsável pelo identificador requerido.
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
 * Argumentos:	self			- estrutura com a informação do nó
 * 				new				- valor que define se tem informações do
 * 								  succi ou não.
 * 
 * Retorna: int
 * 
 * Side-Effects: nenhum
 *
 * Descrição:	Envia mensagem para o succi para se ligar e caso não tenha succi
 * 				envia ao nó de arranque a perguntar quem será o seu succi.
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
 * Argumentos:	self			- estrutura com a informação do nó
 * 				x				- identificador do anel
 * 
 * Retorna: int
 * 
 * Side-Effects: nenhum
 *
 * Descrição: 	Verifica se o anel está inserido no servidor de arranque.
 * 				Caso não esteja inserido, cria o anel com as informações do no.
 * 				Caso já exista anel, actualiza o estado succi com as informaçoes
 * 				recebidas e chama a função join_succi com estas informaçoes.
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
 * Argumentos:	self			- estrutura com a informação do nó
 * 
 * Retorna: (void)
 * 
 * Side-Effects: nenhum
 *
 * Descrição: 	Função para sair do anel. 
 * 				Verifica se é o no de arranque, caso seja torna o seu succi
 * 				o novo nó de arranque.
 * 				Apaga o anel se for o último nó.
 * 				Envia mensagem CON para o predecessor de maneira a ele se ligar
 * 				ao seu sucessor.
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

void exit_app(node * self){
	leave(self);
	// Fazer frees à memória alocada
	sprintf(message, "Exited successfully.\n");
	print_verbose(message);
	exit(0);
}
