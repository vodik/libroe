#ifndef SMALLHTTP_SERVER
#define SMALLHTTP_SERVER

typedef int (*ready_cb)(int id, int fd);

typedef struct sockserver sockserver;

sockserver *startserver(int port);
void pollserver(sockserver *, int timeout, ready_cb callback);

#endif
