#include "../../include/minishell.h"
#include <stdio.h>
#include <string.h>

t_env *env_list = NULL;  // Initialize the custom environment list
 
void split_key_value(char *input, char **key, char **value) 
{
      *key = NULL;
    *value = NULL;
  char *equal_sign = strchr(input, '=');
  if (equal_sign) {
    int key_len = equal_sign - input;
    int val_len = strlen(equal_sign + 1);

    *key = malloc(key_len + 1);
    *value = malloc(val_len + 1);

    if (*key && *value) {
      strncpy(*key, input, key_len);
      (*key)[key_len] = '\0';         // Null-terminate key
      strcpy(*value, equal_sign + 1); // cpy value part after '='
    } else {
      free(*key);
      free(*value);
      *key = NULL;
      *value = NULL;
    }
  } else {
    *key = NULL;
    *value = NULL;
  }
}
char *remove_quotes(const char *input)
 {
    size_t len = strlen(input);
    if ((input[0] == '\'' || input[0] == '"') && input[len - 1] == input[0])
    {
        char *trimmed = malloc(len - 1);
        if (!trimmed)
            return NULL;
        strncpy(trimmed, input + 1, len - 2);
        trimmed[len - 2] = '\0';
        return trimmed;
    }
    return strdup(input); 
}


void split_key_value_with_quotes(const char *input, char **key, char **value)
 {
    *key = NULL;
    *value = NULL;
    char *equal_sign = strchr(input, '=');
    if (equal_sign) {
        size_t key_len = equal_sign - input;
        *key = malloc(key_len + 1);
        if (*key) {
            strncpy(*key, input, key_len);
            (*key)[key_len] = '\0';  
        }
        *value = remove_quotes(equal_sign + 1);  
    } else {
        *key = strdup(input); // if no '=', treat the entire input as key
        *value = NULL;
    }

    // Remove quotes from the key (if any)
    char *temp_key = remove_quotes(*key);
    free(*key);
    *key = temp_key;
}

int is_valid_identifier(const char *key) 
{
    if (!key || !key[0])
        return 0;  

     if (key[0] == '\'' || key[0] == '"' || strchr(key, '\'') || strchr(key, '"'))
        return 0;

    // here  must start with a letter or underscore
    if (!isalpha(key[0]) && key[0] != '_')
        return 0;

    // " ensure the rest of the key contains only alphanumeric characters or underscores "
    int i = 1;
    while (key[i]) 
    {
        if (!isalnum(key[i]) && key[i] != '_')
            return 0;
      i++;
    }

    return 1;
}


void store_env_variable(char *key, char *value) 
{
    t_env *current = env_list;
    if (!key) {
        printf("Error: key is NULL\n");
        return;
    }
    if (!value){
        printf("Error: key is NULL\n");
        return;
    }
     while (current) {
        if (strcmp(current->key, key) == 0)
        {
            free(current->value); 
            if (value) {
                current->value = strdup(value);  
            } else {
                current->value = NULL;  
            }

            return;
        }
        current = current->next;
    }

    t_env *new_env = malloc(sizeof(t_env));
    if (!new_env) {
        return;  
    }

    new_env->key = strdup(key);

    if (value) 
    {
        new_env->value = strdup(value);  
    }
    else
    {
        new_env->value = NULL; 
    }

    new_env->next = env_list;
    env_list = new_env;

    if (value) 
    {
        printf("Stored vari@@@@@able: %s=%s\n", key, value); 
    } 
    else 
     {
        printf("Stored variable: %s=NULL\n", key);
    }
}

char *get_next_token(const char *input, size_t *start)
{
    while (input[*start] && isspace(input[*start]))
    {
        (*start)++; // Skip leading spaces
    }

    if (!input[*start]) {
        return NULL; // No more tokens
    }

    size_t end = *start;
    while (input[end] && !isspace(input[end]))
    {
        end++; // find the end of the token
    }

    size_t token_len = end - *start;
    char *token = malloc(token_len + 1);
    if (token) {
        strncpy(token, input + *start, token_len);
        token[token_len] = '\0';  
    }

    *start = end; // Update the start position for the next token
    return token;
}

