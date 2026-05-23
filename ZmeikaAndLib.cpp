#include <curses.h>
#include <stdlib.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20
#define INIT_LEN 3

typedef struct {
    int x, y;
} Point;

Point snake[WIDTH * HEIGHT];
int snake_len = INIT_LEN;
Point food;
int dx = 1, dy = 0;
int game_over = 0;
int score = 0;

void init_game() {
    for (int i = 0; i < snake_len; i++) {
        snake[i].x = WIDTH / 2 - i;
        snake[i].y = HEIGHT / 2;
    }

    srand(time(NULL));
    do {
        food.x = rand() % WIDTH;
        food.y = rand() % HEIGHT;
    } while (food.x == snake[0].x && food.y == snake[0].y);
}

void draw() {
    clear();

    // рамка
    for (int i = 0; i <= WIDTH + 1; i++) {
        mvprintw(0, i, "#");
        mvprintw(HEIGHT + 1, i, "#");
    }
    for (int i = 0; i <= HEIGHT + 1; i++) {
        mvprintw(i, 0, "#");
        mvprintw(i, WIDTH + 1, "#");
    }

    // еда
    mvprintw(food.y + 1, food.x + 1, "@");

    // змейка
    for (int i = 0; i < snake_len; i++) {
        if (i == 0)
            mvprintw(snake[i].y + 1, snake[i].x + 1, "O");
        else
            mvprintw(snake[i].y + 1, snake[i].x + 1, "o");
    }

    // счёт
    mvprintw(HEIGHT + 3, 1, "Score: %d", score);
    mvprintw(HEIGHT + 4, 1, "Press 'q' to quit");

    refresh();
}

void update() {
    Point new_head = { snake[0].x + dx, snake[0].y + dy };

    // стены
    if (new_head.x < 0 || new_head.x >= WIDTH || new_head.y < 0 || new_head.y >= HEIGHT) {
        game_over = 1;
        return;
    }

    int ate_food = (new_head.x == food.x && new_head.y == food.y);

    for (int i = snake_len - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
    }
    snake[0] = new_head;

    if (ate_food) {
        snake_len++;
        score++;
        int valid = 0;
        while (!valid) {
            valid = 1;
            food.x = rand() % WIDTH;
            food.y = rand() % HEIGHT;
            for (int i = 0; i < snake_len; i++) {
                if (snake[i].x == food.x && snake[i].y == food.y) {
                    valid = 0;
                    break;
                }
            }
        }
    }

    // столкновение с собой
    for (int i = 1; i < snake_len; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            game_over = 1;
            return;
        }
    }
}

int main() {
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);

    init_game();

    while (!game_over) {
        int ch = getch();
        switch (ch) {
        case KEY_UP:
            if (dy == 0) { dx = 0; dy = -1; }
            break;
        case KEY_DOWN:
            if (dy == 0) { dx = 0; dy = 1; }
            break;
        case KEY_LEFT:
            if (dx == 0) { dx = -1; dy = 0; }
            break;
        case KEY_RIGHT:
            if (dx == 0) { dx = 1; dy = 0; }
            break;
        case 'q':
            game_over = 1;
            break;
        }

        update();
        draw();

        napms(150);
    }

    clear();
    mvprintw(HEIGHT / 2, WIDTH / 2 - 5, "GAME OVER!");
    mvprintw(HEIGHT / 2 + 1, WIDTH / 2 - 7, "Final score: %d", score);
    mvprintw(HEIGHT / 2 + 2, WIDTH / 2 - 10, "Press any key to exit");
    refresh();
    nodelay(stdscr, FALSE);
    getch();

    endwin();
    return 0;
}