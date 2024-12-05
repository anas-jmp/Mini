#include "../../include/minishell.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_REDIRECT_SIZE 4 // Initial size for redirection arrays
#define INITIAL_ARGS_SIZE 16
// #define INITIAL_ARGS_SIZE 1024  // Adjust this later if necessary


void free_command(t_command *cmd) {
  if (cmd) {
    free(cmd->command);
    if (cmd->args) {
      for (int i = 0; cmd->args[i]; i++) {
        free(cmd->args[i]);
      }
      free(cmd->args);
    }
    if (cmd->input_redirect) {
      for (int i = 0; cmd->input_redirect[i]; i++) {
        free(cmd->input_redirect[i]);
      }
      free(cmd->input_redirect);
    }
    if (cmd->output_redirect) {
      for (int i = 0; cmd->output_redirect[i]; i++) {
        free(cmd->output_redirect[i]);
      }
      free(cmd->output_redirect);
    }
    free(cmd->append_modes);
    free(cmd);
  }
}

// Helper function to resize an int array (for append_modes)
int *resize_int_array(int *old_array, int old_size, int new_size) {
  int *new_array = malloc(new_size * sizeof(int));
  if (!new_array) {
    return NULL;
  }

  // Copy old array content to new array
  for (int i = 0; i < old_size; i++) {
    new_array[i] = old_array[i];
  }

  // Free old array
  free(old_array);

  return new_array;
}

char **resize_array(char **old_array, int old_size, int new_size) {
  char **new_array = malloc(new_size * sizeof(char *));
  if (!new_array) {
    return NULL;
  }

  // Copy old array content to new array
  for (int i = 0; i < old_size; i++) {
    new_array[i] = old_array[i];
  }

  // Free old array
  free(old_array);

  return new_array;
}
extern t_env *env_list;

char **resize_args(char **old_args, int old_size, int new_size)
 {
  int i = 0;
  char **new_args = malloc(new_size * sizeof(char *));
  if (!new_args)
    return NULL;

  while (i < old_size) {
    new_args[i] = old_args[i]; // Copy over the old arguments
    i++;
  }

  free(old_args);  // Free the old array
  return new_args; // Return the new and resized array
}


