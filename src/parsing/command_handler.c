#include "../../include/minishell.h"
#include <stdio.h>
#include <string.h>

t_env *env_list = NULL;  // initialize the custom environment list
 

int is_valid_identifier(const char *key)
{
    int i;

    if (!key || !key[0])
        return (0);
     if (!((key[0] >= 'a' && key[0] <= 'z') || 
        (key[0] >= 'A' && key[0] <= 'Z') || key[0] == '_'))
        return (0);
    
    i = 1;
    while (key[i])
    {
        if (!((key[i] >= 'a' && key[i] <= 'z') || 
            (key[i] >= 'A' && key[i] <= 'Z') || 
            (key[i] >= '0' && key[i] <= '9') || 
            key[i] == '_'))
            return (0);
        i++;
    }
    return (1);
}
 
void handle_quoted_value(const char *input, int *i, char **key, char **value, char quote_char)
{
    printf("im a here \n");
     (*i)++;
    int value_start = *i;
    while (input[*i] && input[*i] != quote_char)
        (*i)++;
    
    *value = malloc(*i - value_start + 1);
    if (!*value) {
        free(*key);
        *key = NULL;
        return;
    }
    strncpy(*value, input + value_start, *i - value_start);
    printf("\nthe string is_2 [%s]\n",*value);
    (*value)[*i - value_start] = '\0';
    
    if (input[*i] == quote_char)
        (*i)++;

}

void handle_unquoted_value(const char *input, int *i, char **key, char **value)
{
     int value_start = *i;
    while (input[*i])
        (*i)++;
    
    *value = malloc(*i - value_start + 1);
    if (!*value)
    {
        free(*key);
        *key = NULL;
        return;
    }
    strncpy(*value, input + value_start, *i - value_start);
     (*value)[*i - value_start] = '\0';
}

// static void clean_append_fail(t_append *app, char **key, char **value)
// {
 
//     *key = NULL;
//     *value = NULL;
//     app->existing = NULL;
//     app->old_value = NULL;
//     app->new_value = NULL;
// }

void handle_append(char **key, char **value)
{
    t_append app;

    app.existing = env_list;
    while (app.existing)
    {
        if (strcmp(app.existing->key, *key) == 0)
        {
             app.old_len = strlen(app.existing->value);
             app.append_len = strlen(*value);
             app.new_value = malloc(app.old_len + app.append_len + 1);
            if (!app.new_value)
            {
                free(*value);
                *value = NULL;
                return;
            }
            app.new_value[0] = '\0';  // Ensure empty string to start
            strncpy(app.new_value, app.existing->value, app.old_len + 1);
            strncat(app.new_value, *value, app.old_len + app.append_len + 1);
            free(*value);
            *value = app.new_value;
            break;
        }
        app.existing = app.existing->next;
    }
}
 

static void skip_whitespace(const char *input, int *i)
{
    while (input[*i] && (input[*i] == ' ' || input[*i] == '\t'))
        (*i)++;
}

static char *remove_quotes(char *str)
{
    char *temp;

    if (!str || (str[0] != '"' && str[0] != '\''))
        return (str);
    temp = strdup(str + 1);
    if (!temp)
        return (str);
    temp[strlen(temp) - 1] = '\0';
    free(str);
    return (temp);
}

static char *extract_key(const char *input, int start, int end)
{
    char *key;

    key = strndup(input + start, end - start);
    if (!key)
        return (NULL);
    return (remove_quotes(key));
}

static char *extract_value(const char *input, int start)
{
    char *value;

    if (!input[start])
        return (NULL);
    value = strdup(input + start);
    if (!value)
        return (NULL);
    return (remove_quotes(value));
}

static void handle_value_extraction(const char *input, int j, char **key, char **value)
{
    int is_append;
    int i;

    is_append = (input[j] == '+' && input[j + 1] == '=');
    i = j + (is_append ? 2 : 1);  // Skip '+=' or '='
    *value = extract_value(input, i);
    if (!(*value))
    {
        free(*key);
        *key = NULL;
        return;
    }
    if (is_append)
        handle_append(key, value);
}

void parse_export_value(const char *input, char **key, char **value)
{
    int i;
    int j;

    i = 0;
    *key = NULL;
    *value = NULL;
    if (!input)
        return;
    skip_whitespace(input, &i);
    j = i;

    while (input[j] && input[j] != '=' && !(input[j] == '+' && input[j + 1] == '='))
        j++;
    
    *key = extract_key(input, i, j);
    if (!(*key))
        return;
    
    if ((input[j] == '+' && input[j + 1] == '=') || input[j] == '=')
        handle_value_extraction(input, j, key, value);
}

void print_invalid_identifier(char *key)
{
    write(2, "minishell: export: `", 19);
    write(2, key, strlen(key));
    write(2, "`: not a valid identifier\n", 25);
}
void handle_export_command(char *input)
{
    char *key;
    char *value;

    key = NULL;
    value = NULL;
    if (!input)
        return;
    parse_export_value(input, &key, &value);
    if (key)
    {
        if (is_valid_identifier(key))
            store_env_variable(key, value);
        else
        {
            write(2, "minishell: export: `", 19);
            write(2, key, strlen(key));
            write(2, "`: not a valid identifier\n", 25);
        }
    }
    else
            print_invalid_identifier(key);
    free(key);
    free(value);
}
 
static char *create_env_value(const char *value)
{
    if (value)
        return (strdup(value));
    return (strdup(""));
}

static t_env *find_env_variable(const char *key)
{
    t_env *current;

    current = env_list;
    while (current)
    {
        if (strcmp(current->key, key) == 0)
            return (current);
        current = current->next;
    }
    return (NULL);
}

static t_env *create_env_node(const char *key, const char *value)
{
    t_env *new_env;

    new_env = malloc(sizeof(t_env));
    if (!new_env)
        return (NULL);
    new_env->key = strdup(key);
    if (!new_env->key)
    {
        free(new_env);
        return (NULL);
    }
    new_env->value = create_env_value(value);
    if (!new_env->value)
    {
        free(new_env->key);
        free(new_env);
        return (NULL);
    }
    return (new_env);
}

void store_env_variable(char *key, char *value)
{
    t_env   *current;
    t_env   *new_env;
    char    *new_value;

    if (!key)
        return ;
    current = find_env_variable(key);
    if (current)
    {
        new_value = create_env_value(value);
        if (!new_value)
            return ;
        free(current->value);
        current->value = new_value;
        return ;
    }
    new_env = create_env_node(key, value);
    if (!new_env)
        return ;
    new_env->next = env_list;
    env_list = new_env;
}

 
char *expand_variable(char *var_name, char **env)
{
    t_env *current;
    int i;
    int len;
    char *arr;

    current = env_list;
    while (current)
    {
        len=strlen(current->key);
        arr=malloc(len+1);
        strcpy(arr,current->key);
        if (strcmp(arr, var_name) == 0)
            return strdup(current->value);   
        free(arr); 
        current = current->next;
    }
     i = 0;
    while (env[i])
    {
        if (strncmp(env[i], var_name, strlen(var_name)) == 0 && env[i][strlen(var_name)] == '=')
            return strdup(env[i] + strlen(var_name) + 1);   
        i++;
    }
    return NULL;
}