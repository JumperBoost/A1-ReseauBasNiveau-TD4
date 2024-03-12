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
    if(argc == 4) {
        printf("%s, %s, %s\n", argv[1], argv[2], argv[3]);
        fd_in = open(argv[1], O_RDONLY | O_BINARY);
        if(fd_in >= 0) {
            fd_out = open(argv[2], O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0644);
            int nbPerPixel = 1;
            int width, height;
            for(int i = 0; i < 3; i++) {
                nbRead = lireligne(fd_in, buffer, 15);
                write(fd_out, buffer, nbRead);
                if(i == 0 && strcmp(buffer, "P6"))
                    nbPerPixel = 3;
                if(i == 1) {
                    int space_c = 0;
                    while (space_c < nbRead && buffer[space_c] != ' ')
                        space_c++;
                    height = atoi(buffer + space_c);
                    buffer[space_c] = '\0';
                    width = atoi(buffer);
                }
            }

            if(nbRead > 0) {
                if(strcmp(argv[3], "-v") == 0) {
                    do {
                        nbRead = read(fd_in, buffer, width * nbPerPixel);
                        if (nbRead >= 0) {
                            for (int i = 0; i < nbRead/2; i += nbPerPixel) {
                                for (int j = 0; j < nbPerPixel; j++) {
                                    int pixel_left = buffer[i + j];
                                    int pixel_right = buffer[nbRead-nbPerPixel - i + j];
                                    buffer[i + j] = pixel_right;
                                    buffer[nbRead-nbPerPixel - i + j] = pixel_left;
                                }
                            }
                            write(fd_out, buffer, nbRead);
                        } else {
                            perror("Une erreur est survenue lors de la lecture verticale du fichier source.");
                            exit(1);
                        }
                    } while (nbRead > 0);
                } else if(strcmp(argv[3], "-h") == 0) {
                    unsigned char *buffer2 = malloc(height * width * nbPerPixel * sizeof (unsigned char));
                    nbRead = read(fd_in, buffer2, height * width * nbPerPixel);
                    if(nbRead >= 0) {
                        for(int i = 0; i < height / 2; i++) {
                            for(int j = 0; j < width*nbPerPixel; j+=nbPerPixel) {
                                for(int k = 0; k < nbPerPixel; k++) {
                                    int pixel_up = buffer2[i * width * nbPerPixel + j+k];
                                    int pixel_down = buffer2[(height - 1 - i) * width * nbPerPixel + j+k];
                                    buffer2[i * width * nbPerPixel + j+k] = pixel_down;
                                    buffer2[(height - 1 - i) * width * nbPerPixel + j+k] = pixel_up;
                                }
                            }
                        }
                        write(fd_out, buffer2, nbRead);
                    } else {
                        perror("Une erreur est survenue lors de la lecture horizontale du fichier source.");
                        exit(1);
                    }
                }
            } else {
                perror("Une erreur est survenue lors de la lecture de fichier source.");
                exit(1);
            }
        } else {
            perror("Une erreur est survenue lors de l'ouverture du fichier source.");
            exit(1);
        }
    } else {
        perror("Vous n'avez pas renseigne trois parametres.");
        exit(1);
    }
    return 0;
}