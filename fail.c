/*
 *
 * uBash
 *
 * fail.c
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

void fail_errno( char* error_msg){

    perror(error_msg);
    exit(EXIT_FAILURE);

}

void fail( char* error_msg ){

    fprintf(stderr,"%s\n" ,error_msg);
    exit(EXIT_FAILURE);

}
