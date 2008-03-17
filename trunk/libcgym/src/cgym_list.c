#include "libcgym.h"
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include "libcgym_priv.h"
#include "unistd.h"
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
	char * message = malloc(strlen(dir)+8);//LIST +strlen(dir)+\r\n
	sprintf(message,"%s %s\r\n","LIST",dir);
	printf("mesajul |%s|",message);
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

const int ADD = 16;
int cgym_recv_list_reply(cgym_sock_t *sock, cgym_entry_t ***e){
	//int size =128;
	
	char c;
	int s;
	char *entr=malloc(1024);
	int i=0;
	
	char *type= NULL;
	char *size ;
	char *md5;
	char *fil;
	
	int siz=0;
	cgym_entry_t ** point=NULL;//=malloc(sizeof(cgym_entry_t*));
	cgym_entry_t ** head=NULL;// point;
	
	//p = malloc(1);
	printf("orice\n");
	int k=0;
	while ((k=recv(sock->sockfd,&c,1,0))==1){
	
	//	printf("%c",c);
		
		entr[i]=c;
		
		i++;
		if ('\n'==c){
		       
						if (entr[0]=='\r'){
							printf("am iesit \n");
								break;
						}
				
						if ((entr[0]=='E')&&(entr[1]=='R')&&(entr[2]=='R'))
						{
							printf("server sent ERR! folder may not exist, bye bye!");
							exit(1);
						}
					    if (entr[0]=='O'){
						printf("serverul zice ok, acuma trimite lista:\n");
						}
					    else{
						
						type = strtok(entr," ");
						
						size = strtok(NULL," ");
						md5  = strtok(NULL," ");
						fil  = strtok(NULL,"\r");
						printf("tipul :%s ,marimea: %s ,md5: %s ,numele :%s \n",type,size,md5,fil);
						
						if (type != NULL &&
							size != NULL &&
							md5  != NULL &&
							fil  != NULL   ){
							
									
									s = strtol(size,NULL,10);
										if((point=realloc(head,(((siz)+2)*sizeof(cgym_entry_t*))))){
											head=point;
										}
										else{
											perror("realloc");
										}
									
									head[siz++]=cgym_entry_init(fil,md5,(type[0]=='d'),s);
								
						}			
					}
					
					i=0;
					clear(entr);
				}
		
		
	}
	printf("k=%d\n",k);
	if (k==-1)
	perror("recv");
	printf("siz:%d",siz);
	head[siz]=NULL;
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
	int k=0;
	while((*e)!=NULL){
	i++;
	
	printf(">%s",(*e)->file);
		if ((*e)->type != 0){
		printf("/\n");
		}
		else{
			for (k=(40-strlen((*e)->file));k>0;k--)
				printf("-");
			printf("file size:%ld B\n",(*e)->size);
		}
	e++;
	}
	return i;
	
}

