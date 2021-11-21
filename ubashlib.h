/*
 *
 * uBash
 *
 * ubashlib.h
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

#ifndef UBASHLIB_H
#define UBASHLIB_H

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <signal.h>

/* Defines */
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define FALSE 1
#define TRUE 0
#define READ 0
#define WRITE 1

/* Globals */
char* HOME;
int exit_status;
char* input;
char** cmd;
int cmdc;
char* last_dir;
int SIGEXT;

/***** ubashutils.c *****/
extern char** get_prompt( void );
extern char** tokenizer( char*, char* );

/***** exec.c *****/
extern void exec_all( void );
extern int is_native( void );
extern void do_cd( char** );
extern void do_exit( void );
extern void do_version( void );
extern char** redirect( char* );
extern void kill_prev( pid_t*, pid_t);

/***** fail.c *****/
extern void fail_errno( char* );
extern void fail( char* );

#endif /* #ifdef UBASHLIB_H */
