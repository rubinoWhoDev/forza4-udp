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
	for (int i = 0; i < n; i++){
		for (int j = 0; j < m; j++)
			(matr[i][j] == -1) ? printf("%3c ", '-') : printf("%3d ", matr[i][j]);
		printf("\n");
	}
}

void aggiornaGriglia(Mossa mossa, int griglia[RIGHE][COLONNE], int n, int m) {
	if (griglia[0][mossa.pos] != -1 || mossa.pos > n){
		return;
	}

	int i;
	for (i = 0; griglia[i][mossa.pos] == -1 && i < m; i++);
	i--;
	griglia[i][mossa.pos] = mossa.id;
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

	// Scorrimento obliquo (dall'alto a sinistra al basso a destra)
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

	// Scorrimento obliquo (dal basso a sinistra all'alto a destra)
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

	return 1; // Nessun vincitore trovato
}


int main(int argc, char** argv){
	int sockid, n;
	socklen_t len = sizeof(struct sockaddr_in);
	struct sockaddr_in remote_addr[2], local_addr;
	int griglia[RIGHE][COLONNE] = { {-1, -1, -1, -1, -1, -1, -1},
									{-1, -1, -1, -1, -1, -1, -1},
									{-1, -1, -1, -1, -1, -1, -1},
									{-1, -1, -1, -1, -1, -1, -1},
									{-1, -1, -1, -1, -1, -1, -1},
									{-1, -1, -1, -1, -1, -1, -1}, };
	Mossa mossaAttuale;
	int giocatori = 0;
	int connectNum;

	printGriglia(griglia, RIGHE, COLONNE);

	if (argc != 2){
		printf("Uso: receiver <porta>\n");
		return 0;
	}
	
	sockid = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockid < 0){
		printf("Errore nell'apertura del socket.\n");
		return -1;
	}

	memset(&local_addr, 0, len);
	local_addr.sin_family = AF_INET;
	local_addr.sin_port = htons(atoi(argv[1]));

	if (bind(sockid, (struct sockaddr *)&local_addr, len) < 0) {
		printf("Errore nell'assegnamento della porta.\n");
		return -1;
	}
	printf("Giocatori connessi: %d\n", giocatori);
	while (giocatori != 2){
		recvfrom(sockid, &connectNum, sizeof(int), 0, (struct sockaddr *) &remote_addr[giocatori], &len);
		if (connectNum != CONNECT_NUM) continue;
		sendto(sockid, &giocatori, sizeof(int), 0, (struct sockaddr *) &remote_addr[giocatori], len);
		giocatori++;
		printf("Giocatori connessi: %d\n", giocatori);
	}
	int currGiocatore = 0;
	do {
		printf("\n\nTurno del Giocatore %d\n", currGiocatore % 2);
		printGriglia(griglia, RIGHE, COLONNE);
		sendto(sockid, griglia, sizeof(griglia), 0, (struct sockaddr*)&remote_addr[currGiocatore % 2], len);
		n = recvfrom(sockid, &mossaAttuale, sizeof(Mossa), 0, (struct sockaddr *) &remote_addr[currGiocatore % 2], &len);
		aggiornaGriglia(mossaAttuale, griglia, RIGHE, COLONNE);
		currGiocatore++;
	} while (nessunVincitore(griglia, RIGHE, COLONNE));
	for (int i = 0; i < 2; i++) sendto(sockid, griglia, sizeof(griglia), 0, (struct sockaddr*)&remote_addr[i], len);
	printf("\n\nGriglia finale:\n");
	printGriglia(griglia, RIGHE, COLONNE);
	printf("Vincitore: Giocatore %d.\n", (currGiocatore - 1) % 2);

	close(sockid);
	return 0;
}
