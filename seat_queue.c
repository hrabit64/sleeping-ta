#include "seat_queue.h"

#include <stdio.h>

void seat_queue_init(SeatQueue *queue) {
    queue->pop_next = 0;
    queue->push_next = 0;
    queue->size = 0;

    for (int i = 0; i < SEAT_QUEUE_CAPACITY; i++) {
        queue->seats[i] = -1;
    }
}

bool is_seat_queue_empty(const SeatQueue *queue) {
    return queue->size == 0;
}

bool is_seat_queue_full(const SeatQueue *queue) {
    return queue->size == SEAT_QUEUE_CAPACITY;
}

int seat_queue_size(const SeatQueue *queue) {
    return queue->size;
}

int seat_queue_capacity(void) {
    return SEAT_QUEUE_CAPACITY;
}

bool seat_queue_push(SeatQueue *queue, int student_id) {
    if (is_seat_queue_full(queue)) {
        return false;
    }

    queue->seats[queue->push_next] = student_id;
    queue->push_next = (queue->push_next + 1) % SEAT_QUEUE_CAPACITY;
    queue->size++;

    return true;
}

bool seat_queue_pop(SeatQueue *queue, int *student_id) {
    if (is_seat_queue_empty(queue)) {
        return false;
    }

    *student_id = queue->seats[queue->pop_next];
    queue->seats[queue->pop_next] = -1;

    queue->pop_next = (queue->pop_next + 1) % SEAT_QUEUE_CAPACITY;
    queue->size--;

    return true;
}

void seat_queue_print(const SeatQueue *queue) {
    printf("[SeatQueue] seats=[");

    for (int i = 0; i < SEAT_QUEUE_CAPACITY; i++) {
        if (i > 0) {
            printf(", ");
        }

        if (queue->seats[i] == -1) {
            printf("-");
        } else {
            printf("%d", queue->seats[i]);
        }
    }

    printf("], pop_next=%d, push_next=%d, size=%d\n",
           queue->pop_next,
           queue->push_next,
           queue->size);
}