
char *sanitize_quotes(char *tempchar);

char *sanitize_quotes(char *tempchar)
{
    unsigned long c;

    for (c = 0; c < strlen(tempchar); c++){
        if (tempchar[c] == '"' || tempchar[c] == SINGLE_QUOTE_ASCII){
            strcpy(tempchar + c, tempchar + c + 1);
        }
    }
    return tempchar;
}



char *Get_current_path()
{
    int size = 80;
    char *cwd = malloc(size);

    if(!cwd){
        fprintf(stderr, "Could not allocate cwd\n");
    }
    
    if(!getcwd(cwd, size)){
        perror("Error. Could not get the current path");    
    }
    return cwd;
}

