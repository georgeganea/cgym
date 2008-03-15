#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include "libcgym.h"
#include "cgymd.h"
#define BACKLOG 10     // how many pending connections queue will hold

void *client_handler(void *p);
char *homedir;
int main(int argc, char**argv){
	int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct sockaddr_in my_addr;    // my address information
	struct sockaddr_in their_addr; // connector's address information
	int port;
	int yes=1;
	socklen_t sin_size;
	if (argc<2 || argc>3){
		fprintf(stderr,"usage:%s directory [port]",argv[0]);
		exit(1);
	}
	homedir=malloc(strlen(argv[1])+1);
	strcpy(homedir,argv[1]);
	if (argc==2){
		printf("Using default port:%d\n",CGYM_DEFAULT_PORT);
		port = CGYM_DEFAULT_PORT;
	}
	else{
		printf("Using port:%s\n",argv[2]);
		port = strtol(argv[2],NULL,10);
	}
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        exit(1);
    }
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
    }
    my_addr.sin_family = AF_INET;         // host byte order
    my_addr.sin_port = htons(port);     // short, network byte order
    my_addr.sin_addr.s_addr = INADDR_ANY; // automatically fill with my IP
    memset(my_addr.sin_zero, '\0', sizeof my_addr.sin_zero);
    
    if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof my_addr) == -1) {
        perror("bind");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }
    while(1) {  // main accept() loop
    	sin_size = sizeof their_addr;
    	if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size)) == -1) {
    		perror("accept");
    	    continue;
    	}
    	printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));
    	pthread_t client_thread;
    	if (pthread_create(&client_thread, NULL, client_handler, (void *)&new_fd)!=0){
    	    	fprintf(stderr,"pthread create error\n");
    	}
    	pthread_join(client_thread, NULL);
    	close(new_fd);  // parent doesn't need this
    }
	return 0;
}

char *getNextArg(char *line,int *space){
	int i,j=0,n=0;
	char *tmp=NULL;
	tmp=malloc(255*sizeof(char));
	for(i=0;i<strlen(line);i++){
		if((i==0)&&(isspace(line[i]))){
			n++;
			continue;
		}
		if(!isspace(line[i])){
			tmp[j]=line[i];
			j++;
		}
		else{
			if((isspace(line[i-1]))&&(isspace(line[i]))){
				n++;
				continue;
			}
			*space=n;
			return tmp;	
		}
	}
	*space=n;
	return tmp;

}
int checkCommand(char* cmd){	
	char* tmp;
	int i;
	tmp=malloc(strlen(cmd)+1);
	strcpy(tmp,cmd);
	for(i=0;i<strlen(cmd);i++){
		*(tmp+i)=toupper(*(tmp+i));
	}
	if(strcmp(tmp,"LIST")==0) 
		return 0;
	if(strcmp(tmp,"SIZE")==0) 
		return 1;
	if(strcmp(tmp,"GET")==0) 
		return 2;
	if(strcmp(tmp,"QUIT")==0) 
		return 3;
	return -1;
}
int minOf2(int a,int b){
	return a > b ? b:a;
}

