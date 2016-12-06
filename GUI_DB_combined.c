#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <curses.h>
#include <pthread.h>

// NCURSES GLOBAL VARIABLES
unsigned int z = 0;
int counter1 = 1;
int counter2 = -1;
void *navDiv(void *arg);
void *displayDiv(void *arg);
void *statusDiv(void *arg);
pthread_mutex_t ncurses;

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
void print_dashes (MYSQL_RES *res_set);


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
	WINDOW *left_window_ptr;
	WINDOW *right_window_ptr;
	WINDOW *status_window_ptr;
	initscr(); // initialize ncurses
	cbreak();
	timeout(1);
	refresh();
	noecho();
	int main_height = LINES - 1;
	int half_width = COLS/2;
	int status_height = 2;
	left_window_ptr = newwin(main_height, half_width,0,0);
	right_window_ptr = newwin(main_height, half_width,0,half_width);
	status_window_ptr = newwin(status_height,COLS,main_height,0); 
	scrollok(left_window_ptr, TRUE);
	scrollok(right_window_ptr, TRUE);
	scrollok(status_window_ptr,FALSE);

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
	
    // CREATE THREADS
	int res_1;
	int res_2;
	int res_line;
	pthread_t a_thread_1;
	pthread_t a_thread_2;
	pthread_t a_thread_line;
	void *thread_result_1;
	void *thread_result_2;
	void *thread_result_line;

    // PERFORM THREADS
	res_1 = pthread_create(&a_thread_1, NULL, navDiv, (void *)left_window_ptr);
	res_2 = pthread_create(&a_thread_2, NULL, displayDiv, (void *)right_window_ptr);
	res_line = pthread_create(&a_thread_line, NULL, statusDiv, (void *)status_window_ptr);
	if (res_1 != 0 || res_2 != 0 || res_line != 0)
	{
		perror("Thread creation failed");
		exit(EXIT_FAILURE); 
	}

	// JOIN THREADS
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


// TO BE REPLACED WITH THE NAVIGATION WINDOW HANDLER
void *navDiv(void *arg) 
{
	while(1)
	{
		pthread_mutex_lock(&ncurses); 	// Use pthread_mutex_lock() and pthread_mutex_unlock() to create the critical sections
		wrefresh(arg);
		wattron(arg, COLOR_PAIR(1));
		//box((WINDOW * )arg,0,0);
		//wprintw((WINDOW *)arg," Counter = %d\n",counter1++);
		//sleep(1); //removed to make it go at full speed
												// LIST REPOS
												if(mysql_query(conn, "SHOW tables"))
												{
													wprintw(arg, "%s\n", mysql_error(conn));
													exit(EXIT_FAILURE);
												}
												res = mysql_use_result(conn);
												// output table name
												wprintw(arg," MySQL Tables in mysql database:\n");
												while((row = mysql_fetch_row(res)) != NULL)
												{
													wprintw(arg,"%s\n",row[0]);
												}
												mysql_free_result(res);
		box(arg,0,0);
		pthread_mutex_unlock(&ncurses);

		if(getch() > 1) // replace with a handler for the up and down arrows to select a repo
		{
			break;
		}
	}
	pthread_exit(NULL);
}


// TO BE REPLACED WITH THE DISPLAY WINDOW HANDLER
void *displayDiv(void *arg)
{
	while(1)
	{
	pthread_mutex_lock(&ncurses); 	// Use pthread_mutex_lock() and pthread_mutex_unlock() to create the critical sections
	wrefresh(arg);
	wattron(arg, COLOR_PAIR(3));
	//wprintw((WINDOW *)arg," Counter = %d\n",counter2--);
	//sleep(1); //removed to make it go at full speed

	process_statement(conn, "SELECT actionable_title FROM actionable;", (WINDOW *)arg);

	box(arg,0,0);
	pthread_mutex_unlock(&ncurses);

		if(getch() > 1) // replace with a handler for the 
		{
			break;
		}
	}
	pthread_exit(NULL);
}

