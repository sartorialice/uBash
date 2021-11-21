#
#
# uBash
#
# Makefile
#
# 8-12-2020
#
# Progetto svolto a supporto del corso di Sistemi di Elaborazione e Trasmissione dell' Informazione binaria
# dell'Universita' degli Studi di Genova a.a. 2020/2021
#
# Alice Sartori: Matricola: 4819455
# Andrea Valenzano: Matricola: 4548315
# Davide Giovanetti: Matricola: 4823205
#
#
#


CC = clang
FLAGS = -Wall #-pedantic -Werror -std=gnu11 -O3
LINKFLAGS = -L/usr/include -lreadline
DEBUG_FLAGS = #-ggdb -fsanitize=address -fsanitize=leak

BIN_DIR = bin
TARGETS_DIR = targets

UBASH = $(BIN_DIR)/ubash
EXEC_OBJS = $(TARGETS_DIR)/exec.o
FAIL_OBJS = $(TARGETS_DIR)/fail.o
UBASHUTILS_OBJS = $(TARGETS_DIR)/ubashutils.o
UBASH_OBJS = $(TARGETS_DIR)/ubash.o

EXEC = $(UBASH)

all: $(EXEC)

$(TARGETS_DIR):
	mkdir targets
$(BIN_DIR):
	mkdir bin

$(EXEC_OBJS): libs/exec.c | $(TARGETS_DIR)
	$(CC) $(FLAGS) $(DEBUG_FLAGS) -c -o $@ libs/exec.c

$(FAIL_OBJS): libs/fail.c | $(TARGETS_DIR)
	$(CC) $(FLAGS) $(DEBUG_FLAGS) -c -o $@ libs/fail.c

$(UBASHUTILS_OBJS): libs/ubashutils.c | $(TARGETS_DIR)
	$(CC) $(FLAGS) $(DEBUG_FLAGS) -c -o $@ libs/ubashutils.c

$(UBASH_OBJS): ubash/ubash.c | $(TARGETS_DIR)
	$(CC) $(FLAGS) $(DEBUG_FLAGS) -c -o $@ ubash/ubash.c

$(UBASH): $(EXEC_OBJS) $(FAIL_OBJS) $(UBASHUTILS_OBJS) $(UBASH_OBJS) | $(BIN_DIR)
	$(CC) $(LINKFLAGS) $(DEBUG_FLAGS) -o $@ $(UBASH_OBJS) $(EXEC_OBJS) $(FAIL_OBJS) $(UBASHUTILS_OBJS)

clean:
	rm -rf $(EXEC) $(TARGETS_DIR) $(BIN_DIR)

tgz:
	rm -rf $(TARGETS_DIR) $(BIN_DIR)
	cd ..; tar cvzf uBash.tgz --exclude='.git' uBash

.PHONY: clean tgz
