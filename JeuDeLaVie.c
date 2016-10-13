/*
	Jeu de la vie
	PELLOIN VALENTIN - L2 SPI - Université Du Maine

	ATTENTION : Pour fonctionner correctement, la fenêtre du terminal doit être suffisamment grande :
	 		==> MINIMUM M+6 x N+15
			    (M et N définissent la taille de la matrice, cf ligne 28 et 29)

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define COLOR_RED     "\x1b[31m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_BLUE    "\x1b[34m"
#define COLOR_MAGENTA "\x1b[35m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_BLACK   "\x1b[30m"
#define COLOR_WHITE   "\x1b[37m"

#define COLOR_RESET   "\x1b[0m"

#define N 30  //X : COLONNE
#define M 100 //Y : LIGNE

#define XSHIFT 5
#define YSHIFT 10

#define DEBUG_MODE 0
#define DISPLAY_WARNING 1


void clearScreen(){
	printf("\e[1;1H\e[2J");
}

void changeColor(char color[]){
	printf("%s", color);
}

//Wait for miliS miliseconds
void waitmiliS(int miliS){
	usleep(miliS*1000);
}

//Move the cursor to the x ; y coordinates
void gotoxy(int x, int y){
	printf("\033[%d;%dH", y, x);
}

//Move the cursor to the end of the matrix
void gotoEndDisplay(int shift){
	gotoxy(0, N + YSHIFT + 2 + shift);
}

void displayCell(int valueCell){

	switch(valueCell){
		case 0: changeColor(COLOR_CYAN);  printf("█"); break;
		case 1: changeColor(COLOR_RED);   printf("█"); break;
		default: changeColor(COLOR_WHITE); printf("X"); break;
	}

}

//Display the matrix and the current generation
void displayMatrix(int Mat[N][M], int generation){

	int i, j, k;

	clearScreen();

	changeColor(COLOR_WHITE);
	printf("\n\n Jeu de la vie : \n");
	printf("   > Matrice de %ix%i.\n", M, N);
	printf("   > Génération %i.\n\n", generation);





	//Print table border
	printf("┏");
	for(i = 0 ; i <= M+3 ; i++) printf("━");
	printf("┓\n┃\n┃   ");

	//Print row numbers
	changeColor(COLOR_BLUE);
	for(i = 0 ; i < M ; i++) printf("%i",i%10);




	changeColor(COLOR_WHITE);

	//Print table border
	gotoxy(M+XSHIFT+1,YSHIFT-1);
	printf("┃\n┃  %s0",COLOR_BLUE);

	for(i = 0 ; i < N ; i++){
		for(j = 0 ; j < M ; j++){
			displayCell(Mat[i][j]); //Display each cell
		}

		//Print table border
		changeColor(COLOR_WHITE);
		printf(" ┃\n┃ ");

		//Print lines numbers
		changeColor(COLOR_BLUE);
		if(i+1 <= 9) printf(" %i", i+1);
		else printf("%i", i+1);



	}

	//Print table border
	changeColor(COLOR_WHITE);
	printf("\n┗");

	for(i = 0 ; i <= M+3 ; i++) printf("━");
	printf("┛");
	gotoxy(M+XSHIFT+1,N+YSHIFT);
	printf("┃");

	gotoxy(M+XSHIFT+1,YSHIFT-2);
	printf("┃");

	gotoxy(3,N+YSHIFT);
	printf("   ");


	gotoEndDisplay(4);

}



/* Utilisation :
	N : LIGNES
	M : COLONNES
	Mat[N][M];
	updateMatrix(Mat , INDICE DE LA LIGNE (Y => N), INDICE DE LA COLONNE (X => M));
*/
void updateMatrix(int Mat[N][M], int y, int x){

	gotoxy(x+XSHIFT,y+YSHIFT);

	displayCell(Mat[y][x]);

	gotoEndDisplay(3);
}


//Retourne un entier aléatoire, 0 ou 1
int rand_0_1(){

	return rand()%2;

}

//Cette fonction initialise les cellules de la matrice, en ouvrant un fichier indiqué par l'utilisateur
void initCells(int Mat[N][M]){

	char fileName[255];
	FILE *file;

	int cellX, cellY;

	char askSentence[255] = "   > Veuillez indiquer un nom de fichier contenant la liste des cellules à ouvrir : ";

	clearScreen();
	changeColor(COLOR_WHITE);
	printf("\n\n Jeu de la vie : \n");
	printf("%s\n",askSentence);
	printf("      => %srandom%s pour une génération initiale aléatoire.%s", COLOR_MAGENTA, COLOR_WHITE, COLOR_MAGENTA);
	gotoxy(strlen(askSentence),4);
	scanf("%s",fileName);


	if(strcmp(fileName, "random") == 0){

		for(cellX = 0 ; cellX < N ; cellX++){
			for(cellY = 0 ; cellY < M ; cellY++){
				Mat[cellX][cellY] = rand_0_1();
			}
		}


	}
	else {

		file = fopen(fileName, "r");

		while(file == NULL){
			clearScreen();
			changeColor(COLOR_RED);
			printf("\n /!\\ Le fichier %s%s%s n'existe pas ! /!\\\n", COLOR_MAGENTA, fileName, COLOR_RED);
			changeColor(COLOR_WHITE);
			printf(" Jeu de la vie : \n");
			printf("   > Veuillez indiquer un nom de fichier contenant la liste des cellules à ouvrir : ");
			scanf("%s",fileName);
			file = fopen(fileName, "r");
		}


		fscanf(file, "%i", &cellX);
		while(!feof(file)){
			fscanf(file, "%i", &cellY);
			Mat[cellY][cellX] = 1;
			fscanf(file, "%i", &cellX);
		}

		fclose(file);

	}
}

