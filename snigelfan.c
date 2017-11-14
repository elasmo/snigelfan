/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <Johan> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return. diox
 * ----------------------------------------------------------------------------
 */

#include <stdio.h>
#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h> 
#include <string.h>

#define INCREASE_SPEED 45
#define REDUCE_SPEED   43
#define ESCAPE         27
#define SPACE	       32
#define PROGNAME       "Snigelfan v0.1b 2004"

struct coord {
    int y;
    int x;
} snake[100];


void usage();
void game_over(WINDOW *play_area, unsigned int score);
void error(const char *error_message);


int main(int argc, char *argv[])
{
    unsigned int        n = 0,m = 0;
    unsigned int        cmd_opts = 0;
    char                *headskin = "O"; 
    unsigned int        wrapping = 0;
    unsigned int        current_y=0, current_x=0;
    const unsigned int  MAX_ROWS=22, MAX_COLS=80; /* Size of play area */
    WINDOW              *play_area, *help_screen;
    unsigned int        y=2, x=2, rand_y=20, rand_x=20;
    unsigned int        illegal_coordinate=0; 
    unsigned int        current_direction=2;
    unsigned int        pause=0, score=0, speed=80000, tail_length=1;

    srand(time(NULL));

    /* Clear the snake struct to avoid corrupt data */
    for(n=0; n<100; n++) {
        snake[n].x = 0;
        snake[n].y = 0;
    }

    /* Command line options and arguments */
    while((cmd_opts = getopt(argc, argv,"hs:w")) != -1) {
        switch(cmd_opts) {
        case 'h':
            usage();
            exit(0);
            break;
        case 's':
            if(*optarg == SPACE) {
                printf("%s: Don't go invisible\n", argv[0]);
                usage();
            }
            if(strlen(optarg) > 1) {
                printf("%s: Only 1 byte character allowed\n", argv[0]);
                usage();
            }

            headskin = optarg;
            break;
        case 'w':
            wrapping = 1;
            break;
        default:
            usage();
            break;
        }
    }

    /* Initialize curses and give some options */
    initscr();
    noecho();
    curs_set(0);
    raw();
    getmaxyx(stdscr, current_y, current_x);

    /* Check minimum resoultion requirements */
    if(current_y < MAX_ROWS || current_x<MAX_COLS) {
        error("At least 24x80 pixels needed, resize your Terminal window!");
    }

    /* Create windows */
    play_area   = newwin(MAX_ROWS, MAX_COLS, 1, 0);
    help_screen = newwin((MAX_ROWS/2)+5, (MAX_COLS/2)+2, 4, 18);
    nodelay(play_area, TRUE);
    keypad(play_area, TRUE);

#ifdef COLOR
    /* Color stuff */
    if(!has_colors()) {
        error("Your terminal doesn't support colors");
    }

    start_color();

    init_pair(1, COLOR_YELLOW, COLOR_YELLOW);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_BLACK, COLOR_WHITE);
