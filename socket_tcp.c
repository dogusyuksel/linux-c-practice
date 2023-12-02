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

//only differences are listen and accept on server side
//and connect on client side

#define BUF_SIZE 500

void work_as_client(char *portnum, char *data)
{
	int sock;
	int s;
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	struct sockaddr_in6 clientsideaddr;
	char buffer[BUF_SIZE], ip[64];
	int enable = 1;

	if (!portnum || !data) {
		perror("data is NULL\n");
		return;
	}

	sock = socket(PF_INET6, SOCK_STREAM, 0);
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR , &enable, sizeof(int)) < 0) {
		perror("setsockopt reuseport fail\n");
		return;
	}

	memset(&clientsideaddr, 0, sizeof(struct sockaddr_in6));
	clientsideaddr.sin6_port = htons(9191); //0 makes OS selected suitable port number
	clientsideaddr.sin6_family = AF_INET6;
	clientsideaddr.sin6_addr = in6addr_loopback;

	s = bind(sock, (struct sockaddr *)&clientsideaddr, sizeof(struct sockaddr_in6));

	if(-1 == s) {
		perror("bind");
		exit(EXIT_FAILURE);
	}

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;	 /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
	hints.ai_flags = AI_NUMERICSERV; // this is for numeric port //AI_PASSIVE;	 /* For wildcard IP address */
	hints.ai_protocol = IPPROTO_TCP;

	
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
	
	connect(sock, (struct sockaddr *)rp->ai_addr, rp->ai_addrlen);


	if (send(sock, data, strlen(data), MSG_NOSIGNAL) == -1) {
		perror("send failed\n");
	}

	if (recv(sock, buffer, sizeof(buffer), 0) < 0) {
		perror("recv failed\n");
	}

	printf("Received : %s\n", buffer);

	freeaddrinfo(result);		 /* No longer needed */
	close(sock);
}

void work_as_server(char *portnum, char *file_path)
{
	struct addrinfo hints;
	struct addrinfo *result, *rp;
	int sfd, s;
	char buf[BUF_SIZE];
	char ip[64] = {0};
	int enable = 1;

	if (!portnum || !file_path) {
		perror("data is NULL\n");
	}

	memset(&hints, 0, sizeof(hints));

	hints.ai_family = AF_UNSPEC;	 /* Allow IPv4 or IPv6 */
	hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
	hints.ai_flags = AI_NUMERICSERV; // this is for numeric port //AI_PASSIVE;	 /* For wildcard IP address */
	hints.ai_protocol = IPPROTO_TCP;

	s = getaddrinfo("::", portnum, &hints, &result); //since null, return addres will be loopback, "::1" could be written aswell
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

	if (listen(sfd, 20) == -1)
		perror("listen fail\n");

	/* Read datagrams and echo them back to sender. */

	for (;;) {
		struct sockaddr_in6 client_addr;
		socklen_t client_addr_len = sizeof(client_addr);
		int client_sock_fd = accept(sfd,
				(struct sockaddr*)&client_addr,
				&client_addr_len);
		if (client_sock_fd == -1) {
			perror("accept()");
			close(sfd);
		}

		char str_addr[64];
 
		inet_ntop(AF_INET6, &(client_addr.sin6_addr),
				str_addr, sizeof(str_addr));
		printf("New connection from: %s:%d ...\n",
				str_addr,
				ntohs(client_addr.sin6_port));
 
		/* Wait for data from client */
		if (recv(client_sock_fd, buf, sizeof(buf), 0) == -1) {
			perror("read()");
			close(client_sock_fd);
			continue;
		}
		printf("received-->\n %s\n", buf);

		memset(buf, 0, sizeof(buf));


		FILE *fs = fopen(file_path, "r");
		if (!fs) {
			perror("open failed\n");
			exit(EXIT_FAILURE);
		}
		if (fgets(buf, sizeof(buf), fs) == NULL) {
			perror("fgets failed\n");
			fclose(fs);
			continue;
		}

		if (send(client_sock_fd, buf, strlen(buf) + 1, MSG_NOSIGNAL) == -1) {
			perror("read()");
			close(client_sock_fd);
		}
		fclose(fs);
	}
}

int main(int argc, char *argv[])
{
	int option = 0;
	bool is_server = true;
	char port[32] = {0};
	char data[BUF_SIZE] = {0};
	char file[BUF_SIZE] = {0};

	memset(port, 0, sizeof(port));
	memset(data, 0, sizeof(data));
	memset(file, 0, sizeof(file));

	while ((option = getopt(argc, argv, "hcp:d:f:")) != -1) {
		switch (option) {
			case 'h':
				// "optarg" variable is used to get option itself
				perror("read the source code\n");
				exit(EXIT_SUCCESS);
			case 'c':
				is_server = false;
				break;
			case 'p':
				strncpy(port, optarg, sizeof(port) - 1);
				break;
			case 'd':
				strncpy(data, optarg, sizeof(data) - 1);
				break;
			case 'f':
				strncpy(file, optarg, sizeof(file) - 1);
				break;
			default:
				perror("unknown argument\n");
				return 2;
		}
	}

	if (strlen(port) == 0) {
		perror("specify port number\n");
		exit(EXIT_FAILURE);
	}

	if (is_server) {
		if (strlen(file) == 0) {
			perror("specify file\n");
			exit(EXIT_FAILURE);
		}
		printf("work as server\n");
		work_as_server(port, file);
	} else {
		if (strlen(data) == 0) {
			perror("specify data\n");
			exit(EXIT_FAILURE);
		}
		printf("work as client\n");
		work_as_client(port, data);
	}
}
