#include "../../include/minishell.h"


int is_numeric(const char *str)
{
    if (!str || *str == '\0') 
        return 0;  

    if (*str == '-' || *str == '+') 
        str++;  
    while (*str)
    {
        if (!isdigit(*str)) 
            return 0; 
        str++;
    }
    return 1; // All characters are numeric
}
// if multipl arg or one arg alpha retunr is : 2
// if arg no numric : 2
// if mutl arg num : 1
// if numric return : [num%256]
//
void handle_exit(char **args)
{
   //store_env_variable("?", "1"); // Default exit code
     int nbr;
     char *num;
     if(args[1]!=NULL && is_numeric(args[1])==1)
     { 
            nbr = ft_atoi(args[1]);
     }
      printf("arg  1 ---------->%s",args[1]);
      printf("\narg 2 ---------->%s",args[2]);
          /********************************/
    
    if(is_numeric(args[1]) && args[2]==NULL)
    {
        int nbr = ft_atoi(args[1]);
        printf("\nthe number is %d  \n" ,nbr);
             nbr %= 256;
           if (nbr < 0) 
           {
               nbr += 256; // Handle negative values
           }
            num = ft_itoa(nbr);
        store_env_variable("?", num);

        printf("\nexit value is (%d)  \n" ,nbr);
    }

/*******************************************/
    if (args[1]==NULL && args[2]==NULL) 
     {
         // No arguments provided: Default exit value 0
           store_env_variable("?", "0");
         printf("\nthe return is 0\n");
    }
    if(is_numeric(args[1])==0 && args[1]!=NULL&& nbr != 0 ) 
    {
        printf("\nbash: exit: %s: numeric argument required",args[1]);
        store_env_variable("?", "2");
        printf("\nthe return is 2\n");
    }
    else if((is_numeric(args[1]) && nbr > 0 ) && args[2]!=NULL)
    {
        printf("\nbash: exit: too many arguments\n");
        store_env_variable("?", "1");
        printf("\nthe return is 1\n");
    }
    /**************************************** */    
 }