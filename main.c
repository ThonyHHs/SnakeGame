#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#ifdef _WIN32
    #include <windows.h>
    #include <conio.h>

    #define sleep Sleep

    #define keyPressed kbhit
    #define keyInput getch
    #define clear "cls"

#else
    #include <termios.h>
    #include <unistd.h>
    
    #define sleep usleep
    #define keyInput getchar
    #define clear "clear"

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


#define HEIGHT 15
#define WIDTH 20
#define MAX_H 20
#define MAX_TAIL 100


typedef struct input{
    int x;
    int y;
} Input;

typedef struct position{
    int x;
    int y;
} Position;

typedef struct tail{
    int size;
    int x[MAX_TAIL];
    int y[MAX_TAIL];
} Tail;


typedef struct player{ 
    Input input;
    Position pos;
    Tail tail;
} Player;

typedef struct fruit{
    int x;
    int y;
} Fruit;


Player player = {0};
Fruit fruit = {0};


int debbug = 0;
int quit;
int gameover;


void fruitRand(){
    fruit.x = rand() % WIDTH;
    fruit.y = rand() % HEIGHT;
}

void tailLogic(){
    for(int i = player.tail.size-1; i>0; i--) {
        player.tail.x[i] = player.tail.x[i-1];
        player.tail.y[i] = player.tail.y[i-1];
    }
    player.tail.x[0] = player.pos.x;
    player.tail.y[0] = player.pos.y;
}

void playerInput(){
    if(keyPressed()) {
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
        case 39:
            if(debbug == 1){
                debbug = 0;
                system(clear);
            } else debbug = 1;
        case '+':
        case '=':
            player.tail.size++;
            player.tail.x[player.tail.size] = player.tail.y[player.tail.size] = -1;
        default:   
            break;
        }
    }
}

void debugger(){
    printf("inX: %d | inY: %d \n", player.input.x, player.input.y);
    printf("fx: %d | fy: %d \n", fruit.x, fruit.y);
    printf("hx: %d | hy: %d \n", player.pos.x, player.pos.y);
    printf("nt: %d \n", player.tail.size);
    for(int i = 0; i < player.tail.size; i++){
        printf("tx[%d]: %d | ", i, player.tail.x[i]);
    }printf("\n");
    for(int i = 0; i < player.tail.size; i++){
        printf("ty[%d]: %d | ", i, player.tail.y[i]);
    }printf("\n");
}

void drawMap(){
    for(int i = 0; i < HEIGHT; i++){
        for(int j = 0; j < WIDTH; j++){
            if(player.pos.x == j && player.pos.y == i){
                printf("$ ");
            }
            else if(fruit.x == j && fruit.y == i){
                printf("%c ", 220); // ▄
            }
            else{
                int drwTail = 0;
                for(int k = 0; k < player.tail.size; k++){
                    if(player.tail.x[k] == j && player.tail.y[k] == i){
                        printf("$ ");
                        drwTail = 1;
                    }
                    if(player.tail.x[k] == player.pos.x && player.tail.y[k] == player.pos.y){
                        gameover = 1;
                    }
                }
                if(!drwTail)
                    printf("%c ",250); // ·
            }
            // when player get the fruit
            if(player.pos.x == fruit.x && player.pos.y == fruit.y && player.tail.size < 100){
                player.tail.size++;
                fruitRand();
            }
        }printf("\n");
    }

    tailLogic();

    player.pos.x += player.input.x;
    player.pos.y += player.input.y;

    // wall position reset
    if(player.pos.x > WIDTH-1) player.pos.x = 0; else if(player.pos.x < 0) player.pos.x = WIDTH-1;
    if(player.pos.y > HEIGHT-1) player.pos.y = 0; else if(player.pos.y < 0) player.pos.y = HEIGHT-1;
    printf("SCORE: %d\n", player.tail.size);
    
    if(debbug==1){
        debugger();
    }
    printf("\33[%iA", HEIGHT+MAX_H); // go back h+n lines up
}

void gameReset(){
    system(clear);
    quit = gameover = 0;
    player.input.x = 1; // the player will start moving to the right
    player.input.y = 0;
    player.pos.x = WIDTH/2;
    player.pos.y = HEIGHT/2;
    player.tail.size = 0;
}

int main(){
    printf("\33[?25l");
    
    while(!quit){

        gameReset();

        for(int i = 0; i < HEIGHT; i++){
            for(int j = 0; j < WIDTH; j++){
                if(i == HEIGHT/2){
                    printf("        PRESS ANY KEY TO PLAY       ");
                    break;
                }else printf("%c ", 250); // ·
            }printf("\n");
        }
        printf("\n\nCONTROLS: W-A-S-D\n\n\n");
        printf("[X] QUIT");
        if(toupper(keyInput()) == 'X'){
            quit = 1;
        }
        system(clear);

        fruitRand();

        while(!gameover && !quit){
            playerInput();
            drawMap();
            sleep(90);
        }
        
        if(gameover){
            system(clear);
            printf("************************************\n");
            printf("              GAME OVER             \n");
            printf("************************************\n");
            printf("TOTAL SCORE: %d\n", player.tail.size);
            system("pause");
        }
    }

    printf("\33[?25h");
    return 0;
}