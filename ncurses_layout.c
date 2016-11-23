#include <curses.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
    WINDOW *command_window_ptr;
    WINDOW *nav_window_ptr;
    WINDOW *display_window_ptr;
    WINDOW *status_window_ptr;

    // inititalize ncurses
    initscr();
    //cbreak(); // raw mode
    timeout(1);
    refresh();
    //noecho(); // turn off echo

    // establish colors
    has_colors();
    start_color();
    init_pair(1, COLOR_RED, COLOR_BLACK);

    // set heights, widths, starting points
    int command_window_height = 4;
    int status_window_height = 4;
    int middle_windows_height = 10;
    int bottom_window_start_y = status_window_height + middle_windows_height;
    int half_screen_width = COLS / 2;
    

    // newwin(int lines, int cols, int begin_y, int begin_x)
    command_window_ptr = newwin(command_window_height, COLS, 0, 0);
    nav_window_ptr = newwin(middle_windows_height, half_screen_width, command_window_height, 0);
    display_window_ptr = newwin(middle_windows_height, half_screen_width, command_window_height, half_screen_width);
    status_window_ptr = newwin(status_window_height, COLS, bottom_window_start_y, 0);

    // scroll options
    scrollok(command_window_ptr, FALSE);
    scrollok(nav_window_ptr, TRUE);
    scrollok(display_window_ptr, TRUE);
    scrollok(status_window_ptr, FALSE);


    // draw borders around windows
    box(command_window_ptr,0,0);
    box(nav_window_ptr,0,0);
    box(display_window_ptr,0,0);
    box(status_window_ptr,0,0);





    // close ncurses
    sleep(3);
    nocbreak(); // reset 
    echo(); // make sure echo is on
    endwin(); // stop ncurses
    return 0l

}
