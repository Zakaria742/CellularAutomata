#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/ioctl.h>
#include <time.h>
#include "raw.h"

//█
#define PLAYER "█"
#define BACKGROUND ' '
#define SPEED 0.1

enum cell_type{
	DEAD,
	ALIVE
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
	unsigned int **grid = calloc(height, sizeof(unsigned int*));
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

//Function to print the grid
void printGrid(unsigned int** grid, int width, int height){
	for(int i = 0; i < height - 1; i++){
		for(int j = 0; j < width; j++){
			if(grid[i][j] == ALIVE){
				printf("\x1b[38;2;158;168;47m%s\x1b[0m", PLAYER);
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

//Rules to follow;
void Conways(unsigned int **grid, int width, int height){
	unsigned int **new_generation_grid = Init_grid(width, height);
	cpy_grid(new_generation_grid, grid, width, height);
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			int neighbor_cells = 0;
			for(short k = -1; k <= 1; k++){
				for(short l = -1; l <= 1; l++){
					if (k == 0 && l == 0) continue;
					int row = ((i + k)%height < 0) ? -(i+k)%height : (i+k)%height;
					int column = ((j + l)%width < 0) ? -(j+l)%width : (j+l)%width;
					if(grid[row][column] == ALIVE) neighbor_cells++;
				}
			}

			switch(neighbor_cells){
				case 2:
					break;
				case 3:
					if(new_generation_grid[i][j] == DEAD){
						new_generation_grid[i][j] = ALIVE;
					}
					break;
				default:
					new_generation_grid[i][j] = DEAD;
					break;

			}
			}
			}	
			

	cpy_grid(grid, new_generation_grid, width, height);
	freeGrid(new_generation_grid, height);
	return ;
}

void brian(unsigned int **grid, int width, int height){
	unsigned int **new_generation_grid = Init_grid(width, height);
	for(int i = 0; i < height; i++){
		for(int j = 0; j < width; j++){
			int neighbor_cells = 0;
			for(short k = -1; k <= 1; k++){
				for(short l = -1; l <= 1; l++){
					if (k == 0 && l == 0) continue;
					int row = ((i + k)%height < 0) ? -(i+k)%height : (i+k)%height;
					int column = ((j + l)%width < 0) ? -(j+l)%width : (j+l)%width;
					if(grid[row][column] == ALIVE) neighbor_cells++;
				}
			}

			switch(grid[i][j]){
				case ALIVE:
					new_generation_grid[i][j] = DEAD;
					break;
				case DEAD:
					if(neighbor_cells == 2){
						new_generation_grid[i][j] = ALIVE;
					}
					break;

			}
		}
	}


	cpy_grid(grid, new_generation_grid, width, height);
	freeGrid(new_generation_grid, height);
	return ;
}


unsigned int** resize(unsigned int **grid, int term_w, int term_h){
        freeGrid(grid, term_h);
        grid = Init_grid(term_w, term_h);
        for(int i = rand()%term_h; i < term_h; i++){
                for(int j = rand()%term_w; j < term_w; j++){
                        grid[i][j] = ALIVE;
                }
        }
	return grid;
}

//Function to free the grid
void freeGrid(unsigned int **grid, int h){
	for(int i = 0; i < h; i++){
		free(grid[i]);
	}
	free(grid);
}

int main(){

	//Getting terminal size using ioctl
	struct termsize tSize = getTsize();
	int term_h = tSize.rows;
	int term_w = tSize.cols;
	//Getting a randow number to generate a random set of alive cells;
	srand(time(0));
	int temp_h, temp_w;
	unsigned int **grid = Init_grid(term_w, term_h);
	for(int i = rand()%term_h; i < term_h; i++){
		for(int j = rand()%term_w; j < term_w; j++){
			grid[i][j] = ALIVE;
		}	
	}
	
	char c = ' ';
	printf("\x1b[?25l");
	system("clear");
	raw();
	while(read(STDIN_FILENO, &c, 1) || c != 'q'){
		//old size
                temp_h = term_h;
                temp_w = term_w;

                //new size
                tSize = getTsize();
                term_h = tSize.rows;
                term_w = tSize.cols;
                if(term_h > temp_h || term_w > temp_w){
                        grid=resize(grid, term_w, term_h);
                }

		printf("\x1b[2J\x1b[H");
		printGrid(grid, term_w, term_h);
		Conways(grid, term_w, term_h);
		usleep(SPEED*pow(10, 6));//0.1s is good
	}
	printf("\x1b[?25h\x1b[0m");
	freeGrid(grid, term_h);
	return 0;
}
