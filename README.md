# Sleeping TA Simulation

한양대에리카 인공지능학과 26년 1학기 운영체제 과제입니다.

## How to Run

```bash
cd /path/to/your-ta-directory
git clone . 
make
./sleeping_ta
```

## 개요

* seat_queue.c: 대기열 관리
* student.c: 학생 스레드
* ta.c: TA 스레드
* main.c: 초기화 및 스레드 생성

## 구현

* mutex
- `seat_mutex` : 대기열 접근 제어

* semaphore
- `waiting_Students` : 대기 중인 학생 수
- `called_Student[]` : TA가 호출한 학생을 위한 세마포어
- `done_student[]` : 학생이 TA와 상담을 마쳤음을 알리는 세마포어


### 1. 학생 자리 잡기

학생은 `seat_mutex`를 잠그고 대기열에 자리가 있는지 확인

#### a) 자리가 있을때
자리가 있으면 학생은 대기열에 앉고 `waiting_Students` 세마포어를 증가.
그런 다음 학생은 `called_Student[student->id]` 세마포어에서 TA가 자신을 호출할 때까지 대기

#### b) 자리가 없을 때
학생은 자리가 없다는 메시지를 출력하고 `seat_mutex`를 해제한 후 programming state

### 2. TA 상담
TA는 `waiting_Students` 세마포어에서 대기 중인 학생이 있는지 확인. 
'학생이 있으면 TA는 `seat_mutex`를 잠그고 대기열에서 학생을 호출.
TA는 `called_Student[student_id]` 세마포어를 증가시켜 학생을 깨우고 `seat_mutex`를 해제.
TA는 학생과 상담을 진행한 후 `done_student[student_id]` 세마포어를 증가시켜 학생에게 상담이 끝났음을 알림.

### 3. 학생 상담 종료
학생은 `done_student[student->id]` 세마포어에서 TA가 상담이 끝났음을 알 때까지 대기. 
상담이 끝나면 학생은 메시지를 출력하고 programming state로 돌아감.