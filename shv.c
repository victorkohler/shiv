
/************************************

    [THE SHiV SHELL]

    * Version: 0.0.1

*************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <unistd.h>
#include <readline/history.h>
#include <readline/readline.h>

#define SHV_VERSION "0.0.1"
#define SHV_RL_BUFSIZE 1024
#define SHV_TOK_BUFSIZE 64
#define SHV_TOK_DELIM " \t\r\n\a"
#define SINGLE_QUOTE_ASCII 39
#define SHV_HISTORY "/etc/shv_history"

#include"builtin.h"
#include"helper.h"

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

    read_history(SHV_HISTORY);

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
    
    bool start_string = false;
    bool end_string = false;
    char string[1024] = {0,};

    while(token != NULL){

        int i;
        for (i = 0; i < strlen(token); i++){
            if((token[i] == '"' || token[i] == SINGLE_QUOTE_ASCII) && start_string == false){
                start_string = true;
            }else if((token[i] == '"' || token[i] == SINGLE_QUOTE_ASCII) && start_string == true){
                end_string = true;
                start_string = false;
            }
        }

        if (start_string == true){
            strcat(string, token);
            int l = strlen(string);
            string[l] = ' ';
        }else if (end_string == true){
            strcat(string, token);
            tokens[position] = string;
            end_string = false;
            position ++;    
        }else{
            tokens[position] = token;
            position ++;    
        }

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
    //pid_t pid, wpid;
    pid_t pid;
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
            waitpid(pid, &status, WUNTRACED);
        }while(!WIFEXITED(status) && !WIFSIGNALED(status));
        
        if(WCOREDUMP(status)){
            printf("\nCore Dumped Error. Code %d\n", status);   
        }

    }
    return 1;
}


int shv_execute(char **args)
{
    int i;
    int count = 0;
    
    char **temp = args;
    //char **temp = malloc(1024 * sizeof(char));
    //temp = args;

    if (args[0] == NULL){
        return 1;
    }
   

    //REMOVE QUOTES FROM STRING
    while(*temp != NULL){
        
        char *tempchar = *temp;
        tempchar = sanitize_quotes(tempchar);
        args[count] = tempchar;
        count ++;
        *temp++;
    }

    //CHECK IF BUILT IN FUNCTION  
    for (i = 0; i < shv_num_builtins(); i++){
        if (strcmp(args[0], builtin_str[i]) == 0){
            return (*builtin_func[i])(args);   
        }
    }
    return shv_launch(args);
}


