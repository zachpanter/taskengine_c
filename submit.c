#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <pthread.h>
#include <menu.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))

struct window_struct 
{
	WINDOW *window_one;
	WINDOW *window_two;
} window_info;
struct window_struct *window_info_ptr;

// NCURSES GLOBAL VARIABLES
unsigned int z = 0;
int counter1 = 1;
int counter2 = -1;
void navDiv(struct window_struct);
void *statusDiv(void *arg);
pthread_mutex_t ncurses;
int main_height;
int half_width;
int status_height;
WINDOW *left_window_ptr;
WINDOW *right_window_ptr;
WINDOW *status_window_ptr;

// MYSQL GLOBAL VARIABLES
static char *opt_host_name = NULL; // server host (default = localhost)
static char *opt_user_name = "teguest"; // username
static char *opt_password = "guest"; // password
static unsigned int opt_port_num = 3306; // port number
static char *opt_socket_name = NULL; //socket name
static char *opt_db_name = "taskengine"; // database name
static unsigned int opt_flags = 0; // connection flags
static MYSQL *conn; // pointer to connection handler
static MYSQL_RES *res;    
static MYSQL_ROW row;


// MYSQL FUNCTION DECLARATIONS
void process_statement (MYSQL *conn, char *stmt_str, WINDOW * arg);
void process_result_set (MYSQL *conn, MYSQL_RES *res_set, WINDOW * arg);


// MENU VARIABLES
ITEM **my_items;
MENU *my_menu;
int k = 0;
int c;
int task_repo_max = 5;
int string_length = 50;
int n_choices = 0;
int my_num_rows = 0;
void func(char *name);
ITEM *cur;
void (*p)(char *);
 
int main()
{
    // INITIALIZE DATABASE CONNECTION HANDLER
    conn = mysql_init(NULL);
    if (conn == NULL)
    {
        fprintf(stderr, "mysql_init() failed - probably out of memory\n");
        exit(EXIT_FAILURE);
    }

	// CONNECT TO SERVER
    if (mysql_real_connect (conn, opt_host_name, opt_user_name, opt_password, opt_db_name, opt_port_num, opt_socket_name, opt_flags) == NULL)
    {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }


    // INITIALIZE NCURSES & WINDOWS


	initscr(); // initialize ncurses
	cbreak();
	timeout(1);
	refresh();
	noecho();
	main_height = LINES - 4;
	half_width = COLS/2;
	status_height = 3;
	left_window_ptr = newwin(main_height, half_width,0,0);
	right_window_ptr = newwin(main_height, half_width,0,half_width);
	status_window_ptr = newwin(status_height,COLS,main_height,0); 
	scrollok(left_window_ptr, TRUE);
	scrollok(right_window_ptr, TRUE);
	scrollok(status_window_ptr,FALSE);

	// INITIALIZE WINDOW struct
	window_info_ptr = &window_info;
	window_info.window_one = left_window_ptr;
	window_info.window_two = right_window_ptr;

    // INITIALIZE COLORS
	has_colors(); // initialize color support
	start_color();
	init_pair(1,COLOR_GREEN, COLOR_BLACK);
	init_pair(2,COLOR_BLUE,COLOR_BLUE);
	init_pair(3,COLOR_RED, COLOR_BLACK);

    // CREATE MUTEX
	int res_mutex;
	res_mutex = pthread_mutex_init(&ncurses, NULL);
	if (res_mutex != 0)
	{
		perror("Mutex creation failed");
		exit(EXIT_FAILURE);
	}
	
    // CREATE THREAD VARIABLES
	int res_line;
	pthread_t a_thread_line;
	void *thread_result_line;

	navDiv(window_info);
	
	// CREATE STATUS THREAD
	res_line = pthread_create(&a_thread_line, NULL, statusDiv, (void *)status_window_ptr);
	if (res_line != 0)
	{
		perror("Thread creation failed");
		exit(EXIT_FAILURE); 
	}

	// JOIN STATUS THREAD TO MAIN
	int res_join_line;
	res_join_line = pthread_join(a_thread_line, &thread_result_line);
	if (res_join_line != 0)
	{
		perror("Thread join failed");
		exit(EXIT_FAILURE);
	}

	//sleep(3);
	unpost_menu(my_menu);
	free_menu(my_menu);
	free(my_items);
	nocbreak();	
	echo();
	endwin();	// Call endwin() before exiting the program.		
	return 0;

} 


