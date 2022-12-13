#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#include <ifaddrs.h>
#include <net/if.h>

typedef struct tcpconnection{
    char ipaddr[15];
    char portno[4];
} tcpconnection;

void *get_in_addr(struct sockaddr *sa)
{
	if (sa->sa_family == AF_INET) {
		return &(((struct sockaddr_in*)sa)->sin_addr);
	}

	return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    if(argc != 2){
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }
    char mylisteningport[4];
    strcpy(mylisteningport, argv[1]);
}
