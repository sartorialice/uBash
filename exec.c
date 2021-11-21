/*
 *
 * uBash
 *
 * exec.c
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

void exec_all( void ){

    /* Controllo se il comando e' nativo della shell e in caso affermativo lo eseguo */
    if(is_native() == 0) return;

    /* In caso contrario eseguo il comando */
    int fd[2];
    int in_pipe = 0;
    pid_t* pids = malloc(sizeof(pid_t) * cmdc);
    if(pids == NULL){
        fail("malloc : Can't allocate more memory");
    }

    for(int i=0; i<cmdc; i++) {

        /* Creo la pipe e imposto la flag in modo che si chiuda una volta completata la exec */
        if(pipe(fd) == -1){
            fail_errno("pipe");
        }

        int flagswrite = fcntl(fd[WRITE], F_GETFL);
        if(flagswrite == -1){
            fail_errno("fcntl");
        }
        if(fcntl(fd[WRITE], F_SETFL, O_CLOEXEC) == -1){
            fail_errno("fcntl");
        }

        int flagsread = fcntl(fd[READ], F_GETFL);
        if(flagsread == -1){
            fail_errno("fcntl");
        }
        if(fcntl(fd[READ], F_SETFL, O_CLOEXEC) == -1){
            fail_errno("fcntl");
        }

        /* Faccio fork */
        switch(pids[i] = fork()){

            case -1:

                perror("fork()");
                return;

            break;

            /* Il figlio esegue il comando */
            case 0:
                if(i != 0){
                    if(dup2(in_pipe, STDIN) == -1){
                        fail_errno("dup2");
                    }
                }
                if(i != cmdc-1) {
                    if(dup2(fd[WRITE], STDOUT) == -1){
                        fail_errno("dup2");
                    }
                }
                if(close(fd[READ]) == -1){
                    fail_errno("close");
                }
                char** to_execute = redirect(cmd[i]);
                int ext = execvp(to_execute[0], to_execute);
                free(to_execute);
                exit(ext);

            break;

            /* Il padre memorizza l'uscita della pipe per il figlio successivo */
            default:

                in_pipe = fd[READ];
                if(close(fd[WRITE]) == -1){
                    fail_errno("close");
                }

            break;
        }
    }

    int status;
    pid_t last_pid;

    for(int i=0; i<cmdc; i++){

        last_pid = wait(&status);

         if (last_pid == -1) {
                       fail_errno("can't wait the process");
                       exit(EXIT_FAILURE);
                   }

            if (WIFSTOPPED(status)){
                exit_status = WSTOPSIG(status);
            }
            if(WIFEXITED(status)){

                if(WEXITSTATUS(status) == 255) {
                    fprintf(stderr, "ubash : Unknown command\n");
                    exit_status = WEXITSTATUS(status);
                }
                else {
                    exit_status = WEXITSTATUS(status);
                    kill_prev(pids, last_pid);
                }
 
            }
    }

    free(pids);
    return ;
}

int is_native(){

    for(int i=1; cmd[i]!=NULL; i++){
        if(strncmp(cmd[i], "cd", 2) == 0){
            fprintf(stderr, "cd : Can't pipe cd\n");
            return 0;
        }
    }

    char first_cmd[1024];
    strcpy(first_cmd, cmd[0]);
    char** aux = tokenizer(first_cmd, " ");

    if(strcmp(aux[0],"exit") == 0){
        do_exit();
        free(aux);
        return 0;
    }
    if(strcmp(aux[0], "cd") == 0){
        do_cd(aux);
        free(aux);
        return 0;
    }
    if(strcmp(aux[0], "_WHO_AM_I") == 0){
        do_version();
        free(aux);
        return 0;
    }
    free(aux);
    return -1;

}

void do_cd( char** cd_in){

    exit_status = 0;

    if(cmd[1] != NULL){
        printf("cd : Can't pipe CD\n");
        return;
    }
    if(cd_in[1] == NULL){
        free(last_dir);
        if((last_dir = getcwd(NULL, 0)) == 0){
            fail_errno("Can't get current Directory");
        }
        if(chdir(HOME) == -1){
            perror("cd");
        }
        return;
    }
    if(cd_in[2] != NULL){
        printf("cd : Too Many Arguments\n");
        return;
    }
    if(strcmp(cd_in[1], "-") == 0){
        char* aux = NULL;
        if((aux = getcwd(NULL, 0)) == 0){
            fail_errno("Can't get current Directory");
        }
        printf("%s\n", last_dir);
        if(chdir(last_dir) == -1){
            perror("cd");
            free(aux);
            return;
        }
        free(last_dir);
        last_dir = aux;
        return;
    }
    if(strncmp(cd_in[1], "~", 1) == 0){
        cd_in[1]++;
        char aux[512];
        strcpy(aux, HOME);
        if(chdir(strcat(aux,cd_in[1])) == -1){
            perror("cd");
        }
        return;
    }
    if(chdir(cd_in[1]) == -1){
        perror("cd");
    }
}

