#include "ta.h"
#include "seat_queue.h"

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define TEACHING_SECONDS 5

extern SeatQueue seat_queue;

extern pthread_mutex_t seat_mutex;

extern sem_t waiting_Students;
extern sem_t called_Student[];
extern sem_t done_student[];

extern int simulation_running;

static TAState ta_state = TA_SLEEPING;

const char *ta_state_name(TAState state) {
    switch (state) {
        case TA_SLEEPING:
            return "Sleeping";
        case TA_TEACHING:
            return "Teaching";
        default:
            return "Unknown";
    }
}

static void teach_student(int student_id) {
    ta_state = TA_TEACHING;

    printf("[TA] State = %s. Helping Student %d for %d seconds.\n",
           ta_state_name(ta_state),
           student_id,
           TEACHING_SECONDS);

    sleep(TEACHING_SECONDS);

    printf("[TA] Finished helping Student %d.\n", student_id);
}

void *ta_thread(void *arg) {
    (void)arg;

    while (1) {
        ta_state = TA_SLEEPING;

        printf("[TA] State = %s. Waiting for students.\n",
               ta_state_name(ta_state));

        // 학생이 기다리고 있음을 알리는 세마포어를 기다림
        sem_wait(&waiting_Students);
        
        // 깨어나면, 부를 학생을 가져와야하니, seat_mutex 락을 획득해야함
        pthread_mutex_lock(&seat_mutex);
        
        // 시뮬레이션이 종료되었고, 대기중인 학생이 없는 경우, TA 스레드 종료
        if (!simulation_running && is_seat_queue_empty(&seat_queue)) {
            pthread_mutex_unlock(&seat_mutex);
            break;
        }

        int student_id;
        // seat에서 학생을 pop
        if (!seat_queue_pop(&seat_queue, &student_id)) {
            // 이때 pop 실패면 그냥 락 해제하고 다시 학생이 기다리는 세마포어를 기다리러 감
            pthread_mutex_unlock(&seat_mutex);
            continue;
        }

        printf("[TA] Pop Student %d from seat queue.\n", student_id);
        seat_queue_print(&seat_queue);

        // 학생을 pop했으면, 락 해제
        pthread_mutex_unlock(&seat_mutex);

        // TA가 학생을 부름
        sem_post(&called_Student[student_id]);

        // TA가 학생을 가르침
        teach_student(student_id);

        // TA가 학생 가르치는 거 끝났으면, 학생이 도움 다 받았다고 알려줌
        sem_post(&done_student[student_id]);
    }

    printf("[TA] Thread finished.\n");
    return NULL;
}