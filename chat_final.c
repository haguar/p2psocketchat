/*
** selectserver.c -- a cheezy multiperson chat server
*/

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
// get sockaddr, IPv4 or IPv6:
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


    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number

    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;

    char buf[256];    // buffer for client data
    int nbytes;

	char remoteIP[INET6_ADDRSTRLEN];


    int yes=1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;

	struct addrinfo hints, *ai, *p, *p2;

    tcpconnection *list = (tcpconnection*)malloc(10 * sizeof(tcpconnection));
    int cnxncount = 0;
    char ipstr[INET6_ADDRSTRLEN];


    FD_ZERO(&master);    // clear the master and temp sets
    FD_ZERO(&read_fds);
	// get us a socket and bind it
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	if ((rv = getaddrinfo(NULL, argv[1], &hints, &ai)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}

 	for(p = ai; p != NULL; p = p->ai_next) {
    	listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (listener < 0) {
			continue;
		}

		// lose the "address already in use" error message
		setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

		if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
			close(listener);
			continue;
		}

		break;
	}

	// if we got here, it means we didn't get bound
	if (p == NULL) {
		fprintf(stderr, "selectserver: failed to bind\n");
		exit(2);
	}
	if ((rv = getaddrinfo("192.168.1.122", NULL, &hints, &ai)) != 0) {
		fprintf(stderr, "selectserver: %s\n", gai_strerror(rv));
		exit(1);
	}
    for(p2 = ai;p2 != NULL; p2 = p2->ai_next) {
        void *addr;
        char *ipver;

        // get the pointer to the address itself,
        // different fields in IPv4 and IPv6:
        if (p2->ai_family == AF_INET) { // IPv4
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p2->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { // IPv6
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p2->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        // convert the IP to a string and print it:
        inet_ntop(p2->ai_family, addr, ipstr, sizeof ipstr);
        printf("You're online as:  %s: %s\n\n", ipver, ipstr);
    }
	freeaddrinfo(ai); // all done with this

    // listen
    if (listen(listener, 10) == -1) {
        perror("listen");
        exit(3);
    }

    // add the listener to the master set
    FD_SET(listener, &master);

    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one
    FD_SET(0, &master);
    char inbuf[100];
    char command[100];

    printf("Chat functions: "
                    "\n\nhelp"
                    "\nmyip"
                    "\nmyport"
                    "\nmakeconnection"
                    "\nlist"
                    "\nterminate <connection id>"
                    "\nsend <connection id> <message>"
                    "\nexit\n\n");

    // main loop
    while(strcmp(command, "exit") != 0){
        read_fds = master; // copy it

        int looprv, sockfd, id;
        char s[INET6_ADDRSTRLEN];

        printf("\n\nChat is operating normally...(type help to see commands)\n");
        if (select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1) {
            perror("select");
            exit(4);
        }
        if(FD_ISSET(0, &read_fds)){ //if we have input from user in the STDIN
            memset(command, 0, 100);
            fgets(inbuf, 100, stdin);
            printf("processing==> %s\n", inbuf);
            sscanf(inbuf, "%s", &command);


            if (strcmp(command, "help") == 0){ //if else tree for commands
            printf("Chat functions: "
                    "\n\nhelp"
                    "\nmyip"
                    "\nmyport"
                    "\nmakeconnection"
                    "\nlist"
                    "\nterminate"
                    "\nsend"
                    "\nexit\n");
            }
            else if (strcmp(command, "myip") == 0){
                printf("You're online as: %s\n\n", ipstr);
            }
            else if (strcmp(command, "myport") == 0){
                printf("Listening on port: %s\n", mylisteningport);
            }
            else if (strcmp(command, "makeconnection") == 0){
                cnxncount++;
                struct addrinfo *servinfo, *port;
                char newip[15];
                char newportno[4];
                printf("Destination (ip address): \n");
                scanf("%s", newip);
                while ((getchar()) != '\n'); //flush stdin correctly
                strcpy(list[cnxncount].ipaddr, newip);
                printf("Port number: (4 digit)\n");
                scanf("%s", newportno);
                while ((getchar()) != '\n'); //flush stdin correctly
                strcpy(list[cnxncount].portno, newportno);
                printf("Added connection %d, ", cnxncount);
                printf("%s, ", list[cnxncount].ipaddr);
                printf("port: %s\n", list[cnxncount].portno);
                if ((looprv = getaddrinfo(list[cnxncount].ipaddr, list[cnxncount].portno, &hints, &servinfo)) != 0) {
                    fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(looprv));
                    return 1;
                }
                printf("\nconnecting to server...\n");
                for(port = servinfo; port != NULL; port = port->ai_next) {
                    if ((sockfd = socket(port->ai_family, port->ai_socktype,
                            port->ai_protocol)) == -1) {
                        perror("client: socket");
                        continue;
                    }
                    printf("\nsockfd init\n");
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
                freeaddrinfo(servinfo);

            }
            else if (strcmp(command, "list") == 0){
                printf("id: IP address----------Port No.\n");
                for(int i = 1; i < cnxncount+1; i++){
                    printf("%d: ", i);printf("%s ----------", list[i].ipaddr);printf("%s\n", list[i].portno);
                }
            }
            else if (strcmp(command, "terminate") == 0){
                printf("Terminate which connection id?\n");
                scanf("%d", &id);
                while ((getchar()) != '\n'); //flush stdin correctly
                strcpy(list[id].ipaddr, "terminated");
                strcpy(list[id].portno, "terminated");
                printf("id: IP address----------Port No.\n");
                printf("%d: ", id);printf("%s ----------", list[id].ipaddr);printf("%s\n", list[id].portno);
                cnxncount--;
                close(id+3);
            }
            else if (strcmp(command, "send") == 0){
                char sendbuf[100];
                char message[150] = "Message from ";
                printf("Send to which connection id?\n");
                scanf("%d", &id);
                while ((getchar()) != '\n'); //flush stdin correctly
                printf("Enter message: ");
                scanf("%[^\n]%*c", &sendbuf);
                while ((getchar()) != '\n'); //flush stdin correctly
                printf("\nSending: %s\n", sendbuf);
                //pack message with sender info
                strncat(message, ipstr, 15);
                strncat(message, "/", 1);
                strncat(message, mylisteningport, 4);
                strncat(message, ": ", 2);
                strncat(message, sendbuf, 100);
                if (send(id+3, message, 150, 0) == -1) {//send myip
                    perror("send");
                }
            }
            else{
                if(strcmp(command, "exit") != 0){
                    printf("Oops! Please enter a recognizable command.\n");
                }
            }
        }










        // run through the existing connections looking for data to read
        for(i = 1; i <= fdmax; i++) {
            if (FD_ISSET(i, &read_fds)) { // we got one!!
                if (i == listener) {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
					newfd = accept(listener,
						(struct sockaddr *)&remoteaddr,
						&addrlen);

					if (newfd == -1) {
                        perror("accept");
                    } else {
                        FD_SET(newfd, &master); // add to master set
                        if (newfd > fdmax) {    // keep track of the max
                            fdmax = newfd;
                        }
                        printf("\nselectserver: new connection from %s on "
                            "socket %d\n",
							inet_ntop(remoteaddr.ss_family,
								get_in_addr((struct sockaddr*)&remoteaddr),
								remoteIP, INET6_ADDRSTRLEN),
							newfd);
                        //cnxncount++;
                        struct sockaddr_in portget;
                        socklen_t clientsz = sizeof(portget);
                        getsockname(newfd, (struct sockaddr *) &portget, &clientsz);
                        int tempport = ntohs(portget.sin_port);
                        strcpy(list[cnxncount].ipaddr, ipstr);
                        sprintf(list[cnxncount].portno, "%d", tempport);
//                         printf("Added connection %d, ", cnxncount);
//                         printf("%s/", list[cnxncount].ipaddr);
//                         printf("%s\n", list[cnxncount].portno);
                    }
                } else {
                    // handle data from a client
                    if ((nbytes = recv(i, buf, sizeof buf, 0)) <= 0) {
                        // got error or connection closed by peer
                        if (nbytes == 0) {
                            // connection closed
                            printf("selectserver: socket %d hung up\n", i);
                        } else {
                            perror("recv");
                        }
                        close(i); // bye!
                        FD_CLR(i, &master); // remove from master set
                        cnxncount--;
                    } else {

                        printf("\n%s\n", buf);//print received message
                    }
                } // END handle data from peer
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END while loop via exit command
    printf("closing connections and terminating application\n");
    free(list);
    return 0;
}
