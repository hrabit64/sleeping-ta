#ifndef STUDENT_H
#define STUDENT_H

#include <pthread.h>

// 학생의 상태를 나타내는 열거형
typedef enum {
    STUDENT_PROGRAMMING,
    STUDENT_ASK_FOR_HELP,
} StudentState;

// 학생 구조체
typedef struct {
    int id;
    int request_count;
    StudentState state;
} Student;

void student_init(Student *student, int id);

void *student_thread(void *arg);

const char *student_state_name(StudentState state);

#endif