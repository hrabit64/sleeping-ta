#ifndef SEAT_QUEUE_H
#define SEAT_QUEUE_H

#include <stdbool.h>

#ifndef SEAT_QUEUE_CAPACITY
#define SEAT_QUEUE_CAPACITY 3
#endif

/*
* pop_next: 다음에 pop할 위치
* push_next: 다음에 push할 위치
* size: 현재 큐에 있는 학생 수
*/
typedef struct {
    int seats[SEAT_QUEUE_CAPACITY];
    int pop_next;
    int push_next;
    int size;
} SeatQueue;


void seat_queue_init(SeatQueue *queue);

bool is_seat_queue_empty(const SeatQueue *queue);

bool is_seat_queue_full(const SeatQueue *queue);

int seat_queue_size(const SeatQueue *queue);

int seat_queue_capacity(void);

bool seat_queue_push(SeatQueue *queue, int student_id);

bool seat_queue_pop(SeatQueue *queue, int *student_id);

void seat_queue_print(const SeatQueue *queue);

#endif