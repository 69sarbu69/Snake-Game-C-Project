// Header files
#include "allheaders.h"

// Data (struct, enum and constants)
#include "dataandconstants.h"

int mod120counter=0;
char playgroundstr[20][20][3];              // 2 characters for each block // never wanna look back on this
struct makeplayground playground;
struct makesnake snake;
struct makemenudata menudata;
struct makegameelements gameelements;
struct makeingameupdate ingameupdate;
struct makegameinfo gameinfo;


// Functions ####################################################
#include "displayfuncs.h"
#include "customcalc.h"
#include "kbandmouse.h"
#include "soundthings.h"

int getsnaketomove();
void printsnake();
void displayinfos(struct makegameinfo);
enum direction getdirection(enum direction, struct keyboardinputs);
void movesnake(enum direction);
void fastness();
void startgame(_Bool);
void displaygameelements();
void gamedataupdates();
COORD goodrandomcoord();
void gameover();

void fastness()
{
    Sleep(900/menudata.level);
}

int main()
{
    srand(time(NULL));
    startgame(FALSE);
}

void startgame(_Bool resumemode)
{
    int s=0;
    int score = 0;

    system("cls");
    ShowConsoleCursor(FALSE);
    if(resumemode)
    {
        score = getsnaketomove();
    }
    else
    {
        menudata.level = 9;

        // snake initilization
        snake.length = 1;
        snake.body[0].location = (COORD) {0,0};
        snake.body[0].going    = right;

        // gameelements initialization
        gameelements.barriercount = 0;

        gameelements.foood = goodrandomcoord();
        gameelements.nofoodduration = 0;

        gameelements.portalcount = 0;

        gameelements.presenceofpowerfood = TRUE;
        gameelements.powerfoood = goodrandomcoord();
        gameelements.powerfoodduration = 30;

        gameelements.eat50counter=0;

        gameelements.powerupsavailable = FALSE;
        gameelements.powerupson = FALSE;

        // ingameupdate initialization
        ingameupdate = (struct makeingameupdate)
        {
            .gameover = FALSE, 
            .lengthincrease = 4, 
            .scoreincrease = 0
        };
    
        score = getsnaketomove();
    }

    if(ingameupdate.gameover)
    {
        menudata.gamerunning = FALSE;
        gameover();
    }
}

int getsnaketomove()
{ 
    enum direction  wheredoigo = snake.body[0].going;
    struct keyboardinputs kb;           // stores all inputs allowed in this game

    displayborder(20,20);
    ShowConsoleCursor(FALSE);

    while(1)
    {
        // Fastness:
        fastness();

        // This is special counter, counts from 0 to 119, which helps with periodic ui things in the game, such as score and things
        incremod(&mod120counter,120);

        // ############################################ controls human or AI mode
        kb=gameinput(0);
    
        // controls exit or pause of game
        if(kb.esc)        break;
        else if(kb.space)
        do
        {   Sleep(100);
            kb = gameinput(0);              // wait for human input when paused
        }while(kb.nothing);

        // decide direcction, and move snake
        wheredoigo = getdirection(snake.body[0].going, kb);          
        //############################## spot for increasing/decreasing snake's length



        movesnake(wheredoigo);
        displaygameelements();
        printsnake();

        gamedataupdates();

        if(ingameupdate.gameover)
        break;

        // Increase score here;
        gameinfo.kb = kb;
        gameinfo.level = menudata.level;
        gameinfo.soundon = 1;
        gameinfo.gametype = 0;
        gameinfo.direction = wheredoigo;
        displayinfos(gameinfo);

        cursorloc(0,30);
    }
    if(ingameupdate.gameover)
    menudata.gamerunning = FALSE;
    else
    menudata.gamerunning = TRUE;
    ShowConsoleCursor(TRUE);
    return gameinfo.score;
}



enum direction getdirection(enum direction currentdirection, struct keyboardinputs kb)
{
    enum direction d=currentdirection;

    switch(currentdirection)          // decide direction according to input
    {
        case right:
        case left:
        {
            if(kb.down)
            d = down;
            else if(kb.up)
            d = up;
        }
        break;

        case up:
        case down:
        {
            if(kb.right)
            d = right;
            else if(kb.left)
            d = left;
        }
        break;
    }
    return d;    
}

