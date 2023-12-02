#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <stdbool.h>
#include <netinet/in.h>
#include <sys/types.h>

#define BUF_SIZE 500

static struct option parameters[] = {
	{"client",		no_argument,		0,		'c'},
	{"help",		no_argument,		0,		'h'},
	{NULL,			0,					0,		0}
};

void print_help(void)
{
	printf("type <port> for port, <client> for client\n");
	printf("otherwise, it is server\n");
	exit(1);
}

void work_as_client(char *portnum)
{
	int sock;
	int s;
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	struct sockaddr_in6 clientsideaddr, peer_addr;
	char buffer[BUF_SIZE], ip[64];
	int peer_addr_len = sizeof(peer_addr);

	sock = socket(PF_INET6, SOCK_DGRAM, 0);

	memset(&clientsideaddr, 0, sizeof(struct sockaddr_in6));
	clientsideaddr.sin6_port = htons(9192); //0 makes OS selected suitable port number
	clientsideaddr.sin6_family = AF_INET6;
	clientsideaddr.sin6_addr = in6addr_loopback;

	s = bind(sock, (struct sockaddr *)&clientsideaddr, sizeof(struct sockaddr_in6));

	if(-1 == s)
		perror("bind"), exit(1);

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;	 /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = AI_NUMERICSERV; // this is for numeric port //AI_PASSIVE;	 /* For wildcard IP address */
	hints.ai_protocol = IPPROTO_UDP;

	
	s = getaddrinfo(NULL, portnum, &hints, &result); //since null, return addres will be loopback, "::1" could be written aswell
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	/* getaddrinfo() returns a list of address structures.
		Try each address until we successfully bind(2).
		If socket(2) (or bind(2)) fails, we (close the socket
		and) try the next address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		switch (rp->ai_family) {
			case AF_INET:
				if (!inet_ntop(AF_INET, &(((struct sockaddr_in *)rp->ai_addr)->sin_addr), ip, sizeof(ip))) {
					freeaddrinfo(result);		 /* No longer needed */
					exit(1);
				}
				break;
			case AF_INET6:
				if (!inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)rp->ai_addr)->sin6_addr), ip, sizeof(ip))) {
					freeaddrinfo(result);		 /* No longer needed */
					exit(1);
				}
				break;
			default:
				freeaddrinfo(result);		 /* No longer needed */
				exit(1);
				break;
		}

		printf("found ip: %s\n", ip);

		if (rp->ai_family == AF_INET) {
			continue; //to pick IPV6 loopback
		}

		break;
	}

	if (rp == NULL)
		exit(2);

	if (sendto(sock, "selam", 6, 0,
				(struct sockaddr *)rp->ai_addr, sizeof(struct sockaddr_in6)) != 6)
		fprintf(stderr, "Error sending response\n");

	if (recvfrom(sock, buffer, sizeof(buffer), 0,
			(struct sockaddr *) &peer_addr, (socklen_t *)&peer_addr_len) == -1)
		perror("recvfrom");

	char host[NI_MAXHOST], service[NI_MAXSERV];

	s = getnameinfo((struct sockaddr *) &peer_addr,
					peer_addr_len, host, NI_MAXHOST,
					service, NI_MAXSERV, NI_NUMERICSERV);


	printf("Received : %s\n", buffer);

	freeaddrinfo(result);		 /* No longer needed */
	close(sock);
}

void work_as_server(char *portnum)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;
	struct sockaddr_storage peer_addr;
	socklen_t peer_addr_len;
	ssize_t nread;
	char buf[BUF_SIZE];
	char ip[64] = {0};
	int enable = 1;

	if (!portnum) {
		perror("data is NULL\n");
	}

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;	 /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_DGRAM; /* Datagram socket */
	hints.ai_flags = AI_NUMERICSERV; // this is for numeric port //AI_PASSIVE;	 /* For wildcard IP address */
	hints.ai_protocol = IPPROTO_UDP;

	s = getaddrinfo(NULL, portnum, &hints, &result); //since null, return addres will be loopback, "::1" could be written aswell
	if (s != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
		exit(EXIT_FAILURE);
	}

	/* getaddrinfo() returns a list of address structures.
		Try each address until we successfully bind(2).
		If socket(2) (or bind(2)) fails, we (close the socket
		and) try the next address. */

	for (rp = result; rp != NULL; rp = rp->ai_next) {
		switch (rp->ai_family) {
			case AF_INET:
				if (!inet_ntop(AF_INET, &(((struct sockaddr_in *)rp->ai_addr)->sin_addr), ip, sizeof(ip))) {
					freeaddrinfo(result);		 /* No longer needed */
					exit(1);
				}
				break;
			case AF_INET6:
				if (!inet_ntop(AF_INET6, &(((struct sockaddr_in6 *)rp->ai_addr)->sin6_addr), ip, sizeof(ip))) {
					freeaddrinfo(result);		 /* No longer needed */
					exit(1);
				}
				break;
			default:
				freeaddrinfo(result);		 /* No longer needed */
				exit(1);
				break;
		}

		printf("found ip: %s\n", ip);

		if (rp->ai_family == AF_INET) {
			continue; //to pick IPV6 loopback
		}

		sfd = socket(rp->ai_family, rp->ai_socktype,
				rp->ai_protocol);
		if (sfd == -1)
			continue;

		if (setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR , &enable, sizeof(int)) < 0) {
			perror("setsockopt reuseport fail\n");
			break;
		}

		if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)
			break;				/* Success */

		close(sfd);
	}

	freeaddrinfo(result);		 /* No longer needed */

	if (rp == NULL) {			/* No address succeeded */
		fprintf(stderr, "Could not bind\n");
		exit(EXIT_FAILURE);
	}

	/* Read datagrams and echo them back to sender. */

	for (;;) {
		peer_addr_len = sizeof(peer_addr);
		nread = recvfrom(sfd, buf, BUF_SIZE, 0,
				(struct sockaddr *) &peer_addr, &peer_addr_len);
		if (nread == -1)
			continue;			/* Ignore failed request */

		char host[NI_MAXHOST], service[NI_MAXSERV];

		s = getnameinfo((struct sockaddr *) &peer_addr,
						peer_addr_len, host, NI_MAXHOST,
						service, NI_MAXSERV, NI_NUMERICSERV);
		if (s == 0)
			printf("Received %zd bytes from %s:%s: %s\n",
					nread, host, service, buf);
		else
			fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

		if (sendto(sfd, "selam2", 7, 0,
					(struct sockaddr *) &peer_addr,
					peer_addr_len) != 7)
			fprintf(stderr, "Error sending response\n");
	}
}

int main(int argc, char *argv[])
{
	int option = 0, longid = 0;
	bool is_server = true;
	char port[5] = "9191";

	while ((option = getopt_long(argc, argv, "hc", parameters, &longid)) != -1) {
		switch (option) {
			case 'h':
				// "optarg" variable is used to get option itself
				print_help();
				break;
			case 'c':
				is_server = false;
				break;
			default:
				perror("unknown argument\n");
				return 2;
		}
	}

	if (is_server) {
		work_as_server(port);
	} else {
		work_as_client(port);
	}
}
