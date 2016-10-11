#include <unistd.h>

/* ######## POLECENIE ################################
*/
typedef struct 
{
	const char **argv;
} command;

/* ######## TWORZENIE DZIECKA ################################
*/
int execute_child(int in, int out, command *cmd)
{
	pid_t pid;
	if((pid = fork()) == 0)
	{
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
	}
	return pid;
}

/* ######## POLECENIE ################################
*/
int execute_all(int n, command *cmd)
{
	int i;
	pid_t pid;
	int in, fd[2];
	// stdin dla pierwszego polecenia
	in = 0;
	// pętla po wszystkich oprócz ostatniego
	for(i = 0; i < n - 1; ++i)
	{
		pipe(fd);
		// f[1] to wyjście poprzedniej kolejki, które podpinamy jako wejście dla następnego polecenia
		execute_child(in, fd[1], &cmd[i]);
		// wyjście do zamknięcia
		close(fd[1]);
		// z obecnego wyjścia przeczyta kolejne dziecko
		in = fd[0];
	}
	// powrót do stdin
	if(in != 0)
		dup2(in, 0);
	// ostatnie polecenie
	return execvp(cmd[i].argv [0],(char * const *)cmd[i].argv);
}

/* ######## MAIN ################################
*/
int main(int argc, char** argv)
{
	int pos = 1;
	int n = 1;

	for (int i=1; i<argc; i++)
	{
		if (argv[i][0]=='.' && argv[i][1] == 0)
		{
			argv[i][0] = 0;
			n++;
		}
	}

	const char *ls[] = { "ls", "-l", 0 };
	const char *awk[] = { "awk", "{print $1}", 0 };
	const char *sort[] = { "sort", 0 };
	const char *uniq[] = { "uniq", 0 };
	command cmd [] = { {ls}, {awk}, {sort}, {uniq} };
	return execute_all(4, cmd);
	//return execute_all(n, cmd);
}
