#include "headerExample.h"
#define BACKLOG 5
int main(void){

	struct sockaddr_in addr;
	int sfd, cfd;
	ssize_t numRead;
	char buf[BUF_SIZE];

	sfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sfd == -1){
		perror("socket");
		exit(EXIT_FAILURE);
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family = AF_INET;

	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(SV_SOCK_PORT);

	if (bind(sfd, (struct sockaddr *) &addr, sizeof(struct sockaddr_in)) == -1){
		perror("bind");
		exit(EXIT_FAILURE);
	}

	if (listen(sfd, BACKLOG) == -1){
		perror("listen");
		exit(EXIT_FAILURE);
	}

	cfd = accept(sfd, NULL, NULL);
	if (cfd == -1) {
		perror("accept");
		exit(EXIT_FAILURE);
	}

	numRead = read(cfd, buf, BUF_SIZE);
	if (numRead == -1) {
		perror("read");
		exit(EXIT_FAILURE);
	}

	write(STDOUT_FILENO, buf, numRead);

	strcpy(buf, "Done! :)\n");
	if (write(cfd, buf, BUF_SIZE) == -1){
		unlink(SV_SOCK_PATH);
		perror("write back");
		exit(EXIT_FAILURE);
	}

	if (close(cfd) == -1) {
		perror("close cfd");
		exit(EXIT_FAILURE);
	}
}