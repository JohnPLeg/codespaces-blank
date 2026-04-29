#include <ncurses.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define QUEUE_SIZE 3

typedef struct {
    int buf[QUEUE_SIZE];
    int head, tail, count;
} DirQueue;

void init_background(int ROWS, int COLS, int background[ROWS][COLS]);
void draw_border(int ROWS, int COLS, int background[ROWS][COLS]);
void move_snake(int ROWS, int COLS, int background[ROWS][COLS], int direction, int *snake_x, int *snake_y, int snake_len);
void queue_push(DirQueue *q, int dir);
int queue_pop(DirQueue *q);

int main() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    int ROWS = w.ws_row;
    int COLS = w.ws_col / 2;
    int background[ROWS][COLS];
    int previous_direction = -1;
    int direction = -1;
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
    nodelay(stdscr, TRUE);

    init_pair(1, COLOR_RED, COLOR_GREEN);
    init_pair(2, -1, -1);
    bkgd(COLOR_PAIR(2));

    draw_border(ROWS, COLS, background);

    for (int i = 0; i < snake_len; i++) {
        background[snake_y[i]][snake_x[i]] = 2;
        attron(COLOR_PAIR(1));
        mvaddstr(snake_y[i], snake_x[i] * 2, "  ");
        attroff(COLOR_PAIR(1));
    }

    refresh();

    DirQueue q = {0};

    while (1) {
        int c;
        while ((c = getch()) != ERR) {
            int new_dir = -1;
            switch (c) {
                case 'q':                 endwin(); return 0;
                case KEY_UP:    case 'w': new_dir = 0; break;
                case KEY_DOWN:  case 's': new_dir = 1; break;
                case KEY_LEFT:  case 'a': new_dir = 2; break;
                case KEY_RIGHT: case 'd': new_dir = 3; break;
            }
            if (new_dir != -1)
                queue_push(&q, new_dir);
        }

        int new_dir = queue_pop(&q);
        if (new_dir != -1) {
            int is_reverse = (previous_direction == 0 && new_dir == 1) ||
                            (previous_direction == 1 && new_dir == 0) ||
                            (previous_direction == 2 && new_dir == 3) ||
                            (previous_direction == 3 && new_dir == 2);
            if (!is_reverse)
                direction = new_dir;
        }

        move_snake(ROWS, COLS, background, direction, snake_x, snake_y, snake_len);
        previous_direction = direction;

        napms(150);
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
    if (direction == -1) return;
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

    if (background[snake_y[0]][snake_x[0]] != 0) {
        endwin();
        printf("Game Over!\n");
        exit(0);
    }

    background[snake_y[0]][snake_x[0]] = 2;
    attron(COLOR_PAIR(1));
    mvaddstr(snake_y[0], snake_x[0] * 2, "  ");
    attroff(COLOR_PAIR(1));

    refresh();
}

void queue_push(DirQueue *q, int dir) {
    if (q->count < QUEUE_SIZE) {
        q->buf[q->tail] = dir;
        q->tail = (q->tail + 1) % QUEUE_SIZE;
        q->count++;
    }
}

int queue_pop(DirQueue *q) {
    if (q->count == 0) return -1;
    int dir = q->buf[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count--;
    return dir;
}