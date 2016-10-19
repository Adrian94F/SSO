#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>

/* ######## POLECENIE ################################
*/
typedef struct 
{
	const char** argv;
} command;
/* ######## TWORZENIE DZIECKA ################################
*/
int execute_child(int in, int out, command* cmd)
{
	pid_t pid = fork();
	/* stworzenie dziecka i przekierowanie strumieni */
	switch (pid)
	{
		case 0:
			if(in != 0)
			{
				dup2(in, 0);
				close(in);
			}
			if(out != 1)
			{
				dup2(out, 1);
				close(out);
			}
			return execvp(cmd->argv[0], (char * const *)cmd->argv);
		default:
			return pid;
	}
	return pid;
}
/* ######## POLECENIE ################################
*/
int execute_children(int n, command** cmd)
{
	int i;
	int in, fd[2];
	/* stdin dla pierwszego polecenia */
	in = 0;
	/* pętla po wszystkich oprócz ostatniego */
	for(i = 0; i < n - 1; ++i)
	{
		pipe(fd);
		/* f[1] to wyjście poprzedniej kolejki, które podpinamy jako wejście dla następnego polecenia */
		execute_child(in, fd[1], cmd[i]) != 0;
		/* wyjście do zamknięcia */
		close(fd[1]);
		/* z obecnego wyjścia przeczyta kolejne dziecko */
		in = fd[0];
	}
	/* powrót do stdin */
	if(in != 0)
		dup2(in, 0);
	/* ostatnie polecenie */
	return execvp(cmd[i]->argv [0],(char * const *)cmd[i]->argv);
}
/* ######## MAIN ################################
*/
int main(int argc, char** argv)
{
	int n = 1, i, j = 1, k, returned;

	if (argc < 2)
	{
		printf("USAGE: shell program_1 program_1_args . program_2 program_2_args . program_n\n");
		printf("EXAMPLE: shell ls -l . awk '{print $9}' . sort\n");
		return -1;
	}

	/* policzenie programów do wykonania */
	for (i = 1; i < argc; i++)
	{
		if (argv[i][0]=='.' && argv[i][1] == 0)
		{
			argv[i][0] = 0; 
			n++;
		}
	}

	/* tablica programów z argumentami */
	command** cmd = (command**) malloc(n * sizeof(command*));

	/* wypełnienie wskaźnikami */
	for (i = 0; i < n; i++)
	{
		cmd[i] = (command*) malloc (sizeof(command));
		int counter = 0;
		while(j + counter < argc && argv[j + counter][0] != 0)
			++counter;
		cmd[i]->argv = (const char**) malloc((counter + 1) * (sizeof(const char*)));
		for(k = 0; k < counter; k++)
			cmd[i]->argv[k] = argv[j + k];

		cmd[i]->argv[counter] = NULL;
		j = j + counter + 1;
	}

	/* wykonanie programów */
	returned = execute_children(n, cmd);

	/*opróżnienie pamięci */
	for (i = 0; i < n; i++)
	{
		free(cmd[i]->argv);
		free(cmd[i]);
	}
	free(cmd);
	return returned;
}