#endif

    while(1) {
        getmaxyx(stdscr, current_y, current_x);
        if(current_y < MAX_ROWS || current_x < MAX_COLS) {
            error("At least 24x80 pixels needed, resize your Terminal window and restart game");
        }

        box(play_area, 0, 0);

#ifdef VERBOSE
        mvprintw(MAX_ROWS + 1, 1, "y:%d, x:%d, direction: %d, rand coords: y:%d, x:%d, wrapping: %d     ", y, x, current_direction, rand_y, rand_x, wrapping);
#endif
        mvprintw(0, 1, "Press 'h' for help screen");			
        mvprintw(0, 57," score: %d, speed %d ms ", score, speed/1000);
        refresh();
#ifdef COLOR 
        wattron(play_area, COLOR_PAIR(1));
#endif
        mvwprintw(play_area, rand_y, rand_x, "#");
#ifdef COLOR
        wattroff(play_area, COLOR_PAIR(1));
#endif

        /* Check if snake scores and generate new "nibbles" */                
        if(y == rand_y && x == rand_x) {
            score++;
            if(tail_length != 99) tail_length++;
            if(speed > 1000) speed -= 500; /* We don't want speed set to zero */


            illegal_coordinate = 0;

            while(1) {
                illegal_coordinate = 0;
                rand_y = rand()%(MAX_ROWS-2)+1; 
                rand_x = rand()%(MAX_COLS-2)+1; 

                for(m=0; m<tail_length; m++) {
                    if(rand_y == snake[m].y && rand_x == snake[m].x) {
                        illegal_coordinate=1;
                        m = tail_length;
                    }
                }

                if(illegal_coordinate == 0) break;
            }																				

        }

        switch(wgetch(play_area)) {
        case 'q':
        case ESCAPE:  
            mvwprintw(play_area, MAX_ROWS/2, (MAX_COLS/2)-7, "Exiting");
            wgetch(play_area);
            endwin();
            exit(0);
            break;
        case KEY_DOWN:
            if(current_direction != 1 && current_direction != 0)
                current_direction=0;
            break;
        case KEY_UP:    
            if(current_direction != 0 && current_direction != 1) 
                current_direction=1;
            break;
        case KEY_RIGHT: 
            if(current_direction != 3 && current_direction != 2)
                current_direction=2;
            break;
        case KEY_LEFT:
            if(current_direction != 2 && current_direction != 3)
                current_direction=3;
            break;
        case INCREASE_SPEED: 
            speed+=500;
            break;
        case REDUCE_SPEED: 
            if(speed>1000) speed-=500; 
            break;
        case 'p':
            pause++;
            mvwprintw(play_area, MAX_ROWS/2, (MAX_COLS/2)-6, "Paused");
            if(pause%2==0)
                mvwprintw(play_area, MAX_ROWS/2, (MAX_COLS/2)-6, "      ");
            break;
        case 'h':
            pause++;

            wattron(help_screen, A_UNDERLINE);
            wattron(help_screen, A_BOLD);
            mvwprintw(help_screen,1,16, "HELP");
            wattroff(help_screen, A_BOLD);
            wattroff(help_screen, A_UNDERLINE);
    
            mvwprintw(help_screen,3,2, "* 'h' to show this screen");
            mvwprintw(help_screen,4,2, "* 'p' to pause current game");
            mvwprintw(help_screen,5,2, "* '+' and '-' to increase and reduce");
            mvwprintw(help_screen,6,2, "  speed manually");
            mvwprintw(help_screen,7,2, "* 'q' or ESC to quit current game");
            mvwprintw(help_screen,8,2, "* Use the arrow keys to move the snake");
            mvwprintw(help_screen,9,2, "* Execute `%s -h` to list", argv[0]);
            mvwprintw(help_screen,10,2,"  command line  options");

            wattron(help_screen, A_BOLD);
            mvwprintw(help_screen,13,3,"  Press ANY key to continue game");
            wattroff(help_screen, A_BOLD);

            box(help_screen,0,0);
            wgetch(help_screen);
            pause++;
            break;
        default:
            break;
        }

        if(pause%2==0) {
            /* Print blanks on old coordinates */
            for(m=0; m < tail_length + 1; m++) {
                mvwprintw(play_area, snake[m].y, snake[m].x, " ");
            }

            /* Save the snakes head to structure */
            snake[0].y = y;
            snake[0].x = x;

            /* Tail collission detect */
            for(m=1; m<tail_length; m++) {
                if(snake[0].y == snake[m].y && snake[0].x == snake[m].x)
                    game_over(play_area, score);
            }							           

            /* Save the snakes tail to structure */
            for(m=tail_length; m > 0; m--) {
                snake[m].x = snake[m - 1].x;
                snake[m].y = snake[m - 1].y;
            }

            /* Move coordinates depending on direction */
            if(current_direction==0) y++;
            if(current_direction==1) y--;
            if(current_direction==2) x++;
            if(current_direction==3) x--;

            /* Draw the snake */
            for(m=0; m < tail_length; m++) {
                /*              mvwaddch(play_area, snake[m].y, snake[m].x, ACS_CKBOARD); */
                mvwprintw(play_area, snake[m].y, snake[m].x, "o");
            }

            /* wrapping */
            if(wrapping==1) {
                if(y == MAX_ROWS-1) y=1;
                if(x == MAX_COLS-1) x=1; 
                if(y < 1) y=MAX_ROWS-2;
                if(x < 1) x=MAX_COLS-2;
            }
            else {
                if(y == MAX_ROWS-1) game_over(play_area, score);
                if(x == MAX_COLS-1) game_over(play_area, score);
                if(y < 1)		    game_over(play_area, score);
                if(x< 1 )		    game_over(play_area, score);
            }
        }

#ifdef COLOR
        wattron(play_area,COLOR_PAIR(2));
#endif
        mvwprintw(play_area,y,x,"%s", headskin);
#ifdef COLOR
        wattroff(play_area,COLOR_PAIR(2));
#endif
        if(current_direction == 0 || current_direction == 1)
            usleep((int) speed * 1.1);
        else
            usleep(speed);
    }

    refresh();
    wrefresh(play_area);
    endwin();

    return 0;
}

void usage() {
    fprintf(stderr,
            "%s\n\n"
            "-h             Show this helpscreen.\n"
            "-s <headskin>  Change headskin. Only one byte character allowed.\n"
            "-w             Enable wrapping.\n", PROGNAME);
    exit(0);
}

void game_over(WINDOW *play_area, unsigned int score)
{
    endwin();
    printf("Game over!\nscore: %d\n", score);
    exit(0);
}

void error(const char *error_message)
{
    endwin();
    printf("%s\n", error_message);
    exit(1);
}
