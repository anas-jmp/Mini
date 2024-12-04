#include "../../include/minishell.h"

int redidrect_stdin_to_herdoc(char *delimiter , int heredoc_fd) {
  char *line;
  char *test;

  if (heredoc_fd < 0)
    output_error_exit("there was an error creating herdoc file", 1);
  while (1) {
    line = get_next_line(STDIN_FILENO);
    if (!line)
        break;
    test = strdup(line);
    test[ft_strlen(test) - 1] = '\0';
    if (!strcmp(test, delimiter)) {
      break;
    }
    write(heredoc_fd, line, ft_strlen(line));
  }
  close(heredoc_fd);
  heredoc_fd = open(".heredoc", O_RDONLY);
  return heredoc_fd;
}
/*
EOF Handling (Ctrl+D):
If get_next_line(STDIN_FILENO) returns NULL,
it means EOF has been reached (Ctrl+D).
The function cleans up the .heredoc file 
by closing it and removing it with unlink(".heredoc").
A message is printed to inform the user that heredoc ended prematurely.
The function returns -1 to indicate an interruption/error.
Memory Management:
Ensures line and test are freed properly even if EOF is encountered.
Preserves Original Structure:
The overall structure and 
logic remain unchanged except for 
the added handling of Ctrl+D.
*/


// int redidrect_stdin_to_herdoc(char *delimiter) 
// {
//   int heredoc_fd;
//   char *line;
//   char *test;

//   heredoc_fd = open(".heredoc", O_WRONLY | O_CREAT | O_TRUNC, 0644);
//   if (heredoc_fd < 0)
//     output_error_exit("there was an error creating herdoc file", 1);

//   while (1) 
//   {
//     line = get_next_line(STDIN_FILENO);
//     if (!line) {
//       // Handle Ctrl+D (EOF)
//       write(STDOUT_FILENO, "Heredoc ended prematurely (Ctrl+D).\n", 37);
//       close(heredoc_fd);
//       unlink(".heredoc"); // Clean up the temporary file
//       return -1; // Return error to indicate heredoc was interrupted
//     }

//     test = strdup(line);
//     if (!test) {
//       perror("Error duplicating line");
//       free(line);
//       close(heredoc_fd);
//       unlink(".heredoc");
//       exit(EXIT_FAILURE);
//     }
    
//     test[ft_strlen(test) - 1] = '\0';
//     if (!strcmp(test, delimiter)) 
//     {
//       free(line);
//       free(test);
//       break;
//     }
//     write(heredoc_fd, line, ft_strlen(line));
//     free(line);
//     free(test);
//   }

//   close(heredoc_fd);
//   heredoc_fd = open(".heredoc", O_RDONLY);
//   if (heredoc_fd < 0)
//   {
//     perror("Error reopening heredoc file");
//     unlink(".heredoc");
//     return -1;
//   }

//   return heredoc_fd;
// }

/***************************************************************/

/**************************************************************/


// int redidrect_stdin_to_herdoc(char *delimiter) 
// {
//   int heredoc_fd;
//   char *line;
//   char *test;

//   heredoc_fd = open(".heredoc", O_WRONLY | O_CREAT | O_TRUNC, 0644);
//   if (heredoc_fd < 0)
//     output_error_exit("there was an error creating herdoc file", 1);
//   while (1) 
//   {
//     line = get_next_line(STDIN_FILENO);
//     test = strdup(line);
//     test[ft_strlen(test) - 1] = '\0';
//     if (!strcmp(test, delimiter)) 
//     {
//       break;
//     }
//     write(heredoc_fd, line, ft_strlen(line));
//   }
//   close(heredoc_fd);
//   heredoc_fd = open(".heredoc", O_RDONLY);
//   return heredoc_fd;
// }