void do_exit( void ){
    SIGEXT = TRUE;
    return;
}


char** redirect( char* input){

    int iterator = 0;
    char** arg = NULL;

    /* Con chiamate ripetute a strtok vado a suddividere l'input */
    char* buff = strtok(input, " ");

    while(buff){

        while(isspace(*buff)){
            buff++;
        }

        /* Controllo se devo fare ridirezione\espandere macro e nel caso lo faccio */
        if(strncmp(buff,"$",1) == 0){
            buff++;
            buff=getenv(buff);
            if(buff == NULL){
                fprintf(stderr,"$ : Invalid Argument\n");
                exit(EXIT_FAILURE);
            }
        }

        if(strcmp(buff, ">") == 0){
            buff = strtok(NULL, " ");
            int fd_open = open(buff, O_CREAT|O_WRONLY|O_TRUNC, 0644);
            if(fd_open == -1){
                fail_errno("open");
            }
            if(dup2(fd_open, STDOUT) == -1){
                fail_errno("dup2");
            }
            if(close(fd_open) == -1){
                fail_errno("close");
            }
            buff = strtok(NULL, " ");
            continue;
        }
        else if(strncmp(buff, ">", 1) == 0){
            buff++;
            int fd_open = open(buff, O_CREAT|O_WRONLY|O_TRUNC, 0644);
            if(fd_open == -1){
                fail_errno("open");
            }
            if(dup2(fd_open, STDOUT) == -1){
                fail_errno("dup2");
            }
            if(close(fd_open) == -1){
                fail_errno("close");
            }
            buff = strtok(NULL, " ");
            continue;
        }

        if(strcmp(buff, "<") == 0){
            buff = strtok(NULL, " ");
            int fd_open = open(buff, O_RDONLY);
            if(fd_open == -1){
                fail_errno("open");
            }
            if(dup2(fd_open, STDIN) == -1){
                fail_errno("dup2");
            }
            if(close(fd_open) == -1){
                fail_errno("close");
            }
            buff = strtok(NULL, " ");
            continue;
        }
        else if(strncmp(buff, "<", 1) == 0){
            buff++;
            int fd_open = open(buff, O_RDONLY);
            if(fd_open == -1){
                fail_errno("open");
            }
            if(dup2(fd_open, STDIN) == -1){
                fail_errno("dup2");
            }
            if(close(fd_open) == -1){
                fail_errno("close");
            }
            buff = strtok(NULL, " ");
            continue;
        }

        /* Aggiungo il parametro all'array */
        arg = realloc(arg, sizeof(char*) * ++iterator);
        if(arg == NULL){
        	fail("realloc : Can't allocate more memory");
    	}
        arg[iterator-1] = buff;
        buff = strtok(NULL, " ");

    }

    /* Nel caso abbia terminato l'input usando "\n" vado a rimuoverlo */
    arg[iterator-1] = strtok(arg[iterator-1], "\n");

    /* Aggiungo il terminatore alla fine dell puntatore */
    arg = realloc(arg, sizeof(char*) * ++iterator);
    if(arg == NULL){
        	fail("realloc : Can't allocate more memory");
    }
    arg[iterator-1] = 0;

    /* Ritorno il risultato */
    return arg;
}

void kill_prev( pid_t* pids, pid_t son ){

    for(int j=1; j<cmdc; j++){
        if(pids[j] == son){
            kill(pids[j-1], SIGTERM);
            return;
        }
    }
}

void do_version( void ){

    printf( "\n\t          _____                         _\n"
            "\t         |   _  \\                      | |\n"
            "\t         |  | \\  |                     | |\n"
            "\t         |  |_/  |  _______   _______  | |\n"
            "\t _    _  |      /  |_____  | |  _____| | |____\n"
            "\t| |  | | |   _  \\   _____| | | |_____  |  __  |\n"
            "\t| |  | | |  | \\  | |  ___  | |_____  | | |  | |\n"
            "\t| |__| | |  |_/  | | [___] |  _____| | | |  | |\n"
            "\t|  ____| |_____ /  |_______| |_______| |_|  |_|\n"
            "\t| |\n"
            "\t| |\n"
            "\t| |\n"
            "\t|_|\n\n\n"
            "uBash\n"
            "Programma creato a supporto del corso di Sistemi di Elaborazione e Trasmissione Dell'informazione binaria presso "
            "l'Universita degli Studi di Genova a.a. 2020/2021\n\n"
            "Scritto da Andrea Valenzano, Alice Sartori e Davide Giovannetti\n"
            );

}
