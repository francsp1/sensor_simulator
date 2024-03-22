#ifndef _COMMON_H_
#define _COMMON_H_

#define STATUS_ERROR   (-1)
#define STATUS_SUCCESS (0)

#define MAX_PENDING_CONECTIONS 256

int validate_port(int server_port);

void common(void);

#endif // _COMMON_H_