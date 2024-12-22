#include "../../include/minishell.h"
#include "../../include/execution.h"

int redidrect_stdin_to_herdoc(char *delimiter , int heredoc_fd) {
  char *line;
  char *test;

  if (heredoc_fd < 0)
    output_error_exit("there was an error creating herdoc file", 1);
  while (1) {
    line = get_next_line(STDIN_FILENO);
    if (!line)
        break;
    test = strdup(line);
    test[ft_strlen(test) - 1] = '\0';
    if (!strcmp(test, delimiter)) {
      break;
    }
    write(heredoc_fd, line, ft_strlen(line));
    free(line);
    free(test);
  }
  close(heredoc_fd);
  heredoc_fd = open(".heredoc", O_RDONLY);
  return heredoc_fd;
}