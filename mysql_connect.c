#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>


// https://www.cyberciti.biz/tips/linux-unix-connect-mysql-c-api-program.html
// http://www.kitebird.com/mysql-book/ch06-3ed.pdf

static char *opt_host_name = NULL; // server host (default = localhost)
static char *opt_user_name = "teguest"; // username
static char *opt_password = "guest"; // password
static unsigned int opt_port_num = 3306; // port number
static char *opt_socket_name = NULL; //socket name
static char *opt_db_name = "taskengine"; // database name
static unsigned int opt_flags = 0; // connection flags

static MYSQL *conn; // pointer to connection handler


void
print_dashes (MYSQL_RES *res_set)
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

void
process_result_set (MYSQL *conn, MYSQL_RES *res_set)
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

  print_dashes (res_set);
  fputc ('|', stdout);
  mysql_field_seek (res_set, 0);
  for (i = 0; i < mysql_num_fields (res_set); i++)
  {
    field = mysql_fetch_field (res_set);
/* #@ _PRINT_TITLE_ */
    printf (" %-*s |", (int) field->max_length, field->name);
/* #@ _PRINT_TITLE_ */
  }
  fputc ('\n', stdout);
  print_dashes (res_set);

  while ((row = mysql_fetch_row (res_set)) != NULL)
  {
    mysql_field_seek (res_set, 0);
    fputc ('|', stdout);
    for (i = 0; i < mysql_num_fields (res_set); i++)
    {
      field = mysql_fetch_field (res_set);
/* #@ _PRINT_ROW_VAL_ */
      if (row[i] == NULL)       /* print the word "NULL" */
        printf (" %-*s |", (int) field->max_length, "NULL");
      else if (IS_NUM (field->type))  /* print value right-justified */
        printf (" %*s |", (int) field->max_length, row[i]);
      else              /* print value left-justified */
        printf (" %-*s |", (int) field->max_length, row[i]);
/* #@ _PRINT_ROW_VAL_ */
    }
    fputc ('\n', stdout);
  }
  print_dashes (res_set);
  printf ("Number of rows returned: %lu\n",
          (unsigned long) mysql_num_rows (res_set));
}



void
process_statement (MYSQL *conn, char *stmt_str)
{
MYSQL_RES *res_set;

  if (mysql_query (conn, stmt_str) != 0)  /* the statement failed */
  {
    fprintf(stderr, "Could not execute statement");
    return;
  }

  /* the statement succeeded; determine whether it returned data */
  res_set = mysql_store_result (conn);
  if (res_set)      /* a result set was returned */
  {
    /* process rows and free the result set */
    process_result_set (conn, res_set);
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
      printf ("Number of rows affected: %ld\n",
              (long) mysql_affected_rows (conn));
    }
    else  /* an error occurred */
    {
      fprintf(stderr, "Could not retrieve result set");
    }
  }
}





int main(int argc, char *argv[])
{

    MYSQL_RES *res;    
    MYSQL_ROW row;

    // initialize connection handler
    conn = mysql_init(NULL);
    if (conn == NULL)
    {
        fprintf(stderr, "mysql_init() failed - probably out of memory\n");
        exit(EXIT_FAILURE);
    }
    
    // connect to server
    if (mysql_real_connect (conn, opt_host_name, opt_user_name, opt_password, opt_db_name, opt_port_num, opt_socket_name, opt_flags) == NULL)
    {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(conn);
        exit(EXIT_FAILURE);
    }




    // LIST REPOS
    if(mysql_query(conn, "SHOW tables"))
    {
        fprintf(stderr, "%s\n", mysql_error(conn));
        exit(EXIT_FAILURE);
    }
    res = mysql_use_result(conn);
    // output table name
    printf("MySQL Tables in mysql database:\n");
    while((row = mysql_fetch_row(res)) != NULL)
    {
        printf("%s\n",row[0]);
    }
    mysql_free_result(res);


    // INSERT NEW ACTIONABLE
    // Select taskrepo
    // Enter text
    // Enter content (optional)
    // Enter due date (optional)
    if(mysql_query(conn, "INSERT INTO taskengine.actionable (taskrepo_id, actionable_title, actionable_createddate) VALUES ('1', 'Test', NOW());"))
    {
        fprintf(stderr, "There was an error creating the new actionable.");
    }
    else
    {
        fprintf(stdout, "Insert statement succeeded; rows affected: %ld\n", (long) mysql_affected_rows(conn));
        // Refresh the navigation pane to reflect the newly added item
    }


    process_statement(conn, "SELECT * FROM actionable;");
    // VIEW ALL ACTIONABLES IN A taskrepo
    // Select taskrepo_id
    //if(mysql_query(conn, "SELECT * FROM actionable;"))
    //{
    //    fprintf(stderr, "There was an error viewing all the actionables within the repo specified.\n");
    //}
    //else
    //{
        //fprintf(stdout, "View all actionables succeeded; rows affected: %lu\n", (unsigned long) mysql_affected_rows(conn));
    //  res = mysql_use_result(conn);
    //  process_statement(conn, "SELECT * FROM actionable;");
    //  }
    //}
      


    // VIEW AN ACTIONABLES DATA
    // Get actionable_id
    if(mysql_query(conn, "SELECT actionable_title, actionable_content, actionable_duedate FROM actionable WHERE actionable.actionable_id = '1'"))
    {
        fprintf(stderr, "There was an error viewing the actionables within the repo specified.\n");
    }
    else
    {
        fprintf(stdout, "View single actionable succeeded; rows affected: %ld\n", (long) mysql_affected_rows(conn));
        // Display a popup window showing the actionable's details
    }

    // DELETE AN ACTIONABLE
    // ...
    if(mysql_query(conn, "DELETE FROM actionable WHERE actionable.actionable_title = 'Test'"))
    {
        fprintf(stderr, "There was an error deleting the actionables with the name specified.\n");
    }
    else
    {
        fprintf(stdout, "Delete single actionable succeeded; rows affected: %ld\n", (long) mysql_affected_rows(conn));
    }

    // UPDATE AN ACTIONABLE to be implemented in the future

    
    // disconnect from server
    fprintf(stdout, "mysql connection worked\n");    
    mysql_close(conn);
    exit(EXIT_SUCCESS);
}