t_command *parse_command(token *tokens, char **env)
{
  //  int *status=0;

  if (!tokens)
    return NULL;
  t_command *head = NULL;
  t_command *current = NULL;

    store_env_variable("?","0");
  while (tokens)
  {
     t_command *cmd = malloc(sizeof(t_command));
    if (!cmd)
      return NULL;
    cmd->command = NULL;
    cmd->args = malloc(INITIAL_ARGS_SIZE * sizeof(char *));
    cmd->input_redirect = malloc(INITIAL_REDIRECT_SIZE * sizeof(char *));
    cmd->output_redirect = malloc(INITIAL_REDIRECT_SIZE * sizeof(char *));
    cmd->here_docs = malloc(INITIAL_REDIRECT_SIZE * sizeof(char *));
    cmd->append_modes = malloc(INITIAL_REDIRECT_SIZE * sizeof(int));
    cmd->herdoc_last = 0;
    cmd->here_doc_last_fd = 0 ;
    cmd->next = NULL;
    if (!cmd->here_docs || !cmd->args || !cmd->input_redirect || !cmd->output_redirect || !cmd->append_modes)
    {
      free_command(cmd);
      return NULL;
    }
    int arg_count = 0, max_args = INITIAL_ARGS_SIZE;
    int input_count = 0, max_inputs = INITIAL_REDIRECT_SIZE;
    int output_count = 0, max_outputs = INITIAL_REDIRECT_SIZE;
    int heredoc_count = 0, max_heredocs = INITIAL_REDIRECT_SIZE;
    while (tokens && tokens->token_type != PIPES) 
    {
      // Handle command name
      if (!cmd->command && (tokens->token_type == WORD ||
                            tokens->token_type == DOUBLE_QUOTED_STRING ||
                            tokens->token_type == SINGLE_QUOTED_STRING)) {
        cmd->command = strdup(tokens->value);
        cmd->args[arg_count++] = strdup(cmd->command);
        tokens = tokens->next;
        continue;
      }

               //    printf("\n<<<<<<<<<<<<<<<here_2>>>>>>>>>>>>>>>>\n");
              //     single-quoted strings (no expansion)
            if (tokens->token_type == SINGLE_QUOTED_STRING)
            {
                // Add as-is to the arguments
                cmd->args[arg_count++] = strdup(tokens->value);
                tokens = tokens->next;
                continue;
            }
      // Handle arguments and handle redirection
      if (tokens->token_type == WORD ||
          tokens->token_type == DOUBLE_QUOTED_STRING ||
          tokens->token_type == SINGLE_QUOTED_STRING) {
        char *arg_value = tokens->value;
        // Check for variable expansion
        if (arg_value[0] == '$') {
          char *var_name = arg_value + 1; // Skip the '$'
          char *expanded_value = expand_variable(var_name, env);
          if (expanded_value) {
            arg_value = expanded_value;
          }
        }
        // Resize arguments array if necessary
        if (arg_count >= max_args - 1) {
          max_args *= 2;
          cmd->args = resize_array(cmd->args, arg_count, max_args);
          if (!cmd->args) {
            free_command(cmd);
            return NULL;
          }
        }
        cmd->args[arg_count++] = strdup(arg_value);
      }
        else if (tokens->token_type == SINGLE_QUOTED_STRING)
        {
            cmd->args[arg_count++] = strdup(tokens->value);
        }
      // Handle input redirection
      else if (tokens->token_type == INPUT_REDIRECTION) 
      {
        tokens = tokens->next;
        if (tokens && (tokens->token_type == WORD ||
                       tokens->token_type == DOUBLE_QUOTED_STRING ||
                       tokens->token_type == SINGLE_QUOTED_STRING)) {
          if (input_count >= max_inputs - 1) {
            max_inputs *= 2;
            cmd->input_redirect =
                resize_array(cmd->input_redirect, input_count, max_inputs);
            if (!cmd->input_redirect) {
              free_command(cmd);
              return NULL;
            }
          }
          cmd->input_redirect[input_count++] = strdup(tokens->value);
        } else {
          printf("\nError: No file specified for input redirection\n");
          free_command(cmd);
          return NULL;
        }
        cmd->herdoc_last = 0;
      }

      // Handle output redirection
      else if (tokens->token_type == OUTPUT_REDIRECTION ||
               tokens->token_type == OUTPUT_REDIRECTION_APPEND_MODE) {
        int append = (tokens->token_type == OUTPUT_REDIRECTION_APPEND_MODE);
        tokens = tokens->next;
        if (tokens && (tokens->token_type == WORD ||
                       tokens->token_type == DOUBLE_QUOTED_STRING ||
                       tokens->token_type == SINGLE_QUOTED_STRING)) {
          if (output_count >= max_outputs - 1) {
            max_outputs *= 2;
            cmd->output_redirect =
                resize_array(cmd->output_redirect, output_count, max_outputs);
            cmd->append_modes =
                resize_int_array(cmd->append_modes, output_count, max_outputs);
            if (!cmd->output_redirect || !cmd->append_modes) {
              free_command(cmd);
              return NULL;
            }
          }
          cmd->output_redirect[output_count] = strdup(tokens->value);
          cmd->append_modes[output_count] = append;
          output_count++;
        } else {
          free_command(cmd);
          return NULL;
        }
      } 
      else if (tokens->token_type == HERE_DOC) {
        tokens = tokens->next;
        if (tokens && (tokens->token_type == WORD ||
                       tokens->token_type == DOUBLE_QUOTED_STRING ||
                       tokens->token_type == SINGLE_QUOTED_STRING)) {
          if (heredoc_count >= max_heredocs - 1) {
            max_heredocs *= 2;
            cmd->here_docs =
                resize_array(cmd->here_docs, heredoc_count, max_heredocs);
            if (!cmd->here_docs) {
              free_command(cmd);
              return NULL;
            }
          }
          cmd->here_docs[heredoc_count++] = strdup(tokens->value);
          // cmd->here_docs[heredoc_count++] = NULL;
        } else {
          printf("\nError: No file specified for heredoc redirection\n");
          free_command(cmd);
          return NULL;
        }
        cmd->herdoc_last = 1;
      }

      tokens = tokens->next;
    }

    // Null-terminate arrays
    cmd->args[arg_count] = NULL;
    cmd->input_redirect[input_count] = NULL;
    cmd->output_redirect[output_count] = NULL;
    cmd->here_docs[heredoc_count] = NULL;

    if (!head) {
      head = cmd;
    } else {
      current->next = cmd;
    }
    current = cmd;

    if (tokens && tokens->token_type == PIPES) {
      tokens = tokens->next;
    }
  }

  return head;
}

