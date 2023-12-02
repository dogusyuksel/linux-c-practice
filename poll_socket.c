#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define SERVER_PORT  9191

#define TRUE		1
#define FALSE		0

int main (void)
{
	int rc, on = 1;
	int listen_sd = -1, new_sd = 0;
	int close_conn;
	int timeout;
	int nfds = 1, current_size = 0, i, j;
	char buffer[64];
	struct sockaddr_in6	addr;
	struct pollfd fds[32];

	listen_sd = socket(AF_INET6, SOCK_STREAM, 0);
	if (listen_sd < 0) {
		perror("socket() failed");
		exit(EXIT_FAILURE);
	}

	rc = setsockopt(listen_sd, SOL_SOCKET,  SO_REUSEADDR,
					(char *)&on, sizeof(on));
	if (rc < 0) {
		perror("setsockopt() failed");
		close(listen_sd);
		exit(-1);
	}

	/*************************************************************/
	/* Set socket to be nonblocking. All of the sockets for	 */
	/* the incoming connections will also be nonblocking since	*/
	/* they will inherit that state from the listening socket.	*/
	/*************************************************************/
	//this is important to revisit to poll function to read data
	//otherwise reading cannot be done because it is blocking
	rc = ioctl(listen_sd, FIONBIO, (char *)&on);
	if (rc < 0) {
		perror("ioctl() failed");
		close(listen_sd);
		exit(-1);
	}

	memset(&addr, 0, sizeof(addr));
	addr.sin6_family = AF_INET6;
	memcpy(&addr.sin6_addr, &in6addr_any, sizeof(in6addr_any));
	addr.sin6_port = htons(SERVER_PORT);

	rc = bind(listen_sd,
				(struct sockaddr *)&addr, sizeof(addr));
	if (rc < 0) {
		perror("bind() failed");
		close(listen_sd);
		exit(-1);
	}

	rc = listen(listen_sd, 32);
	if (rc < 0) {
		perror("listen() failed");
		close(listen_sd);
		exit(-1);
	}

	memset(fds, 0 , sizeof(fds));

	fds[0].fd = listen_sd;
	fds[0].events = POLLIN; //poll input events

	timeout = 1000; //time is milliseconds --> it is 1 second now

	while (TRUE) {
		printf("Waiting on poll()...\n");
		rc = poll(fds, nfds, timeout);
		if (rc < 0) {
			perror("  poll() failed");
			break;
		} else if (rc == 0) {
			printf("  poll() timed out.\n");
		}

		current_size = nfds;
		for (i = 0; i < current_size; i++) {
			//go throught the current list and skip if there is no POLLIN
			if(fds[i].revents != POLLIN) {
				continue;
			}

			if (fds[i].fd == listen_sd) {
				printf("  Listening socket is readable\n");

				/*******************************************************/
				/* Accept all incoming connections that are		  */
				/* queued up on the listening socket before we		 */
				/* loop back and call poll again.				  */
				/*******************************************************/
				while (new_sd != -1) {
					errno = 0;
					new_sd = accept(listen_sd, NULL, NULL);
					if (new_sd < 0) {
						//EWOULDBLOCK means blocked operations
						if (errno != EWOULDBLOCK) {
							perror("  accept() failed");
							goto out;
						}
						break;
					}

					printf("  New incoming connection - %d added end of the poll list\n", new_sd);
					fds[nfds].fd = new_sd;
					fds[nfds].events = POLLIN;
					nfds++;
				}
			} else {
				/*********************************************************/
				/* This is not the listening socket, therefore an		*/
				/* existing connection must be readable				*/
				/*********************************************************/

				printf("  Descriptor %d is readable\n", fds[i].fd);
				close_conn = FALSE;

				while(TRUE) {
					memset(buffer, 0, sizeof(buffer));

					errno = 0;
					rc = recv(fds[i].fd, buffer, sizeof(buffer), 0);
					if (rc < 0) {
						if (errno != EWOULDBLOCK) {
							perror("  recv() failed");
							close_conn = TRUE;
						}
						break;
					}

					if (rc == 0) {
						printf("  Connection closed by the client\n");
						close_conn = TRUE;
						break;
					}

					printf("  received data: %s\n", buffer);

					/*****************************************************/
					/* Echo the data back to the client				*/
					/*****************************************************/
					rc = send(fds[i].fd, buffer, strlen(buffer) + 1, 0);
					if (rc < 0) {
						perror("  send() failed");
						close_conn = TRUE;
						break;
					}
				}

				if (!close_conn) {
					continue;
				}

				//if connection is closed, cleanup the polled list
				close(fds[i].fd);
				fds[i].fd = -1;
			}
		}

		//optimize the array
		for (i = 0; i < nfds; i++) {
			if (fds[i].fd == -1) {
				for(j = i; j < nfds; j++) {
					fds[j].fd = fds[j+1].fd;
				}
				i--;
				nfds--;
			}
		}
	}

out:
	//close all before exit
	for (i = 0; i < nfds; i++) {
		if(fds[i].fd >= 0)
		close(fds[i].fd);
	}
}
