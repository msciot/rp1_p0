#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main(int args, char * argv[]){
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	char  buf[81];
	int sfd;
	hints.ai_family = AF_UNSPEC;    /* Allow IPv4 or IPv6 */
        hints.ai_socktype = SOCK_STREAM; /* Stream socket */
        hints.ai_flags = 0;
        hints.ai_protocol = 0;          /* Any protocol */
	getaddrinfo(argv[1],argv[2],&hints,&result);
	for (rp = result; rp != NULL; rp = rp->ai_next) {
               sfd = socket(rp->ai_family, rp->ai_socktype,
                            rp->ai_protocol);
               if (sfd == -1)
                   continue;

               if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1)
                   break;                  /* Success */

               close(sfd);
        }

	if (rp == NULL) {               /* No address succeeded */
               fprintf(stderr, "Could not connect\n");
               exit(EXIT_FAILURE);
	}
	freeaddrinfo(result);
	int nread;
	while(strcmp((const  char*)buf,"quit")){
		printf("TCP request to send:");
		scanf("%s",buf);
		write(sfd,buf,strlen(buf));
		nread=read(sfd,buf,81);
		printf("Received %d bytes: %s\n", nread, buf);
	}
	exit(EXIT_SUCCESS);


}
