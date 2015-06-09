
/*
Function declaration for built in commands
*/
int shv_cd(char **args);
int shv_help(char **args);
int shv_exit(char **args);
int shv_path(char **args);

/*
List of builtin commands, followed by their corresponding functions.
*/

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "path"
};

int (*builtin_func[]) (char **) = {
    &shv_cd,
    &shv_help,
    &shv_exit,
    &shv_path
};

int shv_num_builtins()
{
    return sizeof(builtin_str) / sizeof(char *);
}

int shv_cd(char **args)
{
    if(args[1] == NULL){
        fprintf(stderr, "shv: expected argument to \"cd\"\n");    
    }else{
        if(chdir(args[1]) != 0){
            perror("shv");    
        }
    }
    return 1;
}

int shv_help(char **args)
{
    int i;
    printf("The SHiV Shell version 0.0.1\n");
    printf("Type program names and arguments and hit ender\n");
    printf("The following are built in:\n");
    
    for (i = 0; i < shv_num_builtins(); i++){
        printf("  %s\n", builtin_str[i]);    
    }

    printf("Use the man command for info on other programms.\n");
    return 1;
}


int shv_path(char **args)
{
    char cwd[1024];

    if(!cwd){
        fprintf(stderr, "Could not allocate cwd\n");
    }

    if(getcwd(cwd, sizeof(cwd)) != NULL){
        printf("%s\n", cwd);
    }else{
        perror("Error. Could not get the current path");    
    }

    return 1;


}


int shv_exit(char **args)
{
    write_history(SHV_HISTORY);
    return 0;    
}


