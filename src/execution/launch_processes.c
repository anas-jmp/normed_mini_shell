#include "../../include/execution.h"


void	execute_first_child_process(t_command *prompt, char **env, int **pipefd,
		int i)
{
	if (prompt->command && strcmp(prompt->command,"export"))
	{
		prompt->cmd_path = is_command(prompt->command, env);
		first_child_processe(prompt, env, pipefd, i);
	}
}

void	execute_middle_child_process(t_command *prompt, char **env,
		int **pipefd, int i)
{
	if (prompt->command && strcmp(prompt->command,"export" ) )
	{
		prompt->cmd_path = is_command(prompt->command, env);
		if (pipe(pipefd[i]) < 0)
			output_error_exit("pipe", EXIT_FAILURE);
		middle_child_processes(prompt, pipefd, env, i);
	}
}
void	execute_ending_child_process(t_command *prompt, char **env,
		int **pipefd, int i)
{
	if (prompt->command && strcmp(prompt->command,"export"))
	{
		prompt->cmd_path = is_command(prompt->command, env);
		if (pipe(pipefd[i]) < 0)
			output_error_exit("pipe", EXIT_FAILURE);
		ending_child_processe(prompt, pipefd, env, i);
	}
}


void	exec_one_process(t_command *prompt, char **env)
{
	pid_t	pid;
	char	*cmd_path;
	int		fd_in_out[2];

	fd_in_out[0] = STDIN_FILENO;
	fd_in_out[1] = STDOUT_FILENO;
	if (!prompt->command)
		return ;
	cmd_path = is_command(prompt->command, env);
	if (!cmd_path)
		cmd_path = prompt->command;
	if (handle_builtins(prompt, env))
		return ;
	pid = fork();
	if (pid < 0)
		output_error_exit("fork", EXIT_FAILURE);
	if (!pid)
	{
		fd_in_out[0] = return_input_file(prompt, fd_in_out[0]);
		fd_in_out[1] = open_output_redirection(prompt, fd_in_out[1]);
		dup_fds(fd_in_out[0], STDIN_FILENO);
		dup_fds(fd_in_out[1], STDOUT_FILENO);
		execute_command(cmd_path, prompt->args, env);
	}
	wait(NULL);
}


