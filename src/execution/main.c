#include "../../include/minishell.h"
#include <signal.h>
#include "../../include/execution.h"



int	handle_builtins(t_command *prompt, char **env)
{
	if (!strcmp(prompt->command, "cd"))
	{
		ft_cd((prompt->args)[1], env);
		return (1);
	}
	else if (!strcmp(prompt->command, "pwd"))
	{
		ft_pwd();
		return (1);
	}

	return (0);
}

void	close_pipes(int **pipefd, int number_of_pipes)
{
	int	i;

	i = 0;
	while (i < number_of_pipes)
	{
		close(pipefd[i][0]);
		close(pipefd[i][1]);
		free(pipefd[i]);
		i++;
	}
	free(pipefd);
}
int	**malloc_pipefd(int lst_size)
{
	int	**pipefd;
	int	i;

	i = 0;
	pipefd = malloc(sizeof(int *) * (lst_size));
	while (i < lst_size)
	{
		pipefd[i] = malloc(sizeof(int) * 2);
		i++;
	}
	return (pipefd);
}
void is_exit_with_signal(int *status) 
{            
 
  if (WIFEXITED(*status))
    *status = WEXITSTATUS(*status);
  else if (WIFSIGNALED(*status))
    *status = WTERMSIG(*status) + 128;
//store_env_variable("?","777");
  if(*status==1)
    write_exit_status_to_file(777);
  if(*status==0)
      write_exit_status_to_file(999);
}

int	pipex(t_command *prompt, char **env)
{
	int	**pipefd;
	int	lst_size;
  char **my_env = handle_env( env_list);
    env = my_env;


	if (!prompt)
		return (0);
	heredocs_fds(prompt);
	lst_size = ft_lstsize(prompt);
	if (lst_size == 1 )
	{
		exec_one_process(prompt, env);
		return (0);
	}
	pipefd = malloc_pipefd(lst_size);
	execute_process(prompt, pipefd, env, lst_size);
	return (0);
}

 
void initialize_shell(t_shell_state *shell_state)
{
    shell_state->state = 0;
    signals(shell_state);
}

char *prompt_user(t_shell_state *shell_state)
{
    shell_state->exit_status = read_exit_status_from_file();
    store_env_variable("?", ft_itoa(shell_state->exit_status));
    write_exit_status_to_file(0);
    if (shell_state->exit_status == 0)
        store_env_variable("?", "0");

    return readline("\x1B[31mMini_shell\x1B[33m(*-_-*)\x1B[32m>\x1B[37m");
}

 


int process_input(char *input, t_shell_state *shell_state, char **env)
{
    token *tokens = NULL;
    t_command *cmd_list = NULL;
    int has_pipe = 0;

    char unmatched = check_unmatched_quotes(input);
    if (unmatched)
    {
        printf("syntax error: unmatched %c\n", unmatched);
        free(input);
        return -1;
    }
    tokens = tokenize_input(input, env);
    if (validate_syntax(tokens) == -1)
    {
        free(input);
        return -1;
    }
    cmd_list = parse_command(tokens);
     t_command *temp = cmd_list;
    while (temp && temp->next)
    {
        has_pipe = 1;
        temp = temp->next;
    }
    if (!has_pipe && cmd_list && cmd_list->command && 
        strcmp(cmd_list->command, "exit") == 0)
    {
        handle_exit(cmd_list->args, shell_state);
        printf("(exit)\n");
        printf("Exit status is: %d\n", shell_state->exit_status);
        free(input);
        return -2;  // Special return for exit
    }
    pipex(cmd_list, env);
    return 0;
}

int main(int argc, char **argv, char **env)
{
    char *input;
    t_shell_state shell_state;
    int ret;

    input = NULL;
    shell_state.state = 0;
    (void)argc;
    (void)argv;
    initialize_shell(&shell_state);
    

    while (1)
    {
        input = prompt_user(&shell_state);
        if (!input)
            break;
        if (*input != '\0')
            add_history(input);
        
        ret = process_input(input, &shell_state, env);
        if (ret == -2)  // Exit command
        {
            write_history(".minishell_history");
            return (shell_state.exit_status);
        }
        if (ret == -1)
            continue;
        free(input);
        input = NULL;
    }
    write_history(".minishell_history");
    return (0);
}
