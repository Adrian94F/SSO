all: main minishell shell
main: main.c 
	gcc -o main main.c 
debug: main.c
	gcc main.c -o main -g
	gdb main
minishell: minishell.c
	gcc -o minishell minishell.c
shell: shell.c
	gcc -o shell shell.c -ansi