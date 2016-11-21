all:
	gcc -c `mysql_config --cflags` mysql_connect.c
	gcc -o mysql_connect mysql_connect.o `mysql_config --libs`
