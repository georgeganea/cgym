
```
int main(int argc, char **argv) {
	-citeste si verifica argumente
	-comanda LIST:
		cgym_sock_create()
		cgym_sock_connect() //conecteaza la server1

		cgym_get_list()
		cgym_print_list()
	-comanda GET: (file, N segmente, server1...N)
		cgym_sock_create()
		cgym_sock_connect() // conecteaza la server1

		cgym_send_size_req()
		cgym_recv_size_reply() // daca nu exista fisierul, iesim

		cgym_sock_setblocking(0) // facem socket-ul nonblocking

		/*
		 * cream socket-uri nonblocking catre celelalte servere,
		 * pana gasim N-1 care au fisierul cautat (primul il avem deja)
		 */
		for (nr de segmente - 1) {
			cgym_sock_create()
			cgym_sock_setblocking(0)
			cgym_sock_connect()
		}
		
		while (!gata) { // nu s-au terminat toate segmentele
			if (select() == -1) // eroare

			// avem ceva de facut

			for (toate segmentele) {
				if (FD_ISSET(segment[i].sock.sockfd)) {
					switch (segment[i].sock.state) {
						case CONNECTING:
							connect()
							/*
							da (connected):
							cgym_send_size_req()
							state[i] = RECV_SIZE

							nu (connect failed):
							stergem serverul din lista?
							state[i] = IDLE
							*/
						case RECV_SIZE:
							cgym_recv_size_reply()
							/*
							da (avem info fis.):
							cgym_send_get_req()
							state[i] = RECV_DATA

							nu (eroare la info fis.):
							stergem serverul din lista?
							state[i] = IDLE
							*/
						case RECV_DATA:
							cgym_recv_get_reply()
							/*
							da (primim date):
							daca segmentul e gata,
							il marcam si punem
							state[i] = IDLE

							nu (eroare la primire date):
							stergem serverul din lista?
							state[i] = IDLE
							*/
					}
				}

				if (state[i] == IDLE) { // a devenit intre timp
					/*
					 * daca mai avem segmente neincepute
					 * cream o noua conexiune si vom incepe
					 * sa le luam
					 */
					cgym_sock_create()
					cgym_sock_setblocking(0)
					cgym_sock_connect()
				}
			} // for
		} // while

		cgym_segment_assemble() // asamblam segmentele intr-un fisier
	-gata
}
```

NOTE:

-daca incercam un server si ne putem conecta dar nu este fisierul (da eroare la cerere),
il taiem de pe lista.

-daca nu mai raman servere, anuntam si iesim