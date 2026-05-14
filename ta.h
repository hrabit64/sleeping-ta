#ifndef TA_H
#define TA_H

typedef enum {
    TA_SLEEPING,
    TA_TEACHING
} TAState;

void *ta_thread(void *arg);

const char *ta_state_name(TAState state);

#endif