void navDiv(struct window_struct window_info) 
{
	WINDOW *left_window_ptr;
	WINDOW *right_window_ptr;
	left_window_ptr = window_info.window_one;
	right_window_ptr = window_info.window_two;

	// INITIAL DISPLAY
		//pthread_mutex_lock(&ncurses); 	// Use pthread_mutex_lock() and pthread_mutex_unlock() to create the critical sections
		wrefresh(left_window_ptr);
		wrefresh(right_window_ptr);
		keypad(left_window_ptr, TRUE);



		// LIST REPOS
		if(mysql_query(conn, " SELECT taskrepo_name FROM taskrepo"))
		{
			wprintw(left_window_ptr, " %s\n", mysql_error(conn));
			exit(EXIT_FAILURE);
		}
		res = mysql_store_result(conn);
		wprintw(left_window_ptr,"\n");
		
		char repo_array[task_repo_max][string_length];
		k = 0;
		my_num_rows = mysql_num_rows(res);
		n_choices = my_num_rows + 1;
		my_items = (ITEM **)calloc(n_choices, sizeof(ITEM *));
		while((row = mysql_fetch_row(res)) != NULL)
		{
			// Instead of printing them, I want to add them to a menu
			// wprintw(left_window_ptr," %s\n",row[0]);
			//strcpy(repo_array[k],row[0]);
			my_items[k] = new_item(row[0]," ");
			set_item_userptr(my_items[k], func);
			++k;
		}
		//strcpy(repo_array[k],(char *)NULL); // zero termination

		my_menu = new_menu((ITEM **)my_items); /* SEGFAULT SEGFAULT SEGFAULT  */
		set_menu_win(my_menu, left_window_ptr);
		WINDOW *derp_win = derwin(left_window_ptr, main_height - 6, half_width - 6, 1, 1);
		set_menu_sub(my_menu, derp_win);
		//wborder(derp_win, '|','|','-','-','*','*','*','*'); //box(arg,'*','*');
		set_menu_format(my_menu, half_width - 30, main_height - 4);
		set_menu_mark(my_menu, " * ");
		post_menu(my_menu);
		wrefresh(left_window_ptr);


		mysql_free_result(res); // free up memory
		



		// // LIST ACTIONABLES
		// wprintw(right_window_ptr,"\n");
		// process_statement(conn, "SELECT actionable_title FROM actionable;", right_window_ptr);

		// Draw Borders
		wborder(left_window_ptr, '|','|','-','-','*','*','*','*'); //box(arg,'*','*');
		wborder(right_window_ptr, '|','|','-','-','*','*','*','*'); //box(arg,'*','*');
		wborder(status_window_ptr, '|','|','-','-','*','*','*','*'); //box(arg,'*','*');
		wrefresh(left_window_ptr);
		wrefresh(right_window_ptr);
		wrefresh(status_window_ptr);
		//pthread_mutex_unlock(&ncurses);

	// ACTION HANDLER LOOP
	while((c = wgetch(left_window_ptr)) != 'q')
	{
		switch(c)
	    {	
			case KEY_DOWN:
				menu_driver(my_menu, REQ_NEXT_ITEM);
				break;
			case KEY_RIGHT:
				menu_driver(my_menu, REQ_NEXT_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_PREV_ITEM);
				break;
			case KEY_LEFT:
				menu_driver(my_menu, REQ_PREV_ITEM);
			case KEY_NPAGE:
				menu_driver(my_menu, REQ_SCR_DPAGE);
				break;
			case KEY_PPAGE:
				menu_driver(my_menu, REQ_SCR_UPAGE);
				break;
			case 10: /* ENTER */
				cur = current_item(my_menu);
				p = item_userptr(cur);
				p((char *)item_name(cur));
				pos_menu_cursor(my_menu);
				break;
		}
	}
}

