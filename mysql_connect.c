#include <my_global.h>
#include <my_sys.h>
#include <mysql.h>

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
    // initialize connection handler
    conn = mysql_init(NULL);
    if (conn == NULL)
    {
        fprintf(stderr, "mysql_init() failed - probably out of memory\n");
        exit(1);
    }
    
    // connect to server
    if (mysql_real_connect (conn, opt_host_name, opt_user_name, opt_password, opt_db_name, opt_port_num, opt_socket_name, opt_flags) == NULL)
    {
        fprintf(stderr, "mysql_real_connect() failed\n");
        mysql_close(conn);
        exit(1);
    }
    
    // disconnect from server
    fprintf(stdout, "mysql connection worked\n");    
    mysql_close(conn);
    exit(0);
}
