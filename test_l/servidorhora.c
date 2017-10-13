#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>


void main(int args,char * argv[]){
	struct addrinfo hints, *res;
	struct sockaddr_storage cli;
	char sbuf[81],rbuf[81],host[NI_MAXHOST],serv[NI_MAXSERV];
	
	hints.ai_flags=AI_PASSIVE;
	hints.ai_family=AF_UNSPEC;
	hints.ai_socktype=SOCK_DGRAM;
	hints.ai_protocol=0;

	getaddrinfo(argv[1],argv[2],&hints,&res);
	int sd=socket(res->ai_family,res->ai_socktype,0);
	bind(sd,(struct sockaddr*) res->ai_addr , res->ai_addrlen);
	freeaddrinfo(res);
	int stop=0;
	while(!stop){
		socklen_t clen=sizeof(cli);
		int c = recvfrom(sd,rbuf,1,0,(struct sockaddr *) &cli,&clen);
		getnameinfo((struct sockaddr *)&cli,clen,host,NI_MAXHOST,serv,NI_MAXSERV,NI_NUMERICHOST);
		printf("Message recieved from %s::%s\n",host,serv);
		printf("Length: %d Bytes\tMsg:%s\n",c,rbuf);
		time_t ti = time(NULL);
		struct tm *timeData=gmtime(&ti);
		switch(rbuf[0]){
			case 't':
				sprintf(sbuf,"Hour:\t%02d:%02d:%02d",timeData->tm_hour+2,timeData->tm_min,timeData->tm_sec);
			break;
			case 'd':
				sprintf(sbuf,"Year:20%02d\tMonth:%02d\tDay:%02d",timeData->tm_year-100,timeData->tm_mon+1,timeData->tm_mday);
			break;
			case 'q':
				sprintf(sbuf,"Goodbye client");
				stop=1;
			break;
		}
		c=strlen(sbuf);
		sbuf[c] = '\n';
		sbuf[c+1]='\0';
		sendto(sd, sbuf ,c+1, 0 , (struct sockaddr *) &cli, clen);
	}
	close(sd);
}
