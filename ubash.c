/*
 *
 * uBash
 *
 * ubash .c
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

#include "../libs/ubashlib.h"

int main( int argc, char** argv){

    using_history();

    /* Inizializzo le varibili globali */
    SIGEXT = FALSE;
    HOME = getenv("HOME");
    if((last_dir = getcwd(NULL, 0)) == 0){
        fail_errno("Can't get current Directory");
    }
    exit_status = 0;

    /***** uBash Loop *****/
    while(SIGEXT == FALSE){

        /* Leggo da STDIN un comando e lo divido in tokens*/
        cmd = get_prompt();
        if( cmd == NULL ){
            free(cmd);
            free(input);
            continue;
        }

        /* Mando in esecuzione il il comando */
        exec_all();

        /* Libero la memoria precedentemente allocata */
        free(cmd);
        free(input);

    }
    clear_history();
    free(last_dir);
    exit(EXIT_SUCCESS);
}
