#include <ncurses.h>
#include <string.h>



int
main()
{
    int ch;
    WINDOW *win;

    win=initscr();
    noecho();
    cbreak();
    refresh();
    mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);


    while(1)
    {
        move(1,1);
        printw("%i",ch);
       
        move(1,1);
        ch = wgetch(win);
        printw("                               ");
        //if (ch==KEY_MOUSE) {puts("MOUSE");} else {puts("NO MOUSE");}
    /*    if (ch==27){
        endwin();
        break;}*/
    };


    return 0;
}
