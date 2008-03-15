#include "cgym.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "libcgym_priv.h"
#define MAXDATASIZE 100



int cgym_list(cgym_server_t *s, char *path) {
	printf("creating socket and connecting to server\n");
	cgym_sock_t *my_socket =  cgym_sock_create(s);
	if (cgym_sock_connect(my_socket)==0){
		sleep(1);
		printf("ne-am conectat, amu trimitem LIST%s\n",path);
		//cgym_sock_clear(my_socket);
	cgym_send_list_req(my_socket,path);
	
	}
	
	
	char *file;
	file = malloc(100);
	printf("%s",file);
	
	//e =malloc(sizeof(*e));
	cgym_entry_t **e;
	cgym_recv_list_reply(my_socket,&e);
	//printf("%s\n",((*e)->file));
	//cgym_recv_list_reply(my_socket,NULL);
	//cgym_list_get(my_socket,  path, my_entry);
	
	printf("requesting list of directory %s from\n", path);
	cgym_server_info_print(s);
	printf("\n");
	
	cgym_print_list(e);

	        return 0;

}
/*
int cgym_send_list_req(cgym_sock_t *sock, char *dir){
	
	int socket = cgym_sock_get_sockfd(sock);
	char * message = malloc(sizeof(dir)+6);//LIST+sizeof(dir)+\r\n
	strcpy(message,"LIST\0");
	strcat(message,dir);
	message[strlen(message)]='\r';// \0 = \r
	message[strlen(message)+1]='\n';// se adauga si \n
	send(socket,message,sizeof(message),0);
	return 0;
}
*/
