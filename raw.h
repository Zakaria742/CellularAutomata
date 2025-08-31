#ifndef _RAW_H
#define _RAW_H

#include <termios.h>
#include <errno.h>
void die(const char *c){
        perror(c);
        exit(-1);
}

struct termios origin;
void disable_raw(){
        if(tcsetattr(STDOUT_FILENO, TCSAFLUSH, &origin) == -1){
                die("tcsetattr");
        }
}
void raw(){
        tcgetattr(STDIN_FILENO, &origin);
        atexit(disable_raw);
        struct termios raw = origin;
        raw.c_iflag &= ~(IGNBRK | ISTRIP | IXOFF);
        raw.c_lflag &= ~(ISIG | ICANON | ECHO | IEXTEN);
        raw.c_cc[VTIME] = 0;
        raw.c_cc[VMIN] = 0;
        tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}
#endif
