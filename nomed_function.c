#include "../../include/minishell.h"
#define INITIAL_REDIRECT_SIZE 4 // Initial size for redirection arrays
#define INITIAL_ARGS_SIZE 16


t_command *initialize_command() 
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
    cmd->here_doc_last_fd = 0;
    cmd->next = NULL;

    if (!cmd->args || !cmd->input_redirect || !cmd->output_redirect || 
        !cmd->here_docs || !cmd->append_modes) {
        free_command(cmd);  // Ensure to clean up properly
        return NULL;
    }

    return cmd;
}


void parse_token(t_command *cmd, token **tokens, char **env) 
{
    if (!cmd || !tokens || !*tokens)
        return;

    if ((*tokens)->token_type == WORD || 
        (*tokens)->token_type == DOUBLE_QUOTED_STRING || 
        (*tokens)->token_type == SINGLE_QUOTED_STRING) {
        // Handle command and arguments
        if (!cmd->command) {
            cmd->command = strdup((*tokens)->value);
            cmd->args[0] = strdup((*tokens)->value);
        } else {
            int arg_count = 0; // Track args here or pass as arguments
            cmd->args[arg_count++] = strdup((*tokens)->value);
        }
    } 
    else if ((*tokens)->token_type == INPUT_REDIRECTION) {
        // Handle input redirection
        (*tokens) = (*tokens)->next;  // Move to the file token
        if (*tokens) {
            int input_count = 0; // Track inputs here or pass as arguments
            cmd->input_redirect[input_count++] = strdup((*tokens)->value);
        }
    }
 
    (*tokens) = (*tokens)->next;  // Advance to the next token
}

void finalize_command(t_command *cmd, int arg_count, int input_count, int output_count, int heredoc_count) 
{
    if (!cmd)
        return;

    cmd->args[arg_count] = NULL;
    cmd->input_redirect[input_count] = NULL;
    cmd->output_redirect[output_count] = NULL;
    cmd->here_docs[heredoc_count] = NULL;
}
int handle_input_redirection(t_command *cmd, token **tokens, int *input_count, int *max_inputs)
 {
    if ((*tokens)->token_type == INPUT_REDIRECTION) {
        *tokens = (*tokens)->next;
        if (*tokens && ((*tokens)->token_type == WORD || (*tokens)->token_type == DOUBLE_QUOTED_STRING || (*tokens)->token_type == SINGLE_QUOTED_STRING)) {
            if (*input_count >= *max_inputs - 1) {
                *max_inputs *= 2;
                cmd->input_redirect = resize_array(cmd->input_redirect, *input_count, *max_inputs);
                if (!cmd->input_redirect) {
                    return 0;  // Memory allocation failure
                }
            }
            cmd->input_redirect[(*input_count)++] = strdup((*tokens)->value);
            return 1;
        } else {
            printf("\nError: No file specified for input redirection\n");
            return 0;
        }
    }
    return 1;  // No redirection
}

int handle_output_redirection(t_command *cmd, token **tokens, int *output_count, int *max_outputs) {
    if ((*tokens)->token_type == OUTPUT_REDIRECTION || (*tokens)->token_type == OUTPUT_REDIRECTION_APPEND_MODE) {
        int append = (*tokens)->token_type == OUTPUT_REDIRECTION_APPEND_MODE;
        *tokens = (*tokens)->next;
        if (*tokens && ((*tokens)->token_type == WORD || (*tokens)->token_type == DOUBLE_QUOTED_STRING || (*tokens)->token_type == SINGLE_QUOTED_STRING)) {
            if (*output_count >= *max_outputs - 1) {
                *max_outputs *= 2;
                cmd->output_redirect = resize_array(cmd->output_redirect, *output_count, *max_outputs);
                cmd->append_modes = resize_int_array(cmd->append_modes, *output_count, *max_outputs);
                if (!cmd->output_redirect || !cmd->append_modes) {
                    return 0;  // Memory allocation failure
                }
            }
            cmd->output_redirect[*output_count] = strdup((*tokens)->value);
            cmd->append_modes[*output_count] = append;
            (*output_count)++;
            return 1;
        } else {
            return 0;  // Missing file for redirection
        }
    }
    return 1;  // No redirection
}

