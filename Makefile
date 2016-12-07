submit:
	gcc submit.c `mysql_config --cflags` -o submit `mysql_config --libs` -l menu -l ncurses -l pthread

clean:
	rm submit
