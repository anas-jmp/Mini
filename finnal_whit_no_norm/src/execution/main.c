#include "../../include/minishell.h"

pid_t first_child_processe(t_command *prompt, char *cmd_path, char **argv,
                           char **env, int **pipefd, int j) {
  pid_t pid;
  int i;
  int fd;
  int k;
  int fd_out;

  fd = STDIN_FILENO;
  fd_out = STDOUT_FILENO;
  k = 0;
  if (!strcmp(prompt->command, "cd")) {
    ft_cd((prompt->args)[1], env);
    return 0;
  }
  i = 0;
  pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if (!pid) {
    fd_out = pipefd[j][1]; 
    close(pipefd[j][0]);
    if (prompt->herdoc_last){
      if (prompt->input_redirect) {
        while ((prompt->input_redirect)[k]) {
          fd = open(prompt->input_redirect[k], O_RDONLY);
          k++;
        }
      }
      k = 0;
      if (prompt->here_doc_last_fd)
        fd = prompt->here_doc_last_fd ;
    } else{
      if (prompt->here_doc_last_fd)
        fd = prompt->here_doc_last_fd ;
      k = 0;
      if (prompt->input_redirect) {
        while ((prompt->input_redirect)[k]) {
          fd = open(prompt->input_redirect[k], O_RDONLY);
          k++;
        }
      }
    }
    k = 0;
    if (prompt->output_redirect) {
      while ((prompt->output_redirect)[k]) {
          if (prompt->append_modes[i])
                fd_out = open(prompt->output_redirect[i], O_RDWR | O_CREAT | O_APPEND, 0644);
            else
                fd_out = open(prompt->output_redirect[i], O_RDWR | O_CREAT, 0644);
        k++;
      }
    }
    dup2(fd_out, STDOUT_FILENO);
    dup_fds(fd, STDIN_FILENO);

    while (i < j) {
      close(pipefd[i][0]);
      close(pipefd[i][1]);
      i++;
    }
    execute_command(cmd_path, argv, env);
  }
  return pid;
}

pid_t middle_child_processes(t_command *prompt, char *cmd_path, char **argv,
                             int **pipefd, char **env, int j) {
  pid_t pid;
  int i;
  int fd;
  int k;
  int fd_out ; 

  k = 0;
  i = 0;

  fd = STDIN_FILENO ; 
  fd_out = STDOUT_FILENO ; 

  if (!strcmp(prompt->command, "cd")) {
    ft_cd((prompt->args)[1], env);
    return 0;
  }

  pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if (!pid) {



    fd_out = pipefd[j][1];
    fd = pipefd[j - 1][0]; 
    if (prompt->herdoc_last) {
      if (prompt->input_redirect) {
        while ((prompt->input_redirect)[k]) {
          fd = open(prompt->input_redirect[k], O_RDONLY);
          k++;
        }
      }
      k = 0;
      if (prompt->here_doc_last_fd)
        fd = prompt->here_doc_last_fd ;
    } else {
      if (prompt->here_doc_last_fd)
        fd = prompt->here_doc_last_fd ;
      k = 0;
      if (prompt->input_redirect) {
        while ((prompt->input_redirect)[k]) {
          fd = open(prompt->input_redirect[k], O_RDONLY);
          k++;
        }
      }
    }
    k = 0;
    if (prompt->output_redirect) {
      while ((prompt->output_redirect)[k]) {
          if (prompt->append_modes[i])
                fd_out = open(prompt->output_redirect[i], O_RDWR | O_CREAT | O_APPEND, 0644);
            else
                fd_out = open(prompt->output_redirect[i], O_RDWR | O_CREAT, 0644);
        k++;
      }
    }
    dup_fds(fd_out, STDOUT_FILENO);
    dup_fds(fd, STDIN_FILENO);
    while (i < j) {
      close(pipefd[i][0]);
      close(pipefd[i][1]);
      i++;
    }
    execute_command(cmd_path, argv, env);
  }
  return pid;
}

