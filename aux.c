#include "aux.h"

void print_interface(int n){
	switch (n){
		case 0:
			printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
			printf("|                   DDT - Commands                       |\n");
			printf("|--------------------------------------------------------|\n");
			printf("|join x i                                                |\n");
			printf("|join x i succi succi.IP succi.TCP                       |\n");
			printf("|leave                                                   |\n");
			printf("|show                                                    |\n");
			printf("|search k                                                |\n");
			printf("|exit                                                    |\n");
			break;
		case 1:
			printf("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
			break;
		case 2:
			printf("**********************************************************\n");
			break;
		default:
			break;
	}
	
	
}

struct sockaddr_in getIP(char * ip, int port){
	struct hostent *h;
	struct in_addr *a, *b;
	struct sockaddr_in addr;
	
	if((h = gethostbyname(ip))==NULL){
		exit(1);
	}	
	
	a=(struct in_addr*)h->h_addr_list[0];
	
	memset((void*)&addr,(int)'\0', sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr= *a;
	addr.sin_port=htons(port);

	return addr;
}

node Init_Node(char ** argv, int argc){
	node new;
	int i, n;
	char buffer[_SIZE_MAX_];
	char bootip[_SIZE_MAX_] = "tejo.tecnico.ulisboa.pt";
	int bootport = 58000;
	int ringport = 4949; //voltar ao -1 mais tarde
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
	
	if(gethostname(buffer, _SIZE_MAX_) == -1) printf("error: %s\n", strerror(errno));
	new.id.addr = getIP(buffer, ringport);
	new.id.id = -1;
	new.predi.id = -1;
	new.succi.id = -1;
	new.udp_server = getIP(bootip, bootport);
	
	new.ring = -1;	// Inicialização do numero do anel a -1
	
	new.fd.keyboard = 0;
	new.fd.predi = -1;
	new.fd.succi = -1;
	
	return new;
}

int dist(int k, int id){
	if (id >= k) return (id - k);
	else return (64 + id - k);
}

int search(node * self, int k){
	int n;
	char buffer[_SIZE_MAX_];
	
	if (dist(k, self->id.id) < dist(self->predi.id, self->id.id)){
		printf("Sou eu o responsável! Vou responder!\n");
		printf("O nó %d (eu) é responsavel por %d\n", self->id.id, k);
	}else{
		sprintf(buffer, "QRY %d %d\n", self->id.id, k);
		n = write(self->fd.succi, buffer, _SIZE_MAX_);
		if (n > 0) return 0;
		else return 1;
	}
}

int join_succi(node * self, int new){
	int err, addrlen;
	char buffer[_SIZE_MAX_];
	
	
	self->fd.succi = socket(AF_INET, SOCK_STREAM, 0);
	if(self->fd.succi == -1) exit(1);

	addrlen = sizeof(self->succi.addr);
	err = connect(self->fd.succi, (struct sockaddr *) &self->succi.addr, (socklen_t) addrlen);
	printf("Aberto o socket %d (succi)\n", self->fd.succi);
	
	if (err == -1) printf("error: %s\n", strerror(errno));
	
	memset((void *) buffer, (int) '\0', _SIZE_MAX_);
	
	if(new) 
		sprintf(buffer, "NEW %d %s %hu\n", self->id.id, inet_ntoa(self->id.addr.sin_addr), ntohs(self->id.addr.sin_port));
	else
		sprintf(buffer, "ID %d\n", self->id.id);
		
	write(self->fd.succi, buffer, (size_t) _SIZE_MAX_);
	
	printf("Enviado para o nó %d %s %hu a mensagem %s", self->succi.id, inet_ntoa(self->succi.addr.sin_addr), ntohs(self->succi.addr.sin_port), buffer);
	return(0);
	
}

int join(node * self, int x){
	int fd, addrlen, n, j, tcp, err;
	char command[_SIZE_MAX_], ip[_SIZE_MAX_];
	char buffer[_SIZE_MAX_];
	
	
	fd=socket(AF_INET, SOCK_DGRAM,0);
	if(fd==-1)exit(1);
	
	sprintf(buffer, "BQRY %d", x);
  	n=sendto(fd, buffer,50,0,(struct sockaddr*)&self->udp_server, sizeof(self->udp_server));
	if(n==-1)exit(1);

	memset(buffer, '\0', _SIZE_MAX_);

	addrlen=sizeof(self->udp_server);
	n = recvfrom(fd,buffer,_SIZE_MAX_,0,(struct sockaddr*)&self->udp_server,&addrlen);
	if(n==-1)exit(1);
	
	if(strcmp(buffer, "EMPTY")==0){
		printf("EMPTY\nbora fazer um REG\n");
		sprintf(buffer, "REG %d %d %s %hu", x, self->id.id, inet_ntoa(self->id.addr.sin_addr),ntohs(self->id.addr.sin_port));
		n=sendto(fd, buffer,50,0,(struct sockaddr*)&self->udp_server, sizeof(self->udp_server));
		if(n==-1)exit(1);
		memset(buffer, '\0', _SIZE_MAX_);
		n = recvfrom(fd,buffer,_SIZE_MAX_,0,(struct sockaddr*)&self->udp_server,&addrlen);
		if(n==-1)exit(1);
		if (strcmp(buffer, "OK") == 0){
			printf("Anel %d criado\n", x);
			self->ring = x;
			close(fd);
			return 0;
		}
	}else{
		n = sscanf(buffer, "%s %d %d %s %d", command, &x, &j, ip, &tcp);
		if(n != 5) return 1;
		if(strcmp(command, "BRSP") == 0){
			self->succi.id = j;
			self->succi.addr = getIP(ip, tcp);
			self->ring = x;
			err = join_succi(self, 0);
		}
	}
	close(fd);
	printf("Anel %d já existente\n", x);
	return 1;
}

int show(node * self){
	print_interface(2);
	printf("Olá sou o %s:%hu\n", inet_ntoa(self->id.addr.sin_addr), ntohs(self->id.addr.sin_port));
	if(self->ring != -1){
		printf("Estou inserido no anel: %d e sou o nó: %d\n", self->ring, self->id.id);
	}else{
		printf("Não estou inserido em nenhum anel :(\n");
	}
	if(self->predi.id != -1){
		printf("O meu predi é %s:%hu\n", inet_ntoa(self->predi.addr.sin_addr), ntohs(self->predi.addr.sin_port));
	}else{
		printf("Não tenho predi :(\n");
	}
	if(self->succi.id != -1){
		printf("O meu succi é %s:%hu\n", inet_ntoa(self->succi.addr.sin_addr), ntohs(self->succi.addr.sin_port));
	}else{
		printf("Não tenho succi :(\n");
	}
	return 0;
}

int leave(node * self){	
	int fd, addrlen, n;
	char buffer[_SIZE_MAX_];
	
	if (self->ring == -1){
		printf("O nó não está inserido em nenhum anel\n");
		return 1;
	}
	
	fd=socket(AF_INET, SOCK_DGRAM,0);
	if(fd==-1)exit(1);
	
	sprintf(buffer, "UNR %d", self->ring);
  	n=sendto(fd, buffer,50,0,(struct sockaddr*)&self->udp_server, sizeof(self->udp_server));
	if(n==-1)exit(1);

	memset(buffer, '\0', _SIZE_MAX_);

	addrlen=sizeof(self->udp_server);
	n = recvfrom(fd,buffer,_SIZE_MAX_,0,(struct sockaddr*)&self->udp_server,&addrlen);
	if(n==-1)exit(1);
	
	if(strcmp(buffer, "OK")==0){
		printf("Anel %d apagado\n", self->ring);
		self->ring = -1;
		close(fd);
		return 0;
	}
	printf("Ocorreu algum problema\n");
	return 1;
}

void exit_app(node * self){
	leave(self);
	// Fazer frees à memória alocada
	printf("A sair da aplicação.\n");
	exit(0);
}

int switch_listen(char * command, int fd, node * self){
	char buffer[_SIZE_MAX_], id_ip[_SIZE_MAX_];	
	int n, id, id_tcp, k, j, err;
		
	n = sscanf(command, "%s", buffer);
	if(n != 1) return 1;  //codigo de erro
	
	if(strcmp(buffer, "NEW") == 0){
		n = sscanf(command, "%*s %d %s %d", &id, id_ip, &id_tcp);
		if (n != 3) return 1; //codigo de erro
		
		
		if(self->predi.id != -1){
			memset((void*)buffer, (int)'\0', _SIZE_MAX_);
			sprintf(buffer, "CON %d %s %d\n", id, id_ip, id_tcp);
			write(self->fd.predi, buffer, _SIZE_MAX_);
			printf("Enviado para o nó %d %s %hu a mensagem %s", self->predi.id, inet_ntoa(self->predi.addr.sin_addr), ntohs(self->predi.addr.sin_port), buffer);
			printf("Fechar o socket %d (predi)\n", self->fd.predi);
			close(self->fd.predi);
		}
		self->predi.id = id;
		self->predi.addr = getIP(id_ip, id_tcp);
		self->fd.predi = fd;
		printf("Aberto o socket %d (predi)\n", self->fd.predi);
		
		if(self->succi.id == -1){
			self->succi.id = id;
			self->succi.addr = getIP(id_ip, id_tcp);
			err = join_succi(self, 1);
		}
	}
	if(strcmp(buffer, "CON") == 0){
		n = sscanf(command, "%*s %d %s %d", &id, id_ip, &id_tcp);
		if (n != 3) return 1; //codigo de erro
		self->succi.id = id;
		self->succi.addr = getIP(id_ip, id_tcp);
		printf("Fechar o socket %d (succi)\n", self->fd.succi);
		close(self->fd.succi);
		err = join_succi(self, 1);
	}
	if(strcmp(buffer, "QRY") == 0){
		n = sscanf(command, "%*s %d %d", &id, &k);
		if (n != 2) return 1; //codigo de erro
		if (dist(k, self->id.id) < dist(self->predi.id, self->id.id)){
			printf("Sou eu o responsável! Vou responder!\n");
			sprintf(buffer, "RSP %d %d %d %s %d\n", id, k, self->id.id, inet_ntoa(self->id.addr.sin_addr), ntohs(self->id.addr.sin_port));
			n = write(self->fd.predi, buffer, _SIZE_MAX_);
		}else{
			n = write(self->fd.succi, command, _SIZE_MAX_);
		}
	}
	if(strcmp(buffer, "RSP") == 0){
		n = sscanf(command, "%*s %d %d %d %s %d", &j, &k, &id, id_ip, &id_tcp);
		if (n != 5) return 1; //codigo de erro
		if(j == self->id.id){
			if(fd == -1){
				printf("O nó %d é responsavel por %d\n", id, k);
				err = 0;
			}else{
				sprintf(buffer, "SUCC %d %s %d\n", id, id_ip, id_tcp);
				n = write(fd, buffer, _SIZE_MAX_);
				printf("Enviado para o nó externo a mensagem %s", buffer);
			}
		}else{
			n = write(self->fd.predi, command, _SIZE_MAX_);
			err = 0;
		}
	}	
	if(strcmp(buffer, "ID") == 0){
		n = sscanf(command, "%*s %d", &k);
		if(n != 1) return 1; //codigo de erro
		search(self, k);
		err = -10;
	}
	if(strcmp(buffer, "SUCC") == 0){
		n = sscanf(command, "%*s %d %s %d", &id, id_ip, &id_tcp);
		if (n != 3) return 1; //codigo de erro
		self->succi.id = id;
		self->succi.addr = getIP(id_ip, id_tcp);
		printf("Fechar o socket %d (succi)\n", self->fd.succi);
		close(self->fd.succi);
		err = join_succi(self, 1);
	}
	return err;
}

int switch_cmd(char * command, node * self){
	char buffer[_SIZE_MAX_], succiIP[_SIZE_MAX_];
	int n, err, x, succi, succiTCP;
	
	n = sscanf(command, "%s %d %d %d %s %d", buffer, &x, &self->id.id, &succi, succiIP, &succiTCP);
	switch(n){
		case(1):
			if(strcmp(buffer, "leave") == 0){
				err = leave(self);
			}else{
				if(strcmp(buffer, "show") == 0){
					err = show(self);
				}else{
					if(strcmp(buffer, "exit") == 0){
						exit_app(self);
					}else{
						printf("%s não é um comando válido, ou faltam argumentos\n", buffer);
					}
				}
			}
			break;
		case(2):
			if(strcmp(buffer, "search") == 0){
				err = search(self, x);
			}else{
				printf("%s não é um comando válido, ou faltam argumentos\n", buffer);
			}
			break;
		case(3):
			if(strcmp(buffer, "join") == 0){
				if(self->ring == -1){
					err = join(self, x);
				}else{
					printf("O nó ja está inserido no anel %d, não pode ser adicionado a outro\n", self->ring);
					err = 3;
				}
			}else{
				printf("%s não é um comando válido, ou faltam argumentos\n", buffer);
			}
			break;
		case(6):
			if(strcmp(buffer, "join") == 0){
				if(self->ring == -1){
					self->succi.id = succi;
					self->succi.addr = getIP(succiIP, succiTCP);
					self->ring = x;
					err = join_succi(self, 1);
				}else{
					printf("O nó ja está inserido no anel %d, não pode ser adicionado a outro\n", self->ring);
					err = 3;
				}
			}else{
				printf("%s não é um comando válido, ou faltam argumentos\n", buffer);
			}
			break;
		default:
			printf("%s não é um comando válido, ou faltam argumentos\n", buffer);
			break;
	}
	return err;
}

