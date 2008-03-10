#include <stdio.h>
#include <stdlib.h>
#include "libcgym.h"

int main(int argc, char**argv){
	if (argc<2 || argc>3){
		fprintf(stderr,"usage:%s directory [port]",argv[0]);
		exit(1);
	}
	if (argc==2){
		printf("Using default port:%d",CGYM_DEFAULT_PORT);
	}
	else{
		printf("Using port:%s",argv[2]);
	}
	return 0;
}