pid_t ending_child_processe(t_command *prompt, char *cmd_path, char **argv,
                            int **pipefd, char **env, int j) {
  pid_t pid;
  int i;
  int fd;
  int k;
  int fd_out;


  fd = STDIN_FILENO ; 
  fd_out = STDOUT_FILENO ; 

  k = 0;
  if (!strcmp(prompt->command, "cd")) {
    ft_cd((prompt->args)[1], env);
    return 0;
  }
  i = 0;
  pid = fork();
  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  }
  if (!pid) {
    fd = pipefd[j - 1][0] ;


    if (prompt->herdoc_last) {
      if (prompt->input_redirect) {
        while ((prompt->input_redirect)[k]) {
          fd = open(prompt->input_redirect[k], O_RDONLY);
          k++;
        }
      }
      k = 0;
      if (prompt->here_doc_last_fd)
        fd = prompt->here_doc_last_fd ;
    } else {
      if (prompt->here_doc_last_fd)
        fd = prompt->here_doc_last_fd ;
      k = 0;
      if (prompt->input_redirect) {
        while ((prompt->input_redirect)[k]) {
          fd = open(prompt->input_redirect[k], O_RDONLY);
          k++;
        }
      }
    }
    k = 0;
    if (prompt->output_redirect) {
      while ((prompt->output_redirect)[k]) {
       if (prompt->append_modes[i])
                fd_out = open(prompt->output_redirect[i], O_RDWR | O_CREAT | O_APPEND, 0644);
            else
                fd_out = open(prompt->output_redirect[i], O_RDWR | O_CREAT, 0644);
        k++;
      }
    }
    dup_fds(fd, STDIN_FILENO); 
    dup_fds(fd_out, STDOUT_FILENO);

    while (i < j) {
      close(pipefd[i][0]);
      close(pipefd[i][1]);
      i++;
    }
    execute_command(cmd_path, argv, env);
  }
  return pid;
}

void is_exit_with_signal(int *status)
{
  char *num;

  if (WIFEXITED(*status))
    *status = WEXITSTATUS(*status);
  else if (WIFSIGNALED(*status))
    *status = WTERMSIG(*status) + 128;
num = ft_itoa(*status);   
  store_env_variable("?",num);
}

