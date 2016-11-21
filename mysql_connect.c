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
static char *opt_user_name = "teadmin"; // username
static char *opt_password = "P0wd3r_87"; // password
static unsigned int opt_port_num = 3306; // port number
static char *opt_socket_name = NULL; //socket name
static char *opt_db_name = "taskengine"; // database name
static unsigned int opt_flags = 0; // connection flags

static MYSQL *conn; // pointer to connection handler

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
    

    
    // disconnect from server
    fprintf(stdout, "mysql connection worked\n");    
    mysql_close(conn);
    exit(EXIT_SUCCESS);
}

