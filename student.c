#include "student.h"
#include "seat_queue.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#ifndef PROGRAMMING_SECONDS
#define PROGRAMMING_SECONDS 20
#endif

#ifndef REQUESTS_PER_STUDENT
#define REQUESTS_PER_STUDENT 20
#endif

extern SeatQueue seat_queue;

extern pthread_mutex_t seat_mutex;
extern pthread_mutex_t print_mutex;

extern sem_t waiting_Students;
extern sem_t called_Student[];
extern sem_t done_student[];

void student_init(Student *student, int id) {
    student->id = id;
    student->request_count = 0;
    student->state = STUDENT_PROGRAMMING;
}

// 현재 학생의 상태를 문자열로 반환하는 함수
const char *student_state_name(StudentState state) {
    switch (state) {
        case STUDENT_PROGRAMMING:
            return "Programming";
        case STUDENT_ASK_FOR_HELP:
            return "Ask-for-Help";
        default:
            return "Unknown";
    }
}

static void do_programming(Student *student) {
    student->state = STUDENT_PROGRAMMING;

    pthread_mutex_lock(&print_mutex);
    printf("[Student %d] State = %s. Programming for %d seconds.\n",
           student->id,
           student_state_name(student->state),
           PROGRAMMING_SECONDS);
    pthread_mutex_unlock(&print_mutex);

    sleep(PROGRAMMING_SECONDS);
}

static void ask_for_help(Student *student) {
    int id = student->id;

    student->state = STUDENT_ASK_FOR_HELP;

    pthread_mutex_lock(&print_mutex);
    printf("[Student %d] State = %s. Need TA help.\n",
           id,
           student_state_name(student->state));
    pthread_mutex_unlock(&print_mutex);

    // 항상 의자에 접근할 때는 seat_muxtex 락을 획득해야함.
    pthread_mutex_lock(&seat_mutex);
    
    //case 1: 의자에 앉을 수 있는 경우
    if (!is_seat_queue_full(&seat_queue)) {

        // 의자에 앉기
        seat_queue_push(&seat_queue, id);

        pthread_mutex_lock(&print_mutex);
        printf("[Student %d] Sit on a chair.\n", id);
        seat_queue_print(&seat_queue);
        pthread_mutex_unlock(&print_mutex);
        
        // TA에게 학생이 기다리고 있음을 알림
        sem_post(&waiting_Students);

        // 점유 해제
        pthread_mutex_unlock(&seat_mutex);

        // TA가 부를때 까지 대기
        sem_wait(&called_Student[id]);

        pthread_mutex_lock(&print_mutex);
        printf("[Student %d] Called by TA.\n", id);
        pthread_mutex_unlock(&print_mutex);
        // TA가 불렀으면, 도움을 받는 중이므로, TA가 도움을 다 줄 때까지 대기
        sem_wait(&done_student[id]);

        student->request_count++;
        student->state = STUDENT_PROGRAMMING;

        pthread_mutex_lock(&print_mutex);
        printf("[Student %d] Help done. Return to Programming. request_count=%d\n",
               id,
               student->request_count);
        pthread_mutex_unlock(&print_mutex);
    } else {

        //case 2: 의자에 앉을 수 없는 경우
        pthread_mutex_lock(&print_mutex);
        printf("[Student %d] No empty chair. Return to Programming.\n", id);
        seat_queue_print(&seat_queue);
        pthread_mutex_unlock(&print_mutex);

        // 점유 해제
        pthread_mutex_unlock(&seat_mutex);
        
        // 의자에 앉을 수 없으므로, 다시 프로그래밍 상태로 돌아감
        student->state = STUDENT_PROGRAMMING;
    }
}

void *student_thread(void *arg) {
    Student *student = (Student *) arg;

    while (1) {
        do_programming(student);
        ask_for_help(student);
    }

    return NULL;
}