int pipex(t_command *prompt, char **env) {
  int i;
  int j;
  char *cmd_path;
  int **fds;
  int status;
  int final_pid;
  int pid;
  int final_status;
  int lst_size;
  int fd;
  char *str=NULL;
  char **my_env = handle_env(str, env_list);
    env = my_env;

  if (!prompt)
      return 0;

  i = 0;
  heredocs_fds(prompt);

  lst_size = ft_lstsize(prompt);
  fds = malloc(sizeof(int *) * (lst_size));
  j = 0;
  while (j < lst_size) {
    fds[j] = malloc(sizeof(int) * 2);
    j++;
  }
  j = 0;
  if (lst_size == 1 ) {
    if (!prompt->command)
        return 0;
    cmd_path = is_command(prompt->command, env);
    if (!cmd_path)
      cmd_path = prompt->command;
    if (!strcmp(prompt->command, "cd")) {
      ft_cd((prompt->args)[1], env);
      return 0;
    }
    pid = fork();
    if (!pid) {

      if (prompt->herdoc_last) {
        if (prompt->input_redirect) {
          while ((prompt->input_redirect)[i]) {
            fd = open(prompt->input_redirect[i], O_RDONLY);
            i++;
          }
        }
        i = 0;
        if (prompt->here_doc_last_fd)
          fd = prompt->here_doc_last_fd ;
        if (prompt->here_docs[0] || prompt->input_redirect[0]) {
          dup_fds(fd, STDIN_FILENO);
        }
      } else {
        if (prompt->here_doc_last_fd)
          fd = prompt->here_doc_last_fd ;
        i = 0;
        if (prompt->input_redirect) {
          while ((prompt->input_redirect)[i]) {
            fd = open(prompt->input_redirect[i], O_RDONLY);
            i++;
          }
        }
        if (prompt->here_docs[0] || prompt->input_redirect[0]) {
          dup_fds(fd, STDIN_FILENO);
        }
      }
      i = 0;
      if (prompt->output_redirect) {
        while ((prompt->output_redirect)[i]) {
            if (prompt->append_modes[i])
                fd = open(prompt->output_redirect[i], O_RDWR | O_CREAT | O_APPEND, 0644);
            else
                fd = open(prompt->output_redirect[i], O_RDWR | O_CREAT, 0644);
          dup2(fd, STDOUT_FILENO);
          i++;
        }
      }
      execute_command(cmd_path, prompt->args, env);
    }
    wait(NULL);
    if (prompt->here_docs[0])
      unlink(".heredoc");
    return 0;
  }
  if (pipe(fds[j]) < 0)
    output_error_exit("pipe", EXIT_FAILURE);
  if (prompt->command)
    cmd_path = is_command(prompt->command, env);
  if (!cmd_path)
    cmd_path = prompt->command;
  if (prompt->command)
    first_child_processe(prompt, cmd_path, prompt->args, env, fds, j);
  j++;
  prompt = prompt->next;
  while (j < lst_size - 1) {
    if (prompt->command)
      cmd_path = is_command(prompt->command, env);
    if (!cmd_path)
      cmd_path = prompt->command;
    if (pipe(fds[j]) < 0)
      output_error_exit("pipe", EXIT_FAILURE);
    if (prompt->command)
      middle_child_processes(prompt, cmd_path, prompt->args, fds, env, j);
    j++;
    prompt = prompt->next;
  }
  if (prompt->command)
    cmd_path = is_command(prompt->command, env);
  if (!cmd_path) {
    cmd_path = prompt->command;
  }
  if (pipe(fds[j]) < 0)
    output_error_exit("pipe", EXIT_FAILURE);
  if (prompt->command)
    final_pid = ending_child_processe(prompt, cmd_path, prompt->args, fds, env, j);
  j = 0;
  while (j < lst_size) {
    close(fds[j][0]);
    close(fds[j][1]);
    j++;
  }
  while ((pid = wait(&status)) != -1)
  {
    if (pid == final_pid) {
      final_status = status;
      is_exit_with_signal(&final_status);
    }
  }
  return 0;
}

// int main(int argc, char **argv, char **env) {

//   char *input = NULL;
//   token *tokens = NULL;
//   t_command *cmd_list = NULL;

//   (void)argc;
//   (void)argv;

//   while (1337) {
//     input = readline("minishell(*-*)> ");
//     tokens = tokenize_input(input, env);
//     cmd_list = parse_command(tokens, env);
//     pipex(cmd_list, env);
//     free(input);
//     input = NULL;
//   }
//   return 0;
// }



int main(int argc, char **argv, char **env)
{

  char *input = NULL;
  token *tokens = NULL;
  t_command *cmd_list = NULL;


  (void)argc;
  (void)argv;
 
       signals();
 
  while (1337) 
  { 
      //  input = readline("minishell(*-_-*)>");
        input =  readline("\x1B[31mMini_shell\x1B[33m(*-_-*)\x1B[32m>\x1B[37m");  // Prompt for input
   
    if (!input)
     {
            printf("exit\n");
            continue;
          ///  break; // Handle Ctrl+D
     }                
 
      char unmatched = check_unmatched_quotes(input);
        if (unmatched) 
        {
            printf("syntax error: unmatched %c\n", unmatched);
            free(input);
            continue; // Skip the rest of the loop and prompt again
        }
    tokens = tokenize_input(input, env);
      if (validate_syntax(tokens) == -1)
      {
            //dont forget to free tokens
            free(input);
            continue;
      }
    cmd_list = parse_command(tokens, env);
    if (cmd_list && strcmp(cmd_list->command, "exit") == 0) 
        {
            handle_exit(cmd_list->args); // Handle the exit command
            printf("\n--------------\n");
            printf("\nexit found\n");
            //break;
           // continue;
        }
    pipex(cmd_list, env);
    free(input);
    input = NULL;
  }
  return 0;
}
