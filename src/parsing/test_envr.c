#include "../../include/minishell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
// Access the environment
extern char **environ;

static int count_environment_size(char **environ, t_env *env_list)
{
    int count;
    t_env *current;

    count = 0;
    while (environ[count])
        count++;
    current = env_list;
    while (current)
    {
        count++;
        current = current->next;
    }
    return (count);
}

static char **copy_environ(char **environ, int *index)
{
    char **my_env;
    int count;

    count = count_environment_size(environ, NULL);
    my_env = malloc((count + 1) * sizeof(char *));
    if (!my_env)
        return (NULL);
    *index = 0;
    while (environ[*index])
    {
        my_env[*index] = strdup(environ[*index]);
        if (!my_env[*index])
        {
            while (--(*index) >= 0)
                free(my_env[*index]);
            free(my_env);
            return (NULL);
        }
        (*index)++;
    }
    return (my_env);
}
/**************************************/
static void free_env_array(char **env_array, int count)
{
    int i;

    if (!env_array)
        return ;
    i = -1;
    while (++i < count)
    {
        if (env_array[i])
            free(env_array[i]);
    }
    free(env_array);
}

static int add_env_variable(char ***env_array, t_env *current, int *index)
{
    char    *new_var;
    size_t  len;

    if (!current->key || !current->value || !env_array || !*env_array)
        return (0);
    len = strlen(current->key) + strlen(current->value) + 2;
    new_var = malloc(len);
    if (!new_var)
        return (0);
    strcpy(new_var, current->key);
    strcat(new_var, "=");
    strcat(new_var, current->value);
    (*env_array)[*index] = new_var;
    (*index)++;
    return (1);
}

char **handle_env(t_env *env_list)
{
    extern char **environ;
    char    **my_env;
    t_env   *current;
    int     index;

    my_env = copy_environ(environ, &index);
    if (!my_env)
        return (NULL);
    current = env_list;
    while (current)
    {
        if (strcmp(current->key, "?") != 0)
        {
            if (!add_env_variable(&my_env, current, &index))
            {
                free_env_array(my_env, index);
                return (NULL);
            }
        }
        current = current->next;
    }
    my_env[index] = NULL;
    return (my_env);
}






void handle_unset(const char *key)
{
    t_env *current = env_list;
    t_env *previous = NULL;
       
        printf("\nok___unseting  2\n");

    while (current)
     {
        if (strcmp(current->key, key) == 0)
        {
            if (previous == NULL)
            {
                env_list = current->next;
            }
            else
            {
                previous->next = current->next;
            }
            free(current->key);
            free(current->value);
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}
