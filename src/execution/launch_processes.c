#include "../../include/execution.h"
 
int is_builtin_command(const char *command)
{
    return (!strcmp(command, "export") ||  !strcmp(command, "unset") || !strcmp(command, "exit"));
}



void	execute_first_child_process(t_command *prompt, char **env, int **pipefd,
		int i)
{
	t_shell_state shell_state;
      int x;
	  shell_state.state = 0; 
      shell_state.exit_status = 0;

		 x=0;
       if (!strcmp(prompt->command, "export") || !strcmp(prompt->command, "unset") ||  !strcmp(prompt->command, "exit") ) 
				x=1;
if (!strcmp(prompt->command, "exit"))
  {
     handle_exit(prompt->args, &shell_state);  
  }
// if (prompt->command)
if (prompt->command)
	{
		prompt->cmd_path = is_command(prompt->command, env);
	   if(x!=1)
		 {
			first_child_processe(prompt, env, pipefd, i);
		}
	}
 
}

void	execute_middle_child_process(t_command *prompt, char **env,	int **pipefd, int i)
{
	    t_shell_state shell_state;
      shell_state.state = 0; 
      shell_state.exit_status = 0;
	  int x=0;

       if (!strcmp(prompt->command, "export") || !strcmp(prompt->command, "unset") ||  !strcmp(prompt->command, "exit") ) 
        {
				x=1;
         }
  if (!strcmp(prompt->command, "exit"))
    {
      handle_exit(prompt->args, &shell_state);  
    }
 if (prompt->command)
{
		prompt->cmd_path = is_command(prompt->command, env);
		if (pipe(pipefd[i]) < 0)
			output_error_exit("pipe", EXIT_FAILURE);
		if(x!=1)
			{
					middle_child_processes(prompt, pipefd, env, i);
			}
}
}
void	execute_ending_child_process(t_command *prompt, char **env,
		int **pipefd, int i)
{
	
	  t_shell_state shell_state;
 	  int x;
      
	  shell_state.state = 0; 
      shell_state.exit_status = 0;
	  x=0;
       if (!strcmp(prompt->command, "export") || !strcmp(prompt->command, "unset") ||  !strcmp(prompt->command, "exit") ) 
				x=1;
  if (!strcmp(prompt->command, "exit")) {
     handle_exit(prompt->args, &shell_state);  
  }
  if (prompt->command )
	{
		prompt->cmd_path = is_command(prompt->command, env);
		if (pipe(pipefd[i]) < 0)
			output_error_exit("pipe", EXIT_FAILURE);
		if(x!=1)
		{
			ending_child_processe(prompt, pipefd, env, i);
		}
	}
}


void	exec_one_process(t_command *prompt, char **env)
{
	pid_t	pid;
	char	*cmd_path;
	int		fd_in_out[2];

        if (!strcmp(prompt->command, "export") || !strcmp(prompt->command, "unset") )
				return;
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


