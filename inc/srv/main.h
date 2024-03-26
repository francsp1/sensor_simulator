#ifndef _MAIN_SRV_H_
#define _MAIN_SRV_H_

void *handle_client(void *arg);
int init_signal_handlers(struct sigaction *sa);
void signal_handler(int signum);

#endif // _MAIN_SRV_H_