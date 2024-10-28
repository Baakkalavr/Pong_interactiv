#include <ncurses.h>  //для управления вводом и выводом на терминал
#include <stdio.h>
#include <unistd.h>  //для функции usleep

#define WIDTH 80
#define HEIGH 25
#define RACKET_HEIGHT 3
#define BALL_CHAR "*"
#define BALL_STEP_X 1
#define BALL_STEP_Y 1

void game_field(const int* X_L_racket_coordinate, const int* Y_L_racket_coordinate,
                const int* X_R_racket_coordinate, const int* Y_R_racket_coordinate,
                const int* X_ball_coordinate, const int* Y_ball_coordinate, int score[]);

void move_racket(int* Y_L_racket_coordinate, int* Y_R_racket_coordinate);
void move_ball(int* X_ball_coordinate, int* Y_ball_coordinate, int* X_ball_vector, int* Y_ball_vector,
               const int* X_L_racket_coordinate, const int* Y_L_racket_coordinate,
               const int* X_R_racket_coordinate, const int* Y_R_racket_coordinate, int score[]);

int main() {
    int X_L_racket_coordinate = 8;  // начальное положение левой ракетки по оси X
    int Y_L_racket_coordinate = 10;  // начальное положение левой ракетки по оси Y
    int X_R_racket_coordinate = WIDTH - 9;  // начальное положение правой ракетки по оси X
    int Y_R_racket_coordinate = (HEIGH / 2) - 2;  // начальное положение правой ракетки по оси Y
    int X_ball_coordinate = (WIDTH / 2) - 1;  // начальное положение мяча по оси X
    int Y_ball_coordinate = (HEIGH / 2) - 1;  // начальное положение мяча по оси Y

    int score[2] = {0, 0};  // массив игроков {левый, правый}

    int X_ball_vector = 1;  // направление движения мяча (вектор) по оси X
    int Y_ball_vector = 1;  // направление движения мяча (вектор) по оси Y

    initscr();  // initiate stdscr WINDOW (ncurses do not use stdout); инициировать ввод
    cbreak();  // explicitly specify to react to ctrl+C; реагирование (выход из игры) на ctrl+C
    noecho();  // specify no echo pushed buttons; не выводить нажатие на кнопки
    nodelay(stdscr, TRUE);  // specify non-blocking getch; не блокировать getch

    game_field(&X_L_racket_coordinate, &Y_L_racket_coordinate, &X_R_racket_coordinate, &Y_R_racket_coordinate,
               &X_ball_coordinate, &Y_ball_coordinate, score);
    do {
        move_racket(&Y_L_racket_coordinate, &Y_R_racket_coordinate);
        move_ball(&X_ball_coordinate, &Y_ball_coordinate, &X_ball_vector, &Y_ball_vector,
                  &X_L_racket_coordinate, &Y_L_racket_coordinate, &X_R_racket_coordinate,
                  &Y_R_racket_coordinate, score);
        game_field(&X_L_racket_coordinate, &Y_L_racket_coordinate, &X_R_racket_coordinate,
                   &Y_R_racket_coordinate, &X_ball_coordinate, &Y_ball_coordinate, score);
        usleep(100000);  //раскадровка приятная глазу
        if (score[0] == 21) {
            endwin();
            printf("LEFT IS WIN!!!!\n");
            return 0;
        } else if (score[1] == 21) {
            endwin();
            printf("RIGHT IS WIN!!!!\n");
            return 0;
        }
    } while (1);

    endwin();  // restore the terminal after curses activity; функция восстанавливает исходный режим терминала
    return 0;
}

void game_field(const int* X_L_racket_coordinate, const int* Y_L_racket_coordinate,
                const int* X_R_racket_coordinate, const int* Y_R_racket_coordinate,
                const int* X_ball_coordinate, const int* Y_ball_coordinate, int score[]) {
    // printf("\e[1;1H\e[2J");
    clear();  // clear stdsrc
    for (int i = 0; i < HEIGH; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == 0) {
                printw("_");  // print to stdsrc
                if (j == WIDTH - 1) {
                    printw("\n");
                }
            } else if (i == HEIGH - 1) {
                printw("¯");
                if (j == WIDTH - 1) {
                    printw("\n");
                }
            } else {
                if (j == 0) {
                    printw("|");
                } else if (j == WIDTH - 1) {
                    printw("|\n");
                } else if (j == WIDTH / 2 - 1) {
                    if (j != *X_ball_coordinate ||
                        i != *Y_ball_coordinate) {  //рисуем центральную линию поля избегая мяча
                        printw("|");
                    } else {
                        printw(BALL_CHAR);
                    }
                } else if ((*X_L_racket_coordinate == j &&
                            (i - *Y_L_racket_coordinate) >= 0 &&  // рисуем левую ракетку
                            (i - *Y_L_racket_coordinate) < RACKET_HEIGHT) ||
                           (*X_R_racket_coordinate == j &&
                            (i - *Y_R_racket_coordinate) >= 0 &&  // рисуем правую ракетку
                            (i - *Y_R_racket_coordinate) < RACKET_HEIGHT)) {
                    printw("X");
                } else if (j == *X_ball_coordinate && i == *Y_ball_coordinate) {
                    printw(BALL_CHAR);
                } else if (i == (HEIGH / 10) && j == (WIDTH / 3)) {  // счет левого игрока
                    printw("%d", score[0]);
                } else if (i == (HEIGH / 10) && j == (2 * WIDTH / 3)) {  // счет правого игрока
                    printw("%d", score[1]);
                } else {
                    if (i == (HEIGH / 10)) {
                        if (j == (WIDTH / 3) + 1 &&
                            score[0] >=
                                10) {  // IT IS FOR PREVENT LEFT '|' BE PUSHED RIGHT IF NUMBER BECOME 2-digit
                            continue;
                        }
                        if ((j == (2 * WIDTH / 3) + 1 && score[1] >= 10)) {
                            continue;
                        }
                    }
                    printw(" ");
                }
            }
        }
    }
    refresh();  // FROM man:  refresh is called, telling curses to make the user's  screen look like stdscr
}

