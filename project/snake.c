#include <ncurses.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>


int main() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int ROWS w.ws_row;
    int COLS w.ws_col;
    int background[ROWS][COLS];

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (i == 0 || i == ROWS - 1 || j == 0 || j == COLS - 1) {
                background[i][j] = 1;
            } else {
                background[i][j] = 0;
            }
        }
    }


    initscr(); 
    start_color();
    cbreak();
    noecho();
    curs_set(0);

    init_pair(1, COLOR_RED, COLOR_GREEN);
    init_pair(2, COLOR_RED, COLOR_BLACK);

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (background[i][j] == 1) {
                attron(COLOR_PAIR(1));
                addch(' ');
                attroff(COLOR_PAIR(1));
            } else {
                attron(COLOR_PAIR(2));
                addch(' ');
                attroff(COLOR_PAIR(2));
            }
        }
    }

    refresh();              // Update the screen
    getch();                // Wait for user input
    endwin();               // End ncurses mode
    return 0;
}
