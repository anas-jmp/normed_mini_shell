#include "minishell.h"

#ifndef EXEC_H
# define EXEC_H

void output_redirection(char *file_path); 
 void output_redirection_append_mode(char *file_path );
void output_error_exit(char *error_msg , int status);
void  input_redirection (char *file_path );
int redidrect_stdin_to_herdoc(char *delimiter , int heredoc_fd);
char	*is_command(char *cmd, char **env);
char **ft_unset(char **env  , char *var);
void execute_command(char *command_path , char **argv , char **env);
void dup_fds(int old_fd , int new_fd);
char ft_cd(char* path , char **env);
void ft_env(char **envp);
void ft_echo(int line , char *str);
char	*is_command(char *cmd, char **env) ;
void heredocs_fds(t_command *prompt);
 int handle_exit(char **args, t_shell_state *shell_state);

int	open_input_redirection(t_command *prompt, int fd_in) ; 
int	open_output_redirection(t_command *prompt, int fd_out) ;
int	return_heredoc(t_command *prompt, int fd_in) ; 
int	return_input_file(t_command *prompt, int fd_in) ;
void	execute_first_child_process(t_command *prompt, char **env, int **pipefd,
		int i);
void	execute_middle_child_process(t_command *prompt, char **env,
		int **pipefd, int i) ;
void	execute_ending_child_process(t_command *prompt, char **env,
		int **pipefd, int i) ;

void	exec_one_process(t_command *prompt, char **env) ;
pid_t	first_child_processe(t_command *prompt, char **env, int **pipefd, int i) ;
pid_t	middle_child_processes(t_command *prompt, int **pipefd, char **env,
		int i) ;
pid_t	ending_child_processe(t_command *prompt, int **pipefd, char **env,
		int j) ;
void	execute_process(t_command *prompt, int **pipefd, char **env,
		int lst_size) ;
int	handle_builtins(t_command *prompt, char **env) ;
void	close_pipes(int **pipefd, int number_of_pipes) ;
int	**malloc_pipefd(int lst_size) ;
void ft_echo(int line, char *str); 
void ft_pwd();

void signals();
void printing(t_shell_state *shell_state);

 
 #endif