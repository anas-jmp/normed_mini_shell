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

 
void printing2(t_shell_state *shell_state)
{
   printf(">>> %d<<<\n",shell_state->state);
   if(shell_state->exit_status==130)
   {
      printf("\nfrom p 2\n");
           // store_env_variable("?","1300");
           //write_exit_status_to_file(0);
          // shell_state->state=0;
   }
   printf(">>> %d<<<\n",shell_state->exit_status);

}
 

int main(int argc, char **argv, char **env)
{
     char *input = NULL;
    token *tokens = NULL;
    t_command *cmd_list = NULL;

       t_shell_state shell_state;
    
      shell_state.state = 0; 
     (void)argc;
     (void)argv;
   signals(&shell_state);
   while (1)
    {
          shell_state.exit_status = 0;
          shell_state.exit_status = read_exit_status_from_file(); // Replace global variable
          store_env_variable("?", ft_itoa(shell_state.exit_status));
          write_exit_status_to_file(0);
        if(shell_state.exit_status==0)
        {
                  store_env_variable("?", "0"); 
        }
        input = readline("\x1B[31mMini_shell\x1B[33m(*-_-*)\x1B[32m>\x1B[37m"); // Prompt for input
        if (!input) 
        {  
            printf("exit\n");
            break;
        }
        char unmatched = check_unmatched_quotes(input);
        if (unmatched) {
            printf("syntax error: unmatched %c\n", unmatched);
            free(input);
            continue;
        }
        tokens = tokenize_input(input, env);
        cmd_list = parse_command(tokens);

        if (validate_syntax(tokens) == -1)
        {
            free(input);
            continue;
        }
      if(cmd_list && cmd_list->command)
          {
            if (cmd_list && strcmp(cmd_list->command, "exit") == 0)
            {
                handle_exit(cmd_list->args, &shell_state); // Pass the shell_state struct
                printf("\nExit status is: %d\n", shell_state.exit_status);
                break;
            }
          }
        pipex(cmd_list, env);
        free(input);
        input = NULL;
    }
    return 0;
}



// void initialize_shell(t_shell_state *shell_state)
// {
//     shell_state->state = 0;
//     signals(shell_state);
// }

// char *prompt_user(t_shell_state *shell_state)
// {
//     shell_state->exit_status = read_exit_status_from_file();
//     store_env_variable("?", ft_itoa(shell_state->exit_status));
//     write_exit_status_to_file(0);
//     if (shell_state->exit_status == 0)
//         store_env_variable("?", "0");

//     return readline("\x1B[31mMini_shell\x1B[33m(*-_-*)\x1B[32m>\x1B[37m");
// }

// int process_input(char *input, t_shell_state *shell_state, char **env)
// {
//     token *tokens = NULL;
//     t_command *cmd_list = NULL;

//     char unmatched = check_unmatched_quotes(input);
//     if (unmatched)
//     {
//         printf("syntax error: unmatched %c\n", unmatched);
//         free(input);
//         return -1;
//     }
//     tokens = tokenize_input(input, env);
//     if (validate_syntax(tokens) == -1)
//     {
//         free(input);
//         return -1;
//     }
//     cmd_list = parse_command(tokens);
//     if (cmd_list && cmd_list->command && strcmp(cmd_list->command, "exit") == 0)
//     {
//         handle_exit(cmd_list->args, shell_state);
//         printf("\nExit status is: %d\n", shell_state->exit_status);
//         free(input);
//         return -1;
//     }
//     pipex(cmd_list, env);
//     return 0;
// }

 
// #include <stdio.h>
// #include <stdlib.h>
// #include <unistd.h>
// #include <readline/readline.h>
// #include <readline/history.h> // Required for history functions

// int main(int argc, char **argv, char **env)
// {
//     char *input = NULL;
//     t_shell_state shell_state = {0};
//     (void)argc;
//     (void)argv;

//     // Step 1: Initialize shell
//     initialize_shell(&shell_state);

//     // Step 2: Load history
//     if (read_history(".minishell_history") != 0) {
//         fprintf(stderr, "Warning: Could not load history file\n");
//     }

//     // Step 3: Main loop
//     while (1)
//     {
//         // Show prompt and get user input
//         input = prompt_user(&shell_state);
//         if (!input)
//             break;

//         // Add non-empty input to history
//         if (*input != '\0') {
//             add_history(input); // Save command to history in memory
//         }

//         // Process input
//         if (process_input(input, &shell_state, env) == -1) {
//             free(input);
//             continue;
//         }

//         free(input);
//         input = NULL;
//     }

//     // Step 4: Save history before exiting
//     if (write_history(".minishell_history") != 0) {
//         fprintf(stderr, "Warning: Could not save history file\n");
//     }

//    // cleanup_shell(&shell_state); // Free resources
//     return 0;
// }



// int main(int argc, char **argv, char **env)
// {
//     char *input = NULL;
//     t_shell_state shell_state = {0};
//     (void)argc;
//     (void)argv;

//      initialize_shell(&shell_state);
//     while (1)
//     {
//         input = prompt_user(&shell_state);
//         if (!input)
//             break;

//         if (process_input(input, &shell_state, env) == -1)
//             continue;

//         free(input);
//         input = NULL;
//     }
//      return 0;
// }