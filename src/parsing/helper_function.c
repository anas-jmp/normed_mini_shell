#include "../../include/minishell.h"
#include <stdlib.h>

 

void store_env_variable(char *key, char *value)
 {
  t_env *current;
  t_env *new_env;
  
  current = env_list;  
   while (current) 
   {
    if (strcmp(current->key, key) == 0) 
    {
       free(current->value);           // Free the old value
      current->value = strdup(value);  
      return;
    }
    current = current->next;
  }
   new_env = malloc(sizeof(t_env));
  if (!new_env)
    return;
  new_env->key = strdup(key);
  new_env->value = strdup(value);
  new_env->next = env_list;
  env_list = new_env;
}
 