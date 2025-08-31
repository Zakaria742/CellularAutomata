#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>


//█
#define PLAYER "█"
#define BACKGROUND '.'

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


void freeGrid(unsigned int*);

unsigned int *Init_line(int len){
	unsigned int *line = malloc(len*sizeof(unsigned int*));
	if(line == NULL){
		fprintf(stderr, "Error allocating the line!\n");
		exit(-1);
	}
	//printf("\x1b[34mAll went well!\x1b[0m\n");

	return line;
}

//Function to print the line
void printLine(unsigned int* line, int len){
	struct termsize Tsize = getTsize();
	int term_w = Tsize.cols;
	int r = 255;
	int g = r;
	int b = r;
	if(term_w <= len){
		len = term_w;
	}

	for(int i = 0; i < len; i++){
		g = (g+i)%255;
		b = (b+i)%255;
		if(line[i] == ALIVE){
			fprintf(stderr, "\x1b[38;2;%d;%d;%dm%s\x1b[0m",r,g,b, PLAYER);
		}
		else{
			fprintf(stderr, "%c", BACKGROUND);
		}
	}
	printf("\n");
}
//Rules to follow;
void rule(unsigned int* line, int len){
	int count = 0, left_count = 0, surrondings = 0;
	unsigned int* new_line = Init_line(len);
	for(int i = 1; i < len; i++){
		count = 0;
		left_count = 0;
		for(int k = -1; k <= 1 ;k+=2){
			surrondings = (i+k);
			if(line[surrondings] == ALIVE) count++;
		}
		if(line[i] == DEAD && line[i-1] == ALIVE) left_count = 1;
		if(line[i] == ALIVE){
			switch(count){
				case 0: 
					new_line[i] = ALIVE;
					break;

				case 1: 
					new_line[i] = ALIVE;
					break;
				case 2: 
					new_line[i] = DEAD;
					break;
			}
		}
		else{
			switch(count){
				case 0:
					break;
				case 1:
					if(!left_count){
						new_line[i] = ALIVE;	
					}
					break;
				case 2:
					new_line[i] = ALIVE;
					break;
			}
		}

	}

	for(int i = 0; i < len; i++){
		line[i] = new_line[i];
	}		
}


