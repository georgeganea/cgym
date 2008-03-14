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
	homedir=malloc(strlen(argv[1])*sizeof(char));
	strncpy(homedir,argv[1],strlen(argv[1]));
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
int checkCommand(char * cmd){	
	char * tmp;
	int i;
	tmp=malloc(strlen(cmd)*sizeof(char));
	strncpy(tmp,cmd,strlen(cmd));
	for(i=0;i<strlen(cmd);i++){
		tmp[i]=toupper(tmp[i]);
	}
	if(strcmp(tmp,"LIST")==0) return 0;
	if(strcmp(tmp,"SIZE")==0) return 1;
	if(strcmp(tmp,"GET")==0) return 2;
	if(strcmp(tmp,"QUIT")==0) return 3;
	return -1;
}
int minOf2(int a,int b){
	if(a>b) return b;
	return a;
}
void* client_handler(void *p){

	printf("Handling client request\n");
	int fd = *(int*)p;
	char bu[20];
	char *temp,*s2,*comp;
	char * command,**li,*file=NULL,*dir=NULL,*start=NULL,*stop=NULL;
	char *buffer,*s=NULL;
	int r=-1,n=0,size=0,i,t,wrds=0; 
	if (send(fd, CGYM_ACK_MESSAGE, strlen(CGYM_ACK_MESSAGE), 0) == -1){
	      perror("send");
	      exit(1);
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
		if (send(fd, CGYM_ERR_MESSAGE, strlen(CGYM_ERR_MESSAGE), 0) == -1){
			perror("send");
		    exit(1);
		}
	}
		if((temp=getNextArg(s,&t))){
			command=malloc(strlen(temp));
			strncpy(command,temp,strlen(temp));
			strncpy(temp,"",strlen(temp));
		}
		//check command
		if((r=checkCommand(command))<0){
			if (send(fd, CGYM_ERR_MESSAGE, strlen(CGYM_ERR_MESSAGE), 0) == -1){
				perror("send");
				exit(1);
			}
		}
		s2=s+strlen(command)+t;
		s=s2;
		if((temp=getNextArg(s,&t))){
			switch(r){
			case 0:{ //LIST
				dir=malloc(strlen(temp));
				strncpy(dir,temp,strlen(temp));
				strncpy(temp,"",strlen(temp));
				if(dir==NULL){
					if (send(fd, CGYM_ERR_MESSAGE, strlen(CGYM_ERR_MESSAGE), 0) == -1){
						perror("send");
					    exit(1);
					}
				}
				if(strcmp(homedir,dir)>0){
					if (send(fd, CGYM_ERR_MESSAGE, strlen(CGYM_ERR_MESSAGE), 0) == -1){
						perror("send");
					    exit(1);
					}
				}
				break;
			}
			case 1:{ //SIZE
				file=malloc(strlen(temp));
				strncpy(file,temp,strlen(temp));
				strncpy(temp,"",strlen(temp));
				if(file==NULL){
					if (send(fd, CGYM_ERR_MESSAGE, strlen(CGYM_ERR_MESSAGE), 0) == -1){
						perror("send");
					    exit(1);
					}
				}
				if(file!=strstr(file,homedir)){
					if (send(fd, CGYM_ERR_MESSAGE, strlen(CGYM_ERR_MESSAGE), 0) == -1){
						perror("send");
					    exit(1);
					}
				}
				if(strcmp(homedir,file)>0){
					if (send(fd, CGYM_ERR_MESSAGE, strlen(CGYM_ERR_MESSAGE), 0) == -1){
						perror("send");
					    exit(1);
					}
				}
				break;
			}
			case 2:{ //GET
				start=malloc(strlen(temp));
				strncpy(start,temp,strlen(temp));
				strncpy(temp,"",strlen(temp));
				s2=s+strlen(start)+t;
				s=s2;
				if((temp=getNextArg(s,&t))){
					stop=malloc(strlen(temp));
					strncpy(stop,temp,strlen(temp));
					strncpy(temp,"",strlen(temp));
					s2=s+strlen(stop)+t;
					s=s2;
					if((temp=getNextArg(s,&t))){
						file=malloc(strlen(temp));
						strncpy(file,temp,strlen(temp));
						strncpy(temp,"",strlen(temp));
						s2=s+strlen(stop)+t;
						s=s2;
					}
				}
				if((file==NULL)||(start==NULL)||(stop==NULL)){
					if (send(fd, CGYM_ERR_MESSAGE, strlen(CGYM_ERR_MESSAGE), 0) == -1){
						perror("send");
					    exit(1);
					}
				}
				if(file!=strstr(file,homedir)){
					if (send(fd, CGYM_ERR_MESSAGE, strlen(CGYM_ERR_MESSAGE), 0) == -1){
						perror("send");
					    exit(1);
					}
				}
				if(strcmp(homedir,file)>0){
					if (send(fd, CGYM_ERR_MESSAGE, strlen(CGYM_ERR_MESSAGE), 0) == -1){
						perror("send");
						exit(1);
					}
				}
				break;
			}
			case 3:{ //QUIT
				if((strlen(temp)>0)&&((temp[0]!='\r')||(temp[1]!='\n'))){
					if (send(fd, CGYM_ERR_MESSAGE, strlen(CGYM_ERR_MESSAGE), 0) == -1){
						perror("send");
						exit(1);
					}
				}
				break;
			}
		}
	}
		if((temp=getNextArg(s,&t))&&(strcmp(temp,"\r\n")==0)){
			if (send(fd, CGYM_ERR_MESSAGE, strlen(CGYM_ERR_MESSAGE), 0) == -1){
				perror("send");
			    exit(1);
			}
		}
	//exemplu de folosire a functiei list
	
	
	//exemplu de folosire a functiei get
	switch(r){
	case 0:{
		break;
	}
	case 1:{
		FILE_INFO* file_info;
		file_info = list(dir);
		if(file_info==NULL){
			if (send(fd, CGYM_ERR_MESSAGE, strlen(CGYM_ERR_MESSAGE), 0) == -1){
				perror("send");
			    exit(1);
			}
		}
		printf("IN LIST!\n");
		while (file_info->next) { 
			printf("%s\n",cgym_entry_file(file_info->entry_file));
			file_info = file_info->next; 
		}
		break;
	}
	case 2:{
		char* file_contents = get(start,stop,file);
			if (file_contents == NULL){
			//	printf("Eroare\n");
				if (send(fd, CGYM_ERR_MESSAGE, strlen(CGYM_ERR_MESSAGE), 0) == -1){
					perror("send");		
					exit(1);
				}
			}
			else{
				unsigned long dim=strlen(file_contents);
				int j;
				for(j=0;j<(int)(dim/MAX_SIZE);j++){
					if (send(fd, file_contents+j*MAX_SIZE,minOf2(strlen(file_contents+j*MAX_SIZE),MAX_SIZE), 0) == -1){
						perror("send");		
						exit(1);
					}
				}
			}
		break;
	}
	case 3:{
		break;
	}
	}
	
	

	printf("COMANDA:%s\n",command);
	printf("DIR:%s\n",dir);
	printf("File:%s\n",file);
	printf("START:%s\n",start);
	printf("STOP:%s\n",stop);
	printf("END!\n");
	close(fd);
	pthread_exit(NULL);
	
}