void movesnake(enum direction d)
{
    // where whole body moves
    for(int i = snake.length; i>0; i--)
    {
        snake.body[i] = snake.body[i-1];
    }

    // this moves the head to controlled direction
    switch(d)
    {
        case up:
        decremods(&snake.body[0].location.Y, 20);    // Decrements by 1 with modulo 20                  
        break;

        case down:
        incremods(&snake.body[0].location.Y, 20);    // Increments by 1 with modulo 20 (customcalc.h)   
        break;

        case left:
        decremods(&snake.body[0].location.X, 20);    // Comes from opposite end after entering from one 
        break;

        case right:
        incremods(&snake.body[0].location.X, 20);    // Comes from opposite end after entering from one 
        break;
    }
    snake.body[0].going = d;
}

// Important one
void printsnake()
{
    int l = snake.length;
    // Erase tail:
    gameprint(snake.body[l].location.Y, snake.body[l].location.X, "  ");        l--;

    // Print tail:
    gameprint(snake.body[l].location.Y, snake.body[l].location.X, "\260\260");        l--;
    gameprint(snake.body[l].location.Y, snake.body[l].location.X, "\261\261");        l--;
    gameprint(snake.body[l].location.Y, snake.body[l].location.X, "\262\262");        l--;


    // These are half tails
    // for(int i = snake.length-1; i>snake.length -4; i--)
    // {
    //     gameprint(snake.body[i].location.Y, snake.body[i].location.X, bodyblock(doubledirectinof(snake.body[i].going, snake.body[i-1].going)));
    // }


    // Print body:
    for(int i = snake.length-4; i>0; i--)
    {
        gameprint(snake.body[i].location.Y, snake.body[i].location.X, "\333\333");
        
        // Half body mode
        // gameprint(snake.body[i].location.Y, snake.body[i].location.X, bodyblock(doubledirectinof(snake.body[i].going, snake.body[i-1].going)));
    }


    // Print head:
    gameprint(snake.body[0].location.Y, snake.body[0].location.X, "oo");

    cursorloc(0,30);
}

void displayinfos(struct makegameinfo g)
{
    // g.gametype;
    // g.kb;
    // g.level;
    // g.score;
    // g.soundon;
    // g.direction;
    locatecursor(1, 60);
    printf("Level = %10d",g.level);
    locatecursor(2,60);
    printf("Score = %10d",g.score);
    locatecursor(3,60);
    printf("Snake Length = %3d", snake.length);

    locatecursor(5,60);
    switch(g.direction)
    {
        case up:
        printf("Up   ");
        break;

        case down:
        printf("Down ");
        break;

        case left:
        printf("Left ");
        break;

        case right:
        printf("Right");
        break;        
    }

    int starty = 6, startx = 60;

    locprint(starty + 0, startx +4,"\332\304\304\304\277");
    locprint(starty + 1, startx +8,"\263\b\b\b\b\b\263");
    locprint(starty + 2, startx + 0,"\332\304\304\304\305\304\304\304\305\304\304\304\277");
    locprint(starty + 3, startx + 12,"\263\b\b\b\b\b\263\b\b\b\b\b\263\b\b\b\b\b\263");
    locprint(starty + 4, startx + 0,"\300\304\304\304\301\304\304\304\301\304\304\304\331");
    locatecursor(starty + 1, startx + 6);
    if(g.kb.up)
    printf("\030");
    else
    printf(" ");

    locatecursor(starty + 3, startx + 6);
    if(g.kb.down)
    printf("\031");
    else
    printf(" ");

    locatecursor(starty + 3, startx + 2);
    if(g.kb.left)
    printf("\021");
    else
    printf(" ");

    locatecursor(starty + 3, startx + 10);
    if(g.kb.right)
    printf("\020");
    else
    printf(" ");
}

void displaygameelements()
{
    for(int i = 0; i<gameelements.portalcount; i++)
    {
        gameprint(gameelements.portal[i].start.Y, gameelements.portal[i].start.X,portal_b_list[i]);
        gameprint(gameelements.portal[i].end.Y, gameelements.portal[i].end.X,portal_b_list[i]);
    }

    for(int i = 0; i<gameelements.barriercount; i++)
    {
        gameprint(gameelements.barrierblock[i].Y, gameelements.barrierblock[i].X, barrier_b);
    }

    gameprint(gameelements.foood.Y, gameelements.foood.X,food_f[mod120counter%2]);

    if(gameelements.presenceofpowerfood)
    {
        gameprint(gameelements.powerfoood.Y, gameelements.powerfoood.X, powerfood_f[mod120counter%2]);
    }
    else
    {
        gameprint(gameelements.powerfoood.Y, gameelements.powerfoood.X, "  ");
    }
}

