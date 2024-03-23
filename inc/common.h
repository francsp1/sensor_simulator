#ifndef _COMMON_H_
#define _COMMON_H_

#define STATUS_ERROR   ( (int) (-1) )
#define STATUS_SUCCESS ( (int) (0)  )

#define MAX_PENDING_CONECTIONS (256)
#define MAX_BUFFER_SIZE (1024)

#define NUMBER_OF_SENSORS (3)

int validate_port(int server_port);
void disable_buffering(void);

void common(void);

#endif // _COMMON_H_