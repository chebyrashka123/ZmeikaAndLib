#include <curses.h>   // управление экраном (PDCurses)
#include <stdlib.h>   // rand, srand
#include <time.h>     // time

#define WIDTH 40
#define HEIGHT 20
#define INIT_LEN 3

typedef struct { int x, y; } Point;

// глобальные данные игры
Point snake[WIDTH * HEIGHT];
int snake_len = INIT_LEN;
Point food;
int dx = 1, dy = 0;   // текущее направление (вправо)
int game_over = 0;
int score = 0;

// начальная инициализация: положение змейки и первой еды
void init_game() {
    for (int i = 0; i < snake_len; i++) {
        snake[i].x = WIDTH / 2 - i;
        snake[i].y = HEIGHT / 2;
    }
    srand(time(NULL));
    do {
        food.x = rand() % WIDTH;
        food.y = rand() % HEIGHT;
    } while (food.x == snake[0].x && food.y == snake[0].y); // еда не на голове
}

// отрисовка всего на экране
void draw() {
    clear();
    // границы поля
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
        mvprintw(snake[i].y + 1, snake[i].x + 1, i == 0 ? "O" : "o");
    }
    mvprintw(HEIGHT + 3, 1, "Score: %d", score);
    refresh();
}

// обновление состояния: движение, столкновения, поедание
void update() {
    Point new_head = { snake[0].x + dx, snake[0].y + dy };
    // столкновение со стеной
    if (new_head.x < 0 || new_head.x >= WIDTH || new_head.y < 0 || new_head.y >= HEIGHT) {
        game_over = 1;
        return;
    }
    int ate = (new_head.x == food.x && new_head.y == food.y);
    // сдвиг тела (каждый сегмент на место предыдущего)
    for (int i = snake_len - 1; i > 0; i--) snake[i] = snake[i - 1];
    snake[0] = new_head;
    // если съели еду — увеличиваем длину и счёт, генерируем новую еду
    if (ate) {
        snake_len++;
        score++;
        int valid = 0;
        while (!valid) {
            valid = 1;
            food.x = rand() % WIDTH;
            food.y = rand() % HEIGHT;
            for (int i = 0; i < snake_len; i++)
                if (snake[i].x == food.x && snake[i].y == food.y) valid = 0;
        }
    }
    // проверка столкновения головы с телом
    for (int i = 1; i < snake_len; i++)
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) game_over = 1;
}

int main() {
    // настройка curses
    initscr(); cbreak(); noecho(); keypad(stdscr, TRUE); nodelay(stdscr, TRUE); curs_set(0);
    init_game();
    while (!game_over) {
        int ch = getch();
        // управление стрелками, запрет разворота на 180 градусов
        switch (ch) {
            case KEY_UP:    if (dy == 0) { dx = 0; dy = -1; } break;
            case KEY_DOWN:  if (dy == 0) { dx = 0; dy = 1; } break;
            case KEY_LEFT:  if (dx == 0) { dx = -1; dy = 0; } break;
            case KEY_RIGHT: if (dx == 0) { dx = 1; dy = 0; } break;
            case 'q':       game_over = 1; break;
        }
        update();
        draw();
        napms(150); // задержка для регулировки скорости
    }
    // завершение игры, вывод результата
    clear();
    mvprintw(HEIGHT / 2, WIDTH / 2 - 5, "GAME OVER!");
    mvprintw(HEIGHT / 2 + 1, WIDTH / 2 - 7, "Score: %d", score);
    refresh();
    nodelay(stdscr, FALSE);
    getch();
    endwin();
    return 0;
}
