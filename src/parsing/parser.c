#include "../../include/minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_REDIRECT_SIZE 4 // Initial size for redirection arrays
#define INITIAL_ARGS_SIZE 16


char *next_tok4(const char **str, char **env)
{
     (*str)++; // Move past the '$'
    const char *var_start = *str;

    if (!isalnum(**str) && **str != '_' && **str != '?' )//&& strcmp(*str,"$")==0) 
    {
      return strdup("$"); // is followed by nothing valid, just return "$"
    }

     while (isalnum(**str) || **str == '_' || **str == '?')
        (*str)++;

    size_t len = *str - var_start; // Length of the variable name
    char var_name[len + 1];
    strncpy(var_name, var_start, len);
    var_name[len] = '\0'; // Null-terminate the variable name

    char *expanded_value = expand_variable(var_name, env);
    if (expanded_value) 
    {
        return expanded_value;
    } 
        return NULL;  
}
// Helper function to set token type based on the symbol
void set_token_type(char symbol, token_type *type)
{
    if (symbol == '>')
        *type = OUTPUT_REDIRECTION;
    else if (symbol == '<')
        *type = INPUT_REDIRECTION;
    else if (symbol == '|')
        *type = PIPES;
}

char *next_tok5(const char **str, token_type *type)
{
  char symbol;
    char *temp;
   
    symbol = **str;  
    (*str)++;             
     if (symbol == '>' && **str == '>') 
    {
        (*str)++; // Move past the second `>`
        *type = OUTPUT_REDIRECTION_APPEND_MODE;
        return strdup(">>");
    }
     if (symbol == '<' && **str == '<') 
    {
        (*str)++; // Move past the second `<`
        *type = HERE_DOC;
        return strdup("<<");
    }
    temp = malloc(2 * sizeof(char));
    if (temp == NULL)
        return NULL; // Handle memory allocation failure
    temp[0] = symbol;  // Assign the symbol to the first character
    temp[1] = '\0';     // Null-terminate the string
  set_token_type(symbol, type);
    return strdup(temp);
}
 void set_token_type3(char *token_value, token_type *type,size_t len)
{

  token_value[len] = '\0';
  if (token_value[0] == '$')
    *type = EXPANDABLE;
  else
    *type = WORD;
}


char *handle_standalone_dollar(char *prefix)
{
    size_t len = (prefix ? strlen(prefix) : 0) + 2;
    char *token = malloc(len);
    token[0] = '\0';
    if (prefix)
        strcat(token, prefix);
    strcat(token, "$");
    return token;
}

char *combine_prefix_and_expanded(char *prefix, char *expanded)
{
    size_t total_len = (prefix ? strlen(prefix) : 0) + (expanded ? strlen(expanded) : 0) + 1;
    char *result = malloc(total_len);
    result[0] = '\0';
    if (prefix)
        strcat(result, prefix);
    if (expanded)
        strcat(result, expanded);
    return result;
}
 

char *get_env_value(const char *key, char **env)
{
     t_env *current = env_list;
    while (current) {
        if (strcmp(current->key, key) == 0)
            return strdup(current->value);
        current = current->next;
    }
    
     for (int i = 0; env[i]; i++)
      {
        if (strncmp(env[i], key, strlen(key)) == 0 && env[i][strlen(key)] == '=')
            return strdup(env[i] + strlen(key) + 1);
    }
    return NULL;
}

static void free_var_resources(t_var_exp *var)
{
    free(var->var_name);
    free(var->expanded);
    free(var->prefix);
}

static void init_var_exp(t_var_exp *var, const char *str, const char *current)
{
    var->prefix_len = current - str;
    if (var->prefix_len > 0)
        var->prefix = strndup(str, var->prefix_len);
    else
        var->prefix = NULL;
    var->current = current + 1;
    var->var_start = var->current;
}

char *handle_var_expansion(const char **str, const char *current, 
    char **env, token_type *type)
{
    t_var_exp var;

    init_var_exp(&var, *str, current);
    while (*var.current && (isalnum(*var.current) || 
        *var.current == '_' || *var.current == '?'))
        var.current++;
    if (var.var_start == var.current)
    {
        var.result = handle_standalone_dollar(var.prefix);
        *str = var.current;
        *type = WORD;
        return (var.result);
    }
    var.var_name = strndup(var.var_start, var.current - var.var_start);
    var.expanded = get_env_value(var.var_name, env);
    var.result = combine_prefix_and_expanded(var.prefix, var.expanded);
    free_var_resources(&var);
    *str = var.current;
    *type = WORD;
    return (var.result);
}


char *handle_special_cases(t_token_state *state)
{
     if (**(state->str) == '"' || **(state->str) == '\'')
        return next_tok3(state->str, state->env, state->type);

     if (**(state->str) == '>' || **(state->str) == '<' || **(state->str) == '|')
        return next_tok5(state->str, state->type);

    return NULL;
}

char *process_variable_or_word(t_token_state *state)
{
    while (*(state->current) && 
           !isspace(*(state->current)) && 
           *(state->current) != '"' && 
           *(state->current) != '\'' && 
           *(state->current) != '>' && 
           *(state->current) != '<' && 
           *(state->current) != '|') 
    {
        if (*(state->current) == '$')
            return handle_var_expansion(state->str, state->current, state->env, state->type);

        state->current++;
    }

    *(state->type) = WORD;
    *(state->str) = state->current;
    return strndup(state->start, state->current - state->start);
}

char *next_token(const char **str, char **env, token_type *type)
{
    t_token_state state = {NULL, NULL, str, env, type};

     while (isspace(**(state.str)))
        (*(state.str))++;

     if (**(state.str) == '\0')
        return NULL;

     state.start = *(state.str);

     char *special_cmd = next_tok2(state.start, state.str, state.type);
    if (special_cmd)
        return special_cmd;

     char *special_case_result = handle_special_cases(&state);
    if (special_case_result)
        return special_case_result;

     state.current = *(state.str);
    return process_variable_or_word(&state);
}



// Main tokenizer function
// char *next_token(const char **str, char **env, token_type *type)
// {
//     const char *start;
//     char *special_cmd;
//     const char *current;
    
//     while (isspace(**str))
//         (*str)++;
//     if (**str == '\0')
//         return NULL;
//      start = *str;
//      special_cmd = next_tok2(start, str, type);
//     if (special_cmd)
//         return special_cmd;
//     if (**str == '"' || **str == '\'')
//         return next_tok3(str, env, type);
//     if (**str == '>' || **str == '<' || **str == '|')
//         return next_tok5(str, type);
//     current = *str;
//     while (*current && !isspace(*current) && *current != '"' && *current != '\'' && 
//            *current != '>' && *current != '<' && *current != '|')
//       {
//         if (*current == '$')
//             return handle_var_expansion(str, current, env, type);
//         current++;
//      }
//     *type = WORD;
//     *str = current;
//     return strndup(start, current - start);
// }