void rule2(unsigned int* line, int len){
        int count = 0, surrondings = 0;
	unsigned int* new_line = Init_line(len);
        for(int i = 1; i < len; i++){
                count = 0;
                for(int k = -1; k <= 1 ;k+=2){
                        surrondings = (i+k);
                        if(line[surrondings] == ALIVE) count++;
                }
                if(line[i] == ALIVE){
                        switch(count){
                                case 0:
                                        new_line[i] = ALIVE;
                                        break;

                                case 1:
                                        new_line[i] = DEAD;
                                        break;
                                case 2:
                                        new_line[i] = DEAD;
                                        break;
                        }
                }
                else{
                        switch(count){
                                case 0:
					new_line[i] = ALIVE;
                                        break;
                                case 1:
                                        new_line[i] = ALIVE;
                                        break;
                                case 2:
                                        new_line[i] = ALIVE;
                                        break;
                        }
                }

        }

        for(int i = 0; i < len; i++){
                line[i] = new_line[i];
        }
}
//based on the logical operation AND between the two neighbors
void rule3(unsigned int* line, int len){
        int count = 0, surrondings = 0;
        unsigned int* new_line = Init_line(len);
        for(int i = 1; i < len; i++){
                count = 0;
                for(int k = -1; k <= 1 ;k+=2){
                        surrondings = (i+k);
                        if(line[surrondings] == ALIVE) count++;
                }
                if(line[i] == ALIVE){
                        switch(count){
                                case 0://010
                                        new_line[i] = DEAD;
                                        break;

                                case 1://110-011
                                        new_line[i] = DEAD;
                                        break;
                                case 2://111
                                        new_line[i] = ALIVE;
                                        break;
                        }
                }
                else{
                        switch(count){
                                case 0://000
                                        new_line[i] = DEAD;
                                        break;
                                case 1://100-001
                                        new_line[i] = DEAD;
                                        break;
                                case 2://101
                                        new_line[i] = ALIVE;
                                        break;
                        }
                }

        }

        for(int i = 0; i < len; i++){
                line[i] = new_line[i];
        }
}
//rule3 results OR'd with the center cell
void rule4(unsigned int* line, int len){
        int count = 0, surrondings = 0;
        unsigned int* new_line = Init_line(len);
        for(int i = 1; i < len; i++){
                count = 0;
                for(int k = -1; k <= 1 ;k+=2){
                        surrondings = (i+k);
                        if(line[surrondings] == ALIVE) count++;
                }
                if(line[i] == ALIVE){
                        switch(count){
                                case 0://010
                                        new_line[i] = ALIVE;
                                        break;

                                case 1://110-011
                                        new_line[i] = ALIVE;
                                        break;
                                case 2://111
                                        new_line[i] = ALIVE;
                                        break;
                        }
                }
                else{
                        switch(count){
                                case 0://000
                                        new_line[i] = DEAD;
                                        break;
                                case 1://100-001
                                        new_line[i] = DEAD;
                                        break;
                                case 2://101
                                        new_line[i] = ALIVE;
                                        break;
                        }
                }

        }

        for(int i = 0; i < len; i++){
                line[i] = new_line[i];
        }
}
//Survivors: the center can be considered the leader, without it, the other dies.
//in the case of 000 new survivors emerge
void rule5(unsigned int* line, int len){
        int count = 0, surrondings = 0;
        unsigned int* new_line = Init_line(len);
        for(int i = 1; i < len; i++){
                count = 0;
                for(int k = -1; k <= 1 ;k+=2){
                        surrondings = (i+k);
                        if(line[surrondings] == ALIVE) count++;
                }
                if(line[i] == ALIVE){
                        switch(count){
                                case 0://010
                                        new_line[i] = DEAD;
                                        break;

                                case 1://110-011
                                        new_line[i] = ALIVE;
                                        break;
                                case 2://111
                                        new_line[i] = ALIVE;
                                        break;
                        }
                }
                else{
                        switch(count){
                                case 0://000
                                        new_line[i] = ALIVE;
                                        break;
                                case 1://100-001
                                        new_line[i] = DEAD;
                                        break;
                                case 2://101
                                        new_line[i] = DEAD;
                                        break;
                        }
                }

        }

        for(int i = 0; i < len; i++){
                line[i] = new_line[i];
        }
}
//Calculating 2pixels at a time instead of 3
void rule6(unsigned int* line, int len){
        int count = 0, surrondings = 0;
        unsigned int* new_line = Init_line(len);
        for(int i = 1; i < len; i++){
                count = 0;
                for(int k = -1; k <= 0 ;k++){
                        surrondings = (i+k);
                        if(line[surrondings] == ALIVE) count++;
                }
                if(line[i] == ALIVE){
                        switch(count){
                                case 0://01
                                        new_line[i] = ALIVE;
                                        break;

                                case 1://11
                                        new_line[i] = DEAD;
                                        break;
                        }
                }
                else{
                        switch(count){
                                case 0://00
                                        new_line[i] = DEAD;
                                        break;
                                case 1://01
                                        new_line[i] = ALIVE;
                                        break;
                        }
                }

        }

        for(int i = 0; i < len; i++){
                line[i] = new_line[i];
        }
}

void rule7(unsigned int* line, int len){
        int count = 0, surrondings = 0;
        unsigned int* new_line = Init_line(len);
        for(int i = 1; i < len; i++){
                count = 0;
                for(int k = -1; k <= 1 ;k+=2){
                        surrondings = (i+k);
                        if(line[surrondings] == ALIVE) count++;
                }
                if(line[i] == ALIVE){
                        switch(count){
                                case 0://010
                                        new_line[i] = ALIVE;
                                        break;

                                case 1://110-011
                                        new_line[i] = ALIVE;
                                        break;
                                case 2://111
                                        new_line[i] = ALIVE;
                                        break;
                        }
                }
                else{
                        switch(count){
                                case 0://000
                                        new_line[i] = DEAD;
                                        break;
                                case 1://100-001
                                        new_line[i] = DEAD;
                                        break;
                                case 2://101
                                        new_line[i] = ALIVE;
                                        break;
                        }
                }

        }

        for(int i = 0; i < len; i++){
                line[i] = new_line[i];
        }
}

int main(){

	//Getting terminal size using ioctl
	struct termsize tSize = getTsize();
	int term_w = tSize.cols;
	//Getting a randow number to generate a random set of alive cells;
	srand(time(0));

	unsigned int *line = Init_line(term_w);
	for(int i = 0; i < term_w; i++){
		line[i] = rand() % 2;
	}
		
	printf("\x1b[?25l");
	system("clear");
	while(1){
		printLine(line, term_w);
		rule(line, term_w);
		usleep(0.1*pow(10, 6));//0.1s is good
	}
	printf("\x1b[?25h");
	free(line);
	return 0;
}
