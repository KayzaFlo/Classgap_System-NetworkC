#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>


int main(int argc, char const *argv[])
{
	int fdA = open("monfichier.txt", O_RDONLY);  // Lecture seule
	int fdB = open("monfichier.txt", O_WRONLY);  // Écriture seule
	int fdC = open("monfichier.txt", O_RDWR);    // Les deux
	int fdD = open("nouveau.txt", O_WRONLY | O_CREAT, 0644); // Créer si n'existe pas

	printf("%d\n", fdA);
	printf("%d\n", fdB);
	printf("%d\n", fdC);
	printf("%d\n", fdD);

	close(fdA);
	close(fdB);
	close(fdC);
	close(fdD);
}
