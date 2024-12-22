#include "../../include/minishell.h"

/********************** CHECK UNMATCHED QUOTES **********************/
int check_unmatched_quotes(char *input)
{
    char quote_char = '\0';
    int i = 0;

    while (input[i])
    {
        if (!quote_char && (input[i] == '\'' || input[i] == '"'))
            quote_char = input[i];
        else if (quote_char && input[i] == quote_char)
            quote_char = '\0';
        i++;
    }
    return (quote_char ? 1 : 0);
}

/********************** SYNTAX CHECK HELPERS **********************/
int check_next_token(token *current)
{
    if (!current->next)
    {
        printf("syntax error near unexpected token `newline'\n");
        return (-1);
    }
    if (current->next->token_type != WORD &&
        current->next->token_type != SINGLE_QUOTED_STRING &&
        current->next->token_type != DOUBLE_QUOTED_STRING)
    {
        printf("syntax error near unexpected token `%s'\n", current->next->value);
        return (-1);
    }
    return (0);
}

int check_pipe_syntax(token *tokens, token *prev_token)
{
    if ((!prev_token && tokens->token_type == PIPES) || 
        (prev_token && prev_token->token_type == PIPES))
    {
        printf("syntax error near unexpected token `|'\n");
        return (-1);
    }
    return (0);
}

int check_redirection_syntax(token *current)
{
    return check_next_token(current);
}

/********************** MAIN SYNTAX VALIDATION **********************/
int validate_syntax(token *tokens)
{
    token *prev_token = NULL;
    int ret;

    while (tokens)
    {
        if (tokens->token_type == PIPES && 
            (ret = check_pipe_syntax(tokens, prev_token)) == -1)
            return (-1);
        if ((tokens->token_type == HERE_DOC || 
            tokens->token_type == INPUT_REDIRECTION ||
            tokens->token_type == OUTPUT_REDIRECTION || 
            tokens->token_type == OUTPUT_REDIRECTION_APPEND_MODE) &&
            (ret = check_redirection_syntax(tokens)) == -1)
            return (-1);
        prev_token = tokens;
        tokens = tokens->next;
    }
    if (prev_token && prev_token->token_type == PIPES)
    {
        printf("syntax error near unexpected token `|'\n");
        return (-1);
    }
    return (0);
}
