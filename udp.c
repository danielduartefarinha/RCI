#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

void udp(char * command){
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

