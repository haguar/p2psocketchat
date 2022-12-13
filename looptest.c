#include <sys/select.h>
#include <stdio.h>
#include <string.h>

int main(void) {
    char buf[100];
    char command[100];
    fd_set rfds;
    int r;
    printf("pls");
    while(strcmp(command, "exit") != 0){
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        r = select(1, &rfds, NULL, NULL, NULL);
        if(r == 0)
            printf("timeout!\n");
        else {
            fgets(buf, 100, stdin);
            printf("==> %s\n", buf);
            sscanf(buf, "%s", &command);
        }
    }
}

















int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *port;
    int rv;
    char s[INET6_ADDRSTRLEN];

    if (argc != 2) {
        fprintf(stderr,"usage: client hostname\n");
        exit(1);
    }

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(port = servinfo; port != NULL; port = port->ai_next) {
        if ((sockfd = socket(port->ai_family, port->ai_socktype,
                port->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }

        if (connect(sockfd, port->ai_addr, port->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }

        break;
    }

    if (port == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }

    inet_ntop(port->ai_family, get_in_addr((struct sockaddr *)port->ai_addr),
            s, sizeof s);
    printf("client: connecting to %s\n", s);

    freeaddrinfo(servinfo); // all done with this structure

    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(1);
    }

    buf[numbytes] = '\0';

    printf("client: received '%s'\n",buf);

    close(sockfd);
