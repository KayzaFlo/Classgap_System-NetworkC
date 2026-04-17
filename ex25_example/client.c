#include "headerExample.h"

int main(void){

	struct sockaddr_in addr;
	int sfd, rfd;
	ssize_t numWrite, numRead;
	char buf[BUF_SIZE];

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1) {
		perror("socket");
		exit(EXIT_FAILURE);
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(PORT);

	if (connect(sfd, (struct sockaddr *) &addr,
		sizeof (struct sockaddr_in)) == -1){
		perror("connect");
		exit(EXIT_FAILURE);
	}

	rfd = open("./example.txt", O_RDONLY);
	if (rfd == -1){
		perror("open urandom");
		exit(EXIT_FAILURE);
	}

	numWrite = read(rfd, buf, BUF_SIZE);
	if (numWrite == -1) {
		perror("read");
		exit(EXIT_FAILURE);
	} else if (numWrite > 0) {
		if (write(sfd, buf, numWrite) != numWrite) {
			perror("write");
			exit(EXIT_FAILURE);
		}
	}
	numRead = read(sfd,buf,BUF_SIZE);
	if (numRead == -1) {
		perror("read");
		exit(EXIT_FAILURE);
	} else if (numRead == 0) {
		printf("[Client] Le serveur a fermé la connexion.\n");
	} else {
		buf[numRead] = '\0'; // On termine la chaîne de caractères proprement
		printf("Client saw: %s\n", buf);
	}

	close(rfd);
	close(sfd);

	exit(EXIT_SUCCESS);
}