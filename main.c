#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

#include "seat_queue.h"
#include "student.h"
#include "ta.h"

#define NUM_STUDENTS 20

SeatQueue seat_queue;

pthread_mutex_t seat_mutex;
pthread_mutex_t print_mutex;

sem_t waiting_Students;
sem_t called_Student[NUM_STUDENTS];
sem_t done_student[NUM_STUDENTS];

int simulation_running = 1;

static void exit_with_error(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

static void check_pthread(int result, const char *message) {
    if (result != 0) {
        fprintf(stderr, "%s failed: %d\n", message, result);
        exit(EXIT_FAILURE);
    }
}

static void check_sem(int result, const char *message) {
    if (result == -1) {
        exit_with_error(message);
    }
}

int main(void) {
    pthread_t ta;
    pthread_t student_threads[NUM_STUDENTS];
    Student students[NUM_STUDENTS];

    setvbuf(stdout, NULL, _IONBF, 0);

    seat_queue_init(&seat_queue);

    check_pthread(
        pthread_mutex_init(&seat_mutex, NULL),
        "pthread_mutex_init"
    );
    check_pthread(
        pthread_mutex_init(&print_mutex, NULL),
        "pthread_mutex_init print_mutex"
    );

    check_sem(
        sem_init(&waiting_Students, 0, 0),
        "sem_init Waiting_Students"
    );

    for (int i = 0; i < NUM_STUDENTS; i++) {
        check_sem(
            sem_init(&called_Student[i], 0, 0),
            "sem_init Called_Student"
        );

        check_sem(
            sem_init(&done_student[i], 0, 0),
            "sem_init Done_student"
        );
    }

    for (int i = 0; i < NUM_STUDENTS; i++) {
        student_init(&students[i], i);
    }

    printf("===== Sleeping TA Simulation Start =====\n");
    printf("Students: %d\n", NUM_STUDENTS);
    printf("Chairs: %d\n", seat_queue_capacity());
    printf("========================================\n");

    check_pthread(
        pthread_create(&ta, NULL, ta_thread, NULL),
        "pthread_create ta"
    );

    for (int i = 0; i < NUM_STUDENTS; i++) {
        check_pthread(
            pthread_create(&student_threads[i], NULL, student_thread, &students[i]),
            "pthread_create student"
        );
    }

    for (int i = 0; i < NUM_STUDENTS; i++) {
        check_pthread(
            pthread_join(student_threads[i], NULL),
            "pthread_join student"
        );
    }

    simulation_running = 0;

    check_sem(
        sem_post(&waiting_Students),
        "sem_post Waiting_Students"
    );

    check_pthread(
        pthread_join(ta, NULL),
        "pthread_join ta"
    );

    check_sem(
        sem_destroy(&waiting_Students),
        "sem_destroy Waiting_Students"
    );

    for (int i = 0; i < NUM_STUDENTS; i++) {
        check_sem(
            sem_destroy(&called_Student[i]),
            "sem_destroy Called_Student"
        );

        check_sem(
            sem_destroy(&done_student[i]),
            "sem_destroy Done_student"
        );
    }

    check_pthread(
        pthread_mutex_destroy(&seat_mutex),
        "pthread_mutex_destroy"
    );
    check_pthread(
        pthread_mutex_destroy(&print_mutex),
        "pthread_mutex_destroy print_mutex"
    );

    printf("===== Sleeping TA Simulation Finished =====\n");

    return 0;
}