void gamedataupdates()
{
    // Length increases one at a time.
    if(ingameupdate.lengthincrease > 0)
    {
        snake.length++;
        ingameupdate.lengthincrease--;
    }
    else if(ingameupdate.lengthincrease < 0)
    {
        snake.length--;
        ingameupdate.lengthincrease++;

        incremod(&gameelements.length5mod,5);
        printf("%d",gameelements.length5mod);
    }

    // Score increases about half of level at a time.
    int n = ingameupdate.scoreincrease;
    if(ingameupdate.scoreincrease > 0)
    {
        n = (n<(menudata.level/2))?n:menudata.level/2;
        gameinfo.score += n;;
        ingameupdate.scoreincrease-=n;
    }
    else if(ingameupdate.scoreincrease < 0)
    {
        n = -n;
        n = (n<(menudata.level/2))?n:menudata.level/2;
        gameinfo.score -= n;;
        ingameupdate.scoreincrease+=n;
    }


    // Check for portal, and if on there, teleport.
    for(int i = 0; i<gameelements.portalcount; i++)
    {   
        if(coordcmp(gameelements.portal[i].start,snake.body[0].location))
        {
            snake.body[0].location = gameelements.portal[i].end;
        }
        else if(coordcmp(gameelements.portal[i].end,snake.body[0].location))
        {
            snake.body[0].location = gameelements.portal[i].start;
        }
    }

    // check for barriers, and if on there, game is over
    for(int i = 0; i<gameelements.barriercount; i++)
    {
        if(coordcmp(gameelements.barrierblock[i],snake.body[0].location))
        {
            ingameupdate.gameover = TRUE;
        }
    }

    // check if on food, and if on there, score and length increase + new food spawns 
    if(coordcmp(gameelements.foood,snake.body[0].location))
    {
        ingameupdate.scoreincrease += menudata.level;
        ingameupdate.lengthincrease += 1;

        gameelements.nofoodduration = 0;
        gameelements.foood = goodrandomcoord();

        if(gameelements.length5mod == 0)
        {
            gameelements.presenceofpowerfood = TRUE;
            gameelements.powerfoood = goodrandomcoord();
        }
    }

    // no food duration increases with each move
    gameelements.nofoodduration++;

    // if no food duration > 50, snake's length and score decreases, no food duration timer resets 
    if(gameelements.nofoodduration>50)
    {
        ingameupdate.lengthincrease -=1;
        ingameupdate.scoreincrease -= 2*menudata.level;
    }

    // powerfood spawns on every multiple of 5 increase in snake's length

    // if powerfood present
    if(gameelements.presenceofpowerfood)
    {
        // 1. check if snake is eating it, if yes score increase, and length can increase or decrease
        if(coordcmp(gameelements.powerfoood,snake.body[0].location))
        {
            ingameupdate.scoreincrease += menudata.level * gameelements.powerfoodduration;
            ingameupdate.lengthincrease++;

            // After snake eats it, or time runs out, powerfood expires, spawning next time in new location
            gameelements.powerfoodduration =31;
        }

        // if there is time, and powerfood is not eaten, time to eat decreases, if there is no time, powerfood expires
        if(gameelements.powerfoodduration>0)
        gameelements.powerfoodduration--;
        else
        gameelements.presenceofpowerfood = FALSE;
    }

    // Snake cannot eat itself, it causes game over
    for(int i = 4; i<snake.length; i++)
    if(coordcmp(snake.body[i].location,snake.body[0].location))
    {
        ingameupdate.gameover = TRUE;
    }

    // Snake can get so hungry, that it loses its own body parts, if it loses all his body, game is over.
    if(snake.length == 0)
    {
        ingameupdate.gameover = TRUE;
    }
}

COORD goodrandomcoord()
{
    COORD c;
    _Bool doitagain = FALSE;

    do
    {
        c = randomcoord();

        for(int i = 0; i<gameelements.portalcount; i++)
        {
            doitagain = doitagain || coordcmp(gameelements.portal[i].start, c);
            doitagain = doitagain || coordcmp(gameelements.portal[i].end  , c);
        }

        for(int i = 0; i<gameelements.barriercount; i++)
        doitagain = doitagain || coordcmp(gameelements.barrierblock[i],c);

        doitagain = doitagain || coordcmp(gameelements.foood,c);

        doitagain = doitagain || coordcmp(gameelements.powerfoood,c);

        for(int i = 0; i<snake.length; i++)
        doitagain = doitagain || coordcmp(snake.body[i].location,c);

    }while(doitagain);
    return c;
}

void gameover()
{
    locprint(3,5,"Game Over");
    printf("\nYour score was: %d",gameinfo.score);
    Sleep(1000);
}