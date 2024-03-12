#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

int main(int argc, char **argv) {
    chdir(".."); // Requis pour l'utilisation de Cmake
	int fdin, fdout; // descripteurs de fichiers en entrée et en sortie
	char buffer[4096];	// buffer utilisée pour les lectures/écritures
	int nbread;	// nombre d'octets lus après chaque appel à read


	// [1] Ouverture des fichiers en entrée et sortie
	// le nom du fichier à lire est passé en premier argument du programme, le
	// nom du fichier dans lequel écrire le résultat est passé en second
	// argument
    if(argc == 3) {
        printf("%s, %s\n", argv[1], argv[2]);
        fdin = open(argv[1], O_RDONLY | O_BINARY);
        if(fdin >= 0) {
            fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC | O_BINARY, 0644);

            nbread = read(fdin, buffer, 44);
            if(nbread == 44) {
                // Ecriture de l'entete
                int *fileSize = (int *) &buffer[4];
                *fileSize /= 2;
                short int *nbCanal = (short int *) &buffer[22];
                *nbCanal = 1;
                int *bytePerSec = (int *) &buffer[28];
                *bytePerSec /= 2;
                short int *bytePerBlock = (short int *) &buffer[32];
                *bytePerBlock /= 2;
                int *dataSize = (int *) &buffer[40];
                *dataSize /= 2;
                write(fdout, buffer, 44);

                do {
                    nbread = read(fdin, buffer, 4096);
                    if(nbread > 0) {
                        int n = 0;
                        for(int i = 0; i < nbread; i += 4) {
                            buffer[n] = buffer[i];
                        }
                        nbread /= 2;
                        write(fdout, buffer, nbread);
                    }
                } while (nbread > 0);
            } else {
                perror("Une erreur est survenue lors de la lecture de l'entete du fichier source.");
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


	// [2] Lecture de l'entête du fichier en entrée, modification des données
	// nécessaires dans le buffer et écriture de l'entête modifié dans le
	// fichier en sortie


	// [3] Lecture des données du fichier en entrée (par blocs de 4096 octets)
	// et écriture dans le fichier en sortie des octets correspondant aux
	// échantillons du premier canal.


	close(fdin);
	close(fdout);
}