void move_racket(int* Y_L_racket_coordinate, int* Y_R_racket_coordinate) {
    int c;
    c = getch();  // функция принимает символ и возвращает число
    switch (c) {
        case 'z':
        case 'Z':
            if (*Y_L_racket_coordinate + 3 != HEIGH - 1) {
                *Y_L_racket_coordinate += 1;
            }
            break;
        case 'a':
        case 'A':
            if (*Y_L_racket_coordinate != 1) {
                *Y_L_racket_coordinate -= 1;
            }
            break;
        case 'k':
        case 'K':
            if (*Y_R_racket_coordinate != 1) {
                *Y_R_racket_coordinate -= 1;
            }
            break;
        case 'm':
        case 'M':
            if (*Y_R_racket_coordinate + 3 != HEIGH - 1) {
                *Y_R_racket_coordinate += 1;
            }
            break;
        default:  // если все кейсы не сработали, то break
            break;
    }
}

void move_ball(int* X_ball_coordinate, int* Y_ball_coordinate, int* X_ball_vector, int* Y_ball_vector,
               const int* X_L_racket_coordinate, const int* Y_L_racket_coordinate,
               const int* X_R_racket_coordinate, const int* Y_R_racket_coordinate, int score[]) {
    if (*Y_ball_coordinate == HEIGH - 2) {
        *Y_ball_vector = -1;  // удар о нижний край - меняем вектор движения по оси Y на противоположный
                              // (далее уменьшается по Y)
    }
    if (*Y_ball_coordinate == 1) {
        *Y_ball_vector = 1;  // удар о верхний край - меняем вектор движения по оси Y на противоположный
                             // (далее увеличивается по Y)
    }
    if ((*X_ball_coordinate == *X_R_racket_coordinate - 1 &&  // удар в правую ракетку
         *Y_ball_coordinate - *Y_R_racket_coordinate >= 0 &&
         *Y_ball_coordinate - *Y_R_racket_coordinate <= 2) ||
        (*X_ball_coordinate ==
             *X_L_racket_coordinate - 1 &&  // удар в левую ракетку с левой стороны (от стены)
         *Y_ball_coordinate - *Y_L_racket_coordinate >= 0 &&
         *Y_ball_coordinate - *Y_L_racket_coordinate <= 2)) {
        *X_ball_vector = -1;  // изменяет вектор мяча по Х (далее уменьшается по Х)
    }
    if ((*X_ball_coordinate == *X_L_racket_coordinate + 1 &&  // удар в левую ракетку
         *Y_ball_coordinate - *Y_L_racket_coordinate >= 0 &&
         *Y_ball_coordinate - *Y_L_racket_coordinate <= 2) ||
        (*X_ball_coordinate ==
             *X_R_racket_coordinate + 1 &&  // удар в правую ракетку с правой стороны (от стены)
         *Y_ball_coordinate - *Y_R_racket_coordinate >= 0 &&
         *Y_ball_coordinate - *Y_R_racket_coordinate <= 2)) {
        *X_ball_vector = 1;  // меняет вектор мяча по Х (далее увеличивается по Х)
    }
    if (*X_ball_coordinate == WIDTH - 2) {
        *X_ball_vector = -1;  // удар о правый край - меняем вектор движения по оси Х на противоположный
                              // (далее уменьшается по Х)
        score[0] += 1;        // поинт левому игроку
    }
    if (*X_ball_coordinate == 1) {
        *X_ball_vector = 1;  // удар о левый край - меняем вектор движения по оси Х на противоположный (далее
                             // увеличивается по Х)
        score[1] += 1;       // поинт правому игроку
    }
    *X_ball_coordinate += *X_ball_vector * BALL_STEP_X;  // траектория движения меча по Х = вектор * шаг
    *Y_ball_coordinate += *Y_ball_vector * BALL_STEP_Y;  // траектория движения меча по Y = вектор * шаг
}