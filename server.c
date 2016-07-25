#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include "slist.h"

#define bufLen 4096
// client data
typedef struct recvdata_st {
	char *buf;
	struct sockaddr_in *addr;
} recvdata_t;

void usage();

int isDigits(char *str);

int main(int argc, char **argv) {
	if (argc != 2) {
		usage();
		exit(-1);
	}

	int port = -1;
	if ((isDigits(argv[1]) == -1) || (sscanf(argv[1], "%d", &port) != 1)) {
		fprintf(stderr, "wrong input\n");
		exit(-1);
	}

	// create udp socket
	int udp_sd; // UDP socket descriptor
	if ((udp_sd = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("socket\n");
		exit(-1);
	}

	struct sockaddr_in remote;
	remote .sin_family = AF_INET;
	remote.sin_addr.s_addr = htonl(INADDR_ANY);
	remote.sin_port = htons(port);

	if (bind(udp_sd, (struct sockaddr*) &remote, sizeof(remote)) < 0) {
		perror("bind\n");
		exit(-1);
	}

	// queue init
	slist_t Q;
	slist_init(&Q);

	// readset and writeset init
	fd_set wset, rset;

	int ret;
	int n, i;
	while (1) {
		FD_ZERO(&rset);
		FD_SET(udp_sd, &rset);
			FD_ZERO(&wset);
			FD_SET(udp_sd, &wset);
		// check if ready for ioopporation
		if ((ret = select(udp_sd +1, &rset, &wset, NULL, NULL)) < 0) {
			perror("select\n");
			exit(-1);
		}
		else if (ret > 0) {
		// check if ready for read opporation
			if (FD_ISSET(udp_sd, &rset)) {
			printf("server is ready to read\n");
				struct sockaddr_in clientaddr;
memset(&clientaddr, 0, sizeof(clientaddr));
				int clientaddrLen = sizeof(clientaddr);
				char buf[bufLen];
				memset(buf, 0, bufLen);
				if (recvfrom(udp_sd, buf, sizeof(buf), 0, (struct sockaddr*) &clientaddr, &clientaddrLen) < 0) {
					perror("recvfrom\n");
					exit(-1);
				}
				char *pBuf = (char*) calloc((strlen(buf) +1) * sizeof(char), sizeof(char));
				if (!pBuf) {
					perror("malloc\n");
					exit(-1);
				}
				//copy buf to fixed size buf
				n = 0;
				while (n < (strlen(buf) -1)) {
					pBuf[n] = buf[n];
					n++;
				}
pBuf[n] ='\0'; // add \0 at the end
				recvdata_t recvData;
				recvData.addr = &clientaddr;
				recvData.buf = pBuf;
				slist_append(&Q, &recvData);
			}

		// check if q is not empty, then check if ready for write opporation
		if (Q.size > 0) {
				if (FD_ISSET(udp_sd, &wset)) {
			printf("Server is ready to write\n");
					// write through udp socket process 
					recvdata_t *rData = (recvdata_t*) slist_pop_first(&Q);
					char *bufTmp = rData->buf;
					// convert buf to upercase letters only
					i = 0;
					while (bufTmp[i]) {
						bufTmp[i] = toupper(bufTmp[i]);
						i++;
					}
struct sockaddr_in r = *(rData->addr);
					if (sendto(udp_sd, rData->buf, strlen(rData->buf), 0, (struct sockaddr*) &r, sizeof(r)) < 0) {
						perror("sendto\n");
						exit(-1);
					}
					free(bufTmp);
				}
			}
		}
	}

	slist_destroy(&Q, SLIST_LEAVE_DATA);
	close(udp_sd);

	return 0;
}

void usage() {
	printf("Usage: server <port>\n");
}

/*
** check if the a string holds digits only
** returns 0 for only digits and -1 in case of a failure
*/
int isDigits(char *str) {
	int i = 0;
	while (str[i]) {
		if (isdigit(str[i]) == 0) {
			return -1;
		}
		i++;
	}

	return 0;
}