int handle_heredoc_redirection(t_command *cmd, token **tokens, int *heredoc_count, int *max_heredocs)
 {
    if ((*tokens)->token_type == HERE_DOC) {
        *tokens = (*tokens)->next;
        if (*tokens && ((*tokens)->token_type == WORD || (*tokens)->token_type == DOUBLE_QUOTED_STRING || (*tokens)->token_type == SINGLE_QUOTED_STRING)) {
            if (*heredoc_count >= *max_heredocs - 1) {
                *max_heredocs *= 2;
                cmd->here_docs = resize_array(cmd->here_docs, *heredoc_count, *max_heredocs);
                if (!cmd->here_docs) {
                    return 0;  // Memory allocation failure
                }
            }
            cmd->here_docs[(*heredoc_count)++] = strdup((*tokens)->value);
            return 1;
        } else {
            printf("\nError: No file specified for heredoc redirection\n");
            return 0;
        }
    }
    return 1;  // No here-doc
}



t_command *parse_command(token *tokens)
{
  char **env=NULL;
    if (!tokens)
        return NULL;

    t_command *head = NULL;
    t_command *current = NULL;

    while (tokens)
     {
        t_command *cmd = initialize_command();
        if (!cmd)
            return NULL;

        int arg_count = 0, max_args = INITIAL_ARGS_SIZE;
        int input_count = 0, max_inputs = INITIAL_REDIRECT_SIZE;
        int output_count = 0, max_outputs = INITIAL_REDIRECT_SIZE;
        int heredoc_count = 0, max_heredocs = INITIAL_REDIRECT_SIZE;
  while (tokens && tokens->token_type != PIPES) 
    {
       // Handle command name
 
            if (!cmd->command && (tokens->token_type == WORD || tokens->token_type == DOUBLE_QUOTED_STRING || tokens->token_type == SINGLE_QUOTED_STRING))
            {
                    cmd->command = strdup(tokens->value);
                    cmd->args[arg_count++] = strdup(cmd->command);
                    tokens = tokens->next;
                    continue;
            }

               //    printf("\n<<<<<<<<<<<<<<<here_2>>>>>>>>>>>>>>>>\200");
              //     single-quoted strings (no expansion)
            if (tokens->token_type == SINGLE_QUOTED_STRING)
            {
                       printf("\n@@\n");
                // Add as-is to the arguments
                cmd->args[arg_count++] = strdup(tokens->value);
                tokens = tokens->next;
                continue;
            }
            // Handle arguments and handle redirection
        else if (tokens->token_type == SINGLE_QUOTED_STRING)
        {
            cmd->args[arg_count++] = strdup(tokens->value);
        }

      if (tokens->token_type == WORD ||  tokens->token_type == DOUBLE_QUOTED_STRING || tokens->token_type == SINGLE_QUOTED_STRING) 
      {
        char *arg_value = tokens->value;
        // Check for variable expansion
        if (arg_value[0] == '$') 
        {
          char *var_name = arg_value + 1; // Skip the '$'
          char *expanded_value = expand_variable(var_name, env);
          if (expanded_value)
          {
            arg_value = expanded_value;
          }
        }
        if (arg_count >= max_args - 1)
        {
          max_args *= 2;
          cmd->args = resize_array(cmd->args, arg_count, max_args);
          if (!cmd->args) {
            free_command(cmd);
            return NULL;
          }
        }
        cmd->args[arg_count++] = strdup(arg_value);
      }
      // Handle input redirection
      if (!handle_input_redirection(cmd, &tokens, &input_count, &max_inputs))
        {
            free_command(cmd);
            return NULL;
        }
         if (!handle_output_redirection(cmd, &tokens, &output_count, &max_outputs)) {
            free_command(cmd);
            return NULL;
        }
        if (!handle_heredoc_redirection(cmd, &tokens, &heredoc_count, &max_heredocs)) 
        {
            free_command(cmd);
            return NULL;
        }
      tokens = tokens->next;
    }
        finalize_command(cmd, arg_count, input_count, output_count, heredoc_count);
        if (!head)
             head = cmd;
         else
             current->next = cmd;
         current = cmd;
        if (tokens && tokens->token_type == PIPES)
             tokens = tokens->next;
     }
    return head;
}
