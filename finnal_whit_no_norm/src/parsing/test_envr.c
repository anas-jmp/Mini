#include "../../include/minishell.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Access the environment
extern char **environ;

char **handle_env(const char *str, t_env *env_list)
{
    extern char **environ;
    t_env *current = env_list;
    // Count variables to determine array size
    int count = 0;
    for (char **env = environ; *env; env++)count++;
    for (t_env *cur = env_list; cur; cur = cur->next) count++;
    // Allocate memory for my_env
    char **my_env = malloc((count + 1) * sizeof(char *));  // +1 for NULL terminator
    if (!my_env)
    {
        perror("Failed to allocate memory for my_env");
        return NULL;
    }
    int index = 0;
    for (char **env = environ; *env; env++)
    {
        my_env[index++] = strdup(*env);
    }
     // printf("\ncurrent->key3 , %s\n",current->key);
    //   if(strcmp(current->key,"?")==0)
    //     {
    //                       printf("\nwe found  the '?' (^_^)current->key3 , %s\n",current->key);
    //                      my_env[0] ='\0';  // NULL-terminate the array
    //     }
    while (current)
    {
        size_t key_len = strlen(current->key);
        size_t value_len = strlen(current->value);

        my_env[index] = malloc(key_len + value_len + 2);  // +2 for '=' and '\0'
        if (!my_env[index])
        {
            perror("Failed to allocate memory for my_env variable");
            // Free allocated memory in case of failure
            for (int i = 0; i < index; i++) free(my_env[i]);
            free(my_env);
            return NULL;
        }
                    if(strcmp(current->key,"?")==0)
                            {
                                  //printf("\ncurrent->key1 , %s\n",current->key);
                                  // printf("\nwe found  the '?' (^_^)current->value key3 , %s\n",current->value);
                                //my_env[0] ='\0';  // NULL-terminate the array
                            }
             if(strcmp(current->key,"?")!=0)
                    {
                        strcpy(my_env[index], current->key);   // Copy the key
                        strcat(my_env[index], "+=+");           // Add the '='
                        strcat(my_env[index], current->value); // Add the value
                    }
                current = current->next;
                index++;
    }
    my_env[index] = NULL;  // NULL-terminate the array

    // Debugging output (optional)

    for (int i = 0; my_env[i] != NULL; i++)
    {
        //printf("%s\n", my_env[i]);
    }
    return my_env;
}

void handle_unset(const char *key)
{
    t_env *current = env_list;
    t_env *previous = NULL;

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