token *tokenize_input(const char *input, char **env)
{
  token *head = NULL;
  token *current = NULL;
  const char *str = input;

  if (!str)
      return NULL ;
  while (*str) {
    // Skip leading whitespace
    while (isspace(*str))
      str++;

    if (*str == '\0')
      break;

    char *token_value = NULL;
    token_type type; // Declare a variable to hold the token type

    // Get the next token (handles quoted strings)
    token_value =
        next_token(&str, env, &type); // Pass the type variable to next_token

    if (token_value == NULL)
      continue; // Skip invalid or empty tokens

    // Create new token node
    token *new_token = malloc(sizeof(token));
    new_token->value = token_value;
    new_token->token_type = type;
    new_token->next = NULL;

    // Append to the token list
    if (!head)
      head = new_token;
    else
      current->next = new_token;
    current = new_token;
  }

  return head;
}
//******************[ok]*************///Line : 58
char *handle_quoted_string_with_expansion(char *str, char **env)
{
  // int i = 0;
  //  t_env *current = env_list;
  // t_env *previous = NULL;
  // while (current)
  // {
  //   previous = current;
  //   current = current->next;
  // }
  char *expanded_str = malloc(strlen(str) * 2 + 1);
  if (!expanded_str)
    return NULL;

  char *current2 = expanded_str;

  while (*str) {
     if (*str == '\\') {
      str++;
      if (*str == '$' || *str == '`' || *str == '"' || *str == '\\') 
      {
         *current2++ = *str++;
      }
      else 
      {
        *current2++ = '\\';
        if (*str)
          *current2++ = *str++;
      }
    } else if (*str == '$')
    {
      printf("Detected variable expansion: %s\n", str);
      str++; // Skip the '$'
      char *var_start = str;
       while (*str && (isalnum(*str) || *str == '_'))
        str++;
      char var_name[256];
      strncpy(var_name, var_start, str - var_start);
      var_name[str - var_start] = '\0';
       char *expanded_value = expand_variable(var_name, env);
       if (expanded_value)
       {
        printf("Expanded value: %s\n", expanded_value);
        strcpy(current2, expanded_value);
        current2 += strlen(expanded_value);
        free(expanded_value);
      } 
      else 
      {
        printf("Variable '%s' not found\n", var_name);
      }
    } else {

      *current2++ = *str++;
    }
  }
   *current2 = '\0';
   return expanded_str;
}

char *next_tok2(const char *start ,const char **str, token_type *type)
{
  if (strncmp(start, "echo", 4) == 0 && (isspace(start[4]) || start[4] == '\0')) 
  {
    *str += 4;
    // char *remain = strdup(str[0]);
    // echo_exit_status(remain);
    *type = WORD;

    return strdup("echo");
  }
  if (strncmp(start, "unset", 5) == 0 && (isspace(start[5]) || start[5] == '\0')) {
    *str += 5; // Move past 'unset'
    printf("\nunset found\n");

    // Skip any leading whitespace
    while (isspace(**str))
        (*str)++;

    // Process all variable names after 'unset'
    while (**str && !isspace(**str)) {
        // Find the end of the current variable name
        const char *variable_start = *str;
        while (**str && !isspace(**str))
            (*str)++;

        // Extract the variable name
        size_t len = *str - variable_start;
        if (len > 0) {
            char *variable_name = malloc(len + 1);
            strncpy(variable_name, variable_start, len);
            variable_name[len] = '\0';

            // Call handle_unset for each variable name
            printf("\nUnsetting variable: [%s]\n", variable_name);
            handle_unset(variable_name);

            free(variable_name);
        }

        // Skip any additional spaces between variables
        while (isspace(**str))
            (*str)++;
    }

    *type = WORD;
    return strdup("unset");
}

// if (strncmp(start, "unset", 5) == 0 && (isspace(start[5]) || start[5] == '\0'))
//     {
//       *str += 5;  // Move past 'unset'    
//       printf("\nunset found\n");
//       // Skip any leading whitespace
//       while (isspace(**str))
//           (*str)++;
//        const char *variable_name = *str; 
//              printf("\nunseting found: [%s]\n",variable_name);
//       if (variable_name)
//       {       printf("\nok___unseting  \n");
//           handle_unset(variable_name);
//       }
//           *type = WORD;
//       return strdup("unset");
//     }
  if (strncmp(start, "export", 6) == 0 && (isspace(start[6]) || start[6] == '\0')) 
  {
    *str += 6;
    // char *remain = strdup(str[0]);
    char *remain = strdup(*str);
    printf("\n@(%s)\n", remain);
    if(remain!=NULL)
     {
         handle_export_command(remain);
     }
      *type = WORD;
    return strdup("export");
  }
    return NULL; // Return NULL to indicate no result
}
/***************************************************/

