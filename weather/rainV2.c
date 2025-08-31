#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/ioctl.h>
#include <time.h>
#include "../raw.h"

//█
#define PLAYER "█"
#define BACKGROUND ' '
#define SPEED 0.1 //in seconds

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



unsigned int *Init_line(int len){
	unsigned int *line = malloc(len*sizeof(unsigned int*));
	if(line == NULL){
		fprintf(stderr, "Error allocating the line!\n");
		exit(-1);
	}

	return line;
}

//Function to print the line
void printLine(unsigned int* line, int len){
	int r, g;
	for(int i = 0; i < len; i++){
		if(line[i] == ALIVE){
			r = rand()%80;
			g = rand()%80;
			printf("\x1b[38;2;%d;%d;255m%s\x1b[0m",r, g, PLAYER);
			}
		else{
			printf("%c", BACKGROUND);
		}
	}
	printf("\n");
}

void cpy_line(unsigned int* dest, unsigned int* src, int len){
	for(int i = 0; i < len; i++){
		dest[i] = src[i];
        }

	return ;
}

void rain(unsigned int *line, int len){
        unsigned int *new_generation_line = Init_line(len);
	cpy_line(new_generation_line, line, len);
        for(int i = 0; i < len; i++){
		if(line[i] == ALIVE){
			short random = ( (rand()%2) * ( (-1) * (rand()%2) ));
			if(i+random < 0 || i+random > len) continue;
			new_generation_line[i+random] = ALIVE;
			new_generation_line[i] = DEAD;
		}	
        }


        cpy_line(line, new_generation_line, len);
        free(new_generation_line);
        return ;
}

unsigned int* resize(unsigned int *line, int term_w){
        free(line);
        line = Init_line(term_w);
        return line;
}
int main(){

	//Getting terminal size using ioctl
	struct termsize ts = getTsize();
	int term_w = ts.cols;
	int term_h = ts.rows;
	//Getting a randow number to generate a random set of alive cells;
	srand(time(0));

	int temp_w;
	unsigned int *line = Init_line(term_w);
	
	printf("\x1b[?25l");
	int i = 0;
	raw();
	char c = ' ';
	while(read(STDIN_FILENO, &c, 1) || c != 'q'){
		temp_w = term_w;
		ts = getTsize();
		term_w = ts.cols;
		term_h = ts.rows;
		if(term_w > temp_w){
			line=resize(line, term_w);
		}
		line[rand()%(term_w)] = ALIVE;
		printf("\x1b[%d;0H", i%term_h);
		printLine(line, term_w);
		rain(line, term_w);
		i++;
		usleep((SPEED*pow(10, 6)) / (term_h));
	}
	printf("\x1b[?25h");
	free(line);
	return 0;
}
