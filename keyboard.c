#include "keyboard.h"

int switch_cmd(char * command, node * self){
	char buffer[_SIZE_MAX_], succiIP[_SIZE_MAX_];
	int n, err, x, succi, succiTCP;
	
	n = sscanf(command, "%s %d %d %d %s %d", buffer, &x, &self->id.id, &succi, succiIP, &succiTCP);
	if(self->id.id >= 64){
		printf("Error: Node identifier must be less than 64\n");
		self->id.id = -1;
		return 20;
	}
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
						printf("No command found <%s> or missing arguments\n", buffer);
					}
				}
			}
			break;
		case(2):
			if(strcmp(buffer, "search") == 0){
				err = search(self, x);
			}else{
				printf("No command found <%s> or missing arguments\n", buffer);
			}
			break;
		case(3):
			if(strcmp(buffer, "join") == 0){
				if(self->ring == -1){
					err = join(self, x);
				}else{
					printf("Error: Node is already in the ring %d\n", self->ring);
					err = 3;
				}
			}else{
				printf("No command found <%s> or missing arguments\n", buffer);
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
					printf("Error: Node is already in the ring %d\n", self->ring);
					err = 3;
				}
			}else{
				printf("No command found <%s> or missing arguments\n", buffer);
			}
			break;
		default:
			printf("No command found <%s> or missing arguments\n", buffer);
			break;
	}
	return err;
}