char *next_tok3(const char **str, char **env, token_type *type)
{
   char quote = **str; // Store the quote type (' or ")
    (*str)++;           // Move past the opening quote

    const char *quoted_start = *str;

     while (**str && **str != quote)
        (*str)++;

    size_t len = *str - quoted_start; // Calculate the length of the quoted content
    char *token_value = malloc(len + 1);
    strncpy(token_value, quoted_start, len);
    token_value[len] = '\0'; // Null-terminate the extracted token
        printf("\n___Xtok3X %s : \n",token_value);
    if (**str == quote)
        (*str)++; // Move past the closing quote

    if (quote == '\'') 
    {
        *type = SINGLE_QUOTED_STRING;
        return token_value; // Single quotes: return the content as-is
    } 
    else 
    {
       *type = DOUBLE_QUOTED_STRING;
        char *expanded_token = handle_quoted_string_with_expansion(token_value, env);
        free(token_value);
        return expanded_token; // Double quotes: expand variables
    }
    return NULL; // Return NULL to indicate no result
}

char *next_tok4(const char **str, char **env)
{
     (*str)++; // Move past the '$'
    const char *var_start = *str;

     while (isalnum(**str) || **str == '_' || **str == '?')
        (*str)++;

    size_t len = *str - var_start; // Length of the variable name
    char var_name[len + 1];
    strncpy(var_name, var_start, len);
    var_name[len] = '\0'; // Null-terminate the variable name

    // Expand the variable
    char *expanded_value = expand_variable(var_name, env);
    if (expanded_value) 
    {
        return expanded_value;
    } 
    else 
    {
        return strdup(""); // Return an empty string if the variable doesn't exist
    }
        return NULL; // Return NULL to indicate no result
}
char *next_tok5(const char **str, token_type *type)
{
    char symbol = **str;  
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

     char temp[2] = {symbol, '\0'};
    if (symbol == '>')
        *type = OUTPUT_REDIRECTION;
    else if (symbol == '<')
        *type = INPUT_REDIRECTION;
    else if (symbol == '|')
        *type = PIPES;

    return strdup(temp);
}
/***************************************************/
char *next_token(const char **str, char **env, token_type *type)
 {
   while (isspace(**str))
    (*str)++;
  if (**str == '\0')
    return NULL;
  const char *start = *str;
   next_tok2(start,str,type);
    if (**str == '"' || **str == '\'') 
        return next_tok3(str, env, type);
   if (**str == '$') 
        return next_tok4(str, env);
    if (**str == '>' || **str == '<' || **str == '|') 
        return next_tok5(str, type);
  while (**str && !isspace(**str) && **str != '>' && **str != '<' &&
         **str != '|' && **str != '\'' && **str != '"')
    (*str)++;
  size_t len = *str - start;
  char *token_value = malloc(len + 1);
  strncpy(token_value, start, len);
  token_value[len] = '\0';
  if (token_value[0] == '$')
    *type = EXPANDABLE;
  else
    *type = WORD;
  return token_value;
}
