#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>

    #define sleep() Sleep(100)

    #define keyPressed kbhit
    #define keyInput getch
    #define init()

    #define background() printf("%c ", 250) // ·
    #define fruitIcon() printf("%c ", 220) // ▄

#else
    #include <termios.h>
    #include <unistd.h>
    
    #define sleep() usleep(100000)
    #define keyInput getchar
    #define init() enableRaw()

    #define background() printf("· ")
    #define fruitIcon() printf("▄ ")

    struct termios old;

    void disableRaw() {
        tcsetattr(STDIN_FILENO, TCSANOW, &old);
    }

    void enableRaw() {
        tcgetattr(STDIN_FILENO, &old);
        atexit(disableRaw);
        
        struct termios new = old;

        new.c_lflag &= ~(ECHO | ICANON);

        tcsetattr(STDIN_FILENO, TCSANOW, &new);                
    }

    int keyPressed() {
        struct timeval tv;
        fd_set fds;
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);

        return FD_ISSET(STDIN_FILENO, &fds);
    }

#endif

#define KEY_DEBUG 39
#define HEIGHT 15
#define WIDTH 20
#define MAX_H 20
#define MAX_TAIL 100


typedef struct vector {
    int x;
    int y;
} Vector;

typedef struct tail {
    int size;
    int x[MAX_TAIL];
    int y[MAX_TAIL];
} Tail;

typedef struct player { 
    Vector input;
    Vector pos;
    Tail tail;
} Player;


Player player = {0};
Vector fruit = {0};


int debug = 0;
int quit;
int gameover;


// general functions
void clearScreen() {
    printf("\033[H\033[J");
}

// player functions
void playerInput() {
    if (keyPressed()) {
        char in = tolower(keyInput());

        switch (in) {
        case 'w':
        case 'a':
        case 's':
        case 'd':
            player.input.y = (in == 's') - (in == 'w');
            player.input.x = (in == 'd') - (in == 'a');
            break;
        case 'x':
            quit = 1;
            break;
        case KEY_DEBUG:
            if (debug == 1) {
                debug = 0;
                clearScreen();
            } else debug = 1;
            break;
        case '=':
            if (player.tail.size < MAX_TAIL) {
                player.tail.size++;
                player.tail.x[player.tail.size-1] = player.pos.x;
                player.tail.y[player.tail.size-1] = player.pos.y;
            }
            break;
        default:   
            break;
        }
    }
}

void tailLogic() {
    for (int i = player.tail.size-1; i>0; i--) {
        player.tail.x[i] = player.tail.x[i-1];
        player.tail.y[i] = player.tail.y[i-1];
    }
    player.tail.x[0] = player.pos.x;
    player.tail.y[0] = player.pos.y;
}

int tailCollision(Vector player, Tail tail) {
    for (int i = 0; i<tail.size; i++) {
        if (player.x == tail.x[i] && player.y == tail.y[i]) {
            return 1;
        }
    }
    return 0;
}

// fruit functions
void fruitRand() {
    fruit.x = rand() % WIDTH;
    fruit.y = rand() % HEIGHT;
}

int fruitCollision(Vector player, Vector fruit) {
    return player.x == fruit.x && player.y == fruit.y;
}

// debugger functions
void debugger() {
    printf("fx: %d | fy: %d \n", fruit.x, fruit.y);
    printf("inX: %d | inY: %d \n", player.input.x, player.input.y);
    printf("hx: %d | hy: %d \n", player.pos.x, player.pos.y);
    printf("st: %d \n", player.tail.size);
    for (int i = 0; i < player.tail.size; i++) {
        printf("tx[%d]: %d | ", i, player.tail.x[i]);
    } 
    printf("\n");
    for (int i = 0; i < player.tail.size; i++) {
        printf("ty[%d]: %d | ", i, player.tail.y[i]);
    } 
    printf("\n");
}

// draw and update functions
void update() {
    // tail logic
    if (player.tail.size > 0) {
        tailLogic();
    }
    // movement
    player.pos.x += player.input.x;
    player.pos.y += player.input.y;

    // wall position reset
    player.pos.x = (player.pos.x + WIDTH) % WIDTH;
    player.pos.y = (player.pos.y + HEIGHT) % HEIGHT;
    
    // player collide with tail
    if (tailCollision(player.pos, player.tail)) {
        gameover = 1;
    }
    // player collide with fruit
    if (fruitCollision(player.pos, fruit) && player.tail.size < MAX_TAIL) {
        player.tail.size++;
        player.tail.x[player.tail.size-1] = player.pos.x;
        player.tail.y[player.tail.size-1] = player.pos.y;

        fruitRand();
    }
}

void drawMap() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (player.pos.x == j && player.pos.y == i) { // print player
                printf("$ ");
            }
            else if (fruit.x == j && fruit.y == i) { // print fruit
                fruitIcon();
            }
            else {
                int drwTail = 0; // draw tail
                for (int k = 0; k < player.tail.size; k++) {
                    if (player.tail.x[k] == j && player.tail.y[k] == i) {
                        printf("$ ");
                        drwTail = 1;
                    }
                }
                if (!drwTail) {
                    background(); 
                }
            }
        } 
        printf("\n");
    }

    printf("SCORE: %d\n", player.tail.size);
    if (debug==1) { // print debugger
        debugger();
    }
    printf("\33[%iA", HEIGHT+MAX_H); // go back h+n lines up
}

void gameReset() {
    clearScreen();
    fruitRand();
    quit = gameover = 0;
    // the player will start moving to the right
    player.input.x = 1; 
    player.input.y = 0;
    // the init position will be in the middle
    player.pos.x = WIDTH/2;
    player.pos.y = HEIGHT/2;
    player.tail.size = 0;
}

void menu() {
    for (int i = 0; i < HEIGHT; i++) {
        for (int j = 0; j < WIDTH; j++) {
            if (i == HEIGHT/2) {
                printf("        PRESS ANY KEY TO PLAY       ");
                break;
            }
            else {
                background(); 
            }
        }
        printf("\n");
    }
    printf("\n\nCONTROLS: W-A-S-D\n\n\n");
    printf("[X] QUIT");
    if (toupper(keyInput()) == 'X') {
        quit = 1;
    }
    clearScreen();
}

void gameOverMenu() {
    clearScreen();
    printf("************************************\n");
    printf("              GAME OVER             \n");
    printf("************************************\n");
    printf("TOTAL SCORE: %d\n", player.tail.size);
    keyInput();
}

int main() {
    printf("\33[?25l");
    init();
    
    while (!quit) {
        gameReset();
        menu();

        while (!gameover && !quit) {
            playerInput();
            drawMap();
            update();
            sleep();
        }
        
        if (gameover) {
            gameOverMenu();
        }
    }

    printf("\33[?25h");
    return 0;
}