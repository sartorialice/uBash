/*
 *
 * uBash
 *
 * ubashutils.c
 *
 * 8-12-2020
 *
 * Progetto svolto a supporto del corso di Sistemi di Elaborazione e Trasmissione dell' Informazione binaria
 * dell'Universita' degli Studi di Genova a.a. 2020/2021
 *
 * Alice Sartori: Matricola: 4819455
 * Andrea Valenzano: Matricola: 4548315
 * Davide Giovanetti: Matricola: 4823205
 *
 *
 */

#include "ubashlib.h"

char** get_prompt( void ){

    int dim = 0;
    char* prompt;
    char* cwd_aux;
    char* cwd;
    char* user;
    input = NULL;
    char** args;

    /* Trovo al directory corrente e l'username, dopo di che formatto il prompt */
    if((cwd_aux = getcwd(NULL, 0)) == NULL){
        fail_errno("Can't get current directory");
    }
    cwd = cwd_aux;
    if(strncmp(cwd_aux, HOME , strlen(HOME))== 0){
        cwd += strlen(HOME) -1;
        cwd[0] = '~';
    }
    dim += strlen(cwd);
    if((user = getenv("USER")) == NULL){
        fail_errno("Can't get current user");
    }
    dim += strlen(user);

    if(exit_status != 0){
        dim += strlen("[\033[32m%s\033[39m: %s] \033[33m%d \033[39m+  ");
        prompt = malloc(dim+sizeof(int));
        if(prompt == NULL){
            fail("malloc : Can't allocate more memory");
        }
        sprintf(prompt, "[\033[32m%s\033[39m: %s] \033[33m%d \033[39m+  ", user, cwd, exit_status);
    }

    else{
        dim += strlen("[\033[32m%s\033[39m: %s] +  ");
        prompt = malloc(dim);
        if(prompt == NULL){
            fail("malloc : Can't allocate more memory");
        }
        sprintf(prompt, "[\033[32m%s\033[39m: %s] +  ", user, cwd);
    }
    

    /* Leggo da input finche' non ottengo una stringa valida */
    for(;;){
        input = readline(prompt);  

        if(input == NULL){
            write(STDOUT, "\n", 1);
            continue;
        }
        if(input != NULL && strcmp(input, "") != 0){
            break;
        }
            free(input);
    }

    if(strlen(input) > 0){
        add_history(input);
    }

    /* Vado a dividere l'imput in token */
    args = tokenizer(input, "|");

    /* Libero la memoria allocata */
    free(cwd_aux);
    free(prompt);

    /* Ritorno l'array di stringhe contenente i comandi da eseguire ed i relativi parametri */
    return args;

}

char** tokenizer( char* input , char* delim){

    int iterator = 0;
    char** arg = NULL;
    if(strcmp(delim, "|") == 0)cmdc = 0;

    /* Con chiamate ripetute a strtok vado a suddividere l'input */
    char* buff = strtok(input, delim);

    while(buff){

        while(isspace(*buff)){
            buff++;
        }
        if(strcmp(buff, "") == 0){
            fprintf(stderr, "ubash : syntax error \"||\" unexpected\n");
            return NULL;
        }
        arg = realloc(arg, sizeof(char*) * ++iterator);
        if(arg==NULL)
            fail("realloc: can't allocate memory");
        arg[iterator-1] = buff;
        if(strcmp(delim, "|") == 0)cmdc++;
        buff = strtok(NULL, delim);

    }

    /* Nel caso abbia terminato l'input usando "\n" vado a rimuoverlo */
    arg[iterator-1] = strtok(arg[iterator-1], "\n");

    /* Aggiungo il terminatore alla fine dell puntatore */
    arg = realloc(arg, sizeof(char*) * ++iterator);
    if(arg==NULL)
        fail("realloc: can't allocate memory");
    arg[iterator-1] = 0;

    /* Ritorno il risultato */
    return arg;
}