// TO BE REPLACED WITH THE STATUS BAR WINDOW HANDLER
void *statusDiv(void *arg)
{
	// DIVIDING LINE
	pthread_mutex_lock(&ncurses);
	wattron(arg, COLOR_PAIR(2));
	char blueLine[COLS];
	strcpy(blueLine," ");
	for(z = 0; z < COLS-2; z++)
	{
		strcat(blueLine," ");
	}
	wprintw(arg,"%s",blueLine);
	wrefresh(arg);
	pthread_mutex_unlock(&ncurses);
	pthread_exit(NULL);
}

// MYSQL FUNCTIONS
void print_dashes (MYSQL_RES *res_set)
{
MYSQL_FIELD   *field;
unsigned int  i, j;

  mysql_field_seek (res_set, 0);
  fputc ('+', stdout);
  for (i = 0; i < mysql_num_fields (res_set); i++)
  {
    field = mysql_fetch_field (res_set);
    for (j = 0; j < field->max_length + 2; j++)
      fputc ('-', stdout);
    fputc ('+', stdout);
  }
  fputc ('\n', stdout);
}

void process_result_set (MYSQL *conn, MYSQL_RES *res_set, WINDOW *arg)
{
MYSQL_ROW     row;
/* #@ _COL_WID_CALC_VARS_ */
MYSQL_FIELD   *field;
unsigned long col_len;
unsigned int  i;
/* #@ _COL_WID_CALC_VARS_ */

/* #@ _COL_WID_CALCULATIONS_ */
  /* determine column display widths; requires result set to be */
  /* generated with mysql_store_result(), not mysql_use_result() */
  mysql_field_seek (res_set, 0);
  for (i = 0; i < mysql_num_fields (res_set); i++)
  {
    field = mysql_fetch_field (res_set);
    col_len = strlen (field->name);
    if (col_len < field->max_length)
      col_len = field->max_length;
    if (col_len < 4 && !IS_NOT_NULL (field->flags))
      col_len = 4;  /* 4 = length of the word "NULL" */
    field->max_length = col_len;  /* reset column info */
  }
/* #@ _COL_WID_CALCULATIONS_ */

  //print_dashes (res_set);
  //fputc ('|', stdout);
  mysql_field_seek (res_set, 0);
  for (i = 0; i < mysql_num_fields (res_set); i++)
  {
    field = mysql_fetch_field (res_set);
/* #@ _PRINT_TITLE_ */
    wprintw (arg," %-*s \n", (int) field->max_length, field->name);
/* #@ _PRINT_TITLE_ */
  }
  //fputc ('\n', stdout);
  //print_dashes (res_set);

  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    mysql_field_seek (res_set, 0);
    //fputc ('|', stdout);
    for (i = 0; i < mysql_num_fields (res_set); i++)
    {
      field = mysql_fetch_field (res_set);
/* #@ _PRINT_ROW_VAL_ */
      if (row[i] == NULL)       /* print the word "NULL" */
        wprintw (arg," %-*s |", (int) field->max_length, "NULL");
      else if (IS_NUM (field->type))  /* print value right-justified */
        wprintw (arg," %*s ", (int) field->max_length, row[i]);
      else              /* print value left-justified */
        wprintw (arg," %-*s \n", (int) field->max_length, row[i]);
/* #@ _PRINT_ROW_VAL_ */
    }
    //fputc ('\n', stdout);
  }
  //print_dashes (res_set);
  wprintw (arg, "Number of rows returned: %lu\n",
          (unsigned long) mysql_num_rows (res_set));
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
    mysql_free_result (res_set);
  }
  else              /* no result set was returned */
  {
    /*
     * does the lack of a result set mean that the statement didn't
     * return one, or that it should have but an error occurred?
     */
    if (mysql_field_count (conn) == 0)
    {
      /*
       * statement generated no result set (it was not a SELECT,
       * SHOW, DESCRIBE, etc.); just report rows-affected value.
       */
      wprintw(arg, "Number of rows affected: %ld\n",
              (long) mysql_affected_rows (conn));
    }
    else  /* an error occurred */
    {
      wprintw(arg, "Could not retrieve result set");
    }
  }
}