#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>


int lireligne(int fd, char *buffer, int size) {
	ssize_t nbread = read(fd, buffer, size);
	if (nbread == -1) {
		return -1;
	}

	int i;
	for (i = 0; i < nbread; i++) {
		if (buffer[i] == '\n') {
            i++;
            buffer[i] = '\0';
			break;
		}
	}
	lseek(fd, i - nbread, SEEK_CUR);
	return i;
}


int main(int argc, char **argv) {
    chdir(".."); // Requis pour l'utilisation de Cmake
    int fd_in;  // descripteur de fichier du fichier ouvert en lecture
    int fd_out; // descripteur de fichier du fichier ouvert en écriture
    int nbRead;
    unsigned char *buffer = malloc(4096 * sizeof(unsigned char));    // buffer de lecture
    if(argc == 3) {
        printf("%s, %s\n", argv[1], argv[2]);
        fd_in = open(argv[1], O_RDONLY | O_BINARY);
        if(fd_in >= 0) {
            fd_out = open(argv[2], O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0644);
            for(int i = 0; i < 3; i++) {
                nbRead = lireligne(fd_in, buffer, 15);
                write(fd_out, buffer, nbRead);
            }

            if(nbRead > 0) {
                do {
                    nbRead = read(fd_in, buffer, 4096);
                    if (nbRead >= 0) {
                        for(int i = 0; i < nbRead; i++)
                            buffer[i] = 255 - buffer[i];
                        write(fd_out, buffer, nbRead);
                    } else {
                        perror("Une erreur est survenue lors de la lecture de fichier source.");
                        exit(1);
                    }
                } while (nbRead > 0);
            } else {
                perror("Une erreur est survenue lors de la lecture de fichier source.");
                exit(1);
            }
        } else {
            perror("Une erreur est survenue lors de l'ouverture du fichier source.");
            exit(1);
        }
    } else {
        perror("Vous n'avez pas renseigne deux parametres.");
        exit(1);
    }
    return 0;
}