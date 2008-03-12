#include "libcgym.h"
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

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
int cgym_recv_list_reply(cgym_sock_t *sock, cgym_entry_t **e);

/*
 * afiseaza lista de entry-uri la stdout
 *
 * returneaza numarul de entry-uri
 */
int cgym_print_list(char *dir, cgym_entry_t **e);
