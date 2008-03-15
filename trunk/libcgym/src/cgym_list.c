#include "libcgym.h"
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "libcgym_priv.h"
/*
 * 
 * trimite cererea pentru lista catre server
 * returneaza:
 * 	0 la succes
 * 	1 la eroare
 * trimite prin socketul sock sirul de octeti "LIST dir\r\n"
 */

int cgym_send_list_req(cgym_sock_t *sock, char *dir){
	
	int socket = cgym_sock_get_sockfd(sock);
	int i;
	char * message = malloc(strlen(dir)+7);//LIST +strlen(dir)+\r\n
	strcpy(message,"LIST ");
	strcat(message,dir);
	printf("mesajul inatinte de o si n :%s\n",message);
	message[strlen(message)+1]='\r';// \0 = \r
	message[strlen(message)+2]='\n';// se adauga si \n
	i=send(socket,message,strlen(message),0);
	printf("numarul de octeti trimisi:%d\n",i);
	if (i)
	return 0;
	return 1;
}




void clear(char entr[]){
	int i;
	for (i=0;i<1024;i++){
		entr[i]=0;
	}
}
/*
 * citeste de pe socket lista trimisa de server
 * si o salveaza in e
 * tabelul se termina cu NULL
 *
 * returneaza:
 *	0 la succes
 *	1 la incomplet
 *	2 la eroare 
 */
int cgym_recv_list_reply(cgym_sock_t *sock, cgym_entry_t ***e){
	//int size =128;
	char *p;
	char c;
	int s;
	char *entr=malloc(1024);
	int i=0;
	int length=1;
	char *type= malloc(1);
	char *size = malloc(21);
	char *md5=malloc(33);
	char *fil= malloc(512);
	cgym_entry_t ** point=malloc(sizeof(cgym_entry_t*));
	cgym_entry_t ** head = point;
	
	cgym_entry_t *entry = **e;
	//p=malloc(size+2);
	p = malloc(1);
	sock->buf=p;
	
	while (recv(sock->sockfd,p,1,0)==1){
		//printf("%c",*p);
		c = *p;
		entr[i]=c;
		i++;
		if ('\n'==c){
						if (entr[0]=='\r'){
							printf("am iesit \n");
								break;
						}
					//	printf("sirul:\n%s",entr);
					//printf("este n :\n");
					    if (entr[0]=='O'){
						printf("serverul zice ok, acuma trimite lista:\n");}
					    else{
						
						type = strtok(entr," ");
						
						size = strtok(NULL," ");
						md5  = strtok(NULL," ");
						fil  = strtok(NULL,"\r");
						if (type != NULL &&
							size != NULL &&
							md5  != NULL &&
							fil  != NULL   ){
							//printf("tipul :%s ,marimea: %s ,md5: %s ,numele :%s \n",type,size,md5,fil);
									
									s = strtol(size,NULL,10);
									*point=cgym_entry_init(fil,md5,(type[0]=='d'),s);
									
									length++;
									head=realloc(head,length*sizeof(cgym_entry_t*));
									point++;
									
						}			
					}
					
					i=0;
					clear(entr);
				}
		p++;
		p=malloc(1);
		
	}
	point=NULL;
		*e=head;
	return 0;
}

/*
 * afiseaza lista de entry-uri la stdout
 *
 * returneaza numarul de entry-uri
 */
int cgym_print_list(cgym_entry_t **e){
	
	int i=0;
	while(e!=NULL){
	i++;
	printf(">%s\n",(*e)->file);
	e++;
	}
	return i;
	
}
