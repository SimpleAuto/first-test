#ifndef _DEAMON_H_
#define _DEAMON_H_

extern int max_fd_num;

volatile extern int stop;
volatile extern int restart;
volatile extern int term_signal;

extern char** saved_argv;

int daemon_start(int argc, char** argv);
void clean_child_pids();

#endif // _DEAMON_H_
