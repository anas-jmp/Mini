#include "../../include/minishell.h"

/********************** CHECK UNMATCHED QUOTES **********************/
int check_unmatched_quotes(char *input) 
{
    char quote_char = '\0'; 
    int i = 0;

    while (input[i])
    {
        
        if (!quote_char && (input[i] == '\'' || input[i] == '"')) 
        {
            quote_char = input[i]; 
        } 
         
        else if (quote_char && input[i] == quote_char) 
        {
            quote_char = '\0'; 
        }
        i++;
    }

     return quote_char ? 1 : 0;
}

 
int validate_syntax(token *tokens) 
{
    token *prev = NULL;

    while (tokens) 
    {
        //check for invalid heredoc usage
        if (tokens->token_type == HERE_DOC) 
        {
    /*ensure the next token is a valid delimiter (WORD, SINGLE_QUOTED_STRING, DOUBLE_QUOTED_STRING)*/
            if (!tokens->next) 
            {
                printf("syntax error near unexpected token `newline'\n");
                return -1; // Return error
            }

            if (tokens->next->token_type != WORD &&
                tokens->next->token_type != SINGLE_QUOTED_STRING &&
                tokens->next->token_type != DOUBLE_QUOTED_STRING) 
            {
                printf("syntax error near unexpected token `%s'\n", tokens->next->value);
                return -1;  
            }
        }

        // check for consecutive pipes in the beginning
        if (tokens->token_type == PIPES) 
        {
            if (!prev || prev->token_type == PIPES) 
            {
                printf("syntax error near unexpected token `|'\n");
                return -1;  
            }
        }

        /* Checking for invalid redirection*/
        if (tokens->token_type == INPUT_REDIRECTION ||
            tokens->token_type == OUTPUT_REDIRECTION ||
            tokens->token_type == OUTPUT_REDIRECTION_APPEND_MODE) 
        {
            if (!tokens->next) 
            {
                printf("syntax error near unexpected token `newline'\n");
                return -1; // Return error
            }

            /*ensure the next token is valid (WORD, quoted strings)*/
            if (tokens->next->token_type != WORD &&
                tokens->next->token_type != SINGLE_QUOTED_STRING &&
                tokens->next->token_type != DOUBLE_QUOTED_STRING) 
            {
                printf("syntax error near unexpected token `%s'\n", tokens->next->value);
                return -1; // Return error
            }
        }

        /*here check for trailing or invalid tokens*/
        if (tokens->token_type != WORD &&
            tokens->token_type != EXPANDABLE &&
            tokens->token_type != HERE_DOC &&
            tokens->token_type != PIPES &&
            tokens->token_type != INPUT_REDIRECTION &&
            tokens->token_type != OUTPUT_REDIRECTION &&
            tokens->token_type != OUTPUT_REDIRECTION_APPEND_MODE &&
            tokens->token_type != SINGLE_QUOTED_STRING &&
            tokens->token_type != DOUBLE_QUOTED_STRING) 
        {
            printf("syntax error near unexpected token `%s'\n", tokens->value);
            return -1; // Return error
        }

        prev = tokens;
        tokens = tokens->next;
    }

   /* check if the last token is invalid (e.g., a trailing pipe)*/
    if (prev && prev->token_type == PIPES) 
    {
        printf("syntax error near unexpected token `|'\n");
        return -1;
    }
    return 0;
}