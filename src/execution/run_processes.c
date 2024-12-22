#include "../../include/execution.h"
// #include "../../include/minishell.h"


pid_t	first_child_processe(t_command *prompt, char **env, int **pipefd, int i)
{
	pid_t	pid;
	int		fd_in;
	int		fd_out;

	if (handle_builtins(prompt, env))
		return (0);
	pid = fork();
	fd_in = STDIN_FILENO;
	fd_out = STDOUT_FILENO;
	if (pid < 0)
		output_error_exit("fork", EXIT_FAILURE);
	if (!pid)
	{
		fd_out = pipefd[i][1];
		fd_in = return_input_file(prompt, fd_in);
		fd_out = open_output_redirection(prompt, fd_out);
		dup_fds(fd_out, STDOUT_FILENO);
		dup_fds(fd_in, STDIN_FILENO);
		close_pipes(pipefd, i);
		execute_command(prompt->cmd_path, prompt->args, env);
	}
	return (pid);
}

pid_t	middle_child_processes(t_command *prompt, int **pipefd, char **env,
		int i)
{
	pid_t	pid;
	int		fd_in;
	int		fd_out;

	if (handle_builtins(prompt, env))
		return (0);
	fd_in = STDIN_FILENO;
	fd_out = STDOUT_FILENO;
	pid = fork();
	if (pid < 0)
	{
		output_error_exit("fork", EXIT_FAILURE);
	}
	if (!pid)
	{
		fd_out = pipefd[i][1];
		fd_in = pipefd[i - 1][0];
		fd_in = return_input_file(prompt, fd_in);
		fd_out = open_output_redirection(prompt, fd_out);
		dup_fds(fd_out, STDOUT_FILENO);
		dup_fds(fd_in, STDIN_FILENO);
		close_pipes(pipefd, i);
		execute_command(prompt->cmd_path, prompt->args, env);
	}
	return (pid);
}

pid_t	ending_child_processe(t_command *prompt, int **pipefd, char **env,
		int j)
{
	pid_t	pid;
	int		fd_in;
	int		fd_out;

	fd_in = STDIN_FILENO;
	fd_out = STDOUT_FILENO;
	if (handle_builtins(prompt, env))
		return (0);
	pid = fork();
	if (pid < 0)
		output_error_exit("fork", EXIT_FAILURE);
	if (!pid)
	{
		fd_in = pipefd[j - 1][0];
		fd_in = return_input_file(prompt, fd_in);
		fd_out = open_output_redirection(prompt, fd_out);
		dup_fds(fd_out, STDOUT_FILENO);
		dup_fds(fd_in, STDIN_FILENO);
		close_pipes(pipefd, j);
		execute_command(prompt->cmd_path, prompt->args, env);
	}
	return (pid);
}

void	execute_process(t_command *prompt, int **pipefd, char **env,
		int lst_size)
{
	int	i;

	i = 0;
	if (pipe(pipefd[i]) < 0)
		output_error_exit("pipe", EXIT_FAILURE);
	execute_first_child_process(prompt, env, pipefd, i);
	i++;
	prompt = prompt->next;
	while (i < lst_size - 1)
	{
		execute_middle_child_process(prompt, env, pipefd, i);
		i++;
		prompt = prompt->next;
	}
	execute_ending_child_process(prompt, env, pipefd, i);
	i = 0;
	close_pipes(pipefd, lst_size);
	while ((wait(NULL)) != -1)
		;
}