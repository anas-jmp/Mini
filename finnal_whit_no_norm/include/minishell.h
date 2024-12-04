#ifndef MINISHELL_H
# define MINISHELL_H


typedef struct s_command
{
    char *command;            // The command to execute (e.g., "echo", "cat", etc.)
    char **args;              // Arguments for the command
    char **input_redirect;    // Array of input redirection file paths
    char **output_redirect;
    char **here_docs;   // Array of output redirection file paths
    int *append_modes;        // Array of append mode flags (1 for append, 0 for normal)
    int input_count;          // Number of input redirections
    int output_count;    
    int herdoc_last;     // is heredoc after input redirection
    int here_doc_last_fd ;
    struct s_command *next;   // Pointer to the next command (for handling pipelines)
} t_command;
#include "libft.h"
#include "parsing.h"
#include "execution.h"

#endif