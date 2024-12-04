# ifndef MINI_SHELL_H
# define MINI_SHELL_H
# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <readline/readline.h>
# include <readline/history.h>
# include <string.h>
# include <fcntl.h>
# include <sys/stat.h>
#include <ctype.h>  
#include <stdio.h>
 
// token.h

typedef enum {
    WORD,
    EXPANDABLE,
    INPUT_REDIRECTION,
    OUTPUT_REDIRECTION,
    OUTPUT_REDIRECTION_APPEND_MODE,
    PIPES,
    SINGLE_QUOTED_STRING,
    DOUBLE_QUOTED_STRING,
    HERE_DOC
} token_type;


typedef struct token 
{
    char *value;
    token_type token_type;
    struct token *next;
} token;

typedef struct s_env
{
    char *key;
    char *value;
    struct s_env *next;
} t_env;

// char *handle_echo_command(char *input, char **env);
  char	*ft_itoa(int nb);
// pid_t ending_child_processe(t_command *prompt , char *cmd_path , char **argv , int **pipefd , char **env , int j );
void display_ascii_art(void);
 char *expand_variable(char *var_name, char **env,int *status);
int validate_syntax(token *tokens);
void echo_exit_status(const char *input);
int check_unmatched_quotes(char *input);
char **handle_env(const char *str, t_env *env_list); 
char *next_token(const char **str, char **env, token_type *type);
extern t_env *env_list;
/******************************************/
char  *get_redirection_file(const char *command);
int   count_env_vars(t_env *env_list);
void  print_env(t_env *env_list);
char  **build_envp_array(t_env *env_list);
void  execute_command_fa(char *command_path, char **argv, t_env *env_list);
void  execute_env_command(t_env *env_list, const char *output_file); 
/**********************************************/
void split_key_value(char *input, char **key, char **value);
void handle_unset(const char *key); 
void handle_export_command(char *input, char **env);
void store_env_variable(char *key, char *value);
void free_command(t_command *cmd);
char *handle_quoted_string_with_expansion(char *str, char **env);
char *extract_quoted_string(const char **str, char **env);
void handle_expand_command(char *input, char **env);
/**********************************************/
char	*ft_strjoin(char const *s1, char const *s2);
void    is_exit_with_signal(int *status);
int     pipex(t_command *prompt , char **env);
void    dup_fds(int old_fd , int new_fd);
void output_redirection(char *file_path);
int redidrect_stdin_to_herdoc(char *delimiter , int heredoc_fd) ;
void execute_command(char *command_path , char **argv , char **env);
void output_error_exit(char *error_msg , int status);
char	*is_command(char *cmd, char **env);
//char ft_cd(char* path);
char	*cmd_helper(char *cmd, char *PATH_VARIABLE);
char	*get_command_path(char *cmd, char *PATH_VARIABLE);
void 	ft_free_split(char **split);
void handle_echo_command(char *input, char **env);
/**** */
t_command *parse_command(token *tokens, char **env);
int validate_syntax(token *tokens);
void print_parsed_command(t_command *cmd);
void execute_pipeline(t_command *cmd_list);
 
/* Lexer and parser functions */
    typedef int (*redir_func_ptr)(char *token);
// token *tokenizer(char *prompt);
token *tokenizer(char *prompt, char **env);
token *tokenize_input(const char *input, char **env);
/* Function declarations */
void split_key_value(char *input, char **key, char **value);
token_type identify_token_type(char *token_value);
char *extract_quoted_string(const char **str, char **env);
// print_parsed_command
void execute_pipeline(t_command *cmd_list);
// void handle_echo_command(char *input, char **env);
void     free_parsed_command(t_command *cmd);
void     free_token_list(token *head);
void     shell_loop(char **env) ;
void     print_tokens(token *head);
void     free_token_list(token *head);
token    *setup_first_node(char **split_prompt, redir_func_ptr *redirection_functions);
void     setup_nodes(char **split_prompt, redir_func_ptr *redirection_functions, token **first_node, int i);
char	 **ft_split(const char *str, char *delimiters);


int     is_INPUT_REDIRECTION(char *token);
int     is_OUTPUT_REDIRECTION_APPEND_MODE(char *token);
int     is_OUTPUT_REDIRECTION(char *token);
int     is_PIPES(char *token);
int     is_WORD(char *token);

 
// Parse tokens into a t_command structure
t_command *parse_command(token *tokens, char **env);
// Resize the arguments array dynamically
char **resize_args(char **args, int current_size, int new_size);
// Free memory associated with a t_command structure
void free_command(t_command *cmd);
// Handle input redirection, output redirection, and append mode
void handle_redirections(t_command *cmd);
// Debugging utility for tokens (optional)
void print_tokens(token *head);

#endif