#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <time.h>

#define WIDTH 100//259
#define HEIGHT 50 //200

#define CELL_SKIN "█"
#define BACKGROUND ' '

enum cell_state{
	DEAD,
	ALIVE
};

struct cell{
	enum cell_state s;
};
struct cell grid[HEIGHT*WIDTH] = {0};

void init_grid(){
	for(int i = 0; i < HEIGHT*WIDTH; i++){
		grid[i].s = DEAD;	
	}
}

void p_grid(){
	for(int i = 1; i <= WIDTH*HEIGHT; i++){
		if(i%WIDTH == 0){
			printf("\n");
			continue;
		}
		if(grid[i].s == ALIVE){
			//\x1b[38;2;46;165;82m
			printf("\x1b[38;2;46;165;82m%s\x1b[0m", CELL_SKIN);
		}else{
			printf("\x1b[38;2;4131;193;150m%c\x1b[0m", BACKGROUND);
		}

	}
}

int check_neighbors(int cell, int range){
	int i = 0;
	int count = 0;
	int k = 1;
	while(i!=4){
		k = k*(-1);
		for(int l = 1; l <= range; l++){
			if(i >= 2){
				int upper_cells = cell + l*k*( WIDTH - 1);
				if(upper_cells < 0 || upper_cells >= WIDTH*HEIGHT) continue;
				else if(grid[upper_cells].s == ALIVE) count++;
			}else{
				int bottom_cells = cell - l*k*( WIDTH + 1);
				if(bottom_cells < 0 || bottom_cells >= WIDTH*HEIGHT) continue;
				else if(grid[bottom_cells].s == ALIVE) count++;
			}
		}
		i++;

	}

	return count;
}


struct cell new_grid[WIDTH*HEIGHT] = {0};

void cpy_grid(struct cell dest[], struct cell src[]){
	for(int i = 0; i < WIDTH*HEIGHT; i++){
		dest[i] = src[i];
	}
}
void rule(){
	int total_dead = 0;
	int total_alive = 0;
	//cpy_grid(new_grid, grid);
	for(int i = 0; i < HEIGHT*WIDTH; i++){
		int alive = check_neighbors(i, 1);
		switch(grid[i].s){
			case ALIVE:
				total_alive++;
				if(alive >= 2) {
					grid[i].s = DEAD;
					/*if(i + 1 < HEIGHT*WIDTH){
						grid[i+1].s = ALIVE;
					}*/
				}
				break;
			case DEAD:
				total_dead++;
				if(alive < 2) {
					grid[i].s = ALIVE;
					/*if(i - 1 < HEIGHT*WIDTH){
                                                grid[i-1].s = DEAD;
                                        }*/
				}
				break;
		}		
	}
	if(total_dead >= HEIGHT*WIDTH || total_alive >= HEIGHT*WIDTH){
		printf("Game ended\n");
		exit(0);
	}
	//cpy_grid(grid, new_grid);

}

int main()
{
	srand(time(0));
	init_grid();
	
	for(int i = rand()%(HEIGHT*WIDTH); i < HEIGHT*WIDTH; i++){
		i = rand()%(HEIGHT*WIDTH);
		grid[i].s = ALIVE;
	}
	system("clear");
	while(1){
		printf("\x1b[H");
		p_grid();
		rule();
		usleep(100000);
	}
	//int count = 0;
	//int k = 1;
	//int e = 0;
	/*while(1){
		e++;
		if(e >= 5){
			e = 0;
			init_grid();
			grid[cell].s = ALIVE;
		}
		while(count!=4){
			printf("%d\n", (cell + e*k*( WIDTH - 1)) + (cell - e*k*( WIDTH + 1)));
			k = k*(-1);
			if(count >= 2){
				grid[cell + e*k*( WIDTH - 1)].s = ALIVE;
			}else{
				grid[cell - e*k*( WIDTH + 1)].s = ALIVE;
			}
			count++;
		}
		count = 0;
		p_grid();
		usleep(100000);
		printf("\x1b[2J\x1b[H");
	}*/
	return 0;
}	















