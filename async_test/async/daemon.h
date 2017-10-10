#ifndef _DEAMON_H_
#define _DEAMON_H_

#include "atomic.h"
#include "bindconf.h"

extern int max_fd_num;

volatile extern int stop;
volatile extern int restart;
volatile extern int term_signal;

extern char *prog_name;
extern char *current_dir;
extern char** saved_argv;

extern atomic_t child_pids[max_listen_fds];

int daemon_start(int argc, char** argv);
void daemon_set_title(const char *fmt,...);

void clean_child_pids();

#endif // _DEAMON_H_
