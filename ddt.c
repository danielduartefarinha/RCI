int main(int argc, char ** argv){
	int n, i, err, errno, ringport;
	char buffer[128], instruction[128];
	char bootip[128] = "tejo.tecnico.ulisboa.pt";
	int bootport = 58000;
	node self;
		
	//ERROS
	
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

	self.udp_server = getIP(bootip, bootport);
	if(gethostname(buffer,128)==-1) printf("error: %s\n", strerror(errno));
	self.id.addr = getIP(buffer, ringport);
	
	printf("Esperando um comando (join, leave, show, search, exit)\n");
	
	self.ring = -1;
	while(fgets(instruction, 128, stdin) != NULL){
		err = switch_cmd(instruction, &self);
			if (err == 1) exit(1); //código de erro
	}
	exit(0);
}