//Demande à l'utilisateur le nombre de générations à afficher
int askGenerationMax(){
	int maxGenerations;

	do{
		clearScreen();
		changeColor(COLOR_WHITE);
		printf("\n\n Jeu de la vie : \n");
		printf("   > Veuillez indiquer le nombre de générations à afficher : %s",COLOR_MAGENTA);
		scanf("%i", &maxGenerations);
	}while(maxGenerations <= 0);

	return maxGenerations;

}

//Calcule les voisins de chaque cellule
void calcNeighbor(int Mat[N][M], int Neighbor[N][M]){

	int i, j;
	int neighborTemp;

	for(i = 0 ; i < N ; i++){
		for(j = 0 ; j < M ; j++){

			neighborTemp = 0;

			//Si nous pouvons regarder en haut de la matrice
			if(i - 1 >= 0){

				if(Mat[i-1][j] == 1) neighborTemp++;

				if((j - 1 >= 0) && (Mat[i-1][j-1] == 1)) neighborTemp++; //Si nous pouvons regarder en haut à gauche

				if((j + 1 < M) && (Mat[i-1][j+1] == 1)) neighborTemp++; //Si nous pouvons regarder en haut à droite

			}


			if(i + 1 < N){

				if(Mat[i+1][j] == 1) neighborTemp++;

				if((j - 1 >= 0) && (Mat[i+1][j-1] == 1)) neighborTemp++; //Si nous pouvons regarder en haut à gauche

				if((j + 1 < M) && (Mat[i+1][j+1] == 1)) neighborTemp++; //Si nous pouvons regarder en haut à droite

			}

			if((j - 1 >= 0) && (Mat[i][j-1] == 1)) neighborTemp++;
			if((j + 1 < M) && (Mat[i][j+1] == 1)) neighborTemp++;



			Neighbor[i][j] = neighborTemp;

		}
	}

}

//Calc living cells according to the neighbors
void calcLivingCells(int Mat[N][M], int Neighbor[N][M]){


	int i, j;


	for(i = 0 ; i < N ; i++){
		for(j = 0 ; j < M ; j++){

			if(Mat[i][j] == 1 && (Neighbor[i][j] != 2 && Neighbor[i][j] != 3)){
				Mat[i][j] = 0;
				updateMatrix(Mat,i,j);
			}

			else if(Mat[i][j] == 0 && Neighbor[i][j] == 3){
				Mat[i][j] = 1;
				updateMatrix(Mat, i, j);
			}


		}
	}

}

//ONLY IN DEBUG MODE (IF DEBUB = 1), display the neighbors matrix
void displayNeighbor(int Neighbor[N][M]){

	int i, j;
	clearScreen();
	gotoxy(0,N+15);
	for(i = 0 ; i < N ; i++){
		for(j = 0 ; j < M ; j++){
			printf("%i", Neighbor[i][j]);
		}
		printf("\n");
	}

	printf("\n\n");
}


void displayWarning(){

	clearScreen();

	//gotoxy(0,10);
	changeColor(COLOR_RED);
	printf(" ATTENTION : VOTRE FENÊTRE DOIT FAIRE AU MINIMUM %s%i x %i%s POUR QUE LE PROGRAMME PUISSE FONCTIONNER.\n\n",COLOR_GREEN,M+6,N+15,COLOR_RED);
	printf("     REDIMENSIONNEZ-LA SI NECESSAIRE, PUIS APPUYEZ SUR ENTRÉE.\n\n\n\n\n\n\n");

	printf("%sPour ignorer l'avertissement la prochaine fois, modifiez \n  %s#define DISPLAY_WARNING 1\n%s     en \n  %s#define DISPLAY_WARNING 0%s\n     à la ligne %s35%s du programme.\n\n\n",COLOR_WHITE,COLOR_MAGENTA,COLOR_WHITE,COLOR_MAGENTA,COLOR_WHITE,COLOR_MAGENTA,COLOR_WHITE);

	gotoxy(0,6);
	scanf("*c");


}

int main(){

	int Mat[N][M] = {0};

	int Neighbor[N][M] = {0};

	//int i;
	int generation = 1;
	int maxGenerations;

	srand(time(NULL));

	if(DISPLAY_WARNING) displayWarning(); //Warning about the size of the screen

	initCells(Mat);
	maxGenerations = askGenerationMax();

	//First display of the matrix
	displayMatrix(Mat, generation);


	while(generation < maxGenerations){

		calcNeighbor(Mat,Neighbor);

		if(DEBUG_MODE) displayNeighbor(Neighbor); //Print neighbor only if DEBUG_MODE = 1

		calcLivingCells(Mat,Neighbor); //Calc living cells and update display

		generation++;

		//Print the current generation
		gotoxy(17,5);
		changeColor(COLOR_WHITE);
		printf("%i.", generation);

		//Avoid waitmiliS to be executed before display (buffer)
		gotoEndDisplay(2);
		printf("\n");

		waitmiliS(200); //Wait for .2 second
	}



	changeColor(COLOR_RESET);
	gotoEndDisplay(2);


}
