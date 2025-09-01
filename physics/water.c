#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/ioctl.h>
#include <time.h>
#include "../raw.h"

//█▀
#define PLAYER "█"
#define BACKGROUND ' '
#define R_SPEED 0.05

enum cell_type{
	DEAD,
	ALIVE,
};

struct termsize{
	int rows;
	int cols;
};

struct termsize getTsize(){
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	return(struct termsize){.rows = w.ws_row, .cols=w.ws_col};
}	


void freeGrid(unsigned int**, int);

unsigned int **Init_grid(int width, int height){
	unsigned int **grid = calloc(height*width, sizeof(unsigned int*));
	if(grid == NULL){
		fprintf(stderr, "Error allocating the grid!\n");
		exit(-1);
	}
	for(int i = 0; i < height; i++){
		grid[i] = calloc(width, sizeof(unsigned int));
		if(grid == NULL){
			fprintf(stderr, "Error allocating the grid rows!\n");
			exit(-1);
		}
	}


	return grid;
}

unsigned int** resize(unsigned int **grid, int term_w, int term_h){
	freeGrid(grid, term_h);
	grid = Init_grid(term_w, term_h);
	return grid;
}

//Function to print the grid
void printGrid(unsigned int** grid, int width, int height, int r, int g, int b){
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			if(grid[i][j] == ALIVE){
				printf("\x1b[38;2;%d;%d;%dm%s\x1b[0m",r, g, b,PLAYER);
			}
			else{
				printf("%c", BACKGROUND);
			}
		}
		printf("\n");
	}
}
//function to cpy the grids
//using memcpy to cpy the states created a bug. noted
void cpy_grid(unsigned int** dest, unsigned int** src, int width, int height){
	for(int i = 0; i < height; i++){
		for(int j =0 ; j < width; j++){
			dest[i][j] = src[i][j];
		}
	}

	return ;
}

void rain(unsigned int **grid, int width, int height){
	unsigned int **new_generation_grid = Init_grid(width, height);
	cpy_grid(new_generation_grid, grid, width, height);
	for(int i = 0; i < height - 1; i++){
		//int isFull = 0;
		for(int j = 0; j < width; j++){
			int random = (rand()%2) * 2 - 1;
			//Gravity:
			if(grid[i][j] == ALIVE){
				int n_count = 0;
				if(grid[i+1][j] == ALIVE){
					for(int k = -1; k <= 1; k+=2){
						if(j + k < 0 || j + k >= width) continue;
						if(grid[i+1][j+k] == DEAD){
							new_generation_grid[i+1][j+k] = ALIVE;
							new_generation_grid[i][j] = DEAD;
						}else{
							n_count++;
						}

					}
					if(n_count == 2){
						int sides = 0;
						int empty_pos = 0;
						for(int l = -1; l <= 1; l+=2){
							if(grid[i][j+l] == ALIVE) {
								break;
							}else{
								new_generation_grid[i][j+l] = ALIVE;
                                                        new_generation_grid[i][j] = DEAD;
							break;
							}
						}

					}

				}
				else{
					new_generation_grid[i+1][j] = ALIVE;
					new_generation_grid[i][j] = DEAD;
				}
			}
		}
	}


	cpy_grid(grid, new_generation_grid, width, height);
	freeGrid(new_generation_grid, height);
	return ;
}


//Function to free the grid
void freeGrid(unsigned int **grid, int h){
	for(int i = 0; i < h; i++){
		free(grid[i]);
	}
	free(grid);
}

int main(){

	struct termsize tSize = getTsize();
	int term_h = tSize.rows;
	int term_w = tSize.cols;
	//Getting a randow number to generate a random set of alive cells;
	srand(time(0));

	int temp_h, temp_w;
	unsigned int **grid = Init_grid(term_w, term_h);
	
	char c = ' ';
	raw();
	printf("\x1b[?25l");
	while(read(STDIN_FILENO, &c, 1) || c != 'q'){
		temp_h = term_h;
		temp_w = term_w;
		tSize = getTsize();
		term_h = tSize.rows;
		term_w = tSize.cols;
		if(term_h > temp_h || term_w > temp_w){
			grid=resize(grid, term_w, term_h);
		}
		grid[0][rand()%(term_w)] = ALIVE;

		printf("\x1b[2J\x1b[H");
		printGrid(grid, term_w, term_h, 0, 0, 255);
		rain(grid, term_w, term_h);
		usleep(R_SPEED*pow(10, 6));
	}
	printf("\x1b[?25h");
	freeGrid(grid, term_h);

	return 0;
}
