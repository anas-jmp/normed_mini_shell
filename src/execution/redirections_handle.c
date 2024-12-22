#include "../../include/execution.h"

int	open_input_redirection(t_command *prompt, int fd_in)
{
	int	i;

	i = 0;
	if (prompt->input_redirect)
	{
		while ((prompt->input_redirect)[i])
		{
			fd_in = open(prompt->input_redirect[i], O_RDONLY);
			i++;
		}
	}
	return (fd_in);
}

int	open_output_redirection(t_command *prompt, int fd_out)
{
	int	i;

	i = 0;
	if (prompt->output_redirect)
	{
		while ((prompt->output_redirect)[i])
		{
			if (prompt->append_modes[i])
				fd_out = open(prompt->output_redirect[i],
						O_RDWR | O_CREAT | O_APPEND, 0644);
			else
				fd_out = open(prompt->output_redirect[i],
						O_RDWR | O_CREAT | O_TRUNC, 0644);
			i++;
		}
	}
	return (fd_out);
}

int	return_heredoc(t_command *prompt, int fd_in)
{
	if (prompt->here_doc_last_fd)
		fd_in = prompt->here_doc_last_fd;
	return (fd_in);
}
int	return_input_file(t_command *prompt, int fd_in)
{
	if (prompt->herdoc_last)
	{
		fd_in = open_input_redirection(prompt, fd_in);
		fd_in = return_heredoc(prompt, fd_in);
	}
	else
	{
		fd_in = return_heredoc(prompt, fd_in);
		fd_in = open_input_redirection(prompt, fd_in);
	}
	return (fd_in);
}