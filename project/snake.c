#include <ncurses.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>

void init_background(int ROWS, int COLS, int background[ROWS][COLS]);
void draw_border(int ROWS, int COLS, int background[ROWS][COLS]);
void move_snake(int ROWS, int COLS, int background[ROWS][COLS], int direction, int *snake_x, int *snake_y, int snake_len);

int main() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int ROWS = w.ws_row;
    int COLS = w.ws_col / 2;
    int background[ROWS][COLS];
    int direction = 0;
    int snake_x[100];
    int snake_y[100];
    int snake_len = 3;

    for (int i = 0; i < snake_len; i++) {
        snake_x[i] = COLS / 2;
        snake_y[i] = ROWS / 2 + i;
    }

    init_background(ROWS, COLS, background);

    initscr();
    use_default_colors();
    start_color();
    cbreak();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(150);

    init_pair(1, COLOR_RED, COLOR_GREEN);
    init_pair(2, -1, -1);
    bkgd(COLOR_PAIR(2));

    draw_border(ROWS, COLS, background);
    refresh();

    while (1) {
        int c = getch();
        if (c != ERR) {
            switch (c) {
                case KEY_UP:    direction = 0; break;
                case KEY_DOWN:  direction = 1; break;
                case KEY_LEFT:  direction = 2; break;
                case KEY_RIGHT: direction = 3; break;
                case 'q':       endwin(); return 0;
            }
        }
        move_snake(ROWS, COLS, background, direction, snake_x, snake_y, snake_len);
    }

    endwin();
    return 0;
}

void init_background(int ROWS, int COLS, int background[ROWS][COLS]) {
    int center_y = ROWS / 2;
    int center_x = COLS / 2;

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (i == 0 || i == ROWS - 1 || j == 0 || j == COLS - 1) {
                background[i][j] = 1;
            } else {
                background[i][j] = 0;
            }
        }
    }

    background[center_y][center_x] = 2;
    background[center_y - 1][center_x] = 2;
    background[center_y - 2][center_x] = 2;
}

void draw_border(int ROWS, int COLS, int background[ROWS][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            if (background[i][j] == 1) {
                attron(COLOR_PAIR(1));
                mvaddstr(i, j * 2, "  ");
                attroff(COLOR_PAIR(1));
            }
        }
    }
}

void move_snake(int ROWS, int COLS, int background[ROWS][COLS], int direction, int *snake_x, int *snake_y, int snake_len) {
    int old_tail_x = snake_x[snake_len - 1];
    int old_tail_y = snake_y[snake_len - 1];
    background[old_tail_y][old_tail_x] = 0;
    attron(COLOR_PAIR(2));
    mvaddstr(old_tail_y, old_tail_x * 2, "  ");
    attroff(COLOR_PAIR(2));

    for (int i = snake_len - 1; i > 0; i--) {
        snake_x[i] = snake_x[i - 1];
        snake_y[i] = snake_y[i - 1];
    }

    switch (direction) {
        case 0: snake_y[0]--; break;
        case 1: snake_y[0]++; break;
        case 2: snake_x[0]--; break;
        case 3: snake_x[0]++; break;
    }

    background[snake_y[0]][snake_x[0]] = 2;
    attron(COLOR_PAIR(1));
    mvaddstr(snake_y[0], snake_x[0] * 2, "  ");
    attroff(COLOR_PAIR(1));

    refresh();
}