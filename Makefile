GUI_DB_combined:
	gcc GUI_DB_combined.c `mysql_config --cflags` -o GUI_DB_combined.o `mysql_config --libs` -l menu -l ncurses -l pthread