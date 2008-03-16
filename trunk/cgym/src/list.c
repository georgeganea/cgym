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
	int i = 0;
	i= cgym_sock_connect(my_socket);
	/*
	 * conecteaza socket-ul la adresa/portul dat
	 * 
	 * returneaza:
	 * 	0 la succes
	 *	1 la incomplet
	 *	2 la eroare la connect
	 *	3 la eroare la gethostbyname
	 *	4 la eroare la getsockopt
	 *	5 la eroare - nu s-a putut conecta
	 *	6 la eroare la cgym_recv_handshake
	 *	7 la eroare - sock-ul e NULL
	 */
	switch (i){
	case 0:{
		sleep(1);
    	cgym_send_list_req(my_socket,path);
		cgym_entry_t **e;
		cgym_recv_list_reply(my_socket,&e);
		printf("requesting list of directory %s from\n", path);
		cgym_server_info_print(s);
		printf("\n");
	
		cgym_print_list(e);
		break;
		}
	case 1:{printf("incomplet\n"); break;}
	case 2:{printf("eroare la connect\n"); break;}
	case 3:{printf("eroare la gethostbyname\n"); break;}
	case 4:{printf("eroare la getsockopt\n"); break;}
	case 5:{printf("nu s-a putut conecta\n"); break;}
	case 6:{printf("eroare la cgym_recv_handshake\n"); break;}
	case 7:{printf("eroare sock-ul e null\n"); break;}
	}
return 0;
}

