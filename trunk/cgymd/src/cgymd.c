#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include "libcgym.h"

#define BACKLOG 10     // how many pending connections queue will hold

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
    	close(new_fd);  // parent doesn't need this
    }
	return 0;
}
