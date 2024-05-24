#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define RIGHE 6
#define COLONNE 7
#define CONNECT_NUM 7

typedef struct {
	int pos;
	int id;
} Mossa;

void printGriglia(int matr[RIGHE][COLONNE], int n, int m) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++)
			(matr[i][j] == -1) ? printf("%3c ", '-') : printf("%3d ", matr[i][j]);
		printf("\n");
	}
}

int nessunVincitore(int griglia[RIGHE][COLONNE], int n, int m) {
	int conta;

	// Scorrimento orizzontale
	for (int i = 0; i < n; i++) {
		for (int j = 0; j <= m - 4; j++) {
			conta = 1;
			for (int prossimo = 1; prossimo < 4; prossimo++) {
				if (griglia[i][j] == -1 || griglia[i][j] != griglia[i][j + prossimo])
					break;
				conta++;
			}
			if (conta == 4) return 0;
		}
	}

	// Scorrimento verticale
	for (int j = 0; j < m; j++) {
		for (int i = 0; i <= n - 4; i++) {
			conta = 1;
			for (int prossimo = 1; prossimo < 4; prossimo++) {
				if (griglia[i][j] == -1 || griglia[i][j] != griglia[i + prossimo][j])
					break;
				conta++;
			}
			if (conta == 4) return 0;
		}
	}

	// Scorrimento obliquo 
	for (int i = 0; i <= n - 4; i++) {
		for (int j = 0; j <= m - 4; j++) {
			conta = 1;
			for (int prossimo = 1; prossimo < 4; prossimo++) {
				if (griglia[i][j] == -1 || griglia[i][j] != griglia[i + prossimo][j + prossimo])
					break;
				conta++;
			}
			if (conta == 4) return 0;
		}
	}

	// Scorrimento obliquo
	for (int i = 3; i < n; i++) {
		for (int j = 0; j <= m - 4; j++) {
			conta = 1;
			for (int prossimo = 1; prossimo < 4; prossimo++) {
				if (griglia[i][j] == -1 || griglia[i][j] != griglia[i - prossimo][j + prossimo])
					break;
				conta++;
			}
			if (conta == 4) return 0;
		}
	}

	return 1;
}

int Vincitore(int griglia[RIGHE][COLONNE], int n, int m) {
	int conta;

	// Scorrimento orizzontale
	for (int i = 0; i < n; i++) {
		for (int j = 0; j <= m - 4; j++) {
			conta = 1;
			for (int prossimo = 1; prossimo < 4; prossimo++) {
				if (griglia[i][j] == -1 || griglia[i][j] != griglia[i][j + prossimo])
					break;
				conta++;
			}
			if (conta == 4) return griglia[i][j];
		}
	}

	// Scorrimento verticale
	for (int j = 0; j < m; j++) {
		for (int i = 0; i <= n - 4; i++) {
			conta = 1;
			for (int prossimo = 1; prossimo < 4; prossimo++) {
				if (griglia[i][j] == -1 || griglia[i][j] != griglia[i + prossimo][j])
					break;
				conta++;
			}
			if (conta == 4) return griglia[i][j];
		}
	}

	// Scorrimento obliquo 
	for (int i = 0; i <= n - 4; i++) {
		for (int j = 0; j <= m - 4; j++) {
			conta = 1;
			for (int prossimo = 1; prossimo < 4; prossimo++) {
				if (griglia[i][j] == -1 || griglia[i][j] != griglia[i + prossimo][j + prossimo])
					break;
				conta++;
			}
			if (conta == 4) return griglia[i][j];
		}
	}

	// Scorrimento obliquo
	for (int i = 3; i < n; i++) {
		for (int j = 0; j <= m - 4; j++) {
			conta = 1;
			for (int prossimo = 1; prossimo < 4; prossimo++) {
				if (griglia[i][j] == -1 || griglia[i][j] != griglia[i - prossimo][j + prossimo])
					break;
				conta++;
			}
			if (conta == 4) return griglia[i][j];
		}
	}

	return -1;
}

int** allocGriglia(int n, int m) {
	int** matr = malloc(sizeof(int*) * n);
	for (int i = 0; i < m; i++)
		matr[i] = malloc(sizeof(int) * m);
	
	return matr;
}

void aggiornaGriglia(Mossa mossa, int griglia[RIGHE][COLONNE], int n, int m) {
	if (griglia[0][mossa.pos] != -1 || mossa.pos > n) {
		return;
	}

	int i;
	for (i = 0; griglia[i][mossa.pos] == -1 && i < m; i++);
	i--;
	griglia[i][mossa.pos] = mossa.id;
}

int main(int argc, char** argv){
	int sockid, n = CONNECT_NUM;
	int id = -1;
	int griglia[RIGHE][COLONNE];
	struct sockaddr_in remote_addr;
	socklen_t len = sizeof(struct sockaddr_in);

	if (argc != 3){
		printf("Uso: sender <indirizzo> <porta>\n");
		return 0;
	}

	sockid = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockid < 0){
		printf("Errore nell'apertura del socket.\n");
		return -1;
	}
	
	memset(&remote_addr, 0, len);
	remote_addr.sin_family = AF_INET;
	inet_pton(AF_INET, argv[1], &(remote_addr.sin_addr));
	remote_addr.sin_port = htons(atoi(argv[2]));

	while (id == -1){
		sendto(sockid, &n, sizeof(int), 0, (struct sockaddr *) &remote_addr, len);
		printf("In attesa del server...\n");
		recvfrom(sockid, &id, sizeof(int), 0, (struct sockaddr *) &remote_addr, &len);
	}

	do {
		printf("In attesa del server...\n");
		recvfrom(sockid, griglia, sizeof(griglia), 0, (struct sockaddr*)&remote_addr, &len);
		if (!nessunVincitore(griglia, RIGHE, COLONNE)) break;
		printGriglia(griglia, RIGHE, COLONNE);
		Mossa mossa;
		mossa.id = id;
		printf("Tu hai l'etichetta %d.\n", id);
		do {
			printf("Colonna (si parte da 0): ");
			scanf("%d", &mossa.pos);
		} while (mossa.pos >= COLONNE);

		sendto(sockid, &mossa, sizeof(mossa), 0, (struct sockaddr *)&remote_addr, len);
		aggiornaGriglia(mossa, griglia, RIGHE, COLONNE);
		printf("Griglia dopo la mossa:\n");
		printGriglia(griglia, RIGHE, COLONNE);
		printf("\n\n");
	} while (nessunVincitore(griglia, RIGHE, COLONNE));

	printf("Griglia finale:\n");
	printGriglia(griglia, RIGHE, COLONNE);
	(Vincitore(griglia, RIGHE, COLONNE) == id) ? printf("Congratulazioni, hai vinto!\n") : printf("Ops, hai perso :c\n");

	close(sockid);

	return 0;
}
