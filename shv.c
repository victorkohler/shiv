
/************************************

    [THE SHiV SHELL]

    * Version: 0.0.1

   * Compilation: gcc -o shv shv.c
   * With readline: -lreadline
   * With ncurses: -lncurses

*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>
#include <readline/history.h>
#include <readline/readline.h>
#include"builtin.h"

#define SHV_VERSION "0.0.1"
#define SHV_RL_BUFSIZE 1024
#define SHV_TOK_BUFSIZE 64
#define SHV_TOK_DELIM " \t\r\n\a"


/*
SHiV Core function declarations
*/
void shv_loop(void);
char *shv_read_line(void);
char **shv_split_line(char *line);
int shv_launch(char **args);
int shv_execute(char **args);

/*
SHiV core functions
*/
int main(int argc, char *argv)
{
    printf("\x1b[31mSHiV Shell version %s. Type \"help\" for more.\x1b[0m\n", SHV_VERSION); 
    shv_loop();
    return EXIT_SUCCESS;
}


void shv_loop(void)
{
    char *line;
    char **args;
    int status;

    read_history("shv_history");

    do{
        line = shv_read_line();
        args = shv_split_line(line);
        status = shv_execute(args);

        free(line);    
        free(args);
    } while (status);


}


char *shv_read_line(void)
{

    int bufsize = SHV_RL_BUFSIZE;
    char *buffer = malloc(sizeof(char) * bufsize);

    if (!buffer){
        fprintf(stderr, "shv: memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    while ((buffer = readline("[shv]-$ ")) != NULL){
        if(buffer[0] != 0){
            add_history(buffer);
            return buffer;
        }
    }

    free(buffer);

    /* Manual implementation of reading line to buffer
    *************************************************
    int c;
    int position = 0;

    while (1){
        //Read a character
        c = getchar();

        if (c == EOF || c == '\n'){
            buffer[position] = '\0';
            return buffer;    
        }else{
            buffer[position] = c;
        }
        position ++;

        //If we exceed buffer, reallocate!
        if (position >= bufsize){
            bufsize += SHV_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer){
                fprintf(stderr, "shv: memory reallocation error\n");
                exit(EXIT_FAILURE);    
            }    
        }
    }*/
}


char **shv_split_line(char *line)
{
    int bufsize = SHV_TOK_BUFSIZE, position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if(!tokens){
        fprintf(stderr, "shv: memory allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, SHV_TOK_DELIM);

    while(token != NULL){
        tokens[position] = token;
        position ++;    

        if(position >= bufsize){
            bufsize += SHV_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));
            
            if(!tokens){
                fprintf(stderr, "shv: memory reallocation error\n");
                exit(EXIT_FAILURE);
            }
        }
        token = strtok(NULL, SHV_TOK_DELIM);
    }
    tokens[position] = NULL;
    return tokens;
}


int shv_launch(char **args)
{
    pid_t pid, wpid;
    int status;

    pid = fork();
    
    if (pid == 0){
        //child process
        if(execvp(args[0], args) == -1){
            perror("shv");    
        }
        exit(EXIT_FAILURE);
    }else if(pid < 0){
        //Error forking
        perror("shv");
    }else{
        //parent process
        do{
            wpid = waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    return 1;
}


int shv_execute(char **args)
{
    int i;
    if (args[0] == NULL){
        //empty command
        return 1;
    }

    for (i = 0; i < shv_num_builtins(); i++){
        if (strcmp(args[0], builtin_str[i]) == 0){
            return (*builtin_func[i])(args);   
        }
    }
    return shv_launch(args);
}