void* client_handler(void *p){

	printf("Handling client request\n");
	int fd = *(int*)p;
	char bu[20];
	char *tmp;
	char * command,*file=NULL,*dir=NULL,*start=NULL,*stop=NULL;
	char *buffer,*s=NULL;
	int r=-1,n=0,size=0,i,cmd; 
	if (send(fd, CGYM_ACK_MSG, strlen(CGYM_ACK_MSG), 0) == -1){
	      perror("send");
	      pthread_exit(NULL);
	}
	do{
		if((r=recv(fd,bu,sizeof(bu),0))>0){
			n+=r;
			if((buffer=realloc(s,n))){
				s=buffer;
				strcat(s,bu);
				size+=r;
			}
			else{
				perror("Error @ realloc!\n");
				break;
			}
		}
	} while(r==sizeof(bu));
	
	if((s[size-3]!='\r')||(s[size-2]!='\n')){
		if (send(fd, CGYM_ERR_MSG, strlen(CGYM_ERR_MSG), 0) == -1){
			perror("send");
			pthread_exit(NULL);
		}
	}
	
	s[size-3]=' ';s[size-2]=' ';
	char *arg1=NULL,*arg2=NULL,*arg3=NULL;
	printf("INCEPUT\n");
	i=0;
	while((tmp=strtok(s," "))!=NULL){
		switch (i){
		case 0:{
			command=malloc(strlen(tmp)+1);
			strcpy(command,tmp);
			break;
		}
		case 1:{
			arg1=malloc(strlen(tmp)+1);
			strcpy(arg1,tmp);
			break;
		}
		case 2:{
			arg2=malloc(strlen(tmp)+1);
			strcpy(arg2,tmp);
			break;
		}
		case 3:{
			arg3=malloc(strlen(tmp)+1);
			strcpy(arg3,tmp);
			break;
		}
		default:{ //too many arguments
			if (send(fd, CGYM_ERR_MSG, strlen(CGYM_ERR_MSG), 0) == -1){
				perror("send");
				pthread_exit(NULL);
			}
		}
		}
		s=s+strlen(tmp)+1;
		tmp=NULL;
		i++;
	}
	printf("COMMAND:%s\n",command);
	cmd=checkCommand(command);
	printf("CMD=%d\n",cmd);
	switch(cmd){
	case 0 :{ //LIST
		dir=malloc(strlen(arg1)+1);
		strcpy(dir,arg1);
		
		FILE_INFO* file_info;
		file_info = list(dir);
		if(file_info==NULL){
			if (send(fd, CGYM_ERR_MSG, strlen(CGYM_ERR_MSG), 0) == -1){
				perror("send");
				pthread_exit(NULL);
			}
		}
		if (send(fd, CGYM_OK_MSG, strlen(CGYM_OK_MSG), 0) == -1){
			perror("send");
			pthread_exit(NULL);
		}
		printf("IN LIST!\n");
		while (file_info->next) { 
			char *buffer=cgym_entry_tostring(file_info->entry_file);
			unsigned long dim=strlen(buffer);
			int j;
			for(j=0;j<(int)(dim/MAX_SIZE)+1;j++){
				if (send(fd, buffer+j*MAX_SIZE,minOf2(strlen(buffer+j*MAX_SIZE),MAX_SIZE), 0) == -1){
					perror("send");		
					pthread_exit(NULL);
				}
			}
			file_info = file_info->next; 
		}
		if (send(fd, CGYM_END_MSG, strlen(CGYM_END_MSG), 0) == -1){
			perror("send");
			pthread_exit(NULL);
		}
		break;
	}
	case 1 :{ //SIZE
		file=malloc(strlen(arg1)+1);
		strcpy(file,arg1);
		char* buffer;
		if(buffer==NULL){
			if (send(fd, CGYM_ERR_MSG, strlen(CGYM_ERR_MSG), 0) == -1){
				perror("send");
				pthread_exit(NULL);
			}
		}
		if (send(fd, CGYM_OK_MSG, strlen(CGYM_OK_MSG), 0) == -1){
			perror("send");
			pthread_exit(NULL);
		}
		
		break;
	}
	case 2 :{ //GET
		start=malloc(strlen(arg1)+1);
		strcpy(start,arg1);
		stop=malloc(strlen(arg2)+1);
		strcpy(stop,arg2);
		file=malloc(strlen(arg3)+1);
		strcpy(file,arg3);
		
		char* file_contents = get(start,stop,file);
		if (file_contents == NULL){
			//	printf("Eroare\n");
			if (send(fd, CGYM_ERR_MSG, strlen(CGYM_ERR_MSG), 0) == -1){
				perror("send");		
				pthread_exit(NULL);
			}
		}
		else{
			if (send(fd, CGYM_OK_MSG, strlen(CGYM_OK_MSG), 0) == -1){
		      perror("send");
		      pthread_exit(NULL);
			}
			unsigned long dim=strlen(file_contents);
			int j;
			for(j=0;j<(int)(dim/MAX_SIZE)+1;j++){
				if (send(fd, file_contents+j*MAX_SIZE,minOf2(strlen(file_contents+j*MAX_SIZE),MAX_SIZE), 0) == -1){
					perror("send");		
					pthread_exit(NULL);
				}
			}
		//	printf("END MESSAGE\n");
			if (send(fd, CGYM_END_MSG, strlen(CGYM_END_MSG), 0) == -1){
				perror("send");
				pthread_exit(NULL);
			}
		}
		break;
	}
	case 3 :{ //QUIT
		break;
	}
	default:{ //Command not found
		if (send(fd, CGYM_ERR_MSG, strlen(CGYM_ERR_MSG), 0) == -1){
			perror("send");
			pthread_exit(NULL);
		}
	}
	}
	/*
	printf("SFARSIT\n");
	printf("COMANDA:%s\n",command);
	printf("DIR:%s\n",dir);
	printf("File:%s\n",file);
	printf("START:%s\n",start);
	printf("STOP:%s\n",stop);
	printf("END!\n");
	*/
	close(fd);
	pthread_exit(NULL);
	
}
