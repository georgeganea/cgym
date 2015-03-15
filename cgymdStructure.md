
```
int main(int argc, char **argv) {
           -citeste si verifica argumente
	   -deschide port(portul primit pe linia de comanda argv[2] sau 2496 daca nu este specificat)
			 cgym_sock_create()
			 cgymd_bind()  // asociaza adresa IP si portul cu socketul
	   -asculta
			 cgymd_listen()
	   -porneste cate un thread pt fiecare conexiune	
			 while(1){ 
				if(cgymd_accept()){ // a new connection request
					cgymd_new_thread() // cu pointer la cgym_handler()
					cgym_quit() // inchide conexiunea
				} //if	
			 } //while
}

void * cgymd_handler(void * ptr){
     -trimite CGYM\r\n
		cgymd_send_connect_ack() // "CGYM\r\n"
	-asteapta comenzi
		*comanda LIST
			cgymd_list()
		*comanda SIZE
			cgymd_size()
		*comanda GET
			cgymd_get()
} 

void cgymd_list(char * dirname){
	-verifica existenta directorului si a drepturilor asupra acestuia
	-on Success
		cgymd_send_ok() // "OK\r\n"
		cgymd_send_list() // trimite lista din director in urmatorul format: type no_bytes md5 path\r\n
	-on Error
		cgymd_send_err() // "ERR\r\n"
}

void cgymd_size(char * filename){
	-verifica existenta fisierului si a drepturilor asupra acesta
	-on Success
		cgymd_send_ok() // "OK\r\n"
		cgymd_send_size()//trimite dimensiunea fisierului in formatul: type no_bytes md5 path\r\n
	-on Error
		cgymd_send_err() // "ERR\r\n"		
}

void cgymd_get(char* start, char* stop, char* filename){
	-verifica daca parametrii sunt valizi
	-on Succes
		cgymd_send_ok() // "OK\r\n"
		cgymd_send_file_part() //adauga "\r\n" la final
	-on Error	
		cgymd_send_err() // "ERR\r\n"		
}
```