#ifdef _WIN32

#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

#include<windows.h>
#include<conio.h>



#define h 15
#define w 20
#define MAX_H 20


int debbug = 0;
int quit;
int gameover;

int pInput[2]; // movement[x,y];   values: 0 or 1
int playerPos[2]; // pos[x, y]
int tailx[100];
int taily[100];
int ntail;

int fruitPos[2]; // pos[x, y]


void fruitRand(){
    fruitPos[0] = rand() % w;
    fruitPos[1] = rand() % h;
}

void tailLogic(){
    for(int i = ntail-1; i>0; i--){
         tailx[i] = tailx[i-1];
         taily[i] = taily[i-1];
    }
    tailx[0] = playerPos[0];
    taily[0] = playerPos[1];
}

void playerInput(){
    if(kbhit()){
        char in = tolower(getch());
        if(in=='w' || in=='a' || in=='s' || in=='d'){
            pInput[1] = (in=='s')-(in=='w');
            pInput[0] = (in=='d')-(in=='a');
        }
        else if(in == 'x'){
            quit = 1;
        }

        else if(in==39){
            if(debbug==1){
                debbug=0;
                system("cls");
            }else debbug=1;
        }

        else if(in=='+'||in=='='){
            ntail++;
        }
    }
}

void debugger(){
    printf("inX: %d | inY: %d \n", pInput[0], pInput[1]);
    printf("fx: %d | fy: %d \n", fruitPos[0], fruitPos[1]);
    printf("hx: %d | hy: %d \n", playerPos[0], playerPos[1]);
    printf("nt: %d \n", ntail);
    for(int i=0; i<ntail; i++){
        printf("tx[%d]: %d | ", i, tailx[i]);
    }printf("\n");
    for(int i=0; i<ntail; i++){
        printf("tx[%d]: %d | ", i, taily[i]);
    }printf("\n");
}

void drawMap(){
    // int dbhx = playerPos[0], dbhy = playerPos[1];
    // int dbtx = tailx[0], dbty = taily[0];


    for(int i=0; i<h; i++){
        for(int j=0; j<w; j++){
            if(playerPos[0]==j && playerPos[1]==i){
                printf("$ ");
            }
            else if(fruitPos[0]==j && fruitPos[1]==i){
                printf("%c ", 220); // ▄
            }
            else{
                int drwTail = 0;
                for(int k=0; k<ntail; k++){
                    if(tailx[k]==j && taily[k]==i){
                        printf("$ ");
                        drwTail = 1;
                    }
                    if(tailx[k]==playerPos[0] && taily[k]==playerPos[1]){
                        gameover = 1;
                    }
                }
                if(!drwTail)
                    printf("%c ",250); // ·
            }
            // when player get the fruit
            if(playerPos[0]==fruitPos[0] && playerPos[1]==fruitPos[1] && ntail<100){
                ntail+=1;
                fruitRand();
            }
        }printf("\n");
    }

    tailLogic();

    playerPos[0] += pInput[0];
    playerPos[1] += pInput[1];

    // wall position reset
    if(playerPos[0]>w-1) playerPos[0] = 0; else if(playerPos[0]<0) playerPos[0] = w-1;
    if(playerPos[1]>h-1) playerPos[1] = 0; else if(playerPos[1]<0) playerPos[1] = h-1;
    printf("SCORE: %d\n", ntail);
    
    if(debbug==1){
        debugger();
    }
    printf("\33[%iA", h+MAX_H); // go back h+n lines up
}


void gameReset(){
    system("cls");
    quit = gameover = ntail = 0;
    pInput[0] = 1, pInput[1] = 0;
    playerPos[0] = w/2, playerPos[1] = h/2;
    ntail = 0;
}

int main(){
    system("cls");

    printf("\33[?25l");
    
    while(!quit){

        gameReset();

        for(int i=0; i<h;i++){
            for(int j=0;j<w;j++){
                if(i==h/2){
                    printf("        PRESS ANY KEY TO PLAY       ");
                    break;
                }else printf("%c ", 250);
            }printf("\n");
        }
        printf("[X] QUIT");
        if(toupper(getch()) == 'X'){
            quit = 1;
        }
        system("cls");

        fruitRand();

        while(!gameover && !quit){
            playerInput();
            drawMap();
            Sleep(90);
        }
        
        if(gameover){
            system("cls");
            printf("************************************\n");
            printf("              GAME OVER             \n");
            printf("************************************\n");
            printf("TOTAL SCORE: %d\n", ntail);
            system("pause");
        }
    }

    printf("\33[?25h");
    return 0;
}


#else
#error Not compatible system
#endif