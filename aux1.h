/* 					Gestão sinais de erro
 * -------------------------------------------------------------------
 * 		<err> 		|					<meaning>
 * -------------------------------------------------------------------
 * 		  0			|	Está tudo bem, em todos os casos
 * 		  1			|	Search retorna o próprio nó (usado após ID i)
 * 		-10			|	Usado para definir o estado <busy> na main
 * 		 12			|	Usado para sair do estado <busy> na main
 *       20         |   Usado para saber erro join (ID > 64)
 *       30         |   Usado para saber erro join_succi
 * -------------------------------------------------------------------
*/

#ifndef AUX_H
#define AUX_H

#include "node.h"

void print_verbose(char * message);
void print_interface(int n);
struct sockaddr_in getIP(char * ip, int port);
node Init_Node(char ** argv, int argc);
int dist(int k, int id);
int show(node * self);

#endif

