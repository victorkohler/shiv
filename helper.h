
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



