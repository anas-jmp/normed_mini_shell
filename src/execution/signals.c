#include "../../include/minishell.h"
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <unistd.h>

 
void handle_signal(int sig)
{ 
    const char *signal_message = "Unknown signal";

    if (sig == SIGINT) // Ctrl+C
    {
        signal_message = "SIGINT (Ctrl+C)";
        printf("\nSignal received: %s\n", signal_message);
        store_env_variable("?", "130"); // Update $? environment variable
    }
    else if (sig == SIGQUIT)  
    {
        signal_message = "SIGQUIT (Ctrl+\\)";
        printf("\nSignal received: %s\n", signal_message);
    }
    else
    {
        printf("\nSignal received: %s\n", signal_message);
         store_env_variable("?", "0"); 
    }
    write_exit_status_to_file(130);// Update $? environment variable
 
    rl_on_new_line();
    rl_replace_line("\n", 0);
    rl_redisplay();
}

void signals()
{
    struct sigaction sa;

    sa.sa_handler = handle_signal; // Unified handler
    sigemptyset(&sa.sa_mask);      // Clear signal mask
    sa.sa_flags = SA_RESTART;      // Restart interrupted system calls

    sigaction(SIGINT, &sa, NULL);  // Handle Ctrl+C
    sigaction(SIGQUIT, &sa, NULL); // Handle 
}

/*********************************************************/