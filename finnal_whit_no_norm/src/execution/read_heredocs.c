#include "../../include/minishell.h"

void heredocs_fds(t_command *prompt)
{
    int i = 0;
    int fd;

    while (prompt)
    {
        if (prompt->here_docs)
        {
            while (prompt->here_docs[i])
            {
                fd = open(".heredoc", O_RDWR | O_CREAT, 0644);
                fd = redidrect_stdin_to_herdoc(prompt->here_docs[i] , fd );
                prompt->here_doc_last_fd = fd;
                i++;

            }
            //unlink("/tmp/.heredoc");
        }
        i = 0 ;
        prompt = prompt->next;
    }
}