// TO BE REPLACED WITH THE STATUS BAR WINDOW HANDLER
void *statusDiv(void *arg)
{
	// DIVIDING LINE
	pthread_mutex_lock(&ncurses);
	wborder(status_window_ptr, '|','|','-','-','*','*','*','*'); //box(arg,'*','*');
	wattron(arg, COLOR_PAIR(2));
	char blueLine[COLS];
	strcpy(blueLine," ");
	for(z = 0; z < COLS-2; z++)
	{
		strcat(blueLine," ");
	}
	wprintw(arg,"test");
	wprintw(arg,"%s",blueLine);
	wrefresh(arg);
	pthread_mutex_unlock(&ncurses);
	pthread_exit(NULL);
}

void process_result_set (MYSQL *conn, MYSQL_RES *res_set, WINDOW *arg)
{
	MYSQL_ROW     row;
	MYSQL_FIELD   *field;
	unsigned long col_len;
	unsigned int  i;

  wprintw(arg, " \n");
  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    mysql_field_seek (res_set, 0);
    
    for (i = 0; i < mysql_num_fields (res_set); i++)
    {
      field = mysql_fetch_field (res_set);
	  // instead of printing row values, I want to add them to a menu
	  if (row[i] == NULL)
	  {
        wprintw (arg," %-*s", (int) field->max_length, "NULL");
	  }
      else
	  { 
        wprintw (arg," %-*s", (int) field->max_length, row[i]);
	  }
    }
    wprintw(arg, " \n");
  }
}



void
process_statement (MYSQL *conn, char *stmt_str, WINDOW *arg)
{
	MYSQL_RES *res_set;

  if (mysql_query (conn, stmt_str) != 0)  /* the statement failed */
  {
    wprintw(arg, "Could not execute statement");
    return;
  }

  /* the statement succeeded; determine whether it returned data */
  res_set = mysql_store_result (conn);
  if (res_set)      /* a result set was returned */
  {
    /* process rows and free the result set */
    process_result_set (conn, res_set, arg);
	wborder(right_window_ptr, '|','|','-','-','*','*','*','*'); //box(arg,'*','*');
	wrefresh(right_window_ptr);
    mysql_free_result (res_set);
  }
  else              /* no result set was returned */
  {
    if (mysql_field_count (conn) == 0)
    {
      /*
       * statement generated no result set (it was not a SELECT,
       * SHOW, DESCRIBE, etc.); just report rows-affected value.
       */
      //wprintw(arg, "Number of rows affected: %ld\n", (long) mysql_affected_rows (conn));
    }
    else  /* an error occurred */
    {
      wprintw(arg, "Could not retrieve result set");
    }
  }
}

void func(char *name)
{	
	wmove(status_window_ptr, 1, 1);
	wclrtoeol(status_window_ptr);
	//mvprintw(20, 0, "Item selected is : %s", name);
	wprintw(status_window_ptr," Actionables shown for : %s\n", name);
	wborder(status_window_ptr, '|','|','-','-','*','*','*','*'); //box(arg,'*','*');
	wrefresh(status_window_ptr);

	wprintw(right_window_ptr,"\n");

	// PREPARE QUERY STRING
	char query_string[200];
	strcpy(query_string,"SELECT actionable.actionable_title FROM actionable JOIN taskrepo ON (actionable.taskrepo_id = taskrepo.taskrepo_id) WHERE taskrepo.taskrepo_name = '");
	strcat(query_string, name);
	strcat(query_string, "';");
	//wprintw(right_window_ptr,"%s\n", query_string);
	
	// EXECUTE QUERY
	wclear(right_window_ptr);
	wmove(right_window_ptr, 1, 1);
	process_statement(conn, query_string, right_window_ptr);
	wrefresh(right_window_ptr);
}