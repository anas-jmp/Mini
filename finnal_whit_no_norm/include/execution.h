#include "minishell.h"

#ifndef EXEC_H
# define EXEC_H

void output_redirection(char *file_path); 
 void output_redirection_append_mode(char *file_path );
void output_error_exit(char *error_msg , int status);
void  input_redirection (char *file_path );
int redidrect_stdin_to_herdoc(char *delimiter , int heredoc_fd);
char	*is_command(char *cmd, char **env);
char **ft_unset(char **env  , char *var);
void execute_command(char *command_path , char **argv , char **env);
void dup_fds(int old_fd , int new_fd);
char ft_cd(char* path , char **env);
void ft_env(char **envp);
void ft_echo(int line , char *str);
char	*is_command(char *cmd, char **env) ;
void heredocs_fds(t_command *prompt);
void handle_exit(char **args);
void signals();


#endif