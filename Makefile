# choose your compiler
CC=gcc
#CC=gcc -Wall

mysh: sh.o get_path.o linked_list.o watchmail_list.o main.c 
	$(CC) -g main.c sh.o get_path.o linked_list.o watchmail_list.o -o mysh -lpthread
#	$(CC) -g main.c sh.o get_path.o bash_getcwd.o -o mysh

linked_list.o: linked_list.c linked_list.h
	$(CC) -g -c linked_list.c


watchmail_list.o: watchmail_list.c watchmail_list.h
	$(CC) -g -c watchmail_list.c

sh.o: sh.c sh.h
	$(CC) -g -c sh.c

get_path.o: get_path.c get_path.h
	$(CC) -g -c get_path.c

clean:
	rm -rf sh.o get_path.o linked_list.o watchmail_list.o mysh