void handle_export_command(char *input)
{
    printf("\n#exprt#\n");
    size_t pos = 0; // Start position for tokenization
    char *token;

    while ((token = get_next_token(input, &pos)))
    {
        char *key = NULL;
        char *value = NULL;

        split_key_value_with_quotes(token, &key, &value);
        
        printf("\n#exprt coco#\n");
        
        if (key && is_valid_identifier(key))
        {
                    printf("\n#exprt coco2#\n");
            if (value) 
            {
                printf("\n#exprt coco3#\n");
                store_env_variable(key, value);  
            }
            else
            { 
                printf("\n#exprt coco#\n");
                store_env_variable(key, NULL);  
            }
        }
         else
         {
            printf("minishell: export: `%s': not a valid identifier\n", token);
         }

        free(key);
        free(value);
        free(token);
    }
}

 

/////  orgin
/////  handle the "expand" command to store variables like "var=value"



void handle_echo_command(char *input, char **env)
{     
    int in_single_quote = 0;
    int in_double_quote = 0;
    char *buffer = malloc(1024);
    int buf_idx = 0;
    int found_unrecognized_var = 0;
 
while (*input)
{
    // Handle single quote (toggle single quote mode)
    if (*input == '\'' && !in_double_quote)
    {
        in_single_quote = !in_single_quote;
        input++;  // Skip the single quote
        continue;
    }

    // Handle double quote (toggle double quote mode)
    if (*input == '"' && !in_single_quote)
    {
        in_double_quote = !in_double_quote;
        input++;  // Skip the double quote
        continue;
    }

    // Handle escaped characters (only inside double quotes)
    if (*input == '\\' && in_double_quote)
    {
        input++;  // Skip the backslash
        if (*input == '$' || *input == '\\')
        {
            buffer[buf_idx++] = *input;  // Keep the $ or \ as a literal
        }
        else
        {
            buffer[buf_idx++] = '\\';  // Keep the backslash for other chars
            buffer[buf_idx++] = *input;  // Add the next character after the backslash
        }
        input++;
        continue;
    }

    // Handle variable expansion outside of single quotes
    if (*input == '$' && !in_single_quote)
    {
        buffer[buf_idx] = '\0';  // End the current buffer (before $)
       // printf("&%s", buffer);  // Print the buffer content
        buf_idx = 0;

        input++;  // Skip the '$'
        char var_name[256] = {0};  // Buffer for variable name
        int var_idx = 0;

        while (isalnum(*input) || *input == '_')  // Capture valid variable characters
        {
            var_name[var_idx++] = *input++;
        }
        var_name[var_idx] = '\0';  // Null-terminate variable name

        // Expand the variable
        char *expanded_value = expand_variable(var_name, env);
        if (expanded_value)
        {
            //printf("%s", expanded_value);  // Print expanded value
            free(expanded_value);  // Free allocated memory
        }
        else
        {
            found_unrecognized_var = 1; 
            //printf("$%s", var_name);  // Print the original variable as-is
        }
        continue;  // Continue to the next character
    }// Print any remaining buffer content

    else
    {
        // Add regular characters to the buffer
        buffer[buf_idx++] = *input;
    }

    input++;  // Move to the next character
}

 if (buf_idx > 0)
{
    buffer[buf_idx] = '\0';
    printf("%s", buffer);
}
 if (found_unrecognized_var)
    {
        printf("unrecognized variable(s) found\n");
    }
  if (found_unrecognized_var)
    {
        printf("unrecognized variable(s) found\n");
    }
    free(buffer);  // Free allocated memory for buffer
}
char *int_to_string(int value)
 {
    // Determine the buffer size needed (including sign and null terminator)
    int length = snprintf(NULL, 0, "%d", value);
    char *str = malloc(length + 1); // Allocate memory for the string (+1 for '\0')
    if (!str) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    snprintf(str, length + 1, "%d", value); // Convert the integer to a string
    return str;
}
 
char *expand_variable(char *var_name, char **env)
{
    t_env *current  = env_list;
    t_env *previous = NULL; 
 

    while (current)
    {
             int len=strlen(current->key);
              char *arr=malloc(len);
             strcpy(arr,current->key);
 
        if (strcmp(arr, var_name) == 0)
        {
            return strdup(current->value);  // return the shell variable's value
        }
        previous = current;
        current = current->next;
    }

     for (int i = 0; env[i]; i++)
    {
        if (strncmp(env[i], var_name, strlen(var_name)) == 0 && env[i][strlen(var_name)] == '=')
        {
           // printf("match found in system env for %s: %s\n", var_name, env[i] + strlen(var_name) + 1);  // Debug print
            return strdup(env[i] + strlen(var_name) + 1);  // return system environment variables value
        }
    }
    return NULL;
}
