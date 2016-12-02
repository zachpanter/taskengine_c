#include <string.h>
#include <curses.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>

int counter1 = 1;
int counter2 = -1;

void *countUp(void *arg);
void *countDown(void *arg);
void *middleLine(void *arg);

pthread_mutex_t ncurses;

int main(int argc, char *argv[])
{
	//WINDOW *top_window_ptr;
	//WINDOW *middle_window_ptr;
	//WINDOW *bottom_window_ptr;

	WINDOW *left_window_ptr;
	WINDOW *right_window_ptr;
	WINDOW *status_window_ptr;

	initscr(); // initialize ncurses
	cbreak();
	timeout(1);
	refresh();
	noecho();


	//int halfLINES = LINES/2; // set screen heights
	//int top_height = halfLINES - 1;
	//int bottom_height = halfLINES - 1;
	//int divider_start_y = halfLINES - 1;
	//int bottom_start_y = halfLINES + 1;
	//int divider_height = 2;

	int main_height = LINES - 1;
	int half_width = COLS/2;
	int status_height = 2;

	//top_window_ptr = newwin(top_height, COLS, 0, 0); // create top and bottom windows // newwin(int lines, int cols, int beginy, int beginx);
	//middle_window_ptr = newwin(divider_height, COLS, divider_start_y, 0);
	//bottom_window_ptr = newwin(bottom_height, COLS, bottom_start_y, 0);


	left_window_ptr = newwin(main_height, half_width,0,0);
	right_window_ptr = newwin(main_height, half_width,0,half_width);
	status_window_ptr = newwin(status_height,COLS,main_height,0); 


	//scrollok(top_window_ptr, TRUE); // SCROLLING
	//scrollok(middle_window_ptr, TRUE);
	//scrollok(bottom_window_ptr, TRUE);

	scrollok(left_window_ptr, TRUE);
	scrollok(right_window_ptr, TRUE);
	scrollok(status_window_ptr,FALSE);


	has_colors(); // initialize color support
	start_color();
	init_pair(1,COLOR_GREEN, COLOR_BLACK);
	init_pair(2,COLOR_BLUE,COLOR_BLUE);
	init_pair(3,COLOR_RED, COLOR_BLACK);



	// Find where window.c initialized ncurses, create the top and bottom windows and initialze color support
	// Just after the color support, create a mutex for controlling access to ncurses. Why not just call it "ncurses"?
	// Set up a mutex to protect any ncurses system calls from being entered simultaneoulsy by both threads
	// Use pthreat_mutex_init() to create a mutex for a critical section around any ncurses calls in functions countUp() and countDown()
	int res_mutex;
	res_mutex = pthread_mutex_init(&ncurses, NULL);
	if (res_mutex != 0)
	{
		perror("Mutex creation failed");
		exit(EXIT_FAILURE);
	}
	
	// Use pthread_create() to create a thread for each of the counters so they are truly capable of running simultaneously
	// Create two threads, one for each window, calling countUp() and countDown() as appropriate.
	int res_1;
	int res_2;
	int res_line;
	pthread_t a_thread_1;
	pthread_t a_thread_2;
	pthread_t a_thread_line;
	void *thread_result_1;
	void *thread_result_2;
	void *thread_result_line;
	res_1 = pthread_create(&a_thread_1, NULL, countUp, (void *)left_window_ptr);
	res_2 = pthread_create(&a_thread_2, NULL, countDown, (void *)right_window_ptr);
	res_line = pthread_create(&a_thread_line, NULL, middleLine, (void *)status_window_ptr);
	if (res_1 != 0 || res_2 != 0 || res_line != 0)
	{
		perror("Thread creation failed");
		exit(EXIT_FAILURE); 
	}



	// Use pthread_join() as you would wait() to wait in the parent thread for the two child threads to terminate
	int res_join_1;
	int res_join_2;
	int res_join_line;
	res_join_1 = pthread_join(a_thread_1, &thread_result_1);
	res_join_2 = pthread_join(a_thread_2, &thread_result_2);
	res_join_line = pthread_join(a_thread_line, &thread_result_line);
	if (res_join_1 != 0 || res_join_2 != 0 || res_join_line != 0)
	{
		perror("Thread joins failed");
		exit(EXIT_FAILURE);
	}

	sleep(3);
	nocbreak();	
	echo();
	endwin();	// Call endwin() before exiting the program.		
	return 0;

} 


// Create two functions(countUp() and countDown()) that access the appropriate window and do the counting.
// These two functions should be identical except for the direction they count and the window they use for output.
void *countUp(void *arg) 
{
	while(1)
	{
		pthread_mutex_lock(&ncurses); 	// Use pthread_mutex_lock() and pthread_mutex_unlock() to create the critical sections
		wrefresh((WINDOW *)arg);
		wattron((WINDOW *)arg, COLOR_PAIR(1));
		//box((WINDOW * )arg,0,0);
		wprintw((WINDOW *)arg," Counter = %d\n",counter1++);
		//sleep(1); //removed to make it go at full speed
		box((WINDOW * )arg,0,0);
		pthread_mutex_unlock(&ncurses);

		if(getch() > 1)
		{
			break;
		}
	}
	pthread_exit(NULL);
}


void *countDown(void *arg)
{
	while(1)
	{
	pthread_mutex_lock(&ncurses); 	// Use pthread_mutex_lock() and pthread_mutex_unlock() to create the critical sections
	wrefresh((WINDOW *)arg);
	wattron((WINDOW *)arg, COLOR_PAIR(3));
	wprintw((WINDOW *)arg," Counter = %d\n",counter2--);
	//sleep(1); //removed to make it go at full speed
	box((WINDOW * )arg,0,0);
	pthread_mutex_unlock(&ncurses);

		if(getch() > 1)
		{
			break;
		}
	}
	pthread_exit(NULL);
}

void *middleLine(void *arg)
{
	// DIVIDING LINE
	pthread_mutex_lock(&ncurses);
	wattron((WINDOW *)arg, COLOR_PAIR(2));
	char blueLine[COLS];
	strcpy(blueLine," ");
	for(int i; i < COLS-2; i++)
	{
		strcat(blueLine," ");
	}
	wprintw((WINDOW *)arg,"%s",blueLine);
	wrefresh((WINDOW *)arg);
	pthread_mutex_unlock(&ncurses);
	pthread_exit(NULL);
}
